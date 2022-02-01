#pragma once

#include <QMainWindow>
#include <QTextEdit>
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
    static inline QTextEdit* teLog{};

private:
    Ui::MainWindow* ui;

    void searchForThermalChambers();

    std::map<QString, ThermCtrl*> map;
};
