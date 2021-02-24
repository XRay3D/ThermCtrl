#include "chartview.h"

#include <QtCharts>

using namespace QtCharts;

ChartView::ChartView(QWidget* parent)
    : QChartView(parent)
    , chart { new QChart }
    , lineSeries { new QLineSeries(this) }
    , axisX { new QValueAxis(this) }
    , axisY { new QValueAxis(this) }
{
    // setup chartView
    axisX->setLabelFormat("%g");
    axisY->setTitleText("Температура, ºC");
    axisY->setTitleFont(font());

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addSeries(lineSeries);

    lineSeries->attachAxis(axisX);
    lineSeries->attachAxis(axisY);

    chart->legend()->hide();
    chart->setMargins({});
    chart->setTitle("Температура в камере");
    axisX->setRange(0, 100);
    axisY->setRange(0, 100);
    lineSeries->append(0, 0);
    lineSeries->append(100, 100);

    setRenderHint(QPainter::Antialiasing);
    setChart(chart);
}
