#pragma once

#include "pointmodel.h"

#include <QMainWindow>
#include <QThread>

class Automatic;
class Irt5502;

namespace Ui {
class MainWindow;
}

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
    void finished();

    Irt5502* pIrt {};
    PointModel* pPointModel {};

    Automatic* pAutomatic {};

    QThread irtThread;

signals:
    void getValue();

private slots:
    void on_pbtnFind_clicked();

    void on_pbtnAutoStartStop_clicked(bool checked = false);

    void on_pbtnManReadTemp_clicked();
    void on_pbtnManStart_clicked();
    void on_pbtnManStop_clicked();

protected:
    // QWidget interface
    void showEvent(QShowEvent* event) override;
};
