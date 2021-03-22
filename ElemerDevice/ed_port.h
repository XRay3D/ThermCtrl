#pragma once

#include <QMutex>
#include <QSerialPort>
#ifdef EL_LOG
#include <QTime>
#endif

namespace Elemer {

class AsciiDevice;

class Port : public QSerialPort {
    Q_OBJECT
    friend class AsciiDevice;

signals:
    void message(const QString&, int timout = {});

private:
    Port(AsciiDevice* kds);
    ~Port();
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);

    void Read();

    QByteArray m_data;
    QMutex m_mutex;
    AsciiDevice* m_asciiDevice;
    int forceReadTimerId {};
#ifdef EL_LOG
    QTime time;
#endif

    // QObject interface
protected:
    void timerEvent(QTimerEvent* event) override;
};

}
