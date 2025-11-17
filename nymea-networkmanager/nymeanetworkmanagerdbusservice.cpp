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

#include "nymeanetworkmanagerdbusservice.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(dcDBus, "DBus");

NymeaNetworkManagerDBusService::NymeaNetworkManagerDBusService(QDBusConnection::BusType busType, QObject *parent) : QObject(parent),
    m_connection(busType == QDBusConnection::SystemBus ? QDBusConnection::systemBus() : QDBusConnection::sessionBus())
{
    bool status = m_connection.registerService("io.nymea.networkmanager");
    if (!status) {
        qCWarning(dcDBus()) << "Failed to register D-Bus service.";
        return;
    }
    status = m_connection.registerObject("/io/nymea/networkmanager", "io.nymea.networkmanager", this, QDBusConnection::ExportScriptableSlots);
    if (!status) {
        qCWarning(dcDBus()) << "Failed to register D-Bus object.";
        return;
    }
    qCDebug(dcDBus()) << "Registered DBus interface";
}

void NymeaNetworkManagerDBusService::enableBluetoothServer()
{
    qCDebug(dcDBus()) << "Enable bluetooth server called";
    emit enableBluetoothServerCalled();
}

void NymeaNetworkManagerDBusService::startBluetoothServer()
{
    qCDebug(dcDBus()) << "Start bluetooth server requested";
    emit startBluetoothServerRequested();
}

void NymeaNetworkManagerDBusService::stopBluetoothServer()
{
    qCDebug(dcDBus()) << "Stop bluetooth server requested";
    emit stopBluetoothServerRequested();
}
