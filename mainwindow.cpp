#include "mainwindow.h"
#include "irt5502.h"
#include "thermctrl.h"
#include "ui_mainwindow.h"
#include <QProgressDialog>
#include <QSerialPortInfo>
#include <QSettings>
#include <QToolBar>
#include <ranges>

std::vector<QAction*> actions;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow } {
    setIconSize({ 16, 16 });
    QIcon::setThemeName(1 ? "ggeasy-light" : "ggeasy-dark");

    ui->setupUi(this);

    connect(this, &MainWindow::log, ui->teLog, &QTextEdit::append, Qt::QueuedConnection);
    connect(this, &MainWindow::logColor, ui->teLog, &QTextEdit::setTextColor, Qt::QueuedConnection);

    ui->statusbar->setFont(font());

    toolBar = addToolBar("Поиск");
    toolBar->setObjectName("toolBar");
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addAction(QIcon::fromTheme("draw-rectangle"), "Поиск термокамер", this, &MainWindow::searchForThermalChambers);
    ::actions.resize(6);
    int i {};
    ::actions[i++] = toolBar->addSeparator();
    ::actions[i++] = toolBar->addAction(QIcon::fromTheme("document-edit"), "Переименовать термокамеру", [this] { currentTc()->rename(); });
    ::actions[i++] = toolBar->addSeparator();
    ::actions[i++] = toolBar->addAction(QIcon::fromTheme("document-open"), "Загрузить параметры регулирования", [this] { currentTc()->loadPoints(); });
    ::actions[i++] = toolBar->addSeparator();
    ::actions[i++] = toolBar->addAction(QIcon::fromTheme("document-save"), "Сохранить параметры регулирования", [this] { currentTc()->savePoints(); });

    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    ui->splitter->restoreState(settings.value("splitter").toByteArray());
    settings.endGroup();
}

MainWindow::~MainWindow() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.setValue("splitter", ui->splitter->saveState());
    settings.endGroup();
    for (auto& [portName, thermCtrl] : map)
        delete thermCtrl.tc;
    delete ui;
}

void MainWindow::searchForThermalChambers() {
    Elemer::Timer t(__FUNCTION__);
    auto availablePorts { QSerialPortInfo::availablePorts().toVector() };
//    std::ranges::sort(availablePorts, {}, [](const QSerialPortInfo& pi) {
//        return pi.portName().midRef(3).toInt();
//    });

    QProgressDialog progress("Поиск термокамер", "Остановить", 0, availablePorts.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    connect(this, &MainWindow::progressUpdateValue, &progress, &QProgressDialog::setValue, Qt::QueuedConnection);
    std::mutex mutex;
    std::vector<std::future<void>> futures;
    for (int i {}; auto& pi : availablePorts) {
        futures.emplace_back(std::async(std::launch::async, [this, &i, &mutex, &progress, pi]() -> void {
            Elemer::Timer t("Search TC");
            do {
                qDebug() << "    PortName" << pi.portName();
                qDebug() << "    SerialNumber" << pi.serialNumber();
                qDebug() << "    VendorIdentifier" << pi.vendorIdentifier();

#ifndef EL_EMU
                if (pi.vendorIdentifier() != 1027)
                    break;
#endif
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    if (map.contains(pi.portName()))
                        break;
                }

                if (progress.wasCanceled()) {
                    i = -1;
                    return;
                }

                auto irt = new Irt5502;
                if (irt->ping(pi.portName(), 19200, 1)) {
                    std::lock_guard<std::mutex> guard(mutex);
                    map.emplace(pi.portName(), TC { nullptr, irt, pi.serialNumber() });
                    irt->moveToThread(this->thread());
                    break;
                }
                delete irt;
            } while (0);
            emit progressUpdateValue(++i);
        }));
        if (i == -1)
            break;
        QApplication::processEvents();
    }

    for (auto&& future : futures) {
        future.wait();
        QApplication::processEvents();
    }

    progress.setValue(availablePorts.size());

    ui->tabWidget->clear();

    for (auto act : ::actions)
        act->setVisible(false);

    for (auto& [portName, thermCtrl] : map) {
        auto& [tc, irt, sn] = thermCtrl;
        if (!tc) {
            tc = new ThermCtrl(irt, sn, this);
            connect(tc, &ThermCtrl::updateTabText, this, &MainWindow::updateTabText);
            connect(tc, &ThermCtrl::updateIcon, this, &MainWindow::setIcon);
            connect(tc, &ThermCtrl::showMessage, this, &MainWindow::showMessage);
            connect(tc->splitter(), &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);
        }
        ui->tabWidget->addTab(tc, tc->name().size() ? tc->name() : portName);
    }

    if (map.size())
        for (auto act : ::actions)
            act->setVisible(true);
}

void MainWindow::updateTabText(const QString& text) {
    auto tw = ui->tabWidget;
    auto index = tw->indexOf(qobject_cast<QWidget*>(sender()));
    tw->setTabText(index, text);
}

void MainWindow::setIcon(bool runing) {
    auto tw = ui->tabWidget;
    auto index = tw->indexOf(qobject_cast<QWidget*>(sender()));
    tw->setTabIcon(index, runing ? QIcon(QString::fromUtf8(":/res/media-playback-start.svg")) : QIcon {});
    if (index == tw->currentIndex())
        enablePointActions(!static_cast<ThermCtrl*>(sender())->isRunning());
}

void MainWindow::showMessage(const QString& text, int /*timeout*/) {
    //ui->statusbar->showMessage(text, timeout);
    ui->teLog->append(text);
}

void MainWindow::enablePointActions(bool enabled) {
    toolBar->actions().at(2)->setEnabled(enabled);
    toolBar->actions().at(3)->setEnabled(enabled);
}

ThermCtrl* MainWindow::currentTc() const {
    return static_cast<ThermCtrl*>(ui->tabWidget->currentWidget());
}

void MainWindow::splitterMoved(int pos, int index) {
    auto state = static_cast<ThermCtrl*>(ui->tabWidget->currentWidget())->splitter()->saveState();
    for (auto& [portName, thermCtrl] : map) {
        auto& [tc, irt, sn] = thermCtrl;
        if (tc->splitter() != sender())
            tc->splitter()->restoreState(state);
    }
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    if (showEventSkip)
        return;
    showEventSkip = true;
    QTimer::singleShot(200, this, &MainWindow::searchForThermalChambers);
}

void MainWindow::on_tabWidget_currentChanged(int index) {
    if (index > -1)
        enablePointActions(!currentTc()->isRunning());
}
