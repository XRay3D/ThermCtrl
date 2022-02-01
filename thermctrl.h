#pragma once

#include "irt5502.h"

#include <QThread>
#include <QWidget>

class Automatic;
class PointModel;
namespace Ui {
    class ThermCtrl;
}

class ThermCtrl : public QWidget {
    Q_OBJECT

public:
    explicit ThermCtrl(Irt5502*& irt, QWidget* parent = nullptr);
    ~ThermCtrl();

private:
    Ui::ThermCtrl* ui;

    Irt5502* const irt_;
    PointModel* const pointModel;
    Automatic* const automatic;
    QThread irtThread;

    void saveSettings();
    void loadSettings();

    void updateTableViewPointsHeight();
    void finished();
    void checkConnection();

signals:
    void getValue(float* = {});
    void showMessage(const QString&, int = 0);

private slots:
    void on_pbAutoStartStop_clicked(bool checked = false);
    void on_pbManReadTemp_clicked();
    void on_pbManStart_clicked();
    void on_pbManStop_clicked();

    void on_pbSave_clicked();
    void on_pbLoad_clicked();

protected:
    // QWidget interface
    void showEvent(QShowEvent* event) override;
};
