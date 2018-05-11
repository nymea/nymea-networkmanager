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

#include "networkmanager.h"
#include "nymeadservice.h"
#include "bluetooth/bluetoothserver.h"

class Core : public QObject
{
    Q_OBJECT
public:
    static Core* instance();
    void destroy();

    NetworkManager *networkManager() const;
    BluetoothServer *bluetoothServer() const;
    NymeadService *nymeaService() const;

    QString advertiseName() const;
    void setAdvertiseName(const QString &name);

    QString platformName() const;
    void setPlatformName(const QString &name);

    bool testingEnabled() const;
    void setTestingEnabled(bool testing);

    void run();

private:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    static Core *s_instance;

    NetworkManager *m_networkManager = nullptr;
    BluetoothServer *m_bluetoothServer = nullptr;
    NymeadService *m_nymeaService = nullptr;

    QString m_advertiseName;
    QString m_platformName;
    bool m_testing = false;

    void evaluateNetworkManagerState(const NetworkManager::NetworkManagerState &state);

    void startService();
    void stopService();

private slots:
    void onBluetoothServerRunningChanged(bool running);
    void onBluetoothServerConnectedChanged(bool connected);

    void onNetworkManagerAvailableChanged(const bool &available);
    void onNetworkManagerStateChanged(const NetworkManager::NetworkManagerState &state);

    void onNymeaServiceAvailableChanged(bool available);

};

#endif // CORE_H
