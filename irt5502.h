#pragma once

#include <QtSerialPort>
#include <commoninterfaces.h>
#include <elemer_ascii.h>

class IrtPort;

class Irt5502 : public QObject, public CommonInterfaces, public ElemerASCII {
    Q_OBJECT
    friend class IrtPort;

public:
    Irt5502(QObject* parent = nullptr);
    bool ping(const QString& portName = QString(), int baud = 9600, int addr = 0);
    int getDev(int addr);

    bool setCurrent(double value, int addr = 0);
    bool setEnabledCurrent(bool enable, int addr = 0);
    bool getCurrent(int addr = 0);
    bool getVoltage(const QVector<int> addr);

private:
    // uint getUintData(QByteArray data);
    // bool getSuccess(QByteArray data);
    // QByteArray m_data;
    QList<QByteArray> m_data;
    std::map<int, bool> m_load;
    std::map<int, float> m_current;
    QVector<float> m_values { 0.0, 0.0, 0.0, 0.0 };

    int dev;
    int serNum;
    int address;
    int chMum;
    int baudRate;
    int outPin;
    int out;

    enum uartCmd : uint8_t {
        GetType = 0,
        GetData = 1,

        GetProtocolType = 32,
        SetDevAddress = 33,
        SetDevSpeed = 34,

        // ReadPar = 37,
        // WritePar = 38,
        // ModifPar = 39,

        FOpen = 40,
        FSeek = 41,
        FRead = 42,
        FWrite = 43,
        FClose = 44,
        SetPar = 73, // Запись уставок и сотояния кнопок
        GetVer = 0XFE,
        ResetCpu = 0XFF

    };
    enum class DataType : uint8_t {
        Voltage,
        Current,
        Temperature
    };

signals:
    void open(int mode);
    void close();
    void SetValue(const QVector<quint16>&);
    void write(const QByteArray& data);

    void Voltage(const QVector<float>&);
    void Current(const QVector<float>&);

private:
    bool m_connected = false;
    bool m_result = false;
    int m_counter = 0;
    IrtPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;
};

class IrtPort : public QSerialPort, public ElemerASCII {
    Q_OBJECT

public:
    IrtPort(Irt5502* kds);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    Irt5502* m_kds;

private:
    void procRead();
    QByteArray m_data;
    QMutex m_mutex;
};
