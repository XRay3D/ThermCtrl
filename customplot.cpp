#include "customplot.h"
#include "axistag.h"
//static int id = qMetaTypeId<QCPRange>

CustomPlot::CustomPlot(QWidget* parent)
    : QCustomPlot(parent) {

    axisRect()->setupFullAxesBox();
    axisRect()->setRangeDrag(Qt::Horizontal);
    axisRect()->setRangeZoom(Qt::Horizontal);
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //    yAxis->setTickLabels(false);
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));
    yAxis2->setPadding(60); // add some padding to have space for tags

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    xAxis->setTicker(timeTicker);

    // create graphs:
    graph = addGraph(xAxis, yAxis);
    graph->setPen(QPen(QColor(0, 0, 0), 0));
    gradient.setColorAt(0, QColor(255, 0, 0, 50));
    gradient.setColorAt(1, QColor(0, 0, 255, 50));
    gradient.setSpread(QGradient::PadSpread);
    graph->setBrush(gradient);
    graph->setAdaptiveSampling(true);
    // create tags with newly introduced AxisTag class (see axistag.h/.cpp):
    tag = new AxisTag(graph->valueAxis());
    tag->setPen(graph->pen());
}

void CustomPlot::addPoint(double yValue) {
    if (!graph->data()->size())
        begin = QDateTime::currentDateTime();

    // calculate and add a new data point to each graph:
    graph->addData((begin.time().msecsSinceStartOfDay() + begin.msecsTo(QDateTime::currentDateTime())) / 1000., yValue);
    if (!isVisible())
        return;

    // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
    double graph1Value = graph->dataMainValue(graph->dataCount() - 1);
    tag->updatePosition(graph1Value);
    tag->setText(QString::number(graph1Value, 'f', 2));

    rescaleAxes();
    replot();
}

void CustomPlot::reset() {
    graph->data()->clear();
    if (!isVisible())
        return;
    rescaleAxes();
    replot();
}

void CustomPlot::showEvent(QShowEvent* event) {
    QCustomPlot::showEvent(event);
    rescaleAxes();
    replot();
}

void CustomPlot::resizeEvent(QResizeEvent* event) {
    QCustomPlot::resizeEvent(event);
    gradient.setStart(0, 0);
    gradient.setFinalStop(0, height());
    graph->setBrush(gradient);
}

void CustomPlot::mouseDoubleClickEvent(QMouseEvent* event) {
    QCustomPlot::mouseDoubleClickEvent(event);
    rescaleAxes();
    replot();
}
