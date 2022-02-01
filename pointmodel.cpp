#include "pointmodel.h"

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <algorithm>

PointModel::PointModel(QObject* parent)
    : QAbstractTableModel(parent)
    , data_ { {} } {
}

PointModel::~PointModel() {
    save(name_);
}

void PointModel::setCount(size_t count) {
    static QMutex m;
    QMutexLocker lok(&m);
    if (data_.size() == count) {
        return;
    } else if (count_ < count) {
        beginInsertRows({}, count_, count - 1);
        count_ = count;
        if (data_.size() < count)
            data_.resize(count);
        endInsertRows();
    } else if (data_.size() > count) {
        beginRemoveRows({}, count, count_ - 1);
        count_ = count;
        endRemoveRows();
    }
}

int PointModel::rowCount(const QModelIndex&) const { return count_; }

int PointModel::columnCount(const QModelIndex&) const { return Point::RowCount; }

QVariant PointModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        switch (index.column()) {
        case Point::Temp:
            return data_[index.row()].temp;
        case Point::Time:
            return data_[index.row()].time.toString("hч. mmм.");
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
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PointModel::save(const QString& name) {
    if (!name.endsWith(".json"))
        return;
    name_ = name;
    QFile file(name_);
    if (file.open(QFile::WriteOnly)) {
        QJsonArray array;
        for (int i {}; i < data_.size(); ++i) {
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
    name_ = name;
    QFile file(name_);
    if (file.open(QFile::ReadOnly)) {
        auto document { QJsonDocument ::fromJson(file.readAll()) };
        auto array { document.array() };
        data_.resize(array.size());
        for (int i {}; i < data_.size(); ++i) {
            auto obj { array[i].toObject() };
            data_[i].temp = obj["temp"].toDouble();
            data_[i].time = QTime::fromString(obj["time"].toString(), "hh:mm");
        }
        setCount(data_.size());
    } else {
        qWarning() << file.errorString();
    }
}

Point PointModel::point(size_t idx) const {
    return data_[std::clamp(idx, {}, data_.size() - 1)];
}
