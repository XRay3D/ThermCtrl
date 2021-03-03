#include "ascii_device.h"

#define EMU

namespace Elemer {

AsciiDevice::AsciiDevice(QObject* parent)
    : QObject(parent)
    , port(new Port(this))
{
    port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, port, &QObject::deleteLater);
    connect(this, &AsciiDevice::open, port, &Port::Open);
    connect(this, &AsciiDevice::close, port, &Port::Close);
    connect(this, &AsciiDevice::write, port, &Port::Write);
    connect(port, &Port::message, this, &AsciiDevice::message);
    m_portThread.start(QThread::InheritPriority);
}

AsciiDevice::~AsciiDevice()
{
    m_portThread.quit();
    m_portThread.wait();
}

bool AsciiDevice::ping(const QString& portName, int baud, int addr)
{
    QMutexLocker locker(&m_mutex);
    m_connected = true;
#ifdef EMU
    return m_connected;
#endif

    m_semaphore.acquire(m_semaphore.available());
    do {
        emit close();
        if (!m_semaphore.tryAcquire(1, 1000)) // ждём закрытия порта
            break;

        if (!portName.isEmpty())
            port->setPortName(portName);
        if (baud != 0)
            port->setBaudRate(baud);
#ifdef ALWAYS_OPEN
        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000) && port->isOpen())
            break;
        port->setRequestToSend(true);
        port->setDataTerminalReady(false);
        m_portThread.msleep(100);
#endif
        if (getDev(addr) != type()) {
#ifdef ALWAYS_OPEN
            emit close();
#endif
            break;
        }
        return m_connected;
    } while (0);
    return m_connected = false;
}

DeviceType AsciiDevice::getDev(int addr)
{
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(addr, 0));
        if (wait()) {
            address = addr;
            return static_cast<DeviceType>(m_data[1].toInt());
        }
    }
#ifdef EMU
    return type();
#endif
    return {};
}

bool AsciiDevice::success()
{
    m_lastRetCode = m_data.value(1);
    return m_lastRetCode == success_;
}

bool AsciiDevice::checkParcel()
{
    if (int index = m_parcel.indexOf('!'); index > 0)
        m_parcel.remove(0, index);

    if (int index = m_parcel.lastIndexOf('\r'); index > 0)
        m_parcel.resize(index);

    if (int index = m_parcel.lastIndexOf(';') + 1;
        index > 0 && calcCrc(m_parcel.left(index)).toUInt() == m_parcel.right(m_parcel.length() - index).toUInt()) {
        m_data = m_parcel.split(';');
        m_data.front().remove(0, 1);
        return true;
    }
    m_data.clear();
    return false;
}

bool AsciiDevice::wait(int timeout)
{
    if (m_semaphore.tryAcquire(1, timeout)) {
        if (checkParcel())
            return true;
        else
            emit message("Ошибка контрольной суммы");
    } else {
        emit message("Превышено время ожидания");
    }
    return {};
}

QByteArray AsciiDevice::calcCrc(const QByteArray& parcel)
{
    // (X^16 + X^15 + X^2 + 1).
    uint16_t crcLo = 0xFF, crcHi = 0xFF;
    for (auto byteIt = parcel.begin() + 1; byteIt != parcel.end(); ++byteIt) {
        uint8_t index = crcLo ^ *byteIt;
        crcLo = crcHi ^ tableCrc16Lo[index];
        crcHi = tableCrc16Hi[index];
    }
    return QByteArray::number(crcHi << 8 | crcLo);
}

}
