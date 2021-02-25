#pragma once

#include <QtSerialPort>
#include <commoninterfaces.h>
#include <elemer_ascii.h>

class IrtPort;

class Irt5502 : public QObject, public CommonInterfaces, public ElemerASCII {
    Q_OBJECT
    friend class IrtPort;
    friend class PortOener;

public:
    Irt5502(QObject* parent = nullptr);
    ~Irt5502() override;
    bool ping(const QString& portName = QString(), int baud = 9600, int addr = 0) override;
    int getDev(int addr);

    bool setSetPoint(float val);
    bool getMasuredValue();
    bool setEnable(bool run);

#pragma pack(push, 1)
    struct AllChVal {
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

signals:
    void open(int mode) override;
    void close() override;
    void write(const QByteArray& data);
    void message(const QString&, int = {});
    void measuredValue(double);

private:
    bool wait(int timeout = 1000);

    int devType {};
    int address {};

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

    bool m_result = false;
    int m_counter = 0;
    IrtPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;
};

class IrtPort : public QSerialPort {
    Q_OBJECT

public:
    IrtPort(Irt5502* kds);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    Irt5502* m_irt;

signals:
    void message(const QString&, int = {});

private:
    void Read();
    QByteArray m_data;
    QMutex m_mutex;
};
