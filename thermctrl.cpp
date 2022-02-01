#include "thermctrl.h"
#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"
#include "ui_thermctrl.h"

#include <QFileDialog>
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
            timeEdit->setDisplayFormat("hч. mmмин.");
            timeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
            timeEdit->setAlignment(Qt::AlignCenter);
            return timeEdit;
        }
    }
};

ThermCtrl::ThermCtrl(Irt5502*& irt, QWidget* parent)
    : QWidget(parent)
    , ui { new Ui::ThermCtrl }
    , irt_ { irt }
    , pointModel { new PointModel(this) }
    , automatic { new Automatic(irt_, pointModel) } {
    irt = nullptr;
    Elemer::Timer t(__FUNCTION__);
    ui->setupUi(this);

    for (auto childs { findChildren<QPushButton*>() }; auto pb : childs)
        pb->setIconSize({ 16, 16 });

    irt_->moveToThread(&irtThread);
    connect(&irtThread, &QThread::finished, irt_, &QObject::deleteLater);
    irtThread.start();

    connect(irt_, &Irt5502::message, this, &ThermCtrl::showMessage);
    connect(irt_, &Irt5502::measuredValue, ui->dsbxReadTemp, &QDoubleSpinBox::setValue);
    connect(irt_, &Irt5502::measuredValue, ui->chartView, &ChartView::addPoint);
    connect(this, &ThermCtrl::getValue, irt_, &Irt5502::getMasuredValue);

    // setup twPoints
    ui->twPoints->setModel(pointModel);
    ui->twPoints->horizontalHeader()->setDefaultSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setMinimumSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twPoints->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twPoints->setItemDelegate(new MyItemDelegate(ui->twPoints)); // подтюнил для красоты

    connect(pointModel, &PointModel::message, this, &ThermCtrl::showMessage);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), pointModel, &PointModel::setCount);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), [this] {
        QTimer::singleShot(1, [this] { //задержка ожидания обновления модели
            auto hsbv = ui->twPoints->horizontalScrollBar()->isVisible();
            ui->twPoints->horizontalHeader()->setSectionResizeMode(hsbv ? QHeaderView::Fixed
                                                                        : QHeaderView::Stretch);
        });
    });

    loadSettings();
}

ThermCtrl::~ThermCtrl() {
    saveSettings();
    irtThread.quit();
    irtThread.wait();
    delete ui;
}

void ThermCtrl::saveSettings() {
    QSettings settings;

    settings.beginGroup("ThermCtrl");
    settings.setValue("Points", ui->sbxPoints->value());
    settings.setValue("SetPoint", ui->dsbxReadTemp->value());
    settings.setValue("splitterSizes", ui->splitter->saveState());
    settings.endGroup();

    settings.beginGroup(irt_->port()->portName());
    settings.setValue("pointData", pointModel->name());
    settings.endGroup();
}

void ThermCtrl::loadSettings() {
    QSettings settings;

    settings.beginGroup("ThermCtrl");
    ui->sbxPoints->setValue(settings.value("Points", 1).toInt());
    ui->dsbxSetPoint->setValue(settings.value("SetPoint", 25).toDouble());
    ui->splitter->restoreState(settings.value("splitterSizes").toByteArray());
    settings.endGroup();

    settings.beginGroup(irt_->port()->portName());
    pointModel->load(settings.value("pointData", irt_->port()->portName() + ".json").toString());
    ui->sbxPoints->setValue(pointModel->count());
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

void ThermCtrl::finished() {
    on_pbAutoStartStop_clicked();
    QMessageBox::information(this, "", "Время вышло.");
}

void ThermCtrl::checkConnection() {
    while (!irt_->isConnected()
        && QMessageBox::warning(this,
               irt_->port()->portName(),
               "",
               QMessageBox::Retry,
               QMessageBox::Cancel)
            == QMessageBox::Retry) {
        irt_->ping({}, {}, 1);
    }
}

void ThermCtrl::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateTableViewPointsHeight();
}

void ThermCtrl::on_pbAutoStartStop_clicked(bool checked) {
    checkConnection();
    if (irt_->isConnected()) {
        if (checked) {
            int time {};
            for (auto& point : pointModel->data()) {
                time += point.time.msecsSinceStartOfDay();
            }
            if (!time) {
                QMessageBox::information(this, "", "Нулевое время во всех ячейках таблицы.");
                ui->pbAutoStartStop->setChecked(false);
                return;
            }
            // connect pAutomatic
            connect(automatic, &QThread::finished, this, &ThermCtrl::finished);
            ui->chartView->reset(); // чистка графика
            automatic->start();     // запуск потока
            ui->pbAutoStartStop->setText("Стоп");
        } else {
            // disconnect pAutomatic
            automatic->disconnect();
            if (automatic->isRunning()) {
                automatic->requestInterruption(); // остановка потока
                automatic->wait();
            }
            ui->pbAutoStartStop->setText("Старт");
        }
        //        ui->grbxConnection->setEnabled(!checked);
        ui->grbxMan->setEnabled(!checked);
        ui->sbxPoints->setEnabled(!checked);
        ui->twPoints->setEnabled(!checked);
        ui->pbLoad->setEnabled(!checked);
        ui->pbSave->setEnabled(!checked);
    } else {
        checked = false;
    }
    QIcon icon1(QString::fromUtf8(checked ? ":/res/media-playback-stop.svg"
                                          : ":/res/media-playback-start.svg"));
    ui->pbAutoStartStop->setIcon(icon1);
    ui->pbAutoStartStop->setChecked(checked);
}

void ThermCtrl::on_pbManReadTemp_clicked() {
    checkConnection();
    if (irt_->getMasuredValue())
        emit showMessage("Чтение температуры");
}

void ThermCtrl::on_pbManStart_clicked() {
    checkConnection();
    if (irt_->setSetPoint(ui->dsbxSetPoint->value()) && irt_->setEnable(true))
        emit showMessage("Камера включена");
}

void ThermCtrl::on_pbManStop_clicked() {
    checkConnection();
    if (irt_->setEnable(false))
        emit showMessage("Камера остановлена");
}

void ThermCtrl::on_pbSave_clicked() {
    pointModel->save(QFileDialog::getSaveFileName(this, "Сохранить точки", pointModel->name(), "JSON (*.json)"));
}

void ThermCtrl::on_pbLoad_clicked() {
    pointModel->load(QFileDialog::getOpenFileName(this, "Загрузить точки", pointModel->name(), "JSON (*.json)"));
}
