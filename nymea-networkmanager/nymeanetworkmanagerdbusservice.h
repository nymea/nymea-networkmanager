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

#ifndef NYMEANETWORKMANAGERDBUSSERVICE_H
#define NYMEANETWORKMANAGERDBUSSERVICE_H

#include <QObject>
#include <QDBusConnection>

class NymeaNetworkManagerDBusService : public QObject
{
    Q_OBJECT
public:
    explicit NymeaNetworkManagerDBusService(QDBusConnection::BusType busType, QObject *parent = nullptr);

public slots:
    Q_SCRIPTABLE void enableBluetoothServer(); // Deprecated

    Q_SCRIPTABLE void startBluetoothServer();
    Q_SCRIPTABLE void stopBluetoothServer();

signals:
    void enableBluetoothServerCalled();
    void startBluetoothServerRequested();
    void stopBluetoothServerRequested();

private:
    QDBusConnection m_connection;

};

#endif // NYMEANETWORKMANAGERDBUSSERVICE_H
