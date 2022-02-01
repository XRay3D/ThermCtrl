QT       += core gui serialport charts

DESTDIR = $$_PRO_FILE_PWD_/bin
#CONFIG += console

include(ElemerDevice/ElemerDevice.pri)
include(CommonInterfaces/CommonInterfaces.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++17
QMAKE_CXXFLAGS += /std:c++latest
QMAKE_CXXFLAGS += /await
DEFINES += \
#    __cpp_lib_coroutine \
    EL_LOG=1 \
#    FORCE_READ=1

#win32:RC_FILE = myapp.rc

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    automatic.cpp \
    chartview.cpp \
    irt5502.cpp \
    main.cpp \
    mainwindow.cpp \
    pointmodel.cpp \
    thermctrl.cpp

HEADERS += \
    automatic.h \
    chartview.h \
    doublespinbox.h \
    irt5502.h \
    mainwindow.h \
    pointmodel.h \
    thermctrl.h \
    version.h

FORMS += \
    mainwindow.ui \
    thermctrl.ui

DEFINES += \
#    EL_EMU \           #Эмуляция работы с устройством
#    EL_ALWAYS_OPEN \   #Если необходимо держать открытым тогда не использовать PortOener
#    FORCE_READ=1


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    myapp.rc


RESOURCES += \
    rc.qrc
