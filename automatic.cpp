#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"

#include <QDebug>

Automatic::Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent)
    : QThread(parent)
    , irt { irt }
    , pointModel { pointModel } {
}

Automatic::~Automatic() {
}

void Automatic::run() {
    irt->setEnable(true);
    for (int current {}; auto& point : pointModel->data()) {
        irt->setSetPoint(point.temp);
        pointModel->setCurrent(current++);

        while (abs(irt->getMasuredValue() - point.temp) > 1.0) {
            if (isInterruptionRequested()) {
                irt->setEnable(false);
                return;
            }
            msleep(2000);
        }

        timeTo = QDateTime::currentDateTime().addMSecs(point.time.msecsSinceStartOfDay());
        while (QDateTime::currentDateTime() < timeTo) {
            irt->getMasuredValue();
            msleep(2000);
            if (isInterruptionRequested()) {
                irt->setEnable(false);
                return;
            }
        }
    }
    irt->setEnable(false);
}
