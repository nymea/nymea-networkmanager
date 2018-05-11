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

BluetoothServer *Core::bluetoothServer() const
{
    return m_bluetoothServer;
}

NymeadService *Core::nymeaService() const
{
    return m_nymeaService;
}

QString Core::advertiseName() const
{
    return m_advertiseName;
}

void Core::setAdvertiseName(const QString &name)
{
    m_advertiseName = name;
}

QString Core::platformName() const
{
    return m_platformName;
}

void Core::setPlatformName(const QString &name)
{
    m_platformName = name;
}

bool Core::testingEnabled() const
{
    return m_testing;
}

void Core::setTestingEnabled(bool testing)
{
    m_testing = testing;
}

void Core::run()
{
    // Start the networkmanager service
    if (!m_networkManager->available()) {
        m_networkManager->start();
    } else {
        evaluateNetworkManagerState(m_networkManager->state());
    }
}

Core::Core(QObject *parent) :
    QObject(parent)
{
    m_networkManager = new NetworkManager(this);
    connect(m_networkManager, &NetworkManager::availableChanged, this, &Core::onNetworkManagerAvailableChanged);
    connect(m_networkManager, &NetworkManager::stateChanged, this, &Core::onNetworkManagerStateChanged);

    m_bluetoothServer = new BluetoothServer(this);
    connect(m_bluetoothServer, &BluetoothServer::runningChanged, this, &Core::onBluetoothServerRunningChanged);
    connect(m_bluetoothServer, &BluetoothServer::connectedChanged, this, &Core::onBluetoothServerConnectedChanged);

    m_nymeaService = new NymeadService(false, this);
    connect(m_nymeaService, &NymeadService::availableChanged, this, &Core::onNymeaServiceAvailableChanged);
}

Core::~Core()
{
    qCDebug(dcApplication()) << "Shutting down nymea service";
    delete m_nymeaService;
    m_nymeaService = nullptr;

    qCDebug(dcApplication()) << "Shutting down bluetooth service";
    delete m_bluetoothServer;
    m_bluetoothServer = nullptr;

    qCDebug(dcApplication()) << "Shutting down network-manager service";
    delete m_networkManager;
    m_networkManager = nullptr;
}

void Core::evaluateNetworkManagerState(const NetworkManager::NetworkManagerState &state)
{
    if (m_testing && m_networkManager->available()) {
        startService();
        return;
    }


    switch (state) {
    case NetworkManager::NetworkManagerStateConnectedGlobal:
        // We are online
        stopService();
        break;
    case NetworkManager::NetworkManagerStateConnectedSite:
        // We are somehow in the network
        stopService();
        break;

    case NetworkManager::NetworkManagerStateUnknown:
    case NetworkManager::NetworkManagerStateAsleep:
    case NetworkManager::NetworkManagerStateDisconnected:
    case NetworkManager::NetworkManagerStateConnectedLocal:
        // Everything else is not connected, start the service
        if (m_networkManager->available())
            startService();

        break;
    default:
        qCDebug(dcApplication()) << "Ignoring networkmanager state" << state;
        break;
    }
}

void Core::startService()
{
    if (!m_networkManager->available()) {
        qCWarning(dcApplication()) << "Could not start services. There is no network manager available.";
        return;
    }

    // Verify if we have a wireless network available
    if (!m_networkManager->wirelessAvailable()) {
        qCWarning(dcApplication()) << "Could not start services. There is no wireless device available.";
        return;
    }

    if (m_bluetoothServer->running())
        return;

    qCDebug(dcApplication()) << "Start service";

    // Disable bluetooth on nymea in order to not crash with client connections
    m_nymeaService->enableBluetooth(false);

    // Start the bluetooth server for this wireless device
    qCDebug(dcApplication()) << "Start bluetooth service";
    m_bluetoothServer->setAdvertiseName(m_advertiseName);
    m_bluetoothServer->setMachineId(m_platformName);
    m_bluetoothServer->start(m_networkManager->wirelessNetworkDevices().first());
}

void Core::stopService()
{
    if (m_testing) {
        return;
    }

    if (m_bluetoothServer->running())
        qCDebug(dcApplication()) << "Stop bluetooth service";

    m_bluetoothServer->stop();
}

void Core::onBluetoothServerRunningChanged(bool running)
{
    qCDebug(dcApplication()) << "Bluetooth server" << (running ? "started" : "stopped");

    if (!running) {
        // Enable bluetooth on nymea
        m_nymeaService->enableBluetooth(true);
    }
}

void Core::onBluetoothServerConnectedChanged(bool connected)
{
    qCDebug(dcApplication()) << "Bluetooth client" << (connected ? "connected" : "disconnected");
    if (!connected) {
        // Restart bluetooth server if a client disconnected
        m_bluetoothServer->stop();
        m_bluetoothServer->start(m_networkManager->wirelessNetworkDevices().first());
    }
}

void Core::onNetworkManagerAvailableChanged(const bool &available)
{
    if (!available) {
        qCWarning(dcApplication()) << "Networkmanager is not available any more.";
        stopService();
        return;
    }

    qCDebug(dcApplication()) << "Networkmanager is now available.";
    evaluateNetworkManagerState(m_networkManager->state());
}

void Core::onNetworkManagerStateChanged(const NetworkManager::NetworkManagerState &state)
{
    qCDebug(dcApplication()) << state;
    evaluateNetworkManagerState(state);
}

void Core::onNymeaServiceAvailableChanged(bool available)
{
    if (available && m_bluetoothServer->running()) {
        // Check if the bluetooth server is running, disable nymea bt functionality in that case
        m_nymeaService->enableBluetooth(false);
    }
}
