TARGET = nymea-networkmanager1
TEMPLATE = lib

target.path = /usr/lib/$$system('dpkg-architecture -q DEB_HOST_MULTIARCH')
INSTALLS += target

QT += dbus network

QMAKE_CXXFLAGS += -Werror -std=c++11
QMAKE_LFLAGS += -std=c++11

HEADERS += \
    networkmanager.h \
    networkconnection.h \
    networkdevice.h \
    networksettings.h \
    wirednetworkdevice.h \
    wirelessaccesspoint.h \
    wirelessnetworkdevice.h \
    networkmanagerutils.h

SOURCES += \
    networkmanager.cpp \
    networkconnection.cpp \
    networkdevice.cpp \
    networksettings.cpp \
    wirednetworkdevice.cpp \
    wirelessaccesspoint.cpp \
    wirelessnetworkdevice.cpp \
    networkmanagerutils.cpp


# install header file with relative subdirectory
for(header, HEADERS) {
    path = /usr/include/libnymea-networkmanager/$${dirname(header)}
    eval(headers_$${path}.files += $${header})
    eval(headers_$${path}.path = $${path})
    eval(INSTALLS *= headers_$${path})
}
