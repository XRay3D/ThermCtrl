#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "irt5502.h"
#include "pointmodel.h"

#include <QDateTimeEdit>
#include <QDebug>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QStyledItemDelegate>
#include <algorithm>
#include <ranges>

using PortInfo = QSerialPortInfo;

class MyItemDelegate : public QStyledItemDelegate {
public:
    MyItemDelegate(QWidget* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    // QAbstractItemDelegate interface
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const override
    {
        if (!index.row()) {
            auto dsbx = new DoubleSpinBox(parent);
            dsbx->setRange(-999, +999);
            dsbx->setButtonSymbols(QAbstractSpinBox::NoButtons);
            dsbx->setDecimals(2);
            dsbx->setSingleStep(1.0);
            dsbx->setAlignment(Qt::AlignCenter);
            return dsbx;
        } else {
            auto timeEdit = new QTimeEdit(parent);
            timeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
            timeEdit->setAlignment(Qt::AlignCenter);
            return timeEdit;
        }
    }
};

enum { ColumnWidth = 50 };

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui { new Ui::MainWindow }
    , pointModel { new PointModel(this) }
    , irt { new Irt5502() }
{
    ui->setupUi(this);
    ui->statusbar->setFont(font());

    irt->moveToThread(&irtThread);
    connect(&irtThread, &QThread::finished, irt, &QObject::deleteLater);
    irtThread.start();

    // connect irt
    connect(irt, &Irt5502::message, ui->statusbar, &QStatusBar::showMessage);
    connect(irt, &Irt5502::measuredValue, ui->dsbxReadTemp, &QDoubleSpinBox::setValue);
    connect(irt, &Irt5502::measuredValue, ui->chartView, &ChartView::addPoint);
    connect(this, &MainWindow::getValue, irt, &Irt5502::getMasuredValue);

    // setup cmbxPort
    auto ports { PortInfo::availablePorts().toVector() };
    std::ranges::sort(ports, {}, [](const PortInfo& pil) { return pil.portName().midRef(3).toInt(); });
    for (auto& port : ports)
        ui->cmbxPort->addItem(port.portName());

    // setup cmbxDevice
    ui->cmbxDevice->addItem("ИРТ5502");

    // setup tableViewPoints
    ui->tableViewPoints->setModel(pointModel);
    ui->tableViewPoints->horizontalHeader()->setDefaultSectionSize(ColumnWidth);
    ui->tableViewPoints->horizontalHeader()->setMinimumSectionSize(ColumnWidth);
    ui->tableViewPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewPoints->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewPoints->setItemDelegate(new MyItemDelegate(ui->tableViewPoints)); // подтюнил для красоты
    // connect pointModel
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), pointModel, &PointModel::setPointCount);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), [this] {
        QTimer::singleShot(1, [this] { //задержка ожидания обновления модели
            auto hsbv = ui->tableViewPoints->horizontalScrollBar()->isVisible();
            ui->tableViewPoints->horizontalHeader()->setSectionResizeMode(hsbv ? QHeaderView::Fixed
                                                                               : QHeaderView::Stretch);
        });
    });
    connect(pointModel, &PointModel::message, ui->statusbar, &QStatusBar::showMessage);

    loadSettings();
    on_pbtnFind_clicked();

    for (auto childs { findChildren<QPushButton*>() }; auto pb : childs) {
        pb->setIconSize({ 16, 16 });
    }
}

