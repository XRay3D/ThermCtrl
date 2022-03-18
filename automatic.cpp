#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"

#include <QDebug>
static const auto ncm = QString("Нет связи с термокамкрой!");

Automatic::Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent)
    : QThread(parent)
    , irt { irt }
    , pointModel { pointModel } {
}

Automatic::~Automatic() { }

void Automatic::run() {

    struct Enable {
        Irt5502& irt;
        Automatic& automatic;
        Enable(Irt5502& irt, Automatic& automatic)
            : irt { irt }
            , automatic { automatic } {
            if (!irt.enable())
                automatic.message = ncm;
        }
        ~Enable() {
            if (!irt.disable())
                automatic.message = ncm;
        }
    } enable(*irt, *this);

    auto allTime = QDateTime::currentDateTime();
    for (auto& point : pointModel->data())
        allTime = allTime.addMSecs(point.time.msecsSinceStartOfDay());

    //    auto wait = [this, &val](int msec) -> bool {
    //        if (irt->getMasuredTemperature(&val)) {
    //            do {
    //                if (isInterruptionRequested())
    //                    return false;
    //                msleep(100);
    //            } while ((msec -= 100) > 0);
    //            return true;
    //        }
    //        message = ncm;
    //        return false;
    //    };

    message.clear();

    for (int current {}; auto& point : pointModel->data()) {
        irt->setTargetTemperature(point.temp);
        pointModel->setCurrent(current++);

        { // ожидание набора температуры
            float val2 {};
            if (!irt->getMasuredTemperature(&val2)) {
                message = ncm;
                return;
            }

            const auto dateTimeDelay = QDateTime::currentDateTime();
            do {
#ifdef EL_EMU
                if (!wait(20))
#else
                if (!wait(2000))
#endif
                    return;
                emit updateTabText(QString("T%3=%1ºC, ожидание %2ºC").arg(val, 0, 'f', 1).arg(point.temp).arg(current));
                // auto secs = dateTimeDelay.secsTo(QDateTime::currentDateTime());
                // if (secs > 10 * 60 && abs(val - val2) < 2) { //5 min delay
                // message = "Термокамера не набирает температуру!";
                // return;
                // }
            } while (abs(val - point.temp) > 0.2);
            allTime = allTime.addMSecs(dateTimeDelay.msecsTo(QDateTime::currentDateTime()));
        }

#ifdef EL_EMU
        const auto timeTo = QDateTime::currentDateTime().addMSecs(point.time.msecsSinceStartOfDay() / 10);
#else
        const auto timeTo = QDateTime::currentDateTime().addMSecs(point.time.msecsSinceStartOfDay());
#endif
        while (QDateTime::currentDateTime() < timeTo) {
            if (!wait(2000))
                return;
            auto secs = QDateTime::currentDateTime().secsTo(allTime);
            auto sec = secs % 60;
            secs /= 60;
            auto min = secs % 60;
            secs /= 60;
            emit updateTabText(QString("T%5=%1ºC, осталось %2:%3:%4").arg(val, 0, 'f', 1).arg(secs).arg(min, 2, 10, QChar { '0' }).arg(sec, 2, 10, QChar { '0' }).arg(current));
        }
    }
    message = "Термопрогон завершён.";
}

bool Automatic::wait(int msec) {
    if (irt->getMasuredTemperature()) {
        do {
            if (isInterruptionRequested())
                return false;
            msleep(100);
        } while ((msec -= 100) > 0);
        return true;
    }
    message = ncm;
    return false;
}
