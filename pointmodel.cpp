#include "pointmodel.h"

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMutex>
#include <QMutexLocker>
#include <algorithm>

static int id = qRegisterMetaType<QVector<int>>("QVector<int>");

PointModel::PointModel(QObject* parent)
    : QAbstractTableModel(parent)
    , data_ { {} }
    , count_ { data_.size() } {
}

PointModel::~PointModel() {
    save(name_);
}

void PointModel::setCount(size_t newCount) {
    static QMutex m;
    QMutexLocker lok(&m);
    if (count_ == newCount) {
        return;
    } else if (count_ < newCount) {
        beginInsertRows({}, count_, newCount - 1);
        count_ = newCount;
        if (data_.size() < newCount)
            data_.resize(newCount);
        endInsertRows();
    } else if (count_ > newCount) {
        beginRemoveRows({}, newCount, count_ - 1);
        count_ = newCount;
        endRemoveRows();
    }
}

void PointModel::setCurrent(int newCurrent) {
    current_ = newCurrent;
    emit dataChanged(index(current_, Point::Temp), index(current_, Point::Temp), { Qt::BackgroundColorRole });
}

int PointModel::rowCount(const QModelIndex&) const { return count_; }

int PointModel::columnCount(const QModelIndex&) const { return Point::RowCount; }

QVariant PointModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        switch (index.column()) {
        case Point::Temp:
            return data_[index.row()].temp;
        case Point::Time:
            return data_[index.row()].time.toString("hч. mmмин.");
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    } else if (role == Qt::EditRole) {
        switch (index.column()) {
        case Point::Temp:
            return data_[index.row()].temp;
        case Point::Time:
            return data_[index.row()].time;
        }
    } else if (role == Qt::BackgroundColorRole && current_ == index.row())
        return QColor(127, 255, 127);
    return {};
}

bool PointModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::EditRole) {
        switch (index.column()) {
        case Point::Temp:
            data_[index.row()].temp = value.toDouble();
            return true;
        case Point::Time:
            data_[index.row()].time = value.toTime();
            return true;
        }
    }
    return {};
}

QVariant PointModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            static QString headerData[] {
                "Температура, ºC",
                "Время",
            };
            return headerData[section];
        } else {
            return QString("T%1").arg(++section);
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    return {};
}

Qt::ItemFlags PointModel::flags(const QModelIndex&) const {
    return (editable_ ? Qt::ItemIsEditable : Qt::NoItemFlags) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PointModel::save(const QString& name) {
    if (name.endsWith("термокамеры.json")) {
        QMessageBox::warning(qobject_cast<QWidget*>(parent()), "", "Невозможно перезаписать файл!");
        return;
    }

    if (!name.endsWith(".json"))
        return;
    name_ = name;
    QFile file(name_);
    if (file.open(QFile::WriteOnly)) {
        QJsonArray array;
        for (int i {}; i < count_; ++i) {
            QJsonObject obj;
            obj["temp"] = data_[i].temp;
            obj["time"] = data_[i].time.toString("hh:mm");
            array.append(obj);
        }
        file.write(QJsonDocument(array).toJson());
    } else {
        qWarning() << file.errorString();
    }
}

void PointModel::load(const QString& name) {
    if (!name.endsWith(".json"))
        return;
    if (name_ != name)
        save(name_);
    QFile file(name);
    if (file.open(QFile::ReadOnly)) {
        auto document { QJsonDocument::fromJson(file.readAll()) };
        if (document.isArray()) {
            name_ = name;
            auto array { document.array() };
            data_.resize(array.size());
            for (int i {}; i < data_.size(); ++i) {
                auto obj { array[i].toObject() };
                data_[i].temp = obj["temp"].toDouble();
                data_[i].time = QTime::fromString(obj["time"].toString(), "hh:mm");
            }
            setCount(data_.size());
        } else {
            QMessageBox::warning(qobject_cast<QWidget*>(parent()), "", "Некорректный файл!");
        }
    } else {
        qWarning() << file.errorString();
    }
}

Point PointModel::point(size_t idx) const {
    return data_[std::clamp(idx, {}, data_.size() - 1)];
}
