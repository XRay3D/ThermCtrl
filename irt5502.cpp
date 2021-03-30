#include "irt5502.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

#pragma pack(push, 1)

struct AllData {
    uint8_t bitRele;
    uint8_t bitUst1;
    uint8_t bitUst2;
    uint8_t bitUst3;
    uint8_t bitPID;
    uint8_t ch1err;
    float ch1val;
    uint8_t ch2err;
    float ch2val;
    uint8_t ch3err;
    float ch3val;
    uint8_t bitInput;
};

#pragma pack(pop)

Irt5502::Irt5502(QObject* parent)
    : Elemer::Device(parent)
{
}

bool Irt5502::setSetPoint(float val)
{
    QMutexLocker locker(&m_mutex);
    PortOpener po(policy == PortPolicy::AlwaysOpen ? nullptr : this);
    bool success = isConnected() && writeHex<ParamCmd::Write>(Write, Par::SetPoint, Semicolon {}, val) == 0;
    return success;
}

bool Irt5502::getMasuredValue()
{
    QMutexLocker locker(&m_mutex);
    PortOpener po(policy == PortPolicy::AlwaysOpen ? nullptr : this);
    AllData all;
    bool success = isConnected() && writeHex<ParamCmd::Read>(Read, Par::All) == 0 && readHex(all);
    if (success)
        emit measuredValue(all.ch1val);
    return success;
}

bool Irt5502::setEnable(bool run)
{
    QMutexLocker locker(&m_mutex);
    PortOpener po(policy == PortPolicy::AlwaysOpen ? nullptr : this);
    bool success = isConnected() && writeHex<ParamCmd::Write>(Write, Par::Enable, Semicolon {}, run) == 0;
    return success;
}
