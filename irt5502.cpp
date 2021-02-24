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
        if (baud != 0)
            m_port->setBaudRate(baud);

        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000))
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
    dev = 0;

    if (m_connected) {
        emit write(createParcel(addr, 0));
        if (m_semaphore.tryAcquire(1, 1000)) {
            address = addr;
            dev = m_data[1].toInt();
        }
    }
    return dev;
}

//bool Irt5502::setCurrent(double value, int addr)
//{
//    QMutexLocker locker(&m_mutex);
//    m_current[addr] = value;
//    if (m_connected) {
//        emit write(createParcel(addr, SetPar, Hex(m_load[addr]), SkipSemicolon {}, Hex(m_current[addr])));
//        if (m_semaphore.tryAcquire(1, 1000))
//            return true;
//    }
//    return {};
//}

//bool Irt5502::setEnabledCurrent(bool enable, int addr)
//{
//    QMutexLocker locker(&m_mutex);
//    m_load[addr] = enable;
//    if (m_connected) {
//        emit write(createParcel(addr, SetPar, Hex(m_load[addr]), SkipSemicolon {}, Hex(m_current[addr])));
//        if (m_semaphore.tryAcquire(1, 1000))
//            return true;
//    }
//    return {};
//}

//bool Irt5502::getCurrent(int addr)
//{
//    QMutexLocker locker(&m_mutex);
//    QVector<float> values(4, 0.0);
//    bool fl = false;
//    int begin, end;
//    if (m_connected) {
//        if (addr)
//            begin = end = addr;
//        else {
//            begin = 1;
//            end = 5;
//        }
//        do {
//            emit write(createParcel(begin, GetData, DataType::Current));
//            if (m_semaphore.tryAcquire(1, 1000)) {
//                values[begin - 1] = m_data[1].toFloat();
//                fl = true;

//            } else
//                break;
//        } while (++begin < end);
//    }
//    if (addr == 0)
//        fl = begin == 5;
//    if (fl)
//        emit Current(values);
//    return fl;
//}

//bool Irt5502::getVoltage(const QVector<int> addr)
//{
//    QMutexLocker locker(&m_mutex);
//    if (!m_connected)
//        return false;
//    do {
//        //        if (addr) {
//        //            emit write(createParcel(addr, GetData, DataType::Voltage));
//        //            if (!m_semaphore.tryAcquire(1, 1000))
//        //                return false;
//        //            constexpr double k = 0.05;
//        //            if (float val = m_data[1].toFloat(); abs(val - m_values[addr - 1]) > 1.0)
//        //                m_values[addr - 1] = val;
//        //            else
//        //                m_values[addr - 1] = m_values[addr - 1] * (1.0 - k) + val * k;
//        //        } else {
//        t.start();
//        m_values = { 0, 0, 0, 0 };
//        for (int currentAddr : addr) {
//            emit write(createParcel(currentAddr, GetData, DataType::Voltage));
//            if (!m_semaphore.tryAcquire(1, 1000))
//                return false;
//            constexpr double k = 0.05;
//            if (float val = m_data[1].toFloat(); abs(val - m_values[currentAddr - 1]) > 1.0)
//                m_values[currentAddr - 1] = val;
//            else
//                m_values[currentAddr - 1] = m_values[currentAddr - 1] * (1.0 - k) + val * k;
//        }
//        qDebug() << __FUNCTION__ << t.elapsed() << "ms";
//        //        }
//    } while (0);
//    emit Voltage(m_values);
//    return true;
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
    else
        emit message(errorString());
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

