#pragma once

#include <QDateTime>
#include <QThread>

class Irt5502;
class PointModel;

class Automatic final : public QThread {
    Q_OBJECT

    Irt5502* irt;
    PointModel* pointModel;

    QDateTime timeTo;

public:
    explicit Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent = nullptr);
    ~Automatic();
signals:

protected:
    // QThread interface
    void run() override;
};
