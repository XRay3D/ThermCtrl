#pragma once

#include <QChartView>

namespace QtCharts {
class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;
}

class ChartView : public QtCharts::QChartView {
    Q_OBJECT

public:
    explicit ChartView(QWidget* parent = nullptr);

signals:

private:
    QtCharts::QChart* const chart;
    QtCharts::QLineSeries* const lineSeries;
    QtCharts::QValueAxis* const axisX;
    QtCharts::QValueAxis* const axisY;
};
