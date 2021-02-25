#include "pointmodel.h"

#include <QFile>
#include <QMutex>
#include <QMutexLocker>

#define TextStream
//#define DataStream

#ifdef TextStream
#include <QTextStream>
inline QTextStream& operator>>(QTextStream& stream, QTime& time)
{
    QString str;
    stream >> str;
    time = QTime::fromString(str, "hh:mm");
    return stream;
}

inline QTextStream& operator<<(QTextStream& stream, const QTime& time)
{
    stream << time.toString("hh:mm");
    return stream;
}
#elif defined(DataStream)
#include <QDataStream>
inline QDataStream& operator>>(QDataStream& stream, Point& point)
{
    stream >> point.temp >> point.delayTime >> point.measureTime;
    return stream;
}

inline QDataStream& operator<<(QDataStream& stream, const Point& point)
{
    stream << point.temp << point.delayTime << point.measureTime;
    return stream;
}
#endif

PointModel::PointModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_data { {} }
{
    load();
}

PointModel::~PointModel()
{
    save();
}

void PointModel::setPointCount(int count)
{
    static QMutex m;
    QMutexLocker lok(&m);
    if (m_data.size() == count) {
        return;
    } else if (m_setPointCount < count) {
        beginInsertColumns({}, m_setPointCount, count - 1);
        m_setPointCount = count;
        if (m_data.size() < count)
            m_data.resize(count);
        endInsertColumns();
    } else if (m_data.size() > count) {
        beginRemoveColumns({}, count, m_setPointCount - 1);
        m_setPointCount = count;
        endRemoveColumns();
    }
}

int PointModel::rowCount(const QModelIndex&) const { return Point::RowCount; }

int PointModel::columnCount(const QModelIndex&) const { return m_setPointCount; }

QVariant PointModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        switch (index.row()) {
        case Point::Temp:
            return m_data[index.column()].temp;
        case Point::Delay:
            return m_data[index.column()].delayTime; //.toString("hч. mmм.");
        case Point::Measure:
            return m_data[index.column()].measureTime; //.toString("hч. mmм.");
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    return {};
}

bool PointModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole) {
        switch (index.row()) {
        case Point::Temp:
            m_data[index.column()].temp = value.toDouble();
            return true;
        case Point::Delay:
            m_data[index.column()].delayTime = value.toTime();
            return true;
        case Point::Measure:
            m_data[index.column()].measureTime = value.toTime();
            return true;
        }
    }
    return {};
}

QVariant PointModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Vertical) {
            static QString headerData[] { "Температура, ºC", "Задержка", "Измерение" };
            return headerData[section];
        } else {
            return QString("T%1").arg(++section);
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    return {};
}

Qt::ItemFlags PointModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void PointModel::save()
{
#ifdef TextStream
    QFile file("data.txt");
    if (file.open(QFile::WriteOnly)) {
        QTextStream outStr(&file);
        outStr << m_data.size() << '\n';
        for (auto& [t, d, m] : m_data) {
            outStr << t << '\n';
            outStr << d << '\n';
            outStr << m << '\n';
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

void PointModel::load()
{
    enum { TimeOut = 3000 };
#ifdef TextStream
    QFile file("data.txt");
    if (file.open(QFile::ReadOnly)) {
        QTextStream inStr(&file);
        int size;
        inStr >> size;
        m_data.resize(size);
        for (auto& [t, d, m] : m_data) {
            inStr >> t;
            inStr >> d;
            inStr >> m;
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

Point PointModel::point(int idx) const { return m_data.value(idx); }
