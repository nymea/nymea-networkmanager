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

#include "core.h"
#include "loggingcategories.h"

Core* Core::s_instance = nullptr;

Core *Core::instance()
{
    if (!s_instance)
        s_instance = new Core();

    return s_instance;
}

void Core::destroy()
{    
    if (s_instance)
        delete s_instance;

    s_instance = nullptr;
}

NetworkManager *Core::networkManager() const
{
    return m_networkManager;
}

Core::Core(QObject *parent) :
    QObject(parent)
{
    m_networkManager = new NetworkManager(this);
    connect(m_networkManager, &NetworkManager::availableChanged, this, &Core::onNetworkManagerAvailableChanged);

    m_bluetoothServer = new BluetoothServer("nymea", this);

    m_nymeaService = new NymeadService(false, this);

    if (!m_networkManager->start()) {
        qCWarning(dcApplication()) << "Could not start network manager.";
    }
}

Core::~Core()
{
    delete m_networkManager;
    m_networkManager = nullptr;

    delete m_bluetoothServer;
    m_bluetoothServer = nullptr;
}

void Core::onNetworkManagerAvailableChanged(const bool &available)
{
    if (!available) {
        qCWarning(dcApplication()) << "Networkmanager is not available any more.";
    } else {
        qCDebug(dcApplication()) << "Networkmanager is now available.";

    }
}
