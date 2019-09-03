/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of nymea-networkmanager.                                    *
 *                                                                               *
 * nymea-networkmanager is free software: you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License,                *
 * or (at your option) any later version.                                        *
 *                                                                               *
 * nymea-networkmanager is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License along       *
 * with nymea-networkmanager. If not, see <http://www.gnu.org/licenses/>.        *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "nymeadservice.h"
#include "bluetooth/bluetoothserver.h"
#include "libnymea-networkmanager/networkmanager.h"

class Core : public QObject
{
    Q_OBJECT
public:

    enum Mode {
        ModeAlways,
        ModeOffline,
        ModeOnce,
        ModeStart
    };
    Q_ENUM(Mode)

    static Core* instance();
    void destroy();

    NetworkManager *networkManager() const;
    BluetoothServer *bluetoothServer() const;
    NymeadService *nymeaService() const;

    Mode mode() const;
    void setMode(const Mode &mode);

    QString advertiseName() const;
    void setAdvertiseName(const QString &name);

    QString platformName() const;
    void setPlatformName(const QString &name);

    int advertisingTimeout() const;
    void setAdvertisingTimeout(const int advertisingTimeout);

    void run();

private:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    static Core *s_instance;

    NetworkManager *m_networkManager = nullptr;
    BluetoothServer *m_bluetoothServer = nullptr;
    NymeadService *m_nymeaService = nullptr;
    WirelessNetworkDevice *m_wirelessDevice = nullptr;

    QTimer *m_advertisingTimer = nullptr;

    Mode m_mode = ModeOffline;
    QString m_advertiseName;
    QString m_platformName;
    int m_advertisingTimeout = 60;
    bool m_initRunning = true;

    void evaluateNetworkManagerState(const NetworkManager::NetworkManagerState &state);

    void startService();
    void stopService();

private slots:
    void postRun();

    void onAdvertisingTimeout();

    void onBluetoothServerRunningChanged(bool running);
    void onBluetoothServerConnectedChanged(bool connected);

    void onNetworkManagerAvailableChanged(const bool &available);
    void onNetworkManagerNetworkingEnabledChanged(bool enabled);
    void onNetworkManagerWirelessEnabledChanged(bool enabled);
    void onNetworkManagerStateChanged(const NetworkManager::NetworkManagerState &state);
    void onNetworkManagerWirelessDeviceAdded(WirelessNetworkDevice *wirelessDevice);
    void onNetworkManagerWirelessDeviceRemoved(const QString &interface);

    // Wireless device
    void onWirelessDeviceBitRateChanged(int bitRate);
    void onWirelessDeviceModeChanged(WirelessNetworkDevice::Mode mode);
    void onWirelessDeviceStateChanged(const NetworkDevice::NetworkDeviceState state);

    void onNymeaServiceAvailableChanged(bool available);

};

#endif // CORE_H
