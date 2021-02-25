#pragma once

#include <QAbstractTableModel>
#include <QTime>

struct Point {
    double temp {};
    QTime delayTime { 0, 0 };
    QTime measureTime { 0, 0 };
    enum {
        Temp,
        Delay,
        Measure,
        RowCount,
    };
};

class PointModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit PointModel(QObject* parent = nullptr);
    ~PointModel() override;

    void setPointCount(int count);
    void save();
    void load();
    Point point(int idx) const;
    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

signals:
    void message(const QString&, int = {});

private:
    QVector<Point> m_data;
    int m_setPointCount = 1;
};
