#include "irt5502.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

Irt5502::Irt5502(QObject* parent)
    : Elemer::AsciiDevice(parent)
{
}

bool Irt5502::setSetPoint(float val)
{
    QMutexLocker locker(&m_mutex);
#ifdef EL_EMU
    set = val;
    return true;
#endif
    PortOpener po(policy == PortPolicy::CloseAfterExchange ? this : nullptr);
    bool success = isConnected() && writeHex<Cmd::WritePar>(Write, Par::SetPoint, Semicolon {}, val);
    //    if (isConnected()) {
    //        emit write(createParcel(address, , Hex(Write), SkipSemicolon {}, Hex(Par::SetPoint), Hex(val)));
    //        if (wait() && success())
    //            return true;
    //    }
    return success;
}

bool Irt5502::getMasuredValue()
{
    QMutexLocker locker(&m_mutex);
#ifdef EL_EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return true;
#endif
    AllChValIrt5502 val;
    PortOpener po(policy == PortPolicy::CloseAfterExchange ? this : nullptr);
    bool success = isConnected() && readHex<Cmd::ReadPar, Hex { Read, Par::All }>(val);
    return success;
    //    if (isConnected()) {
    //        emit write(createParcel(address, Cmd::ReadPar,
    //            Hex(Read), SkipSemicolon {}, Hex(Par::All)));
    //        if (wait()) {
    //            bool ok;
    //            if (auto val = fromHex<AllChValIrt5502>(1, &ok); ok)
    //                emit measuredValue(val.ch1val);
    //            return true;
    //        }
    //    }
    //    return {};
}

bool Irt5502::setEnable(bool run)
{
    QMutexLocker locker(&m_mutex);
#ifdef EL_EMU
    return true;
#endif

    PortOpener po(policy == PortPolicy::CloseAfterExchange ? this : nullptr);
    bool success = isConnected() && writeHex<Cmd::WritePar>(Write, Par::Enable, Semicolon {}, run);
    return success;
    //    if (isConnected()) {
    //        emit write(createParcel(address, Cmd::WritePar,
    //            Hex(Write), SkipSemicolon {}, Hex(Par::Enable),
    //            Hex(run)));
    //        if (wait() && success())
    //            return true;
    //    }
    //    return {};
}
