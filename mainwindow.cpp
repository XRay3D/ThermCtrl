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
    ui->statusbar->setFont(font());

    auto toolBar = addToolBar("Поиск");
    toolBar->addAction(QIcon::fromTheme(""), "Поиск термокамер", this, &MainWindow::searchForThermalChambers);

    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    settings.endGroup();
}

MainWindow::~MainWindow() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.endGroup();
    delete ui;
}

void MainWindow::searchForThermalChambers() {

    auto availablePorts { QSerialPortInfo::availablePorts().toVector() };
    std::ranges::sort(availablePorts, {}, [](const QSerialPortInfo& pi) {
        return pi.portName().midRef(3).toInt();
    });

    Irt5502 irt;

    QProgressDialog progress("Поиск термокамер", "Остановить", 0, availablePorts.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    for (int i {}; auto& pi : availablePorts) {
        if (progress.wasCanceled())
            break;
        if (irt.ping(pi.portName(), 19200)) {
            if (map.contains(pi.portName()))
                continue;
            auto tc = new ThermCtrl(pi.portName(), this);
            connect(ui->statusbar, &QStatusBar::showMessage, tc, &ThermCtrl::showMessage);
            connect(ui->statusbar, &QStatusBar::showMessage, [this](const QString& text, int) {
                ui->plainTextEdit->appendPlainText(text);
            });
            map.emplace(pi.portName(), tc);
        }
        progress.setValue(i++);
        QApplication::processEvents();
    }
    progress.setValue(availablePorts.size());

    ui->tabWidget->clear();
    for (auto& [portName, thermCtrl] : map)
        ui->tabWidget->addTab(thermCtrl, portName);
}
