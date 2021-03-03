#pragma once

#include <QMutex>
#include <QSerialPort>

namespace Elemer {

class AsciiDevice;

class Port : public QSerialPort {
    Q_OBJECT

public:
    Port(AsciiDevice* kds);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);

signals:
    void message(const QString&, int timout = {});

private:
    void Read();
    QByteArray m_data;
    QMutex m_mutex;
    AsciiDevice* m_asciiDevice;
};

}
