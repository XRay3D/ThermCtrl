#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"

#include <QDebug>

Automatic::Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent)
    : QThread(parent)
    , irt { irt }
    , pointModel { pointModel }
{
    qDebug(__FUNCTION__);
}

Automatic::~Automatic()
{
    qDebug(__FUNCTION__);
}

void Automatic::run()
{
    qDebug(__FUNCTION__);
    for (auto& point : pointModel->data()) {
        irt->setSetPoint(point.temp);
        timeTo = QDateTime::currentDateTime().addMSecs(point.delayTime.msecsSinceStartOfDay() / 60).toSecsSinceEpoch();
        while (QDateTime::currentDateTime().toSecsSinceEpoch() < timeTo) {
            irt->getMasuredValue();
            msleep(500);
            if (isInterruptionRequested())
                return;
        }
        timeTo = QDateTime::currentDateTime().addMSecs(point.measureTime.msecsSinceStartOfDay() / 60).toSecsSinceEpoch();
        while (QDateTime::currentDateTime().toSecsSinceEpoch() < timeTo) {
            irt->getMasuredValue();
            msleep(500);
            if (isInterruptionRequested())
                return;
        }
    }
}
