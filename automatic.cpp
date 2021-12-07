#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"

#include <QDebug>

Automatic::Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent)
    : QThread(parent)
    , irt { irt }
    , pointModel { pointModel }
{
}

Automatic::~Automatic()
{
}

void Automatic::run()
{
    irt->setEnable(true);
    for (int current {}; auto& point : pointModel->data()) {
        irt->setSetPoint(point.temp);
        pointModel->setCurrent(current++);
        timeTo = QDateTime::currentDateTime().addMSecs(point.delayTime.msecsSinceStartOfDay()).toSecsSinceEpoch();
        while (QDateTime::currentDateTime().toSecsSinceEpoch() < timeTo) {
            irt->getMasuredValue();
            msleep(500);
            if (isInterruptionRequested()) {
                irt->setEnable(false);
                return;
            }
        }
        timeTo = QDateTime::currentDateTime().addMSecs(point.measureTime.msecsSinceStartOfDay()).toSecsSinceEpoch();
        while (QDateTime::currentDateTime().toSecsSinceEpoch() < timeTo) {
            irt->getMasuredValue();
            msleep(500);
            if (isInterruptionRequested()) {
                irt->setEnable(false);
                return;
            }
        }
    }
    irt->setEnable(false);
}
