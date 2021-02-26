#include "irt5502.h"

#include <QDebug>
#include <QElapsedTimer>

//#define ALWAYS_OPEN //Если необходимо держать открытым тогда не использовать PortOener
//#define EMU

class PortOpener { // RAII
    Irt5502* const cpIrt;

public:
    explicit PortOpener(Irt5502* t)
        : cpIrt(t)
    {
        emit cpIrt->open(QIODevice::ReadWrite);
        cpIrt->m_connected = cpIrt->m_semaphore.tryAcquire(1, 1000); // ждём открытия порта
    }
    ~PortOpener()
    {
        if (cpIrt->m_connected) {
            emit cpIrt->close();
            cpIrt->m_semaphore.tryAcquire(1, 1000); // ждём закрытия порта
        }
    }
};

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
    QMutexLocker locker(&m_mutex);
    m_connected = true;
    m_semaphore.acquire(m_semaphore.available());
    do {
        emit close();
        if (!m_semaphore.tryAcquire(1, 1000)) // ждём закрытия порта
            break;

        if (!portName.isEmpty())
            m_port->setPortName(portName);
        if (baud != 0)
            m_port->setBaudRate(baud);
#ifdef ALWAYS_OPEN
        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000) && m_port->isOpen())
            break;
#endif
        if (getDev(addr) != IRT5502) {
#ifdef ALWAYS_OPEN
            emit close();
#endif
            break;
        }
        return m_connected;
    } while (0);
    return m_connected = false;
}

int Irt5502::getDev(int addr)
{
    devType = 0;
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        int& a = addr;
        emit write(createParcel(a, 0));
        if (wait()) {
            address = addr;
            devType = m_data[1].toInt();
        }
    }
#ifdef EMU
    return IRT5502;
#endif
    return devType;
}

bool Irt5502::setSetPoint(float val)
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    set = val;
    return true;
#endif
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, Cmd::WritePar,
            Hex(Write), SkipSemicolon {}, Hex(Par::SetPoint),
            Hex(val)));
        if (wait() && success())
            return true;
    }
    return {};
}

bool Irt5502::getMasuredValue()
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return true;
#endif
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, Cmd::ReadPar,
            Hex(Read), SkipSemicolon {}, Hex(Par::All)));
        if (wait()) {
            bool ok;
            if (auto val = fromHex<AllChVal>(1, &ok); ok)
                emit measuredValue(val.ch1val);
            return true;
        }
    }
    return {};
}

bool Irt5502::setEnable(bool run)
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    return true;
#endif
#ifndef ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, Cmd::WritePar,
            Hex(Write), SkipSemicolon {}, Hex(Par::Enable),
            Hex(run)));
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
    setBaudRate(Baud9600);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &IrtPort::Read);
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

void IrtPort::Read()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    if (int index = m_data.indexOf('\r'); ++index > 0) {
        m_irt->m_parcel = m_data.mid(0, index);
        qDebug() << __FUNCTION__ << m_irt->m_parcel;
        m_data.remove(0, index);
        m_irt->m_semaphore.release();
    }
}
