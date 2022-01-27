#include "mainwindow.h"
#include "thermctrl.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow } {
    ui->setupUi(this);
    ui->statusbar->setFont(font());

    auto toolBar = addToolBar("Поиск");
    toolBar->addAction(QIcon::fromTheme(""), "Поиск термокамер", [this] {
        ui->tabWidget->addTab(new ThermCtrl(this), "qwe");
    });

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
