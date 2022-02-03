#pragma once

#include <QJsonObject>
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

signals:
    void log(const QString&);
    void logColor(const QColor&);

private:
    Ui::MainWindow* ui;

    void searchForThermalChambers();
    void updateTabText(const QString& name);
    void setIcon(bool runing);
    void showMessage(const QString &text, int timeout = 0);


    std::map<QString, ThermCtrl*> map;
    bool showEventSkip{};

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};
