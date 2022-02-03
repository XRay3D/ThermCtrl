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
    explicit ThermCtrl(Irt5502*& irt, const QString& serialNumber, QWidget* parent = nullptr);
    ~ThermCtrl();

    const QString& name() const;
    void setName(const QString& newName);

private:
    Ui::ThermCtrl* ui;

    Irt5502* const irt_;
    PointModel* const pointModel;
    Automatic* const automatic;
    QThread irtThread;
    QString name_;
    QString const serialNumber;
    QTimer timer;

    int current {};

    void saveSettings();
    void loadSettings();

    void updateTableViewPointsHeight();
    void finished();
    void checkConnection();
    void timeout();
    void updateGrBxName();

signals:
    void getValue(float* = {});
    void showMessage(const QString&, int = 0);
    void updateIcon(bool);
    void updateTabText(const QString&);

private slots:
    void on_pbAutoStartStop_clicked(bool checked = false);
    void on_pbManReadTemp_clicked();

    void on_pbSave_clicked();
    void on_pbLoad_clicked();
    void on_pbRename_clicked();

    void on_pbManStop_clicked();
    void on_pbManStart_clicked();

protected:
    // QWidget interface
    void showEvent(QShowEvent* event) override;
};
