#pragma once

#include <QObject>
#include <stdint.h>

namespace Elemer {

#pragma pack(push, 1)

enum TProtocolType {
    ptASCII,
    ptModBus
};

struct DeviceInfo {
    uint16_t Tip;
    const char* const Name;
    uint16_t Timeout;
    uint16_t Channels;
    uint16_t Ustavki;
    TProtocolType Protocol;
    // QString Map;
    // .System : = Set<unsigned char, 0, 255> Commands;
    // TCommandType CmdType[256 /*# range 0..255*/];
    // .System : = Set<unsigned char, 0, 255> Errors;
    // QString ErrStr[256 /*# range 0..255*/];
};

// clang-format off
[[maybe_unused]]static constexpr DeviceInfo deviceInfo[] = {
    { .Tip = 0,   .Name = "Unknown device",   .Timeout = 1200, .Channels = 0,   .Ustavki = 0,  .Protocol = ptASCII }, // 0
    { .Tip = 1,   .Name = "TM 5233",          .Timeout = 400,  .Channels = 8,   .Ustavki = 8,  .Protocol = ptASCII }, // 1
    { .Tip = 2,   .Name = "TM 5232",          .Timeout = 400,  .Channels = 4,   .Ustavki = 8,  .Protocol = ptASCII }, // 2
    { .Tip = 3,   .Name = "ИРТ 1730D Old",    .Timeout = 400,  .Channels = 1,   .Ustavki = 2,  .Protocol = ptASCII }, // 3
    { .Tip = 4,   .Name = "ИРТ 1730U",        .Timeout = 400,  .Channels = 1,   .Ustavki = 2,  .Protocol = ptASCII }, // 4
    { .Tip = 5,   .Name = "ИРТ 1730D",        .Timeout = 400,  .Channels = 1,   .Ustavki = 2,  .Protocol = ptASCII }, // 5
    { .Tip = 6,   .Name = "TM 5231",          .Timeout = 400,  .Channels = 8,   .Ustavki = 0,  .Protocol = ptASCII }, // 6
    { .Tip = 7,   .Name = "РМТ 49D",          .Timeout = 400,  .Channels = 3,   .Ustavki = 12, .Protocol = ptASCII }, // 7
    { .Tip = 8,   .Name = "TM 5101",          .Timeout = 400,  .Channels = 6,   .Ustavki = 12, .Protocol = ptASCII }, // 8
    { .Tip = 9,   .Name = "ИРТ 5920(30)",     .Timeout = 150,  .Channels = 1,   .Ustavki = 3,  .Protocol = ptASCII }, // 9
    { .Tip = 10,  .Name = "ИРТМ-2405",        .Timeout = 400,  .Channels = 128, .Ustavki = 0,  .Protocol = ptASCII }, // 10
    { .Tip = 11,  .Name = "PMT 39D",          .Timeout = 400,  .Channels = 6,   .Ustavki = 24, .Protocol = ptASCII }, // 11
    { .Tip = 12,  .Name = "УСД-01",           .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 12
    { .Tip = 13,  .Name = "АИР-2",            .Timeout = 1200, .Channels = 3,   .Ustavki = 0,  .Protocol = ptASCII }, // 13
    { .Tip = 15,  .Name = "ИКСУ-2000",        .Timeout = 2000, .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 14

    { .Tip = 16,  .Name = "TM 513x",          .Timeout = 400,  .Channels = 8,   .Ustavki = 0,  .Protocol = ptASCII }, // 15
    { .Tip = 17,  .Name = "TM 5102(3)",       .Timeout = 400,  .Channels = 8,   .Ustavki = 0,  .Protocol = ptASCII }, // 16
    { .Tip = 18,  .Name = "ИРТ 1730У/А",      .Timeout = 400,  .Channels = 1,   .Ustavki = 2,  .Protocol = ptASCII }, // 17
    { .Tip = 19,  .Name = "ИРТ 1730D/A",      .Timeout = 400,  .Channels = 1,   .Ustavki = 2,  .Protocol = ptASCII }, // 18
    { .Tip = 20,  .Name = "РМТ 39DA",         .Timeout = 400,  .Channels = 6,   .Ustavki = 24, .Protocol = ptASCII }, // 19
    { .Tip = 21,  .Name = "ИКСУ-200",         .Timeout = 2000, .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 20
    { .Tip = 22,  .Name = "РМТ 49DA/3",       .Timeout = 400,  .Channels = 3,   .Ustavki = 12, .Protocol = ptASCII }, // 21
    { .Tip = 23,  .Name = "РМТ 49DA/1",       .Timeout = 400,  .Channels = 1,   .Ustavki = 4,  .Protocol = ptASCII }, // 22

    { .Tip = 25,  .Name = "КДС-02",           .Timeout = 600,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 23
    { .Tip = 27,  .Name = "TM 5122A",         .Timeout = 400,  .Channels = 4,   .Ustavki = 0,  .Protocol = ptASCII }, // 24
    { .Tip = 28,  .Name = "ИПМ 0399/М0",      .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 25
    { .Tip = 30,  .Name = "ИПМ 0399/М3",      .Timeout = 1200, .Channels = 1,   .Ustavki = 3,  .Protocol = ptASCII }, // 26

    { .Tip = 31,  .Name = "РМТ 39DEx",        .Timeout = 400,  .Channels = 6,   .Ustavki = 24, .Protocol = ptASCII }, // 27
    { .Tip = 32,  .Name = "РМТ 49DEx/3",      .Timeout = 400,  .Channels = 3,   .Ustavki = 12, .Protocol = ptASCII }, // 28
    { .Tip = 33,  .Name = "РМТ 49DEx/1",      .Timeout = 400,  .Channels = 1,   .Ustavki = 4,  .Protocol = ptASCII }, // 29

    // далее - новые приборы
    { .Tip = 34,  .Name = "РОСА-10",          .Timeout = 400,  .Channels = 2,   .Ustavki = 0,  .Protocol = ptASCII }, // 30
    { .Tip = 41,  .Name = "ИРТ 5501",         .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 31
    { .Tip = 42,  .Name = "РМТ 69",           .Timeout = 1000, .Channels = 6,   .Ustavki = 0,  .Protocol = ptASCII }, // 32
    { .Tip = 43,  .Name = "АИР 30",           .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 33
    { .Tip = 46,  .Name = "РМТ 59",           .Timeout = 1000, .Channels = 6,   .Ustavki = 0,  .Protocol = ptASCII }, // 34
    { .Tip = 48,  .Name = "БППС 4090/М1x",    .Timeout = 400,  .Channels = 2,   .Ustavki = 0,  .Protocol = ptASCII }, // 35
    { .Tip = 49,  .Name = "БППС 4090 .М12/4", .Timeout = 400,  .Channels = 4,   .Ustavki = 0,  .Protocol = ptASCII }, // 36
    { .Tip = 50,  .Name = "ИКСУ-260",         .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 37
    { .Tip = 51,  .Name = "БППС 4090 Ex",     .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 38
    { .Tip = 53,  .Name = "ТЦМ 9610",         .Timeout = 400,  .Channels = 2,   .Ustavki = 0,  .Protocol = ptASCII }, // 39
    { .Tip = 54,  .Name = "ИРТ 5502",         .Timeout = 400,  .Channels = 2,   .Ustavki = 0,  .Protocol = ptASCII }, // 40
    { .Tip = 55,  .Name = "ИРТ 5503",         .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 41
    { .Tip = 56,  .Name = "ИРТ 5504",         .Timeout = 400,  .Channels = 2,   .Ustavki = 0,  .Protocol = ptASCII }, // 42
    { .Tip = 57,  .Name = "EL-4019",          .Timeout = 400,  .Channels = 8,   .Ustavki = 0,  .Protocol = ptASCII }, // 43
    { .Tip = 58,  .Name = "ИРТ 1730НМ",       .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 44
    { .Tip = 59,  .Name = "EL-4015",          .Timeout = 400,  .Channels = 6,   .Ustavki = 0,  .Protocol = ptASCII }, // 45
    { .Tip = 60,  .Name = "EL-4024",          .Timeout = 400,  .Channels = 4,   .Ustavki = 0,  .Protocol = ptASCII }, // 46
    { .Tip = 65,  .Name = "ИРТ 5940",         .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 47
    { .Tip = 70,  .Name = "РМТ 69M",          .Timeout = 1000, .Channels = 6,   .Ustavki = 0,  .Protocol = ptASCII }, // 48
    { .Tip = 71,  .Name = "ИРТ 5922MB",       .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptModBus }, // 49
    { .Tip = 72,  .Name = "РМТ 69L",          .Timeout = 1000, .Channels = 6,   .Ustavki = 0,  .Protocol = ptASCII }, // 50
    { .Tip = 73,  .Name = "ТМ 5102Д",         .Timeout = 400,  .Channels = 4,   .Ustavki = 0,  .Protocol = ptModBus }, // 51
    { .Tip = 74,  .Name = "ТМ 5103Д",         .Timeout = 400,  .Channels = 8,   .Ustavki = 0,  .Protocol = ptModBus }, // 52
    { .Tip = 75,  .Name = "ТМ 5104Д",         .Timeout = 400,  .Channels = 16,  .Ustavki = 0,  .Protocol = ptModBus }, // 53
    { .Tip = 76,  .Name = "КП-140E",          .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptModBus }, // 54
    { .Tip = 77,  .Name = "РМТ 49",           .Timeout = 1000, .Channels = 3,   .Ustavki = 0,  .Protocol = ptASCII }, // 55
    { .Tip = 78,  .Name = "ДДПН-К",           .Timeout = 400,  .Channels = 1,   .Ustavki = 0,  .Protocol = ptASCII }, // 56
    { .Tip = 80,  .Name = "КС-xE",            .Timeout = 1000, .Channels = 6,   .Ustavki = 0,  .Protocol = ptASCII }, // 57
    { .Tip = 255, .Name = "АКВТ-03",          .Timeout = 400,  .Channels = 3,   .Ustavki = 0,  .Protocol = ptModBus } // 255
};
// clang-format on

struct AllChValIrt5502 {
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

#pragma pack(pop)

enum class Cmd : uint8_t {
    // clang-format off
    GetDevice       = 0,  // Команда "определение типа прибора"                                     Cmd 0 стр.3
    ReadData        = 1,  // Команда "чтение измеряемой величины"                                   Cmd 1 стр.3

    ProtocolType    = 32, // Команда "Определить разновидность протокола"                           Cmd 32 стр.3
    SetAddress      = 33, // Команда address "Установить адрес прибора (модуля связи)"              Cmd 33 стр.4
    SetBaudRate     = 34, // Команда speed "Установить скорость обмена с прибором (модулем связи)"  Cmd 34 стр.4
    ReadStatus      = 35, // Команда reads "прочитать значение регистра статуса модуля связи"       Cmd 35 стр.4
    ReadNByte       = 36, // Команда readb "прочитать N байт из буфера возврата модуля связи"       Cmd 36 стр.4

    //SeekRetBuf    = 47, // Команда seekb "установка адреса в буфере возврата "                    Cmd 47 стр.4
    DevParamRead    = 37, // Команда readp "Чтение параметра прибора"                               Cmd 37 стр.5
    DevParamWrite   = 38, // Команда writep "Запись параметра прибора"                              Cmd 38 стр.5
    DevParamModif   = 39, // Команда modifyp "Модификация параметра прибора"                        Cmd 39 стр.5

    GetVer          = 0XFE,
    ResetCpu        = 0XFF
    // clang-format on
};

enum class FileCmd : uint8_t {
    // clang-format off
    Open        = 40, // Команда open "открыть файл"                                            Cmd 40 стр.5
    Seek        = 41, // Команда seek "установка адреса в файле"                                Cmd 41 стр.6
    Read        = 42, // Команда read "чтение N байт данных из файла с текущего адреса"         Cmd 42 стр.7
    Write       = 43, // Команда write "запись N байта данных в файл по текущему адресу"        Cmd 43 стр.7
    Close       = 44, // Команда close "закрыть файл"                                           Cmd 44 стр.7
    Tell        = 45, // Команда tell "определить позицию в файле"                              Cmd 45 стр.7
    ChMod       = 46, // Команда chmod " изменить разрешенный доступ для файла"                 Cmd 46 стр.8
    Remove      = 47, // Команда remove "удалить файл"                                          Cmd 47 стр.8
    // clang-format on
};

enum class Seek : uint8_t {
    Set, // 0 (SEEK_SET) – начало буфера возврата, <origin> - начальная позиция.
    Cur, // 1 (SEEK_CUR) – текущая позиция указателя на буфер возврата
    End, // 2 (SEEK_END) – конец буфера возврата
};

enum /*class*/ RetCcode : uint8_t {
    Ok
};

enum class PortPolicy {
    AlwaysOpen,
    CloseAfterExchange,
};
//enum class Par : uint16_t {
//    SetPoint = 0x66DA,
//    All = 0xFF00,
//    Enable = 0x65DA,
//};

//enum : uint8_t {
//    Read,
//    Write,
//};

}

//Q_DECLARE_METATYPE(Elemer: = MeasuredValues)
