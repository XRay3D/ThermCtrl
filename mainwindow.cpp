#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "irt5502.h"
#include "pointmodel.h"

#include <QDebug>
#include <QScrollBar>
#include <QSettings>
#include <algorithm>
#include <ranges>

using PortInfo = QSerialPortInfo;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow }
    , pointModel { new PointModel(this) }
    , irt { new Irt5502(this) }
{
    ui->setupUi(this);
    ui->statusbar->setFont(font());

    // connect pointModel
    connect(ui->spinBoxPoints, qOverload<int>(&QSpinBox::valueChanged), pointModel, &PointModel::setPointCount);
    connect(pointModel, &PointModel::message, ui->statusbar, &QStatusBar::showMessage);
    pointModel->load();

    // setup comboBoxPort
    auto ports { PortInfo::availablePorts() };
    std::sort(ports.begin(), ports.end(), [](const PortInfo& pil, const PortInfo& pir) {
        return pil.portName().midRef(3).toInt() < pir.portName().midRef(3).toInt();
    });
    for (auto& port : ports)
        ui->comboBoxPort->addItem(port.portName());

    // setup comboBoxDevice
    ui->comboBoxDevice->addItem("ИРТ5502");

    // setup tableViewPoints
    ui->tableViewPoints->setModel(pointModel);
    ui->tableViewPoints->horizontalHeader()->setDefaultSectionSize(50);
    ui->tableViewPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewPoints->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.setValue("Port", ui->comboBoxPort->currentText());
    settings.setValue("Address", ui->spinBoxAddress->value());
    settings.setValue("Points", ui->spinBoxPoints->value());
    settings.endGroup();

    settings.beginGroup("Splitter");
    settings.setValue("State", ui->splitter->saveState());
    settings.endGroup();
}

void MainWindow::loadSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    ui->comboBoxPort->setCurrentText(settings.value("Port").toString());
    ui->spinBoxAddress->setValue(settings.value("Address", 1).toInt());
    ui->spinBoxPoints->setValue(settings.value("Points", 1).toInt());
    settings.endGroup();

    settings.beginGroup("Splitter");
    ui->splitter->restoreState(settings.value("State").toByteArray());
    settings.endGroup();
}

void MainWindow::updateTableViewPointsHeight()
{
    auto sb = ui->tableViewPoints->horizontalScrollBar();
    int height = ui->tableViewPoints->horizontalHeader()->height()
        + ui->tableViewPoints->rowHeight(0) * 3
        + (sb->isVisible() ? sb->height() : sb->height())
        + 2;
    ui->tableViewPoints->setMinimumHeight(height);
    ui->tableViewPoints->setMaximumHeight(height);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    updateTableViewPointsHeight();
}

void MainWindow::on_pushButtonFind_clicked()
{
    qDebug() << __FUNCTION__;
    ui->statusbar->showMessage("Поиск термокамеры...", 1000);

    ui->statusbar->showMessage("Термокамера найдена", 1000);

    ui->statusbar->showMessage("Нет связи", 1000);
}

void MainWindow::on_pushButtonAutoStartStop_clicked(bool checked)
{
    ui->groupBoxMan->setEnabled(!checked);
    if (checked) {
    } else {
    }
}

void MainWindow::on_pushButtonManStartStop_clicked(bool checked)
{
    if (irt->isConnected()) {
        ui->statusbar->showMessage(checked ? "камера включена" : "камера остановлена");
        ui->pushButtonManStartStop->setText(checked ? "Стоп" : "Старт на уставку");
        ui->groupBoxAuto->setEnabled(!checked);
    } else {
        ui->pushButtonManStartStop->setChecked(false);
        ui->statusbar->showMessage("Нет связи с термокамерой");
    }
}

void MainWindow::on_pushButtonReadTemp_clicked()
{
}

void MainWindow::on_doubleSpinBoxSetPoint_valueChanged(double arg1)
{
}