/*
    EnErr result = OK;
    QByteArray str;
    double SnglVal = 0.0;
    int LIntVal = 0;
    uint32_t DWVal = 0;
    int i = 0;
    str.resize(255);
    BC.CRet = EZERO;
    result = OK;
    if (OpenComPort()) {
        switch (cmd) {
        case 0: {
            result = Transaction(";0;", 500);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    try {
                        if (BC.tdev != strToInt(((QByteArray)str))) {
                            result = EUNDEFTYPE;
                            AddList("UAI", QByteArray("неподдерживаемый тип прибора ->") + ((QByteArray)str));
                        } else {
                            str = buffRx.SubString(IndStart + 1, semi[0] - IndStart - 1);
                            Val.arr[0] = strToInt(((QByteArray)str));
                            Val.n = 1;
                        }
                    } catch (...) {
                        AddList("UAI", QByteArray("ошибка преобразования Str->Int") + ((QByteArray)str));
                        result = EEXCEPT;
                    }
                }
            }
        } break;
        case 1: {
            buffTx = QByteArray(";1;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    try {
                        SnglVal = strToFloat(((QByteArray)str));
                        (PSingle(&Val.arr[0])) = SnglVal;
                        Val.n = 4;
                    } catch (...) {
                        AddList("UAI", QByteArray("ошибка преобразования Str->Float") + "\"" + ((QByteArray)str) + "\"");
                        result = EEXCEPT;
                    }
                }
            }
        } break;
        case 2: {
            buffTx = QByteArray(";2;") + buffTx + ";"; //установить адресс на доступ в еепроме
            result = Transaction(buffTx, 800);
            if (result == OK) {
                str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                BC.CRet = ECodReturn(&str[1]);
                if (BC.CRet != EZERO) {
                    AddList("UAI", QByteArray(" Код ошибки=") + ((QByteArray)str));
                    result = EFAIL;
                } else
                    result = OK;
            }
        } break;
        case 8: {
            buffTx = QByteArray(";8;") + buffTx + ";"; //считать из еепрома buffTx байт.
            result = Transaction(buffTx, 800);
            if (result == OK) {
                str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                if (str[1] == '$') {
                    AddList("UAI", QByteArray(" Код ошибки=") + str[2]);
                    result = EFAIL;
                } else {
                    Val.n = (semi[1] - semi[0] - 1);
                    for (int stop = Val.n - 1, i = 0; i <= stop; i++)
                        Val.arr[i] = ((uchar)(str[i + 1]));
                    result = OK;
                }
            }
        } break;
        case 199: {
            buffTx = ";199;A5;"; //ресет!
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
//                    try
//SnglVal:=strToFloat(str);
//(PSingle(@Val.arr))^:=SnglVal;
//Val.n:=4;
//except
//AddList('UAI','ошибка преобразования Str->Float'+'"'+str+'"');
//result:=EEXCEPT;
//end;
                }
            }
        } break;
        case 32: {
            result = Transaction(";32;", 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    try {
                        if (EnNameBCUAI(BC.TCom, strToInt(((QByteArray)str))))
                            result = OK;
                        else {
                            AddList("UAI", "неподдерживаемый протокол");
                            result = EPROTOCOL;
                        }
                    } catch (...) {
                        AddList("UAI", QByteArray("ошибка преобразования Str->Int") + ((QByteArray)str));
                        result = EEXCEPT;
                    }
                }
            }
        } break;
        case 33: {
            buffTx = QByteArray(";33;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно установ. новый адрес. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 34: {
            buffTx = QByteArray(";34;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно установ. новую скороть. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 35: {
            buffTx = ";35;";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    try {
                        if (EnNameBCStat(BC.Stat, strToInt(((QByteArray)str))) != true) {
                            AddList("UAI", QByteArray("невозможное значение:") + ((QByteArray)str));
                            result = EIMPOSSIBLE;
                        }
                    } catch (...) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    }
                }
            }
        } break;
        case 36: {
            buffTx = QByteArray(";36;") + IntToAnsi(Val.n) + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        if (StrToHex_M(str, Val.arr) == false) {
                            AddList("UAI", "ошибка преобразования строки");
                            result = EEXCEPT;
                        } else
                            Val.n = ((uchar)(str.Length() / 2));
                    } else {
                        AddList("UAI", QByteArray("Невозможно считать буфер возврата") + ((QByteArray)str));
                        result = EFAIL;
                    }
                }
            }
        } break;
        case 37: {
            buffTx = QByteArray(";37;") + buffTx + ";";
            result = Transaction(buffTx, 1500);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        if (StrToHex_M(str, Val.arr) == false) {
                            AddList("UAI", "ошибка преобразования строки");
                            result = EEXCEPT;
                        } else {
                            Val.n = ((uchar)(str.Length() / 2));
                        }
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("код возврата") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 38: {
            buffTx = QByteArray(";38;") + buffTx + ";";
            result = Transaction(buffTx, 5000);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("невозможное значение:") + ((QByteArray)str));
                        result = EIMPOSSIBLE;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("код возврата") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 40: {
            buffTx = QByteArray(";40;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно открыть файл. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 41: {
            buffTx = QByteArray(";41;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно установит адрес в файле. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 42: {
            buffTx = QByteArray(";42;") + buffTx + ";";
            result = Transaction(buffTx, 3000);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        if (StrToHex_M(str, Val.arr) == false) {
                            AddList("UAI", "ошибка преобразования строки:");
                            result = EEXCEPT;
                        } else
                            Val.n = ((uchar)(str.Length() / 2));
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно считать данные из файла. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 43: {
            buffTx = QByteArray(";43;") + buffTx + ";";
            result = Transaction(buffTx, 6800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно записать данные в файл. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 44: {
            buffTx = QByteArray(";44;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно закрыть файл. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 45: {
            buffTx = QByteArray(";45;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        if (TryStrToInt(((QByteArray)str), LIntVal) == false) {
                            AddList("UAI", QByteArray("ошибка преобразования строки:") + "\"" + ((QByteArray)str) + "\"");
                            result = EEXCEPT;
                        } else {
                            PLongint(&Val.arr[0]) = LIntVal;
                            Val.n = 4;
                            BC.CRet = EZERO;
                        }
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Неудалось определить позицию в файле. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 46: {
            buffTx = QByteArray(";46;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно установить атрибут файла. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        case 47: {
            buffTx = QByteArray(";47;") + buffTx + ";";
            result = Transaction(buffTx, 800);
            if (result == OK) {
                if (nSemi == 1) {
                    str = buffRx.SubString(semi[0] + 1, semi[1] - semi[0] - 1);
                    BC.CRet = ECodReturn(&str[1]);
                    if (BC.CRet == EFALSE) {
                        AddList("UAI", QByteArray("ошибка преобразования строки:") + ((QByteArray)str));
                        result = EEXCEPT;
                    } else {
                        if (BC.CRet != EZERO) {
                            AddList("UAI", QByteArray("Невозможно стереть файл. Код ошибки=") + ((QByteArray)str));
                            result = EFAIL;
                        }
                    }
                }
            }
        } break;
        }
    } else
        result = EBUSY;
    return result;

*/
