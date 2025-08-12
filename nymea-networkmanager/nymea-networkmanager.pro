include(../nymea-networkmanager.pri)

TARGET = nymea-networkmanager

greaterThan(QT_MAJOR_VERSION, 5) {
    message("Building using Qt6 support")
    CONFIG *= c++17
    QMAKE_LFLAGS *= -std=c++17
    QMAKE_CXXFLAGS *= -std=c++17
} else {
    message("Building using Qt5 support")
    CONFIG *= c++11
    QMAKE_LFLAGS *= -std=c++11
    QMAKE_CXXFLAGS *= -std=c++11
    DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x050F00
}

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
