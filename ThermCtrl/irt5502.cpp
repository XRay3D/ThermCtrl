#include "irt5502.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;
//#define ALWAYS_OPEN //Если необходимо держать открытым тогда не использовать PortOener
#define EMU

Irt5502::Irt5502(QObject* parent)
    : Elemer::AsciiDevice(parent)
{
}

int Irt5502::getDev(int addr)
{
    devType = 0;
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        int& a = addr;
        emit write(createParcel(a, 0));
        if (wait()) {
            address = addr;
            devType = m_data[1].toInt();
        }
    }
#ifdef EMU
    return IRT5502;
#endif
    return devType;
}

bool Irt5502::setSetPoint(float val)
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    set = val;
    return true;
#endif
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, Cmd::WritePar,
            Hex(Write), SkipSemicolon {}, Hex(Par::SetPoint),
            Hex(val)));
        if (wait() && success())
            return true;
    }
    return {};
}

bool Irt5502::getMasuredValue()
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return true;
#endif
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, Cmd::ReadPar,
            Hex(Read), SkipSemicolon {}, Hex(Par::All)));
        if (wait()) {
            bool ok;
            if (auto val = fromHex<AllChValIrt5502>(1, &ok); ok)
                emit measuredValue(val.ch1val);
            return true;
        }
    }
    return {};
}

bool Irt5502::setEnable(bool run)
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    return true;
#endif
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, Cmd::WritePar,
            Hex(Write), SkipSemicolon {}, Hex(Par::Enable),
            Hex(run)));
        if (wait() && success())
            return true;
    }
    return {};
}

bool Irt5502::wait(int timeout)
{
    if (m_semaphore.tryAcquire(1, timeout)) {
        if (checkParcel())
            return true;
        else
            emit message("Ошибка контрольной суммы");
    } else {
        emit message("Превышено время ожидания");
    }
    return {};
}
