#include "port.h"
#include "ascii_device.h"

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

void Port::Open(int mode)
{
    if (!open(static_cast<OpenMode>(mode)))
        emit message(errorString());
    m_asciiDevice->m_semaphore.release();
}

void Port::Close()
{
    close();
    m_asciiDevice->m_semaphore.release();
}

void Port::Write(const QByteArray& data)
{
    //qDebug() << __FUNCTION__ << write(data) << data;
    write(data);
}

void Port::Read()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    if (int index = m_data.indexOf('\r'); ++index > 0) {
        m_asciiDevice->m_parcel = m_data.mid(0, index);
        //qDebug() << __FUNCTION__ << m_irt->m_parcel;
        m_data.remove(0, index);
        m_asciiDevice->m_semaphore.release();
    }
}

}
