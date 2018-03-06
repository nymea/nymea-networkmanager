include(../nymea-networkmanager.pri)

TARGET = nymea-networkmanager

QT += core network bluetooth dbus
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$top_srcdir/libnymea-networkmanager/
LIBS += -L$$top_builddir/libnymea-networkmanager/ -lnymea-networkmanager1

HEADERS += \
    core.h \
    bluetooth/bluetoothserver.h \
    bluetooth/networkservice.h \
    bluetooth/bluetoothuuids.h \
    bluetooth/wirelessservice.h \
    bluetooth/loggingcategories.h

SOURCES += \
    main.cpp \
    core.cpp \
    bluetooth/bluetoothserver.cpp \
    bluetooth/networkservice.cpp \
    bluetooth/wirelessservice.cpp \
    bluetooth/loggingcategories.cpp

target.path = /usr/bin
INSTALLS += target
