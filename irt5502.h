#pragma once

#include <QtSerialPort>
#include <commoninterfaces.h>
#include <ed_device.h>

#include <QString>
#include <chrono>
#include <map>
#include <string_view>

//using nanoseconds = std::nano;
//using microseconds = std::micro;
//using milliseconds = std::milli;
//using seconds = std::ratio<1>;
//using minutes = std::ratio<60>;
//using hours = std::ratio<3600>;

//using std::is_same_v;
//template <class T = seconds>
//requires                          //
//    is_same_v<T, nanoseconds> ||  //
//    is_same_v<T, microseconds> || //
//    is_same_v<T, milliseconds> || //
//    is_same_v<T, seconds> ||      //
//    is_same_v<T, minutes> ||      //
//    is_same_v<T, hours>           //

//struct Timer {
//#if defined(__gnu_linux__) || defined(__GNUC__)
//    const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t1;
//#else
//    const std::chrono::time_point<std::chrono::steady_clock> t1;
//#endif
//    const std::string_view stringView;

//    constexpr Timer(std::string_view name, T = {})
//        : t1 { std::chrono::high_resolution_clock::now() }
//        , stringView { name } {
//    }

//    ~Timer() {
//        using std::chrono::duration;
//        using std::chrono::high_resolution_clock;

//        duration<double, T> timeout { high_resolution_clock::now() - t1 };

//        /**/ if constexpr (std::is_same_v<T, nanoseconds>)
//            qDebug("%s\n-> %1.3f nanoseconds", stringView.data(), timeout.count());
//        else if constexpr (std::is_same_v<T, microseconds>)
//            qDebug("%s\n-> %1.3f microseconds", stringView.data(), timeout.count());
//        else if constexpr (std::is_same_v<T, milliseconds>)
//            qDebug("%s\n-> %1.3f milliseconds", stringView.data(), timeout.count());
//        else if constexpr (std::is_same_v<T, seconds>)
//            qDebug("%s\n-> %1.3f seconds", stringView.data(), timeout.count());
//        else if constexpr (std::is_same_v<T, minutes>)
//            qDebug("%s\n-> %1.3f minutes", stringView.data(), timeout.count());
//        else if constexpr (std::is_same_v<T, hours>)
//            qDebug("%s\n-> %1.3f hours", stringView.data(), timeout.count());
//    }
//};
//template <class T>
//Timer(std::string_view, T) -> Timer<T>;

class Irt5502 final : public Elemer::Device {
    Q_OBJECT
    using Elemer::Device::Device;

public:
    Irt5502(QObject* parent = nullptr);

    Elemer::DeviceType type() const override { return Elemer::IRT_5502; };

    bool setSetPoint(float val);
    bool getMasuredValue(float* val = {});
    bool setEnable(bool run);

signals:
    void measuredValue(double);

private:
    //    enum class Cmd : uint8_t {
    //        GetType = 0,
    //        GetData = 1,

    //        GetProtocolType = 32,
    //        SetDevAddress = 33,
    //        SetDevSpeed = 34,

    //        ReadPar = 37,
    //        WritePar = 38,
    //        ModifPar = 39,

    //        FOpen = 40,
    //        FSeek = 41,
    //        FRead = 42,
    //        FWrite = 43,
    //        FClose = 44,

    //        GetVer = 0XFE,

    //        ResetCpu = 0XFF
    //    };

    enum class Par : uint16_t {
        SetPoint = 0x66DA,
        All = 0xFF00,
        Enable = 0x65DA,
    };

    enum WR : uint8_t {
        Read,
        Write,
    };

    //    bool m_result = false;
    //    int m_counter = 0;
    //    IrtPort* m_port;
    //    QMutex m_mutex;
    //    QSemaphore m_semaphore;
    //    QThread m_portThread;
};
