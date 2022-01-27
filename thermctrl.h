#pragma once

#include <QThread>
#include <QWidget>

class Automatic;
class Irt5502;
class PointModel;
namespace Ui {
    class ThermCtrl;
}

class ThermCtrl : public QWidget {
    Q_OBJECT

public:
    explicit ThermCtrl(QWidget* parent = nullptr);
    ~ThermCtrl();

private:
    Ui::ThermCtrl* ui;

    Irt5502* pIrt {};
    PointModel* pPointModel;

    Automatic* pAutomatic {};
    QThread irtThread;

    void saveSettings();
    void loadSettings();

    void updateTableViewPointsHeight();
    void finded(bool found);
    void finished();

signals:
    void getValue();

private slots:
    void on_pbFind_clicked();

    void on_pbtnAutoStartStop_clicked(bool checked = false);

    void on_pbtnManReadTemp_clicked();
    void on_pbtnManStart_clicked();
    void on_pbtnManStop_clicked();

protected:
    // QWidget interface
    void showEvent(QShowEvent* event) override;
};
