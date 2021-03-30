#include "chartview.h"

#include <QtCharts>

using namespace QtCharts;

ChartView::ChartView(QWidget* parent)
    : QChartView(parent)
    , pChart { new QChart }
    , pLineSeries { new QLineSeries /*QSplineSeries*/ (this) }
    , pAxisX { new QDateTimeAxis(this) }
    , pAxisY { new QValueAxis(this) }
{
    // setup chartView
    pAxisX->setFormat("h:mm:ss");

    pAxisY->setTitleText("Температура, ºC");
    pAxisY->setTitleFont(font());

    pChart->addAxis(pAxisX, Qt::AlignBottom);
    pChart->addAxis(pAxisY, Qt::AlignLeft);
    pChart->addSeries(pLineSeries);

    pLineSeries->attachAxis(pAxisX);
    pLineSeries->attachAxis(pAxisY);
    //pLineSeries->setUseOpenGL(true); // отключант анимацию

    pChart->legend()->hide();
    pChart->setMargins({});
    pChart->setTitle("Температура в камере");
    pChart->setAnimationOptions(QChart::SeriesAnimations);
    pChart->setAnimationDuration(550);
    pChart->setAnimationEasingCurve(QEasingCurve::Linear);

    setChart(pChart);

    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
}

void ChartView::addPoint(double yValue)
{
    maxX = QDateTime::currentDateTime();
    if (!pLineSeries->count()) {
        minY = maxY = yValue;
        minX = QDateTime::currentDateTime();
    } else {
        maxY = std::max(maxY, yValue);
        minY = std::min(minY, yValue);
    }
    pLineSeries->append(maxX.toMSecsSinceEpoch(), yValue);
    pAxisX->setRange(minX, maxX);
    pAxisY->setRange(minY, maxY);
}

void ChartView::reset()
{
    minY = maxY = {};
    minX = maxX = {};
    pLineSeries->clear();
    pAxisX->setRange(minX, maxX);
    pAxisY->setRange(minY, maxY);
}

bool ChartView::viewportEvent(QEvent* event)
{
    if (event->type() == QEvent::TouchBegin) {
        // By default touch events are converted to mouse events. So
        // after this event we will get a mouse event also but we want
        // to handle touch events as gestures only. So we need this safeguard
        // to block mouse events that are actually generated from touch.
        m_isTouching = true;

        // Turn off animations when handling gestures they
        // will only slow us down.
        chart()->setAnimationOptions(QChart::NoAnimation);
    }
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent* event)
{
    if (m_isTouching)
        return;
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isTouching)
        return;
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_isTouching)
        m_isTouching = false;

    // Because we disabled animations when touch event was detected
    // we must put them back on.
    chart()->setAnimationOptions(QChart::SeriesAnimations);

    QChartView::mouseReleaseEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void ChartView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
    pAxisX->setRange(minX, maxX);
    pAxisY->setRange(minY, maxY);
}
