/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2016 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of loopd.                                                   *
 *                                                                               *
 * Loopd can not be copied and/or distributed without the express                *
 * permission of guh GmbH.                                                       *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDBusArgument>

#include "dbus-interfaces.h"
#include "wirednetworkdevice.h"
#include "wirelessnetworkdevice.h"
#include "networksettings.h"

// Docs: https://developer.gnome.org/NetworkManager/unstable/spec.html

class NetworkManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkManagerState)
    Q_ENUMS(NetworkManagerConnectivityState)
    Q_ENUMS(NetworkManagerError)

public:
    enum NetworkManagerState {
        NetworkManagerStateUnknown = 0,
        NetworkManagerStateAsleep = 10,
        NetworkManagerStateDisconnected = 20,
        NetworkManagerStateDisconnecting = 30,
        NetworkManagerStateConnecting = 40,
        NetworkManagerStateConnectedLocal = 50,
        NetworkManagerStateConnectedSite = 60,
        NetworkManagerStateConnectedGlobal = 70
    };

    enum NetworkManagerConnectivityState {
        NetworkManagerConnectivityStateUnknown = 1,
        NetworkManagerConnectivityStateNone = 2,
        NetworkManagerConnectivityStatePortal = 3,
        NetworkManagerConnectivityStateLimited = 4,
        NetworkManagerConnectivityStateFull = 5
    };

    enum NetworkManagerError {
        NetworkManagerErrorNoError,
        NetworkManagerErrorUnknownError,
        NetworkManagerErrorWirelessNotAvailable,
        NetworkManagerErrorAccessPointNotFound,
        NetworkManagerErrorNetworkInterfaceNotFound,
        NetworkManagerErrorInvalidNetworkDeviceType,
        NetworkManagerErrorWirelessNetworkingDisabled,
        NetworkManagerErrorWirelessConnectionFailed,
        NetworkManagerErrorNetworkingDisabled,
        NetworkManagerErrorNetworkManagerNotAvailable
    };

    explicit NetworkManager(QObject *parent = 0);
    ~NetworkManager();

    bool available();
    bool wirelessAvailable();

    QList<NetworkDevice *> networkDevices() const;
    QList<WirelessNetworkDevice *> wirelessNetworkDevices() const;
    QList<WiredNetworkDevice *> wiredNetworkDevices() const;

    NetworkDevice *getNetworkDevice(const QString &interface);

    // Properties
    QString version() const;
    NetworkManagerState state() const;
    QString stateString() const;
    NetworkManagerConnectivityState connectivityState() const;

    NetworkManagerError connectWifi(const QString &interface, const QString &ssid, const QString &password, const bool &hidden = false);

    // Networking
    bool networkingEnabled() const;
    bool enableNetworking(const bool &enabled);

    // Wireless Networking
    bool wirelessEnabled() const;
    bool enableWireless(const bool &enabled);

private:
    QDBusInterface *m_networkManagerInterface;

    QHash<QDBusObjectPath, NetworkDevice *> m_networkDevices;
    QHash<QDBusObjectPath, WirelessNetworkDevice *> m_wirelessNetworkDevices;
    QHash<QDBusObjectPath, WiredNetworkDevice *> m_wiredNetworkDevices;

    NetworkSettings *m_networkSettings;

    bool m_available;

    QString m_version;

    NetworkManagerState m_state;
    NetworkManagerConnectivityState m_connectivityState;
    bool m_networkingEnabled;
    bool m_wirelessEnabled;

    void loadDevices();

    static QString networkManagerStateToString(const NetworkManagerState &state);
    static QString networkManagerConnectivityStateToString(const NetworkManagerConnectivityState &state);

    void setVersion(const QString &version);
    void setNetworkingEnabled(const bool &enabled);
    void setWirelessEnabled(const bool &enabled);
    void setConnectivityState(const NetworkManagerConnectivityState &connectivityState);
    void setState(const NetworkManagerState &state);

signals:
    void versionChanged();
    void networkingEnabledChanged();
    void wirelessEnabledChanged();
    void wirelessAvailableChanged();
    void stateChanged();
    void connectivityStateChanged();

    void wirelessDeviceAdded(WirelessNetworkDevice *wirelessDevice);
    void wirelessDeviceRemoved(const QString &interface);
    void wirelessDeviceChanged(WirelessNetworkDevice *wirelessDevice);

    void wiredDeviceAdded(WiredNetworkDevice *wiredDevice);
    void wiredDeviceRemoved(const QString &interface);
    void wiredDeviceChanged(WiredNetworkDevice *wiredDevice);

private slots:
    void onDeviceAdded(const QDBusObjectPath &deviceObjectPath);
    void onDeviceRemoved(const QDBusObjectPath &deviceObjectPath);
    void onPropertiesChanged(const QVariantMap &properties);

    void onWirelessDeviceChanged();
    void onWiredDeviceChanged();
};

#endif // NETWORKMANAGER_H
