// SPDX-License-Identifier: GPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-networkmanager.
*
* nymea-networkmanager is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* nymea-networkmanager is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with nymea-networkmanager. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "nymeadservice.h"
#include <gpiobutton.h>
#include <bluetooth/bluetoothserver.h>
#include <networkmanager.h>
#include <bluetooth/bluetoothserver.h>

Q_DECLARE_LOGGING_CATEGORY(dcApplication)

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    enum Mode {
        ModeAlways,
        ModeOffline,
        ModeOnce,
        ModeStart,
        ModeButton,
        ModeDBus
    };
    Q_ENUM(Mode)

    NetworkManager *networkManager() const;
    BluetoothServer *bluetoothServer() const;
    NymeadService *nymeaService() const;

    Mode mode() const;
    void setMode(Mode mode);

    QString advertiseName() const;
    void setAdvertiseName(const QString &name, bool forceFullName = false);

    QString platformName() const;
    void setPlatformName(const QString &name);

    int advertisingTimeout() const;
    void setAdvertisingTimeout(int advertisingTimeout);

    void addGPioButton(int buttonGpio, bool activeLow = false);
    void enableDBusInterface(QDBusConnection::BusType busType);

    void run();

private:
    NetworkManager *m_networkManager = nullptr;
    BluetoothServer *m_bluetoothServer = nullptr;
    NymeadService *m_nymeaService = nullptr;
    WirelessNetworkDevice *m_wirelessDevice = nullptr;
    QList<GpioButton*> m_buttons;

    QTimer *m_advertisingTimer = nullptr;

    Mode m_mode = ModeOffline;
    QString m_advertiseName;
    bool m_forceFullName = false;
    QString m_platformName;
    int m_advertisingTimeout = 60;

    void evaluateNetworkManagerState(NetworkManager::NetworkManagerState state);

private slots:
    void startService();
    void stopService();

    void onAdvertisingTimeout();

    void onDBusStartRequested();
    void onDBusStopRequested();

    void onBluetoothServerRunningChanged(bool running);
    void onBluetoothServerConnectedChanged(bool connected);

    void onNetworkManagerAvailableChanged(bool available);
    void onNetworkManagerStateChanged(NetworkManager::NetworkManagerState state);

    void onNymeaServiceAvailableChanged(bool available);

};

#endif // CORE_H
