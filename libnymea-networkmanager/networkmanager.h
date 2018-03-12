/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of libnymea-networkmanager.                                 *
 *                                                                               *
 * libnymea-networkmanager is free software: you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License,                *
 * or (at your option) any later version.                                        *
 *                                                                               *
 * libnymea-networkmanager is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License along       *
 * with libnymea-networkmanager. If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusServiceWatcher>

#include "networksettings.h"
#include "wirednetworkdevice.h"
#include "networkmanagerutils.h"
#include "wirelessnetworkdevice.h"

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
    Q_ENUM(NetworkManagerState)

    enum NetworkManagerConnectivityState {
        NetworkManagerConnectivityStateUnknown = 0,
        NetworkManagerConnectivityStateNone = 1,
        NetworkManagerConnectivityStatePortal = 2,
        NetworkManagerConnectivityStateLimited = 3,
        NetworkManagerConnectivityStateFull = 4
    };
    Q_ENUM(NetworkManagerConnectivityState)

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
    Q_ENUM(NetworkManagerError)

    explicit NetworkManager(QObject *parent = 0);
    ~NetworkManager();

    bool available() const;
    bool wirelessAvailable() const;

    QList<NetworkDevice *> networkDevices() const;
    QList<WirelessNetworkDevice *> wirelessNetworkDevices() const;
    QList<WiredNetworkDevice *> wiredNetworkDevices() const;

    NetworkDevice *getNetworkDevice(const QString &interface);

    // Properties
    QString version() const;
    NetworkManagerState state() const;
    QString stateString() const;
    NetworkManagerConnectivityState connectivityState() const;

    NetworkManagerError connectWifi(const QString &interface, const QString &ssid, const QString &password, bool hidden = false);

    // Networking
    bool networkingEnabled() const;
    bool enableNetworking(bool enabled);

    // Wireless Networking
    bool wirelessEnabled() const;
    bool enableWireless(bool enabled);

private:
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    QDBusInterface *m_networkManagerInterface  = nullptr;
    NetworkSettings *m_networkSettings  = nullptr;

    QHash<QDBusObjectPath, NetworkDevice *> m_networkDevices;
    QHash<QDBusObjectPath, WirelessNetworkDevice *> m_wirelessNetworkDevices;
    QHash<QDBusObjectPath, WiredNetworkDevice *> m_wiredNetworkDevices;

    bool m_available = false;
    QString m_version;
    NetworkManagerState m_state = NetworkManagerStateUnknown;
    NetworkManagerConnectivityState m_connectivityState = NetworkManagerConnectivityStateUnknown;
    bool m_networkingEnabled = false;
    bool m_wirelessEnabled = false;

    void loadDevices();

    static QString networkManagerStateToString(const NetworkManagerState &state);
    static QString networkManagerConnectivityStateToString(const NetworkManagerConnectivityState &state);

    void setAvailable(bool available);
    void setVersion(const QString &version);
    void setNetworkingEnabled(bool enabled);
    void setWirelessEnabled(bool enabled);
    void setConnectivityState(const NetworkManagerConnectivityState &connectivityState);
    void setState(const NetworkManagerState &state);

signals:
    void availableChanged(bool available);
    void versionChanged(const QString &version);
    void networkingEnabledChanged(bool enabled);
    void wirelessEnabledChanged(bool enabled);
    void wirelessAvailableChanged(bool available);
    void stateChanged(const NetworkManagerState &state);
    void connectivityStateChanged(const NetworkManagerConnectivityState &state);

    void wirelessDeviceAdded(WirelessNetworkDevice *wirelessDevice);
    void wirelessDeviceRemoved(const QString &interface);
    void wirelessDeviceChanged(WirelessNetworkDevice *wirelessDevice);

    void wiredDeviceAdded(WiredNetworkDevice *wiredDevice);
    void wiredDeviceRemoved(const QString &interface);
    void wiredDeviceChanged(WiredNetworkDevice *wiredDevice);

private slots:
    void onServiceRegistered();
    void onServiceUnregistered();

    void onDeviceAdded(const QDBusObjectPath &deviceObjectPath);
    void onDeviceRemoved(const QDBusObjectPath &deviceObjectPath);
    void onPropertiesChanged(const QVariantMap &properties);

    void onWirelessDeviceChanged();
    void onWiredDeviceChanged();

public slots:
    bool start();
    void stop();

};

#endif // NETWORKMANAGER_H
