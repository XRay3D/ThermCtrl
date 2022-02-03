#include "automatic.h"
#include "irt5502.h"
#include "pointmodel.h"

#include <QDebug>

Automatic::Automatic(Irt5502* irt, PointModel* pointModel, QObject* parent)
    : QThread(parent)
    , irt { irt }
    , pointModel { pointModel } {
}

Automatic::~Automatic() { }

const auto ncm = QString("Нет связи с термокамкрой!");

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

    float val {};
    auto wait = [this, &val](int msec) -> bool {
        if (irt->getMasuredTemperature(&val)) {
            do {
                if (isInterruptionRequested())
                    return false;
                msleep(100);
            } while ((msec -= 100) > 0);
            return true;
        }
        message = ncm;
        return false;
    };

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
                if (!wait(2000))
                    return;
                updateTabText(QString("T%3=%1ºC, ожидание %2ºC").arg(val, 0, 'f', 1).arg(point.temp).arg(current));
                auto secs = dateTimeDelay.secsTo(QDateTime::currentDateTime());
                if (secs > 5 * 60 && abs(val - val2) < 2) { //5 min delay
                    message = "Термокамера не набирает температуру!";
                    return;
                }
            } while (abs(val - point.temp) > 0.2);
            allTime = allTime.addMSecs(dateTimeDelay.msecsTo(QDateTime::currentDateTime()));
        }

        const auto timeTo = QDateTime::currentDateTime().addMSecs(point.time.msecsSinceStartOfDay());
        while (QDateTime::currentDateTime() < timeTo) {
            if (!wait(2000))
                return;
            auto secs = QDateTime::currentDateTime().secsTo(allTime);
            auto sec = secs % 60;
            secs /= 60;
            auto min = secs % 60;
            secs /= 60;
            updateTabText(QString("T%5=%1ºC, осталось %2:%3:%4").arg(val, 0, 'f', 1).arg(secs).arg(min, 2, 10, QChar { '0' }).arg(sec, 2, 10, QChar { '0' }).arg(current));
        }
    }
    message = "Термокамера звершила термопрогон.";
}
