#pragma once

#include <QtSerialPort>
#include <commoninterfaces.h>
#include <ed_device.h>

class Irt5502 final : public Elemer::AsciiDevice {
    Q_OBJECT

public:
    Irt5502(QObject* parent = nullptr);

    Elemer::DeviceType type() const override { return Elemer::IRT5502; };

    bool setSetPoint(float val);
    bool getMasuredValue();
    bool setEnable(bool run);

signals:
    void measuredValue(double);

private:
    bool wait(int timeout = 1000);

    int devType {};
    int address {};
    double val { 25.0 };
    double set { 1 };

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

    //    bool m_result = false;
    //    int m_counter = 0;
    //    IrtPort* m_port;
    //    QMutex m_mutex;
    //    QSemaphore m_semaphore;
    //    QThread m_portThread;
};
