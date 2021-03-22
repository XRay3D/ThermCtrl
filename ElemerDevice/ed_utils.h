#pragma once

//Qt
#include <QByteArray>
#include <QDebug>
//std
#include <array>
#include <charconv>
#include <concepts>

namespace Elemer {

struct SkipSemicolon {
};

struct Semicolon {
};

struct Hex {
    const QByteArray hex;

    template <typename T>
    explicit Hex(T&& val) requires(std::is_arithmetic_v<std::decay_t<T>> || std::is_enum_v<std::decay_t<T>>)
        : hex(QByteArray(reinterpret_cast<const char*>(&val), sizeof(std::decay_t<T>)).toHex().toUpper())
    {
    }

    template <typename T>
    explicit Hex(T&& val) requires std::is_same_v<T, QString>
        : hex(val.toLocal8Bit().toHex().toUpper())
    {
    }

    template <typename T>
    explicit Hex(T&& val) requires std::is_same_v<T, QByteArray>
        : hex(val.toHex().toUpper())
    {
    }

    template <typename T>
    explicit Hex(T&&) requires std::is_same_v<T, Semicolon>
        : hex(1, ';')
    {
    }

    template <typename... Ts>
    explicit Hex(Ts&&... vals)
        : hex((Hex(vals) + ...))
    {
    }

    operator QByteArray() const noexcept { return hex; }
    size_t size() const noexcept { return hex.size(); }
};

template <typename T>
struct FromHex {
    T& val;
    using value_type = T;

    operator T&() noexcept { return val; }
    operator T&() const noexcept { return val; }

    auto operator=(const QByteArray& arr) requires(std::is_array_v<T>)
    {
        auto data { QByteArray::fromHex(arr) };
        std::memcpy(val, data.data(), data.size());
        return (*this);
    }

    auto operator=(const QByteArray& arr)
    {
        val = *reinterpret_cast<T*>(QByteArray::fromHex(arr).data());
        return (*this);
    }
};
template <typename T>
FromHex(T&) -> FromHex<T>; // template deduction guide

template <typename T>
struct FromStr {
    T& val;
    bool ok {};
    using value_type = T;

    operator T&() noexcept { return val; }
    operator T&() const noexcept { return val; }

    bool isOk() noexcept { return ok; }

    auto operator=(const QByteArray& arr) requires(std::is_integral_v<T> || std::is_enum_v<T>)
    {
        val = arr.toLongLong(&ok);
        return (*this);
    }
    auto operator=(const QByteArray& arr) requires(std::is_floating_point_v<T>)
    {
        val = arr.toDouble(&ok);
        return (*this);
    }
    auto operator=(const QByteArray& arr) requires(std::is_same_v<QByteArray, T>)
    {
        val = arr;
        return (*this);
    }
};
template <typename T>
FromStr(T&) -> FromStr<T>; // template deduction guide

struct Parcel {
    QByteArray parcel;

    template <typename... Ts>
    Parcel(Ts&&... args)
    {
        parcel.clear();
        parcel.append(':');
        (func(args), ...);
    }

    operator QByteArray() const { return parcel; }
    // integral // enum
    template <typename T>
    void func(T&& arg) requires(std::is_integral_v<std::decay_t<T>> || std::is_enum_v<std::decay_t<T>>)
    {
        std::array<char, 16> str { 0 };
        auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), static_cast<int64_t>(arg));
        if (!(ec == std::errc {}))
            qDebug() << static_cast<int>(ec);
        parcel.append(str.data()).append(';');
    };

    // floating point
    template <typename T>
    void func(T&& arg) requires std::is_floating_point_v<std::decay_t<T>>
    {
        std::array<char, 16> str { 0 };
        auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), arg, std::chars_format::fixed, 5);
        if (!(ec == std::errc {}))
            qDebug() << static_cast<int>(ec);
        parcel.append(str.data()).append(';');
    };

    // Hex
    template <typename T>
    void func(T&& arg) requires std::is_same_v<std::decay_t<T>, Hex>
    {
        if (arg.size())
            parcel.append(arg).append(';');
    };

    // QByteArray
    template <typename T>
    void func(T&& arg) requires std::is_same_v<std::decay_t<T>, QByteArray>
    {
        if (arg.size())
            parcel.append(arg).append(';');
    };

    // QString
    template <typename T>
    void func(T&& arg) requires std::is_same_v<std::decay_t<T>, QString>
    {
        if (arg.size())
            parcel.append(arg.toLocal8Bit()).append(';');
    };

    // SkipSemicolon
    template <typename T>
    void func(T&&) requires std::is_same_v<std::decay_t<T>, SkipSemicolon>
    {
        parcel.resize(parcel.size() - 1);
    };
    template <typename T>
    void func(T&&) requires std::is_same_v<std::decay_t<T>, Semicolon>
    {
        parcel.append(';');
    };
};

}
