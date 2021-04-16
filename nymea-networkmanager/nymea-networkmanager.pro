include(../nymea-networkmanager.pri)

TARGET = nymea-networkmanager

QT += core network bluetooth dbus
QT -= gui

CONFIG += console link_pkgconfig
CONFIG -= app_bundle

TEMPLATE = app
PKGCONFIG += nymea-networkmanager nymea-gpio

HEADERS += \
    application.h \
    core.h \
    nymeadservice.h \
    nymeanetworkmanagerdbusservice.h \
    pushbuttonagent.h \


SOURCES += \
    main.cpp \
    application.cpp \
    core.cpp \
    nymeadservice.cpp \
    nymeanetworkmanagerdbusservice.cpp \
    pushbuttonagent.cpp \

target.path = /usr/bin
INSTALLS += target
