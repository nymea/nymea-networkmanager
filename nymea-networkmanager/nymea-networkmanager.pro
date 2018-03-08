include(../nymea-networkmanager.pri)

TARGET = nymea-networkmanager

QT += core network bluetooth dbus
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$top_srcdir/libnymea-networkmanager/
LIBS += -L$$top_builddir/libnymea-networkmanager/ -lnymea-networkmanager

HEADERS += \
    application.h \
    core.h \
    loggingcategories.h \
    nymeadservice.h \
    pushbuttonagent.h \
    bluetooth/bluetoothserver.h \
    bluetooth/networkservice.h \
    bluetooth/bluetoothuuids.h \
    bluetooth/wirelessservice.h \

SOURCES += \
    main.cpp \
    application.cpp \
    core.cpp \
    loggingcategories.cpp \
    nymeadservice.cpp \
    pushbuttonagent.cpp \
    bluetooth/bluetoothserver.cpp \
    bluetooth/networkservice.cpp \
    bluetooth/wirelessservice.cpp \

target.path = /usr/bin
INSTALLS += target
