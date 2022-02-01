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
        float val {};

        do {
            if (!irt->getMasuredValue(&val) || isInterruptionRequested()) {
                irt->setEnable(false);
                return;
            }
            msleep(2000);
        } while (abs(val - point.temp) > 0.2);

        timeTo = QDateTime::currentDateTime().addMSecs(point.time.msecsSinceStartOfDay());
        while (QDateTime::currentDateTime() < timeTo) {
            if (!irt->getMasuredValue(&val) || isInterruptionRequested()) {
                irt->setEnable(false);
                return;
            }
            msleep(2000);
        }
    }
    irt->setEnable(false);
}
