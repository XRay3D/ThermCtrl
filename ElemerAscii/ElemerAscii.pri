QT += core serialport

include(../CommonInterfaces/CommonInterfaces.pri)

HEADERS += \
    $$PWD/ascii_device.h \
    $$PWD/common_types.h \
    $$PWD/port.h

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/ascii_device.cpp \
    $$PWD/port.cpp
