#pragma once

#include <QAbstractTableModel>
#include <QTime>
#include <ranges>

struct Point {
    double temp { 25.0 };
    QTime time { 0, 30 };
    enum {
        Temp,
        Time,
        RowCount,
    };
};

template <class T, class A>
class VectorView : public std::ranges::view_interface<VectorView<T, A>> {
public:
    // We need to define a default constructor to satisfy the view concept.
    VectorView() = default;
    VectorView(const std::vector<T, A>& vec, size_t count)
        : m_begin(vec.cbegin())
        , m_end(vec.cbegin() + count)
    {
    }
    auto begin() const { return m_begin; }
    auto end() const { return m_end; }

private:
    typename std::vector<T, A>::const_iterator m_begin, m_end;
};

class PointModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit PointModel(QObject* parent = nullptr);
    ~PointModel() override;

    void setPointCount(size_t count);
    void save();
    void load();
    Point point(size_t idx) const;
    auto data() { return VectorView(m_data, m_setPointCount); }

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int current() const;
    void setCurrent(int newCurrent);

signals:
    void message(const QString&, int = {});

private:
    std::vector<Point> m_data;
    size_t m_setPointCount = 1;
    int current_{};
};
