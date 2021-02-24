#pragma once

#include <QtSerialPort>
#include <commoninterfaces.h>
#include <elemer_ascii.h>

class IrtPort;

struct rIRT5502_AllCHval {
    uint8_t bitRele;
    uint8_t bitUst1;
    uint8_t bitUst2;
    uint8_t bitUst3;
    uint8_t bitPID;
    uint8_t ch1err;
    float ch1val;
    uint8_t ch2err;
    float ch2val;
    uint8_t ch3err;
    float ch3val;
    uint8_t bitInput;
};

class Irt5502 : public QObject, public CommonInterfaces, public ElemerASCII {
    Q_OBJECT
    friend class IrtPort;

public:
    Irt5502(QObject* parent = nullptr);
    bool ping(const QString& portName = QString(), int baud = 9600, int addr = 0);
    int getDev(int addr);

    //    bool setCurrent(double value, int addr = 0);
    //    bool setEnabledCurrent(bool enable, int addr = 0);
    //    bool getCurrent(int addr = 0);
    //    bool getVoltage(const QVector<int> addr);

    bool CreatePwOn(double val)
    {
        /*
constructor ThHCC_UN.CreatePwOn(var rcd_HCC: TRcd_HCC; fix_T: float);
  begin
    inherited Create(true);
    FreeOnTerminate := true;
    OEvt := TEvent.Create(false);

    self.PRcd_HCC := @rcd_HCC;
    self.fix_T := fix_T;
    self.HCC_UN := THCC_UN.enHCC_UN_PwrOn;

    valSingle.val := fix_T;
    valSingle.ID.pVal := @valSingle.val;
    valSingle.ID.ID := $DA66;
    valSingle.ID.TP := eSingle;
    valSingle.ID.sign := SignChng;

    valByte.val := $01;
    valByte.ID.pVal := @valByte.val;
    valByte.ID.ID := $DA65;
    valByte.ID.TP := eByte;
    valByte.ID.sign := SignChng;
  end;
*/
    }

    bool CreatePwOff()
    {
        /*
constructor ThHCC_UN.CreatePwOff(var rcd_HCC: TRcd_HCC);
  begin
    inherited Create(true);
    OEvt := TEvent.Create(false);

    FreeOnTerminate := true;
    self.PRcd_HCC := @rcd_HCC;
    self.HCC_UN := THCC_UN.enHCC_UN_PwrOff;

    valByte.val := $00;
    valByte.ID.pVal := @valByte.val;
    valByte.ID.ID := $DA65;
    valByte.ID.TP := eByte;
    valByte.ID.sign := SignChng;
  end;
*/
    }

    bool CreateGetT(double& val)
    {
        /*
constructor ThHCC_UN.CreateGetT(var rcd_HCC: TRcd_HCC);
  begin
    inherited Create(true);
    OnTerminate := nil;
    FreeOnTerminate := false;
    OEvt := TEvent.Create(false);
    self.PRcd_HCC := @rcd_HCC;
    self.HCC_UN := THCC_UN.enHCC_UN_GetT;

    valAll.ID.pVal := @valAll.val;
    valAll.ID.ID := $00FF;
    valAll.ID.TP := eIRT5502_AllCH;
    valAll.ID.sign := SignChng;

  end;
*/
    }

private:
    // uint getUintData(QByteArray data);
    // bool getSuccess(QByteArray data);
    // QByteArray m_data;
    QList<QByteArray> m_data;
    std::map<int, bool> m_load;
    std::map<int, float> m_current;
    QVector<float> m_values { 0.0, 0.0, 0.0, 0.0 };

    int dev;
    int serNum;
    int address;
    int chMum;
    int baudRate;
    int outPin;
    int out;

    enum uartCmd : uint8_t {
        GetType = 0,
        GetData = 1,

        GetProtocolType = 32,
        SetDevAddress = 33,
        SetDevSpeed = 34,

        // ReadPar = 37,
        // WritePar = 38,
        // ModifPar = 39,

        FOpen = 40,
        FSeek = 41,
        FRead = 42,
        FWrite = 43,
        FClose = 44,
        SetPar = 73, // Запись уставок и сотояния кнопок
        GetVer = 0XFE,
        ResetCpu = 0XFF

        // case 199: { buffTx = ";199;A5;"; //ресет!
        // case 2: { buffTx = QByteArray(";2;") + buffTx + ";"; //установить адресс на доступ в еепроме
        // case 32: { result = Transaction(";32;", 800); AddList("UAI", "неподдерживаемый протокол");
        // case 33: {buffTx = QByteArray(";33;") + buffTx + ";"; AddList("UAI", QByteArray("Невозможно установ. новый адрес. Код ошибки=") + ((QByteArray)str));
        // case 34: { AddList("UAI", QByteArray("Невозможно установ. новую скороть. Код ошибки=") + ((QByteArray)str));
        // case 35: { buffTx = ";35;"; AddList("UAI", QByteArray("невозможное значение:") + ((QByteArray)str));
        // case 36: { buffTx = QByteArray(";36;") + IntToAnsi(Val.n) + ";"; result = Transaction(buffTx, 800); AddList("UAI", QByteArray("Невозможно считать буфер возврата") + ((QByteArray)str));
        // case 37: { buffTx = QByteArray(";37;") + buffTx + ";"; AddList("UAI", QByteArray("код возврата") + ((QByteArray)str));
        // case 38: {
        // case 40: { buffTx = QByteArray(";40;") + buffTx + ";"; AddList("UAI", QByteArray("Невозможно открыть файл. Код ошибки=") + ((QByteArray)str));
        // case 41: { AddList("UAI", QByteArray("Невозможно установит адрес в файле. Код ошибки=") + ((QByteArray)str));
        // case 42: { AddList("UAI", QByteArray("Невозможно считать данные из файла. Код ошибки=") + ((QByteArray)str));
        // case 43: { AddList("UAI", QByteArray("Невозможно записать данные в файл. Код ошибки=") + ((QByteArray)str));
        // case 44: { AddList("UAI", QByteArray("Невозможно закрыть файл. Код ошибки=") + ((QByteArray)str));
        // case 45: { AddList("UAI", QByteArray("Неудалось определить позицию в файле. Код ошибки=") + ((QByteArray)str));
        // case 46: { AddList("UAI", QByteArray("Невозможно установить атрибут файла. Код ошибки=") + ((QByteArray)str));
        // case 47: { AddList("UAI", QByteArray("Невозможно стереть файл. Код ошибки=") + ((QByteArray)str));
        // case 8: { buffTx = QByteArray(";8;") + buffTx + ";"; //считать из еепрома buffTx байт.
        //// case 1: { AddList("UAI", QByteArray("ошибка преобразования Str->Float") + "\"" + ((QByteArray)str) + "\"");
        //case 0: "неподдерживаемый тип прибора ->") + ((QByteArray)str));

    };
    enum class DataType : uint8_t {
        Voltage,
        Current,
        Temperature
    };

signals:
    void open(int mode);
    void close();
    void SetValue(const QVector<quint16>&);
    void write(const QByteArray& data);

    void Voltage(const QVector<float>&);
    void Current(const QVector<float>&);

    void message(const QString&, int = {});

private:
    bool m_connected = false;
    bool m_result = false;
    int m_counter = 0;
    IrtPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;
};

class IrtPort : public QSerialPort, public ElemerASCII {
    Q_OBJECT

public:
    IrtPort(Irt5502* kds);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    Irt5502* m_kds;

signals:
    void message(const QString&, int = {});

private:
    void procRead();
    QByteArray m_data;
    QMutex m_mutex;
};
