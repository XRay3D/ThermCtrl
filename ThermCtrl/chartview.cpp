#include "chartview.h"

#include <QtCharts>

using namespace QtCharts;

ChartView::ChartView(QWidget* parent)
    : QChartView(parent)
    , chart { new QChart }
    , lineSeries { new QLineSeries(this) }
    , axisX { new QDateTimeAxis(this) }
    , axisY { new QValueAxis(this) }
{
    // setup chartView
    axisX->setFormat("h:mm:ss");

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
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(550);
    chart->setAnimationEasingCurve(QEasingCurve::Linear);
    setRenderHint(QPainter::Antialiasing);
    setChart(chart);
}

void ChartView::addPoint(double yValue)
{
    maxX = QDateTime::currentDateTime();
    if (!lineSeries->count()) {
        minY = maxY = yValue;
        minX = QDateTime::currentDateTime();
    } else {
        maxY = std::max(maxY, yValue);
        minY = std::min(minY, yValue);
    }
    lineSeries->append(maxX.toMSecsSinceEpoch(), yValue);
    axisX->setRange(minX, maxX);
    axisY->setRange(minY, maxY);
}

void ChartView::reset()
{
    minY = maxY = {};
    minX = maxX = {};
    lineSeries->clear();
    axisX->setRange(minX, maxX);
    axisY->setRange(minY, maxY);
}
