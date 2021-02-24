#pragma once

#include <QAbstractTableModel>
#include <QTime>

struct Point {
    double temp = 25;
    QTime delayTime = { 1, 0 };
    QTime measureTime = { 1, 0 };
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

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

signals:
    void message(const QString&, int = {});

private:
    QVector<Point> m_data;
    int m_setPointCount = 1;
};
