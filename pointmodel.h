#pragma once

#include <QAbstractTableModel>
#include <QTime>
#include <ranges>
#include <span>

struct Point {
    double temp { 25.0 };
    QTime time { 0, 30 };
    enum {
        Temp,
        Time,
        RowCount,
    };
};

class PointModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit PointModel(QObject* parent = nullptr);
    ~PointModel() override;

    void setCount(size_t newCount);
    int count() const { return count_; }

    void setCurrent(int newCurrent);
    int current() const { return current_; }

    void save(const QString& name);
    void load(const QString& name);
    QString name() const { return name_; }

    void setEditable(bool editable) { editable_ = editable; }

    Point point(size_t idx) const;
    auto data() { return std::span(data_.data(), count_); }

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
    QString name_;
    std::vector<Point> data_;
    size_t count_ {};
    int current_ {};
    bool editable_ { true };
};
