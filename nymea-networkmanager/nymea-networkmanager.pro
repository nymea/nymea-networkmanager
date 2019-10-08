include(../nymea-networkmanager.pri)

TARGET = nymea-networkmanager

QT += core network bluetooth dbus
QT -= gui

CONFIG += console link_pkgconfig
CONFIG -= app_bundle

TEMPLATE = app
PKGCONFIG += nymea-networkmanager

HEADERS += \
    application.h \
    core.h \
    loggingcategories.h \
    nymeadservice.h \
    pushbuttonagent.h \


SOURCES += \
    main.cpp \
    application.cpp \
    core.cpp \
    loggingcategories.cpp \
    nymeadservice.cpp \
    pushbuttonagent.cpp \

target.path = /usr/bin
INSTALLS += target
