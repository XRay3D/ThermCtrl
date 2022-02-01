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
    teLog = ui->teLog;
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
    QTimer::singleShot(100, this, &MainWindow::searchForThermalChambers);
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

        if (pi.vendorIdentifier() != 1027) {
            progress.setValue(i++);
            continue;
        }

        if (progress.wasCanceled())
            break;
        if (!irt)
            irt = new Irt5502;
        if (irt->ping(pi.portName(), 19200, 1)) {

            if (map.contains(pi.portName())) {
                progress.setValue(i++);
                continue;
            }
            auto tc = new ThermCtrl(irt, this);
            connect(tc, &ThermCtrl::showMessage, ui->statusbar, &QStatusBar::showMessage);
            connect(tc, &ThermCtrl::showMessage, [this](const QString& text, int) {
                ui->teLog->append(text);
            });
            map.emplace(pi.portName(), tc);
        }
        progress.setValue(i++);
        QApplication::processEvents();
    }

    progress.setValue(availablePorts.size());

    ui->tabWidget->clear();

    for (auto& [portName, thermCtrl] : map) {
        Elemer::Timer t("for");
        ui->tabWidget->addTab(thermCtrl, portName);
    }
}