MainWindow::~MainWindow()
{
    irtThread.quit();
    irtThread.wait();
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.setValue("Port", ui->cmbxPort->currentText());
    settings.setValue("Address", ui->sbxAddress->value());
    settings.setValue("Points", ui->sbxPoints->value());
    settings.setValue("SetPoint", ui->dsbxReadTemp->value());
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
    ui->cmbxPort->setCurrentText(settings.value("Port").toString());
    ui->sbxAddress->setValue(settings.value("Address", 1).toInt());
    ui->sbxPoints->setValue(settings.value("Points", 1).toInt());
    ui->dsbxSetPoint->setValue(settings.value("SetPoint", 25).toDouble());
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

void MainWindow::finded(bool found)
{
    ui->statusbar->showMessage(found ? "Термокамера найдена"
                                     : "Нет связи с термокамерой",
        1000);
    ui->grbxAuto->setEnabled(found);
    ui->grbxMan->setEnabled(found);
    ui->grbxConnection->setEnabled(!found);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    updateTableViewPointsHeight();
}

void MainWindow::on_pbtnFind_clicked()
{
    qDebug() << __FUNCTION__;
    ui->statusbar->showMessage("Поиск термокамеры...", 1000);
    finded(irt->ping(ui->cmbxPort->currentText(), 19200, ui->sbxAddress->value()));
}

void MainWindow::on_pbtnAutoStartStop_clicked(bool checked)
{
    ui->grbxMan->setEnabled(!checked);
    ui->grbxConnection->setEnabled(!checked);
    if (irt->isConnected()) {
        if (timerId)
            killTimer(timerId);

        if (checked) {
            ui->chartView->reset();
            delayType = 0;
            timerId = startTimer(500);
            ui->pbtnAutoStartStop->setText("Стоп");
        } else {
            timerId = 0;
            ui->pbtnAutoStartStop->setText("Старт");
        }
        ui->sbxPoints->setEnabled(!checked);
        ui->tableViewPoints->setEnabled(!checked);
    } else {
        checked = false;
        finded(false);
    }
    QIcon icon1(QString::fromUtf8(checked ? ":/res/media-playback-stop.svg"
                                          : ":/res/media-playback-start.svg"));
    ui->pbtnAutoStartStop->setIcon(icon1);
    ui->pbtnAutoStartStop->setChecked(checked);
}

void MainWindow::on_pbtnManReadTemp_clicked()
{
    if (irt->getMasuredValue())
        ui->statusbar->showMessage("Чтение температуры");
    else
        finded(false);
}

void MainWindow::on_pbtnManStart_clicked()
{
    if (irt->setSetPoint(ui->dsbxSetPoint->value()) && irt->setEnable(true)) {
        ui->statusbar->showMessage("Камера включена");
        ui->grbxAuto->setEnabled(false);
    } else {
        finded(false);
    }
}

void MainWindow::on_pbtnManStop_clicked()
{
    if (irt->setEnable(false)) {
        ui->statusbar->showMessage("Камера остановлена");
        ui->grbxAuto->setEnabled(true);
    } else {
        finded(false);
    }
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == timerId) {

        switch (delayType) {
        case 0: // init
            currentPoint = 0;
            point = pointModel->point(currentPoint);
            timeTo = QDateTime::currentDateTime().addMSecs(point.delayTime.msecsSinceStartOfDay()).toSecsSinceEpoch();
            if (!(irt->setSetPoint(point.temp) && irt->setEnable(true))) {
                on_pbtnAutoStartStop_clicked(false);
                finded(false);
                return;
            }
            delayType = 1;
            break;
        case 1: // delayTime
            if (QDateTime::currentDateTime().toSecsSinceEpoch() > timeTo) {
                timeTo = QDateTime::currentDateTime().addMSecs(point.measureTime.msecsSinceStartOfDay()).toSecsSinceEpoch();
                delayType = 2;
            }
            break;
        case 2: // measureTime
            if (QDateTime::currentDateTime().toSecsSinceEpoch() > timeTo) {
                if (++currentPoint == pointModel->columnCount()) {
                    on_pbtnAutoStartStop_clicked(false);
                    QMessageBox::information(this, "", "Время вышло.");
                    return;
                }
                ui->tableViewPoints->selectColumn(currentPoint);
                point = pointModel->point(currentPoint);
                if (!(irt->setSetPoint(point.temp) && irt->setEnable(true))) {
                    on_pbtnAutoStartStop_clicked(false);
                    finded(false);
                    return;
                }
                timeTo = QDateTime::currentDateTime().addMSecs(point.delayTime.msecsSinceStartOfDay()).toSecsSinceEpoch();
                delayType = 1;
            }
            break;
        }
        emit getValue();
    }
}
