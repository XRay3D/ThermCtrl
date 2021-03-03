#pragma once

#include <QObject>
#include <stdint.h>

namespace Elemer {

#pragma pack(push, 1)

struct RawAdcData {
    uint8_t dummy1;
    uint8_t stat;
    float v1;
    float v2;
    float v3;
    uint8_t dummy2;
};

struct AllChValIrt5502 {
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

enum class Cmd : uint8_t {
    GetType = 0,
    GetData = 1,

    GetProtocolType = 32,
    SetDevAddress = 33,
    SetDevSpeed = 34,

    ReadPar = 37,
    WritePar = 38,
    ModifPar = 39,

    FOpen = 40,
    FSeek = 41,
    FRead = 42,
    FWrite = 43,
    FClose = 44,

    GetVer = 0XFE,

    ResetCpu = 0XFF
};

enum class Par : uint16_t {
    SetPoint = 0x66DA,
    All = 0xFF00,
    Enable = 0x65DA,
};

enum : uint8_t {
    Read,
    Write,
};

}

int inline id1 = qRegisterMetaType<Elemer::RawAdcData>("Elemer::RawAdcData");

int inline id2 = qRegisterMetaType<Elemer::RawAdcData*>("Elemer::RawAdcData*");

//Q_DECLARE_METATYPE(Elemer::MeasuredValues)
