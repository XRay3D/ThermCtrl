#include "mainwindow.h"
#include "irt5502.h"
#include "thermctrl.h"
#include "ui_mainwindow.h"
#include <QProgressDialog>
#include <QSerialPortInfo>
#include <QSettings>
#include <QToolBar>
#include <ranges>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow } {
    ui->setupUi(this);

    connect(this, &MainWindow::log, ui->teLog, &QTextEdit::append, Qt::QueuedConnection);
    connect(this, &MainWindow::logColor, ui->teLog, &QTextEdit::setTextColor, Qt::QueuedConnection);

    ui->statusbar->setFont(font());

    auto toolBar = addToolBar("Поиск");
    toolBar->setObjectName("toolBar");
    toolBar->addAction(QIcon::fromTheme(""), "Поиск термокамер", this, &MainWindow::searchForThermalChambers);

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
        delete thermCtrl;
    delete ui;
}

void MainWindow::searchForThermalChambers() {
    Elemer::Timer t(__FUNCTION__);
    auto availablePorts { QSerialPortInfo::availablePorts().toVector() };
    std::ranges::sort(availablePorts, {}, [](const QSerialPortInfo& pi) {
        return pi.portName().midRef(3).toInt();
    });

    QProgressDialog progress("Поиск термокамер", "Остановить", 0, availablePorts.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    Irt5502* irt {};

    for (int i {}; auto& pi : availablePorts) {
        Elemer::Timer t("availablePorts");
        qDebug() << "vendorIdentifier" << pi.vendorIdentifier();
        qDebug() << "portName" << pi.portName();
        qDebug() << "serialNumber" << pi.serialNumber();

        if (0&&pi.vendorIdentifier() != 1027) {
            progress.setValue(i++);
            continue;
        }

        if (progress.wasCanceled())
            break;
        irt = new Irt5502;
        if (irt->ping(pi.portName(), 19200, 1)) {

            if (map.contains(pi.portName())) {
                progress.setValue(i++);
                delete irt;
                continue;
            }

            auto tc = new ThermCtrl(irt, pi.serialNumber(), this);

            connect(tc, &ThermCtrl::updateTabText, this, &MainWindow::updateTabText);
            connect(tc, &ThermCtrl::updateIcon, this, &MainWindow::setIcon);
            connect(tc, &ThermCtrl::showMessage, ui->statusbar, &QStatusBar::showMessage);
            connect(tc, &ThermCtrl::showMessage, [this](const QString& text, int) {
                ui->teLog->append(text);
            });
            map.emplace(pi.portName(), tc);
        }
        if (irt)
            delete irt;
        progress.setValue(i++);
        QApplication::processEvents();
    }

    progress.setValue(availablePorts.size());

    ui->tabWidget->clear();

    for (auto& [portName, thermCtrl] : map)
        ui->tabWidget->addTab(thermCtrl, thermCtrl->name().size() ? thermCtrl->name() : portName);
}

void MainWindow::updateTabText(const QString& text) {
    auto tw = ui->tabWidget;
    tw->setTabText(tw->indexOf(qobject_cast<QWidget*>(sender())), text);
}

void MainWindow::setIcon(bool runing) {
    auto tw = ui->tabWidget;
    tw->setTabIcon(tw->indexOf(qobject_cast<QWidget*>(sender())), runing ? QIcon(QString::fromUtf8(":/res/media-playback-start.svg")) : QIcon {});
}

void MainWindow::showEvent(QShowEvent* event) {
    if (showEventSkip)
        return;
    showEventSkip = true;
    QTimer::singleShot(200, this, &MainWindow::searchForThermalChambers);
}
