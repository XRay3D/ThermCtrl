#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class PointModel;
class Irt5502;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    void saveSettings();
    void loadSettings();

    void updateTableViewPointsHeight();

    // QtCharts
    QVector<QPointF> chartsData;

    PointModel* pointModel;
    Irt5502* irt;

protected:
    // QWidget interface
    void showEvent(QShowEvent* event) override;

private slots:
    void on_pushButtonFind_clicked();

    void on_pushButtonAutoStartStop_clicked(bool checked);
    void on_pushButtonManStartStop_clicked(bool checked);

    void on_pushButtonReadTemp_clicked();
    void on_doubleSpinBoxSetPoint_valueChanged(double arg1);
};
