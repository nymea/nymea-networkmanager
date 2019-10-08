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
#include "nymea-gpio/gpiobutton.h"
#include "bluetooth/bluetoothserver.h"
#include "nymea-networkmanager/networkmanager.h"
#include "nymea-networkmanager/bluetooth/bluetoothserver.h"

class Core : public QObject
{
    Q_OBJECT
public:

    enum Mode {
        ModeAlways,
        ModeOffline,
        ModeOnce,
        ModeStart,
        ModeButton
    };
    Q_ENUM(Mode)

    static Core* instance();
    void destroy();

    NetworkManager *networkManager() const;
    BluetoothServer *bluetoothServer() const;
    NymeadService *nymeaService() const;

    Mode mode() const;
    void setMode(Mode mode);

    QString advertiseName() const;
    void setAdvertiseName(const QString &name);

    QString platformName() const;
    void setPlatformName(const QString &name);

    int advertisingTimeout() const;
    void setAdvertisingTimeout(int advertisingTimeout);

    int buttonGpio() const;
    void setButtonGpio(int buttonGpio);

    void run();

private:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    static Core *s_instance;

    NetworkManager *m_networkManager = nullptr;
    BluetoothServer *m_bluetoothServer = nullptr;
    NymeadService *m_nymeaService = nullptr;
    WirelessNetworkDevice *m_wirelessDevice = nullptr;
    GpioButton *m_button = nullptr;

    QTimer *m_advertisingTimer = nullptr;

    Mode m_mode = ModeOffline;
    QString m_advertiseName;
    QString m_platformName;
    int m_advertisingTimeout = 60;
    bool m_initRunning = true;
    int m_buttonGpio = -1;

    void evaluateNetworkManagerState(NetworkManager::NetworkManagerState state);

private slots:
    void startService();
    void stopService();

    void postRun();

    void onAdvertisingTimeout();

    void onBluetoothServerRunningChanged(bool running);
    void onBluetoothServerConnectedChanged(bool connected);

    void onNetworkManagerAvailableChanged(bool available);
    void onNetworkManagerStateChanged(NetworkManager::NetworkManagerState state);

    void onButtonLongPressed();

    void onNymeaServiceAvailableChanged(bool available);

};

#endif // CORE_H
