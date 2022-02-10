QT       += core gui serialport widgets printsupport

DESTDIR = $$_PRO_FILE_PWD_/bin
#CONFIG += console

include(ElemerDevice/ElemerDevice.pri)
include(CommonInterfaces/CommonInterfaces.pri)

#CONFIG += c++17
gcc:{
QMAKE_CXXFLAGS += -std=gnu++20
}
msvc:{
QMAKE_CXXFLAGS += /std:c++latest
QMAKE_CXXFLAGS += /await
}
win32:RC_FILE = myapp.rc

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 \    # disables all the APIs deprecated before Qt 6.0.0
#    EL_ALWAYS_OPEN \   #Если необходимо держать открытым тогда не использовать PortOener
    EL_EMU=1 \         #Эмуляция работы с устройством
    EL_LOG=1 \
#    FORCE_READ=1
#    __cpp_lib_coroutine \

SOURCES += \
    automatic.cpp \
    axistag.cpp \
    customplot.cpp \
    irt5502.cpp \
    main.cpp \
    mainwindow.cpp \
    pointmodel.cpp \
    qcustomplot.cpp \
    thermctrl.cpp

HEADERS += \
    automatic.h \
    axistag.h \
    customplot.h \
    doublespinbox.h \
    irt5502.h \
    mainwindow.h \
    pointmodel.h \
    qcustomplot.h \
    thermctrl.h \
    version.h

FORMS += \
    mainwindow.ui \
    thermctrl.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    myapp.rc


RESOURCES += \
    rc.qrc
