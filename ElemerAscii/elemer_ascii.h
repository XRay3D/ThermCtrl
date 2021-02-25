#pragma once

#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>
#include <QList>
#include <array>
#include <charconv>
#include <concepts>
enum {
    COMMUTATOR = 16,
    IKSU_2000 = 15,
    IRT = 9,
    IRT5502 = 54,
    MAN = 50,
};

template <typename T>
constexpr bool false_v = false;

struct Hex {
    QByteArray hex;
    template <typename T>
    Hex(T&& val)
        : hex(toHex(val))
    {
    }
    template <typename T>
    static QByteArray toHex(T&& val)
    {
        using Ty = std::decay_t<T>;
        //qDebug() << __FUNCTION__ << typeid(Ty).name() << sizeof(Ty);
        if /*  */ constexpr (std::is_integral_v<Ty> || std::is_enum_v<Ty> || std::is_floating_point_v<Ty>)
            return QByteArray(reinterpret_cast<const char*>(&val), sizeof(Ty)).toHex().toUpper();
        else if constexpr (std::is_same_v<Ty, QString>)
            return val.toLocal8Bit().toHex().toUpper();
        else if constexpr (std::is_same_v<Ty, QByteArray>)
            return val.toHex().toUpper();
        else
            static_assert(false_v<Ty>, "Unimplemeted conversion");
    }
};

struct SkipSemicolon {
};

struct ByteArray : QByteArray {
};

template <typename T>
concept Integral = requires(T) { std::is_integral_v<T>; };

class ElemerASCII {
    static inline QByteArray parcel;

    template <typename T>
    void func(T&& arg) requires std::is_integral_v<std::decay_t<T>>
    {
        std::array<char, 16> str { 0 };
        if (auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), static_cast<int64_t>(arg)); !(ec == std::errc {}))
            qDebug() << static_cast<int>(ec);
        parcel.append(str.data()).append(';');
    };

    template <typename T>
    void func(T&& arg) requires std::is_floating_point_v<std::decay_t<T>>
    {
        std::array<char, 16> str { 0 };
        if (auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), arg, std::chars_format::fixed, 5); !(ec == std::errc {}))
            qDebug() << static_cast<int>(ec);
        parcel.append(str.data()).append(';');
    };

    template <typename T>
    void func(T&& arg) requires std::is_enum_v<std::decay_t<T>>
    {
        std::array<char, 16> str { 0 };
        if (auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), static_cast<int>(arg)); !(ec == std::errc {}))
            qDebug() << static_cast<int>(ec);
        parcel.append(str.data()).append(';');
    };

    template <typename T>
    void func(T&& arg) requires std::is_same_v<std::decay_t<T>, Hex> { parcel.append(arg.hex).append(';'); };

    template <typename T>
    void func(T&& arg) requires std::is_same_v<std::decay_t<T>, QByteArray> { parcel.append(arg).append(';'); };

    template <typename T>
    void func(T&& arg) requires std::is_same_v<std::decay_t<T>, QString> { parcel.append(arg.toLocal8Bit()).append(';'); };

    template <typename T>
    void func(T&& /*arg*/) requires std::is_same_v<std::decay_t<T>, SkipSemicolon> { parcel.resize(parcel.size() - 1); };

    static constexpr uint8_t tableCrc16Lo[] {
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
    };
    static constexpr uint8_t tableCrc16Hi[] {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
        0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
        0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
        0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
        0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
        0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
        0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
        0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
        0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
        0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
        0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
        0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
        0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
        0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
        0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40

    };
    static inline const QByteArray success_ { "$0" };

public:
    bool success()
    {
        m_lastRetCode = m_data.value(1);
        return m_lastRetCode == success_;
    };

    //bool getSuccess(QByteArray data)
    //{
    //    qDebug() << data;
    //    if (data.isEmpty()) {
    //        return false;
    //    }
    //    int i = 0;
    //    while (data[0] != '!' && data.size()) {
    //        data = data.remove(0, 1);
    //    }
    //    while (data[data.size() - 1] != '\r' && data.size()) {
    //        data = data.remove(data.size() - 1, 1);
    //    }
    //    data = data.remove(data.size() - 1, 1);
    //    QList<QByteArray> list = data.split(';');
    //    data.clear();
    //    while (i < list.count() - 1) {
    //        data.append(list[i++]).append(';');
    //    }
    //    if (calcCrc(data).toInt() == list.last().toInt() && list.count() > 2) {
    //        if (list.at(1) == "$0") {
    //            return true;
    //        }
    //    }
    //    return false;
    //}

    QByteArray calcCrc(const QByteArray& parcel)
    {
        // (X^16 + X^15 + X^2 + 1).
        uint16_t crcLo = 0xFF, crcHi = 0xFF;
        uint8_t index;
        for (auto byteIt = parcel.begin() + 1; byteIt != parcel.end(); ++byteIt) {
            index = crcLo ^ *byteIt;
            crcLo = crcHi ^ tableCrc16Lo[index];
            crcHi = tableCrc16Hi[index];
        }
        return QByteArray::number(crcHi << 8 | crcLo);
    }

    bool checkParcel()
    {
        if (int index = m_parcel.indexOf('!'); index > 0)
            m_parcel.remove(0, index);

        if (int index = m_parcel.lastIndexOf('\r'); index > 0)
            m_parcel.resize(index);

        if (int index = m_parcel.lastIndexOf(';') + 1; index > 0 && calcCrc(m_parcel.left(index)).toUInt() == m_parcel.right(m_parcel.length() - index).toUInt()) {
            m_data = m_parcel.split(';');
            m_data.first().remove(0, 1);
            return true;
        }
        m_data.clear();
        return false;
    }

    template <typename... Ts>
    QByteArray createParcel(Ts&&... args)
    {
        parcel.clear();
        parcel.append(':');
        (func(args), ...);
        parcel.append(calcCrc(parcel)).append('\r');
        return QByteArray(1, 0xFF) + parcel;
    }

    template <typename T>
        auto fromHex(int index, bool* ok = nullptr) requires std::is_pod_v<T> || std::is_trivial_v<T>
    {
        T t {};
        auto data { QByteArray::fromHex(m_data.value(index)) };
        if (data.size() == sizeof(T)) {
            ok ? * ok = true : true;
            memcpy(&t, data.data(), data.size());
        } else
            ok ? * ok = false : false;
        return t;
    }

protected:
    QList<QByteArray> m_data;
    QByteArray m_lastRetCode;
    QByteArray m_parcel;
};