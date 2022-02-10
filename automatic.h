#pragma once

#include <QDateTime>
#include <QThread>

class Irt5502;
class PointModel;

class Automatic final : public QThread {
    Q_OBJECT

    Irt5502* irt;
    PointModel* pointModel;

public:
    explicit Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent = nullptr);
    ~Automatic();
    QString message;

signals:
    void updateTabText(const QString& name);

protected:
    // QThread interface
    void run() override;
};
