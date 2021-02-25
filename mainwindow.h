#pragma once

#include "pointmodel.h"

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

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
    void finded(bool found);

    PointModel* pointModel {};

    Irt5502* irt {};
    QThread irtThread;

    int timerId {};
    int currentPoint {};
    int delayType {};

    qint64 timeTo;

    Point point;

signals:
    void getValue();

private slots:
    void on_pbtnFind_clicked();

    void on_pbtnAutoStartStop_clicked(bool checked);

    void on_pbtnManReadTemp_clicked();
    void on_pbtnManStart_clicked();
    void on_pbtnManStop_clicked();

protected:
    // QObject interface
    void timerEvent(QTimerEvent* event) override;
    // QWidget interface
    void showEvent(QShowEvent* event) override;
};
