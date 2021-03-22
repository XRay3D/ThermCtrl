#include "ed_device.h"

namespace Elemer {

AsciiDevice::AsciiDevice(QObject* parent, DTR dtr, DTS dts, PortPolicy policy)
    : QObject(parent)
    , m_port(new Port(this))
    , dtr(dtr)
    , dts(dts)
    , policy(policy)
{
    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    connect(this, &AsciiDevice::open, m_port, &Port::Open);
    connect(this, &AsciiDevice::close, m_port, &Port::Close);
    connect(this, &AsciiDevice::write, m_port, &Port::Write);
    connect(m_port, &Port::message, this, &AsciiDevice::message);
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
#ifdef EL_EMU
    return m_connected;
#endif

    m_semaphore.acquire(m_semaphore.available());
    do {
        emit close();
        if (!m_semaphore.tryAcquire(1, 1000)) // ждём закрытия порта
            break;

        if (!portName.isEmpty())
            m_port->setPortName(portName);
        if (baud != 0)
            m_port->setBaudRate(baud);

        if (policy == PortPolicy::AlwaysOpen) {
            emit open(QIODevice::ReadWrite);
            if (!m_semaphore.tryAcquire(1, 1000) && m_port->isOpen())
                break;
            m_port->setDataTerminalReady(dtr == DTR::On);
            m_port->setRequestToSend(dts == DTS::On);
            m_portThread.msleep(50);
        }

        if (getDevType(addr) != type()) {
            if (policy == PortPolicy::AlwaysOpen) {
                emit close();
            }
            break;
        }
        return m_connected;
    } while (0);
    return m_connected = false;
}

DeviceType AsciiDevice::getDevType(int addr)
{
    PortOpener po(policy == PortPolicy::CloseAfterExchange ? this : nullptr);
    if (isConnected()) {
        emit write(createParcel(addr, 0));
        if (wait(100)) {
            m_address = m_data[0].toInt();
            return static_cast<DeviceType>(m_data[1].toInt());
        }
    }
#ifdef EL_EMU
    return type();
#endif
    return {};
}

bool AsciiDevice::success()
{
    m_lastRetCode = m_data.value(1).mid(1).toInt();
    return !m_lastRetCode && m_data.value(1).startsWith('$');
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
    if (m_connected && m_semaphore.tryAcquire(1, timeout)) {
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
    union {
        struct {
            uint8_t lo8;
            uint8_t hi8;
        };
        uint16_t crc16;
    } crc = { .crc16 = 0xFFFF };
    for (auto byteIt = parcel.begin() + 1; byteIt != parcel.end(); ++byteIt) {
        uint8_t index = crc.lo8 ^ *byteIt;
        crc.lo8 = crc.hi8 ^ tableCrc16Lo[index];
        crc.hi8 = tableCrc16Hi[index];
    }
    return QByteArray::number(crc.crc16);
}

Port* AsciiDevice::port() const { return m_port; }

uint8_t AsciiDevice::address() const { return m_address; }

bool AsciiDevice::setAddress(uint8_t address)
{
    m_address = address;
    return true;
}

bool AsciiDevice::fileOpen()
{
    bool success = isConnected() && writeStr<FileCmd::Open>() == RetCcode::Ok;
    return success;
}

bool AsciiDevice::fileClose()
{
    bool success = isConnected() && writeStr<FileCmd::Close>() == RetCcode::Ok;
    return success;
}

bool AsciiDevice::fileSeek(uint16_t offset, Seek seek)
{
    bool success = isConnected() && writeStr<FileCmd::Seek>(offset, seek) == RetCcode::Ok;
    return success;
}

////////////////////////////////////////////////////////////
/// \brief PortOpener::PortOpener
/// \param ad
///
PortOpener::PortOpener(AsciiDevice* ad)
    : pAsciiDevice(ad)
{
    if (!pAsciiDevice)
        return;
    emit pAsciiDevice->open(QIODevice::ReadWrite);
    pAsciiDevice->m_connected = pAsciiDevice->m_semaphore.tryAcquire(1, 1000); // ждём открытия порта
    if (pAsciiDevice->m_connected) {
        pAsciiDevice->m_port->setDataTerminalReady(pAsciiDevice->dtr == DTR::On);
        pAsciiDevice->m_port->setRequestToSend(pAsciiDevice->dts == DTS::On);
        pAsciiDevice->m_portThread.msleep(50);
    }
}

PortOpener::~PortOpener()
{
    if (!pAsciiDevice)
        return;
    if (pAsciiDevice->m_connected) {
        emit pAsciiDevice->close();
        pAsciiDevice->m_semaphore.tryAcquire(1, 1000); // ждём закрытия порта
    }
}

}
