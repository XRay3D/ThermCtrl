#pragma once

#include "qcustomplot.h"

class AxisTag;

class CustomPlot : public QCustomPlot {
    Q_OBJECT

public:
    CustomPlot(QWidget* parent = nullptr);
    void addPoint(double yValue);
    void reset();

private:
    QPointer<QCPGraph> graph;
    //QPointer<QCPGraph> graph2;
    AxisTag* tag;
    QDateTime begin;
    QLinearGradient gradient;

    // QWidget interface
protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};
