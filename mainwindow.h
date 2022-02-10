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
class Irt5502;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void log(const QString&);
    void logColor(const QColor&);
    void progressUpdateValue(int);

private:
    Ui::MainWindow* ui;

    void searchForThermalChambers();
    void updateTabText(const QString& name);
    void setIcon(bool runing);
    void showMessage(const QString& text, int timeout = 0);
    void enablePointActions(bool enable);
    struct TC {
        ThermCtrl* tc {};
        Irt5502* irt;
        QString sn;
    };

    std::map<QString, TC> map;
    bool showEventSkip {};
    QToolBar* toolBar;
    ThermCtrl* currentTc() const;
    void splitterMoved(int pos, int index);
    // QWidget interface
protected:
    void showEvent(QShowEvent* event) override;
private slots:
    void on_tabWidget_currentChanged(int index);
};
