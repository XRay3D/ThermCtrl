#pragma once

#include <QMainWindow>
#include <QThread>
#include <set>

namespace Ui {
    class MainWindow;
}
class ThermCtrl;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    void searchForThermalChambers();

    std::map<QString, ThermCtrl*> map;
};
