#pragma once

#include <QChartView>
#include <QDateTime>

namespace QtCharts {
class QChart;
class QChartView;
class QDateTimeAxis;
class QLineSeries;
class QValueAxis;
}

class ChartView final : public QtCharts::QChartView {
    Q_OBJECT

public:
    explicit ChartView(QWidget* parent = nullptr);

    void addPoint(double yValue);
    void reset();

private:
    QtCharts::QChart* const pChart;
    QtCharts::QLineSeries* const pLineSeries;
    QtCharts::QDateTimeAxis* const pAxisX;
    QtCharts::QValueAxis* const pAxisY;

    double minY {}, maxY {};
    QDateTime minX {}, maxX {};
    bool m_isTouching {};

protected:
    bool viewportEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
};
