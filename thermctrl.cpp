#include "thermctrl.h"
#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"
#include "ui_thermctrl.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QTimeEdit>

using PortInfo = QSerialPortInfo;

enum { ColumnWidth = 50 };

struct MyItemDelegate : QStyledItemDelegate {
    MyItemDelegate(QWidget* parent = nullptr)
        : QStyledItemDelegate(parent) {
    }
    // QAbstractItemDelegate interface
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
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

ThermCtrl::ThermCtrl(const QString& portName, QWidget* parent)
    : QWidget(parent)
    , ui { new Ui::ThermCtrl }
    , pIrt { new Irt5502() }
    , pPointModel { new PointModel(this) }
    , pAutomatic { new Automatic(pIrt, pPointModel) } {
    ui->setupUi(this);

    for (auto childs { findChildren<QPushButton*>() }; auto pb : childs)
        pb->setIconSize({ 16, 16 });

    pIrt->moveToThread(&irtThread);
    connect(&irtThread, &QThread::finished, pIrt, &QObject::deleteLater);
    irtThread.start();

    // connect pIrt
    //    connect(pIrt, &Irt5502::message, ui->statusbar, &QStatusBar::showMessage);
    connect(pIrt, &Irt5502::measuredValue, ui->dsbxReadTemp, &QDoubleSpinBox::setValue);
    connect(pIrt, &Irt5502::measuredValue, ui->chartView, &ChartView::addPoint);
    connect(this, &ThermCtrl::getValue, pIrt, &Irt5502::getMasuredValue);

    // setup cbxPort
    auto ports { PortInfo::availablePorts().toVector() };
    std::ranges::sort(ports, {}, [](const PortInfo& pil) { return pil.portName().midRef(3).toInt(); });
    for (auto& port : ports) {
        ui->cbxPort->addItem(port.portName(), port.serialNumber());
    }

    ui->cbxBaud->addItems({ "1200",
        "2400",
        "4800",
        "9600",
        "19200",
        "38400",
        "57600",
        "115200" });

    // setup cbxDevice
    ui->cbxDevice->addItem("ИРТ5502");

    // setup twPoints
    ui->twPoints->setModel(pPointModel);
    ui->twPoints->horizontalHeader()->setDefaultSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setMinimumSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twPoints->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twPoints->setItemDelegate(new MyItemDelegate(ui->twPoints)); // подтюнил для красоты

    // connect pPointModel
    // connect(pPointModel, &PointModel::message, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), pPointModel, &PointModel::setPointCount);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), [this] {
        QTimer::singleShot(1, [this] { //задержка ожидания обновления модели
            auto hsbv = ui->twPoints->horizontalScrollBar()->isVisible();
            ui->twPoints->horizontalHeader()->setSectionResizeMode(hsbv ? QHeaderView::Fixed
                                                                        : QHeaderView::Stretch);
        });
    });

    loadSettings();

    ui->cbxPort->setCurrentText(portName);

    on_pbFind_clicked();
}

ThermCtrl::~ThermCtrl() {
    irtThread.quit();
    irtThread.wait();
    saveSettings();
    delete ui;
}

void ThermCtrl::saveSettings() {
    QSettings settings;

    settings.beginGroup("ThermCtrl");
    settings.setValue("Port", ui->cbxPort->currentText());
    settings.setValue("Baud", ui->cbxBaud->currentText());
    settings.setValue("PortSn", ui->cbxPort->currentData());
    settings.setValue("Address", ui->sbxAddress->value());
    settings.setValue("Points", ui->sbxPoints->value());
    settings.setValue("SetPoint", ui->dsbxReadTemp->value());
    settings.endGroup();

    settings.beginGroup("Splitter");
    settings.setValue("State", ui->splitter->saveState());
    settings.endGroup();
}

void ThermCtrl::loadSettings() {
    QSettings settings;

    settings.beginGroup("ThermCtrl");
    if (auto sn { settings.value("PortSn").toString() }; sn.isEmpty())
        ui->cbxPort->setCurrentText(settings.value("Port").toString());
    else {
        for (int i {}; i < ui->cbxPort->count(); ++i)
            if (ui->cbxPort->itemData(i).toString() == sn) {
                ui->cbxPort->setCurrentIndex(i);
                break;
            }
    }
    ui->cbxBaud->setCurrentText(settings.value("Baud", "9600").toString());
    ui->sbxAddress->setValue(settings.value("Address", 1).toInt());
    ui->sbxPoints->setValue(settings.value("Points", 1).toInt());
    ui->dsbxSetPoint->setValue(settings.value("SetPoint", 25).toDouble());
    settings.endGroup();

    settings.beginGroup("Splitter");
    ui->splitter->restoreState(settings.value("State").toByteArray());
    settings.endGroup();
}

void ThermCtrl::updateTableViewPointsHeight() {
    return;
    auto sb = ui->twPoints->horizontalScrollBar();
    int height = ui->twPoints->horizontalHeader()->height()
        + ui->twPoints->rowHeight(0) * 3
        + (sb->isVisible() ? sb->height() : sb->height())
        + 2;
    ui->twPoints->setMinimumHeight(height);
    ui->twPoints->setMaximumHeight(height);
}

void ThermCtrl::finded(bool found) {
    emit showMessage(found ? "Термокамера найдена"
                           : "Нет связи с термокамерой",
        1000);
    ui->pbtnAutoStartStop->setEnabled(found);
    ui->grbxMan->setEnabled(found);
    ui->grbxConnection->setEnabled(!found);
}

void ThermCtrl::finished() {
    on_pbtnAutoStartStop_clicked();
    QMessageBox::information(this, "", "Время вышло.");
}

void ThermCtrl::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateTableViewPointsHeight();
}

void ThermCtrl::on_pbFind_clicked() {
    qDebug() << __FUNCTION__;
    emit showMessage("Поиск термокамеры...", 1000);
    finded(pIrt->ping(ui->cbxPort->currentText(), ui->cbxBaud->currentText().toUInt(), ui->sbxAddress->value()));
}

void ThermCtrl::on_pbtnAutoStartStop_clicked(bool checked) {
    if (checked) { // проверка
        int time {};
        for (auto& point : pPointModel->data()) {
            time += point.time.msecsSinceStartOfDay();
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
            connect(pAutomatic, &QThread::finished, this, &ThermCtrl::finished);
            ui->chartView->reset(); // чистка графика
            pAutomatic->start();    // запуск потока
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

void ThermCtrl::on_pbtnManReadTemp_clicked() {
    if (pIrt->getMasuredValue()) {
        emit showMessage("Чтение температуры");
    } else
        finded(false);
}

void ThermCtrl::on_pbtnManStart_clicked() {
    if (pIrt->setSetPoint(ui->dsbxSetPoint->value()) && pIrt->setEnable(true)) {
        emit showMessage("Камера включена");
    } else {
        finded(false);
    }
}

void ThermCtrl::on_pbtnManStop_clicked() {
    if (pIrt->setEnable(false)) {
        emit showMessage("Камера остановлена");
    } else {
        finded(false);
    }
}
