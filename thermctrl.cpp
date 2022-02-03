#include "thermctrl.h"
#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"
#include "ui_thermctrl.h"
#include <QFileDialog>
#include <QInputDialog>
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

ThermCtrl::ThermCtrl(Irt5502*& irt, const QString& serialNumber, QWidget* parent)
    : QWidget(parent)
    , ui { new Ui::ThermCtrl }
    , irt_ { irt }
    , pointModel { new PointModel(this) }
    , automatic { new Automatic(irt_, pointModel) }
    , serialNumber { serialNumber } {
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
    connect(this, &ThermCtrl::getValue, irt_, &Irt5502::getMasuredTemperature);

    // setup twPoints
    ui->twPoints->setModel(pointModel);
    ui->twPoints->horizontalHeader()->setDefaultSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setMinimumSectionSize(ColumnWidth);
    ui->twPoints->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twPoints->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twPoints->setItemDelegate(new MyItemDelegate(ui->twPoints)); // подтюнил для красоты

    connect(&timer, &QTimer::timeout, this, &ThermCtrl::timeout);

    connect(pointModel, &PointModel::message, this, &ThermCtrl::showMessage);

    connect(automatic, &Automatic::updateTabText, [this](const QString& text) {
        emit updateTabText(QString("%1: %2").arg(name_).arg(text));
    });
    connect(automatic, &QThread::finished, this, &ThermCtrl::finished);

    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), pointModel, &PointModel::setCount);
    connect(ui->sbxPoints, qOverload<int>(&QSpinBox::valueChanged), [this] {
        QTimer::singleShot(1, [this] { //задержка ожидания обновления модели
            auto hsbv = ui->twPoints->horizontalScrollBar()->isVisible();
            ui->twPoints->horizontalHeader()->setSectionResizeMode(hsbv ? QHeaderView::Fixed
                                                                        : QHeaderView::Stretch);
        });
    });

    if (QFile file("термокамеры.json"); file.open(QFile::ReadOnly)) {
        auto document { QJsonDocument::fromJson(file.readAll()) };
        QJsonObject objectTCName(document.object());
        if (objectTCName.contains(serialNumber))
            name_ = objectTCName[serialNumber].toString();
        setObjectName(name_);
        file.close();
    }

    loadSettings();
}

ThermCtrl::~ThermCtrl() {
    irt_->disable();
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
    updateGrBxName();
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
    if (automatic->message.size()) {
        QMessageBox::information(this, "", automatic->message);
        qCritical() << name_ << automatic->message;
    }
}

void ThermCtrl::checkConnection() {
    while (!irt_->isConnected()
        && QMessageBox::warning(this,
               name_,
               "Нет связи с термокамкрой!",
               QMessageBox::Retry,
               QMessageBox::Cancel)
            == QMessageBox::Retry) {
        qWarning() << name_<< "Нет связи с термокамкрой!";
        irt_->ping({}, {}, 1);
    }
}

void ThermCtrl::timeout() {
}

void ThermCtrl::updateGrBxName() {
    ui->grbxAuto->setTitle("Автоматическое управление: " + QFileInfo(pointModel->name()).fileName());
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
            ui->chartView->reset(); // чистка графика
            automatic->start();     // запуск потока
            ui->pbAutoStartStop->setText("Стоп");
            emit updateIcon(true);
        } else {
            // disconnect pAutomatic
            if (automatic->isRunning()) {
                automatic->requestInterruption(); // остановка потока
                automatic->wait();
            }
            ui->pbAutoStartStop->setText("Старт");
            emit updateIcon(false);
            emit updateTabText(name_);
        }
        //        ui->grbxConnection->setEnabled(!checked);
        pointModel->setEditable(!checked);
        ui->grbxMan->setEnabled(!checked);
        ui->pbLoad->setEnabled(!checked);
        ui->pbSave->setEnabled(!checked);
        ui->sbxPoints->setEnabled(!checked);
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
    if (irt_->getMasuredTemperature())
        emit showMessage("Чтение температуры");
}

void ThermCtrl::on_pbSave_clicked() {
    pointModel->save(QFileDialog::getSaveFileName(this, "Сохранить точки", pointModel->name(), "JSON (*.json)"));
    updateGrBxName();
}

void ThermCtrl::on_pbLoad_clicked() {
    pointModel->load(QFileDialog::getOpenFileName(this, "Загрузить точки", pointModel->name(), "JSON (*.json)"));
    updateGrBxName();
}

void ThermCtrl::on_pbRename_clicked() {
    QString newName = QInputDialog::getText(this, irt_->port()->portName(), "Ведите название термокамеры", QLineEdit::Normal, name_);
    if (newName.size() && newName != name_) {
        name_ = newName;
        QFile file("термокамеры.json");
        QJsonObject objectTCName;

        if (file.open(QFile::ReadOnly)) {
            objectTCName = QJsonDocument::fromJson(file.readAll()).object();
            objectTCName[serialNumber] = name_;
            setObjectName(name_);
            file.close();
        }

        if (file.open(QFile::WriteOnly)) {
            file.write(QJsonDocument(objectTCName).toJson());
            file.close();
        }

        emit updateTabText(name_);
    }
}

const QString& ThermCtrl::name() const {
    return name_;
}

void ThermCtrl::setName(const QString& newName) {
    if (name_ == newName)
        return;
    name_ = newName;
    emit updateTabText(name_);
}

void ThermCtrl::on_pbManStop_clicked() {
    checkConnection();
    if (irt_->setTargetTemperature(ui->dsbxSetPoint->value()) && irt_->enable()) {
        emit showMessage("Камера выключена");
        emit updateIcon(false);
    }
    emit updateTabText(name_);
}

void ThermCtrl::on_pbManStart_clicked() {
    checkConnection();
    if (irt_->disable()) {
        emit showMessage("Камера включена");
        emit updateIcon(true);
        emit updateTabText(QString("%1: T=%2").arg(name_).arg(ui->dsbxSetPoint->text()));
    }
}
