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

class ChartView : public QtCharts::QChartView {
    Q_OBJECT

public:
    explicit ChartView(QWidget* parent = nullptr);

    void addPoint(double yValue);
    void reset();

private:
    QtCharts::QChart* const chart;
    QtCharts::QLineSeries* const lineSeries;
    QtCharts::QDateTimeAxis* const axisX;
    QtCharts::QValueAxis* const axisY;

    double minY {}, maxY {};
    QDateTime minX {}, maxX {};
};
