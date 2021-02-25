#include "irt5502.h"

#include <QDebug>
#include <QElapsedTimer>

QElapsedTimer t;

Irt5502::Irt5502(QObject* parent)
    : QObject(parent)
    , m_port(new IrtPort(this))
{
    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    connect(this, &Irt5502::open, m_port, &IrtPort::Open);
    connect(this, &Irt5502::close, m_port, &IrtPort::Close);
    connect(this, &Irt5502::write, m_port, &IrtPort::Write);
    connect(m_port, &IrtPort::message, this, &Irt5502::message);
    m_portThread.start(QThread::InheritPriority);
}

Irt5502::~Irt5502()
{
    m_portThread.quit();
    m_portThread.wait();
}

bool Irt5502::ping(const QString& portName, int baud, int addr)
{
    qDebug(__FUNCTION__);
    QMutexLocker locker(&m_mutex);
    m_connected = true;
    m_semaphore.acquire(m_semaphore.available());
    do {
        emit close();
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        if (!portName.isEmpty())
            m_port->setPortName(portName);
        if (baud != 0)
            m_port->setBaudRate(baud);

        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000) && m_port->isOpen())
            break;

        if (getDev(addr) != IRT5502) {
            emit close();
            m_connected = false;
        }
    } while (0);
    return m_connected;
}

int Irt5502::getDev(int addr)
{
    devType = 0;
    if (m_connected) {
        emit write(createParcel(addr, 0));
        if (wait()) {
            address = addr;
            devType = m_data[1].toInt();
        }
    }
    return devType;
}

bool Irt5502::setSetPoint(float val)
{
    QMutexLocker locker(&m_mutex);
    if (m_connected) {
        constexpr uint16_t hex { 0x66DA };
        emit write(createParcel(address, Cmd::WritePar, Hex(true), SkipSemicolon {}, Hex(hex), Hex(val)));
        if (wait() && success())
            return true;
    }
    return {};
}

bool Irt5502::getMasuredValue()
{
    QMutexLocker locker(&m_mutex);
    if (m_connected) {
        constexpr uint16_t hex { 0xFF00 };
        emit write(createParcel(address, Cmd::ReadPar, Hex(false), SkipSemicolon {}, Hex(hex)));
        if (wait()) {
            bool ok {};
            auto val = fromHex<AllChVal>(1, &ok);
            if (ok)
                emit measuredValue(val.ch1val);
            return true;
        }
    }
    return {};
}

bool Irt5502::setEnable(bool run)
{
    QMutexLocker locker(&m_mutex);
    if (m_connected) {
        constexpr uint16_t hex { 0x65DA };
        emit write(createParcel(address, Cmd::WritePar, Hex(true), SkipSemicolon {}, Hex(hex), Hex(run)));
        if (wait() && success())
            return true;
    }
    return {};
}

bool Irt5502::wait(int timeout)
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

///////////////////////////////////////////
/// \brief Port::Port
/// \param t
///
IrtPort::IrtPort(Irt5502* kds)
    : m_irt(kds)
{
    setBaudRate(Baud19200);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &IrtPort::procRead);
}

void IrtPort::Open(int mode)
{
    if (!open(static_cast<OpenMode>(mode)))
        emit message(errorString());
    m_irt->m_semaphore.release();
}

void IrtPort::Close()
{
    close();
    m_irt->m_semaphore.release();
}

void IrtPort::Write(const QByteArray& data)
{
    qDebug() << __FUNCTION__ << write(data) << data;
    //write(data);
}

void IrtPort::procRead()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    if (int index = m_data.indexOf('\r'); ++index > 0) {
        m_irt->m_parcel = m_data.mid(0, index);
        m_data.remove(0, index);
        m_irt->m_semaphore.release();
    }
}
