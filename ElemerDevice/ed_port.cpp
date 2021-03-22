#include "ed_port.h"
#include "ed_device.h"

#include <qcoreevent.h>

namespace Elemer {

Port::Port(AsciiDevice* asciiDevice)
    : m_asciiDevice(asciiDevice)
{
    setBaudRate(Baud9600);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &Port::Read);
}

Port::~Port()
{
#ifdef FORCE_READ
    killTimer(forceReadTimerId);
#endif
}

void Port::Open(int mode)
{
    if (!open(static_cast<OpenMode>(mode)))
        emit message(errorString());
    m_asciiDevice->m_semaphore.release();
#ifdef FORCE_READ
    if (isOpen())
        forceReadTimerId = startTimer(10ms);
#endif
}

void Port::Close()
{
    close();
    m_asciiDevice->m_semaphore.release();
#ifdef FORCE_READ
    if (forceReadTimerId)
        killTimer(forceReadTimerId), forceReadTimerId = 0;
#endif
}

void Port::Write(const QByteArray& data)
{
    if (!isOpen())
        return;
#ifdef EL_LOG
    time.start();
    qDebug() << "Write" << data << write(data) << time;
#else
    write(data);
#endif
}

void Port::Read()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    if (int index = m_data.indexOf('\r'); ++index > 0) {
        m_asciiDevice->m_parcel = m_data.mid(0, index);
#ifdef EL_LOG
        qDebug() << "Read" << m_asciiDevice->m_parcel << m_asciiDevice->m_parcel.size() << time.elapsed() << "ms";
#endif
        m_data.remove(0, index);
        m_asciiDevice->m_semaphore.release();
    }
}
}

void Elemer::Port::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == forceReadTimerId)
        Read();
}
