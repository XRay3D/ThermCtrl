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
    m_portThread.start(QThread::InheritPriority);
}

bool Irt5502::ping(const QString& portName, int baud, int addr)
{
    Q_UNUSED(baud)
    Q_UNUSED(addr)
    QMutexLocker locker(&m_mutex);
    m_connected = true;
    m_semaphore.acquire(m_semaphore.available());
    do {
        emit close();
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        if (!portName.isEmpty())
            m_port->setPortName(portName);

        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        if (!(getDev(1) == MAN && getDev(2) == MAN && getDev(3) == MAN && getDev(4) == MAN)) {
            emit close();
            m_connected = false;
        }
    } while (0);
    return m_connected;
}

int Irt5502::getDev(int addr)
{
    dev = 0;
    if (m_connected) {
        emit write(createParcel(addr, 0));
        if (m_semaphore.tryAcquire(1, 1000))
            dev = m_data[1].toInt();
    }
    return dev;
}

bool Irt5502::setCurrent(double value, int addr)
{
    QMutexLocker locker(&m_mutex);
    m_current[addr] = value;
    if (m_connected) {
        emit write(createParcel(addr, SetPar, Hex(m_load[addr]), SkipSemicolon {}, Hex(m_current[addr])));
        if (m_semaphore.tryAcquire(1, 1000))
            return true;
    }
    return {};
}

bool Irt5502::setEnabledCurrent(bool enable, int addr)
{
    QMutexLocker locker(&m_mutex);
    m_load[addr] = enable;
    if (m_connected) {
        emit write(createParcel(addr, SetPar, Hex(m_load[addr]), SkipSemicolon {}, Hex(m_current[addr])));
        if (m_semaphore.tryAcquire(1, 1000))
            return true;
    }
    return {};
}

bool Irt5502::getCurrent(int addr)
{
    QMutexLocker locker(&m_mutex);
    QVector<float> values(4, 0.0);
    bool fl = false;
    int begin, end;
    if (m_connected) {
        if (addr)
            begin = end = addr;
        else {
            begin = 1;
            end = 5;
        }
        do {
            emit write(createParcel(begin, GetData, DataType::Current));
            if (m_semaphore.tryAcquire(1, 1000)) {
                values[begin - 1] = m_data[1].toFloat();
                fl = true;

            } else
                break;
        } while (++begin < end);
    }
    if (addr == 0)
        fl = begin == 5;
    if (fl)
        emit Current(values);
    return fl;
}

bool Irt5502::getVoltage(const QVector<int> addr)
{
    QMutexLocker locker(&m_mutex);
    if (!m_connected)
        return false;
    do {
        //        if (addr) {
        //            emit write(createParcel(addr, GetData, DataType::Voltage));
        //            if (!m_semaphore.tryAcquire(1, 1000))
        //                return false;
        //            constexpr double k = 0.05;
        //            if (float val = m_data[1].toFloat(); abs(val - m_values[addr - 1]) > 1.0)
        //                m_values[addr - 1] = val;
        //            else
        //                m_values[addr - 1] = m_values[addr - 1] * (1.0 - k) + val * k;
        //        } else {
        t.start();
        m_values = { 0, 0, 0, 0 };
        for (int currentAddr : addr) {
            emit write(createParcel(currentAddr, GetData, DataType::Voltage));
            if (!m_semaphore.tryAcquire(1, 1000))
                return false;
            constexpr double k = 0.05;
            if (float val = m_data[1].toFloat(); abs(val - m_values[currentAddr - 1]) > 1.0)
                m_values[currentAddr - 1] = val;
            else
                m_values[currentAddr - 1] = m_values[currentAddr - 1] * (1.0 - k) + val * k;
        }
        qDebug() << __FUNCTION__ << t.elapsed() << "ms";
        //        }
    } while (0);
    emit Voltage(m_values);
    return true;
}

//bool KDS::setOut(int addr, int value)
//{
//    QMutexLocker locker(&m_mutex);
//    if (m_connected) {
//        QByteArray data = QString(":%1;4;%2;").arg(addr).arg(value).toLocal8Bit();
//        data.append(CalcCrc(data)).append('\r');
//        Write(data);
//        if (m_semaphore.tryAcquire(1, 1000)) {
//            return getSuccess(m_data);
//        }
//    }
//    return false;
//}

//uint MAN::getUintData(QByteArray data)
//{
//    if (checkParcel(data)) {
//        return getValues()[1].toInt();
//    }
//    return 0;
//}

//bool MAN::getSuccess(QByteArray data)
//{
//    if (data.isEmpty())
//        return false;
//    int i = 0;
//    while (data[0] != '!' && data.size())
//        data = data.remove(0, 1);
//    while (data[data.size() - 1] != '\r' && data.size())
//        data = data.remove(data.size() - 1, 1);
//    data = data.remove(data.size() - 1, 1);
//    QList<QByteArray> list = data.split(';');
//    data.clear();
//    while (i < list.count() - 1)
//        data.append(list[i++]).append(';');
//    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2)
//        if (list.at(1) == "$0")
//            return true;
//    return false;
//}

///////////////////////////////////////////
/// \brief Port::Port
/// \param t
///
IrtPort::IrtPort(Irt5502* kds)
    : m_kds(kds)
{
    setBaudRate(Baud19200);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &IrtPort::procRead);
}

void IrtPort::Open(int mode)
{
    if (open(static_cast<OpenMode>(mode)))
        m_kds->m_semaphore.release();
}

void IrtPort::Close()
{
    close();
    m_kds->m_semaphore.release();
}

void IrtPort::Write(const QByteArray& data)
{
    //qDebug() << __FUNCTION__ << write(data) << data;
    write(data);
}

void IrtPort::procRead()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    int index = m_data.indexOf('\r');
    if (m_data.indexOf('\r') > 0 && checkParcel(m_data, m_kds->m_data)) {
        //qDebug() << __FUNCTION__ << m_data;
        m_data.remove(0, index + 1);
        m_kds->m_semaphore.release();
    }
}
