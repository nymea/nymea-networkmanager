#ifndef NETWORKMANAGERUTILS_H
#define NETWORKMANAGERUTILS_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dcNetworkManager)

class NetworkManagerUtils
{
    Q_GADGET

public:
    static QString networkManagerServiceString();

    static QString networkManagerPathString();
    static QString settingsPathString();

    static QString deviceInterfaceString();
    static QString wirelessInterfaceString();
    static QString wiredInterfaceString();
    static QString accessPointInterfaceString();
    static QString settingsInterfaceString();
    static QString connectionsInterfaceString();

};

#endif // NETWORKMANAGERUTILS_H
