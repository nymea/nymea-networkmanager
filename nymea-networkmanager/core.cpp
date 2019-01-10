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

#include <QTimer>

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

Core::Mode Core::mode() const
{
    return m_mode;
}

void Core::setMode(const Core::Mode &mode)
{
    m_mode = mode;
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

int Core::advertisingTimeout() const
{
    return m_advertisingTimeout;
}

void Core::setAdvertisingTimeout(const int advertisingTimeout)
{
    m_advertisingTimeout = advertisingTimeout;
}

void Core::run()
{
    // Start the networkmanager
    if (!m_networkManager->start()) {
        qCWarning(dcApplication()) << "Could not start network manager. Please make sure the networkmanager is available.";
        return;
    }

    // Note: give network-manager more time to start and get online status
    QTimer::singleShot(5000, this, &Core::postRun);
}

Core::Core(QObject *parent) :
    QObject(parent)
{
    m_networkManager = new NetworkManager(this);
    connect(m_networkManager, &NetworkManager::availableChanged, this, &Core::onNetworkManagerAvailableChanged);
    connect(m_networkManager, &NetworkManager::stateChanged, this, &Core::onNetworkManagerStateChanged);
    connect(m_networkManager, &NetworkManager::networkingEnabledChanged, this, &Core::onNetworkManagerNetworkingEnabledChanged);
    connect(m_networkManager, &NetworkManager::wirelessEnabledChanged, this, &Core::onNetworkManagerWirelessEnabledChanged);
    connect(m_networkManager, &NetworkManager::wirelessDeviceAdded, this, &Core::onNetworkManagerWirelessDeviceAdded);
    connect(m_networkManager, &NetworkManager::wirelessDeviceRemoved, this, &Core::onNetworkManagerWirelessDeviceRemoved);

    m_bluetoothServer = new BluetoothServer(this);
    connect(m_bluetoothServer, &BluetoothServer::runningChanged, this, &Core::onBluetoothServerRunningChanged);
    connect(m_bluetoothServer, &BluetoothServer::connectedChanged, this, &Core::onBluetoothServerConnectedChanged);

    m_nymeaService = new NymeadService(false, this);
    connect(m_nymeaService, &NymeadService::availableChanged, this, &Core::onNymeaServiceAvailableChanged);

    m_advertisingTimer = new QTimer(this);
    m_advertisingTimer->setSingleShot(true);

    connect(m_advertisingTimer, &QTimer::timeout, this, &Core::onAdvertisingTimeout);
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
    if (m_mode != ModeOffline)
        return;

    // Note: if the wireless device is in the access point mode, the bluetooth server should stop
    if (m_wirelessDevice && m_wirelessDevice->mode() == WirelessNetworkDevice::ModeAccessPoint) {
        stopService();
        return;
    }

    switch (state) {
    case NetworkManager::NetworkManagerStateConnectedGlobal:
        // We are online
        if (m_bluetoothServer->running() && !m_bluetoothServer->connected()) {
            qCDebug(dcApplication()) << "Stop the bluetooth service because of \"offline\" mode.";
            stopService();
        }
        break;
    case NetworkManager::NetworkManagerStateConnectedSite:
        // We are somehow in the network
        if (m_bluetoothServer->running() && !m_bluetoothServer->connected()) {
            qCDebug(dcApplication()) << "Stop the bluetooth service because of \"offline\" mode.";
            stopService();
        }
        break;
    case NetworkManager::NetworkManagerStateUnknown:
    case NetworkManager::NetworkManagerStateAsleep:
    case NetworkManager::NetworkManagerStateDisconnected:
    case NetworkManager::NetworkManagerStateConnectedLocal:
        // Everything else is not connected, start the service
        qCDebug(dcApplication()) << "Start the bluetooth service because of \"offline\" mode.";
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

    qCDebug(dcApplication()) << "Start service";

    // Disable bluetooth on nymea in order to not crash with client connections
    m_nymeaService->enableBluetooth(false);

    // Start the bluetooth server for this wireless device
    qCDebug(dcApplication()) << "Start bluetooth service";
    m_bluetoothServer->setAdvertiseName(m_advertiseName);
    m_bluetoothServer->setMachineId(m_platformName);

    QTimer::singleShot(5000, m_bluetoothServer, &BluetoothServer::start);
}

void Core::stopService()
{
    if (m_bluetoothServer && m_bluetoothServer->running()) {
        qCDebug(dcApplication()) << "Stop bluetooth service";
        m_bluetoothServer->stop();
    }
}

void Core::postRun()
{
    qCDebug(dcApplication()) << "Post run service";
    m_initRunning = false;

    switch (m_mode) {
    case ModeAlways:
        qCDebug(dcApplication()) << "Start the bluetooth service because of \"always\" mode.";
        startService();
        break;
    case ModeStart:
        qCDebug(dcApplication()) << "Start the bluetooth service because of \"start\" mode.";
        startService();
        m_advertisingTimer->start(m_advertisingTimeout * 1000);
        break;
    case ModeOffline:
        evaluateNetworkManagerState(m_networkManager->state());
        break;
    case ModeOnce:
        if (m_networkManager->networkSettings()->connections().isEmpty()) {
            qCDebug(dcApplication()) << "Start the bluetooth service because of \"once\" mode and there is currenlty no network configured yet.";
            startService();
        } else {
            qCDebug(dcApplication()) << "Not starting the bluetooth service because of \"once\" mode. There are" << m_networkManager->networkSettings()->connections().count() << "network configurations.";
        }
        break;
    }
}

void Core::onAdvertisingTimeout()
{
    if (m_mode != ModeStart)
        return;

    qCDebug(dcApplication()) << "Advertising timeout. Shutting down the bluetooth server.";
    stopService();
}

void Core::onBluetoothServerRunningChanged(bool running)
{
    qCDebug(dcApplication()) << "Bluetooth server" << (running ? "started" : "stopped");

    if (!running) {
        // Enable bluetooth on nymea
        m_nymeaService->enableBluetooth(true);
        m_advertisingTimer->stop();

        switch (m_mode) {
        case ModeAlways:
            qCDebug(dcApplication()) << "Restart the bluetooth service because of \"always\" mode.";
            startService();
            break;
        case ModeStart:
            break;
        case ModeOffline:
            evaluateNetworkManagerState(m_networkManager->state());
            break;
        case ModeOnce:
            if (m_networkManager->networkSettings()->connections().isEmpty()) {
                qCDebug(dcApplication()) << "Start the bluetooth service because of \"once\" mode and there is currenlty no network configured yet.";
                startService();
            } else {
                qCDebug(dcApplication()) << "Not starting the bluetooth service because of \"once\" mode. There are" << m_networkManager->networkSettings()->connections().count() << "network configurations.";
            }
            break;
        }
    }
}

void Core::onBluetoothServerConnectedChanged(bool connected)
{
    qCDebug(dcApplication()) << "Bluetooth client" << (connected ? "connected" : "disconnected");
    if (connected) {
        m_advertisingTimer->stop();

        m_bluetoothServer->onNetworkManagerAvailableChanged(m_networkManager->available());
        m_bluetoothServer->onNetworkManagerStateChanged(m_networkManager->state());
        m_bluetoothServer->onNetworkingEnabledChanged(m_networkManager->networkingEnabled());
        m_bluetoothServer->onWirelessNetworkingEnabledChanged(m_networkManager->wirelessEnabled());

        if (m_wirelessDevice) {
            m_bluetoothServer->onWirelessDeviceStateChanged(m_wirelessDevice->deviceState());
            m_bluetoothServer->onWirelessDeviceModeChanged(m_wirelessDevice->mode());
        }

    } else {
        m_advertisingTimer->stop();
        m_bluetoothServer->stop();
    }
}

void Core::onNetworkManagerAvailableChanged(const bool &available)
{
    if (!available) {
        qCWarning(dcApplication()) << "Networkmanager is not available any more.";
        m_bluetoothServer->onNetworkManagerAvailableChanged(m_networkManager->available());
        return;
    }

    if (m_initRunning) {
        qCDebug(dcApplication()) << "Init is running";
        return;
    }

    qCDebug(dcApplication()) << "Networkmanager is now available.";
    m_bluetoothServer->onNetworkManagerAvailableChanged(available);

    switch (m_mode) {
    case ModeAlways:
        qCDebug(dcApplication()) << "Start the bluetooth service because of \"always\" mode.";
        startService();
        break;
    case ModeStart:
        break;
    case ModeOffline:
        evaluateNetworkManagerState(m_networkManager->state());
        break;
    case ModeOnce:
        if (m_networkManager->networkSettings()->connections().isEmpty()) {
            qCDebug(dcApplication()) << "Start the bluetooth service because of \"once\" mode and there is currenlty no network configured yet.";
            startService();
        } else {
            qCDebug(dcApplication()) << "Not starting the bluetooth service because of \"once\" mode. There are" << m_networkManager->networkSettings()->connections().count() << "network configurations.";
        }
        break;
    }
}

void Core::onNetworkManagerNetworkingEnabledChanged(bool enabled)
{
    qCDebug(dcApplication()) << "Networkmanager networking is now" << (enabled ? "enabled" : "disabled");
    m_bluetoothServer->onNetworkingEnabledChanged(enabled);
    evaluateNetworkManagerState(m_networkManager->state());
}

void Core::onNetworkManagerWirelessEnabledChanged(bool enabled)
{
    qCDebug(dcApplication()) << "Networkmanager wireless networking is now" << (enabled ? "enabled" : "disabled");
    m_bluetoothServer->onWirelessNetworkingEnabledChanged(enabled);
    evaluateNetworkManagerState(m_networkManager->state());
}

void Core::onNetworkManagerStateChanged(const NetworkManager::NetworkManagerState &state)
{
    qCDebug(dcApplication()) << state;
    if (!m_bluetoothServer)
        return;

    m_bluetoothServer->onNetworkManagerStateChanged(state);
    evaluateNetworkManagerState(state);
}

void Core::onNetworkManagerWirelessDeviceAdded(WirelessNetworkDevice *wirelessDevice)
{
    if (m_wirelessDevice) {
        // We already have a wireless device
        return;
    }

    m_wirelessDevice = wirelessDevice;
    connect(m_wirelessDevice, &WirelessNetworkDevice::stateChanged, this, &Core::onWirelessDeviceStateChanged);
    connect(m_wirelessDevice, &WirelessNetworkDevice::modeChanged, this, &Core::onWirelessDeviceModeChanged);
}

void Core::onNetworkManagerWirelessDeviceRemoved(const QString &interface)
{
    if (!m_wirelessDevice) {
        // Have no wireless device...
        return;
    }

    if (m_wirelessDevice->interface() == interface) {
        disconnect(m_wirelessDevice, &WirelessNetworkDevice::stateChanged, this, &Core::onWirelessDeviceStateChanged);
        m_wirelessDevice = nullptr;
    }
}

void Core::onWirelessDeviceBitRateChanged(int bitRate)
{
    qCDebug(dcApplication()) << "Wireless device changed bitrate" << bitRate;
    m_bluetoothServer->onWirelessDeviceBitRateChanged(bitRate);
}

void Core::onWirelessDeviceModeChanged(WirelessNetworkDevice::Mode mode)
{
    qCDebug(dcApplication()) << "Wireless device mode" << mode;

    // TODO: check what to do if in ap mode

    m_bluetoothServer->onWirelessDeviceModeChanged(mode);
}

void Core::onWirelessDeviceStateChanged(const NetworkDevice::NetworkDeviceState state)
{
    qCDebug(dcApplication()) << state;
    m_bluetoothServer->onWirelessDeviceStateChanged(state);
}

void Core::onNymeaServiceAvailableChanged(bool available)
{
    if (available)
        m_nymeaService->enableBluetooth(!m_bluetoothServer->running());
}
