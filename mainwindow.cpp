#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QTimeEdit>

using PortInfo = QSerialPortInfo;

struct MyItemDelegate : QStyledItemDelegate {
    MyItemDelegate(QWidget* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }
    // QAbstractItemDelegate interface
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        if (!index.column()) {
            auto dsbx = new DoubleSpinBox(parent);
            dsbx->setRange(-999, +999);
            dsbx->setButtonSymbols(QAbstractSpinBox::NoButtons);
            dsbx->setDecimals(2);
            dsbx->setSingleStep(1.0);
            dsbx->setAlignment(Qt::AlignCenter);
            return dsbx;
        } else {
            auto timeEdit = static_cast<QTimeEdit*>(QStyledItemDelegate::createEditor(parent, option, index));
            timeEdit->setDisplayFormat("hч. mmм.");
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
    , pIrt { new Irt5502() }
    , pPointModel { new PointModel(this) }
    , pAutomatic { new Automatic(pIrt, pPointModel) }
{
    ui->setupUi(this);
    ui->statusbar->setFont(font());

    pIrt->moveToThread(&irtThread);
    connect(&irtThread, &QThread::finished, pIrt, &QObject::deleteLater);
    irtThread.start();

    // connect pIrt
    connect(pIrt, &Irt5502::message, ui->statusbar, &QStatusBar::showMessage);
    connect(pIrt, &Irt5502::measuredValue, ui->dsbxReadTemp, &QDoubleSpinBox::setValue);
    connect(pIrt, &Irt5502::measuredValue, ui->chartView, &ChartView::addPoint);
    connect(this, &MainWindow::getValue, pIrt, &Irt5502::getMasuredValue);

    // setup cmbxPort
    auto ports { PortInfo::availablePorts().toVector() };
    std::ranges::sort(ports, {}, [](const PortInfo& pil) { return pil.portName().midRef(3).toInt(); });
    for (auto& port : ports)
        ui->cmbxPort->addItem(port.portName());

    // setup cmbxDevice
    ui->cmbxDevice->addItem("ИРТ5502");

    // setup twPoints
    ui->twPoints->setModel(pPointModel);
    ui->twPoints->horizontalHeader()->setDefaultSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setMinimumSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twPoints->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twPoints->setItemDelegate(new MyItemDelegate(ui->twPoints)); // подтюнил для красоты

    // connect pPointModel
    connect(pPointModel, &PointModel::message, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), pPointModel, &PointModel::setPointCount);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), [this] {
        QTimer::singleShot(1, [this] { //задержка ожидания обновления модели
            auto hsbv = ui->twPoints->horizontalScrollBar()->isVisible();
            ui->twPoints->horizontalHeader()->setSectionResizeMode(hsbv ? QHeaderView::Fixed
                                                                        : QHeaderView::Stretch);
        });
    });

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
    return;
    auto sb = ui->twPoints->horizontalScrollBar();
    int height = ui->twPoints->horizontalHeader()->height()
        + ui->twPoints->rowHeight(0) * 3
        + (sb->isVisible() ? sb->height() : sb->height())
        + 2;
    ui->twPoints->setMinimumHeight(height);
    ui->twPoints->setMaximumHeight(height);
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

void MainWindow::finished()
{
    on_pbtnAutoStartStop_clicked();
    QMessageBox::information(this, "", "Время вышло.");
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
    finded(pIrt->ping(ui->cmbxPort->currentText(), 19200, ui->sbxAddress->value()));
}

void MainWindow::on_pbtnAutoStartStop_clicked(bool checked)
{
    if (checked) { // проверка
        int time {};
        for (auto& point : pPointModel->data()) {
            time += point.delayTime.msecsSinceStartOfDay();
            time += point.measureTime.msecsSinceStartOfDay();
        }
        if (!time) {
            QMessageBox::information(this, "", "Нулевое время во всех ячейках таблицы.");
            ui->pbtnAutoStartStop->setChecked(false);
            return;
        }
    }

    if (pIrt->isConnected()) {
        if (checked) {
            // connect pAutomatic
            connect(pAutomatic, &QThread::finished, this, &MainWindow::finished);
            ui->chartView->reset(); // чистка графика
            pAutomatic->start(); // запуск потока
            ui->pbtnAutoStartStop->setText("Стоп");
        } else {
            // disconnect pAutomatic
            pAutomatic->disconnect();
            if (pAutomatic->isRunning()) {
                pAutomatic->requestInterruption(); // остановка потока
                pAutomatic->wait();
            }
            ui->pbtnAutoStartStop->setText("Старт");
        }
        ui->grbxConnection->setEnabled(!checked);
        ui->grbxMan->setEnabled(!checked);
        ui->sbxPoints->setEnabled(!checked);
        ui->twPoints->setEnabled(!checked);
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
    if (pIrt->getMasuredValue())
        ui->statusbar->showMessage("Чтение температуры");
    else
        finded(false);
}

void MainWindow::on_pbtnManStart_clicked()
{
    if (pIrt->setSetPoint(ui->dsbxSetPoint->value()) && pIrt->setEnable(true)) {
        ui->statusbar->showMessage("Камера включена");
    } else {
        finded(false);
    }
}

void MainWindow::on_pbtnManStop_clicked()
{
    if (pIrt->setEnable(false)) {
        ui->statusbar->showMessage("Камера остановлена");
    } else {
        finded(false);
    }
}
