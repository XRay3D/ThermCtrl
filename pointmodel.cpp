#include "pointmodel.h"

#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <algorithm>

#define TextStream
//#define DataStream

#ifdef TextStream
#include <QColor>
#include <QTextStream>
inline QTextStream& operator>>(QTextStream& stream, QTime& time) {
    QString str;
    stream >> str;
    time = QTime::fromString(str, "hh:mm");
    return stream;
}

inline QTextStream& operator<<(QTextStream& stream, const QTime& time) {
    stream << time.toString("hh:mm");
    return stream;
}
#elif defined(DataStream)
#include <QDataStream>
inline QDataStream& operator>>(QDataStream& stream, Point& point) {
    stream >> point.temp >> point.time >> point.measureTime;
    return stream;
}

inline QDataStream& operator<<(QDataStream& stream, const Point& point) {
    stream << point.temp << point.time << point.measureTime;
    return stream;
}
#endif

PointModel::PointModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_data { {} } {
    load();
}

PointModel::~PointModel() {
    save();
}

void PointModel::setPointCount(size_t count) {
    static QMutex m;
    QMutexLocker lok(&m);
    if (m_data.size() == count) {
        return;
    } else if (m_setPointCount < count) {
        beginInsertRows({}, m_setPointCount, count - 1);
        m_setPointCount = count;
        if (m_data.size() < count)
            m_data.resize(count);
        endInsertRows();
    } else if (m_data.size() > count) {
        beginRemoveRows({}, count, m_setPointCount - 1);
        m_setPointCount = count;
        endRemoveRows();
    }
}

int PointModel::rowCount(const QModelIndex&) const { return m_setPointCount; }

int PointModel::columnCount(const QModelIndex&) const { return Point::RowCount; }

QVariant PointModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        switch (index.column()) {
        case Point::Temp:
            return m_data[index.row()].temp;
        case Point::Time:
            return m_data[index.row()].time.toString("hч. mmм.");
         }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    } else if (role == Qt::EditRole) {
        switch (index.column()) {
        case Point::Temp:
            return m_data[index.row()].temp;
        case Point::Time:
            return m_data[index.row()].time;

        }
    } else if (role == Qt::BackgroundColorRole && current_ == index.row())
        return QColor(127, 255, 127);
    return {};
}

bool PointModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::EditRole) {
        switch (index.column()) {
        case Point::Temp:
            m_data[index.row()].temp = value.toDouble();
            return true;
        case Point::Time:
            m_data[index.row()].time = value.toTime();
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

int PointModel::current() const { return current_; }

void PointModel::setCurrent(int newCurrent) {
    current_ = newCurrent;
}

void PointModel::save() {
#ifdef TextStream
    QFile file("data.txt");
    if (file.open(QFile::WriteOnly)) {
        QTextStream outStr(&file);
        outStr << m_data.size() << '\n';
        for (auto& [time, temp] : m_data) {
            outStr << time << '\n';
            outStr << temp << '\n';
        }
    }
#elif defined(DataStream)
    QFile file("data.bin");
    if (file.open(QFile::WriteOnly)) {
        QDataStream outStr(&file);
        outStr << m_data;
    }
#endif
    else {
        emit message(file.errorString());
    }
}

void PointModel::load() {
    enum { TimeOut = 3000 };
#ifdef TextStream
    QFile file("data.txt");
    if (file.open(QFile::ReadOnly)) {
        QTextStream inStr(&file);
        int size;
        inStr >> size;
        m_data.resize(size);
        for (auto& [time, temp] : m_data) {
            inStr >> time;
            inStr >> temp;
        }
        emit message("Файл точек \"data.txt\"загружен.", TimeOut);
    }
#elif defined(DataStream)
    QFile file("data.bin");
    if (file.open(QFile::ReadOnly)) {
        QDataStream outStr(&file);
        outStr >> m_data;
        emit message("Файл точек \"data.bin\"загружен.", TimeOut);
    }
#endif
    else {
        emit message(file.errorString(), TimeOut);
    }
}

Point PointModel::point(size_t idx) const {
    return m_data[std::clamp(idx, {}, m_data.size() - 1)];
}
