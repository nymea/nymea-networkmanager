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

void Core::setMode(Mode mode)
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

void Core::setAdvertisingTimeout(int advertisingTimeout)
{
    m_advertisingTimeout = advertisingTimeout;
}

int Core::buttonGpio() const
{
    return m_buttonGpio;
}

void Core::setButtonGpio(int buttonGpio)
{
    m_buttonGpio = buttonGpio;
}

void Core::run()
{
    // Start the networkmanager
    if (!m_networkManager->start()) {
        qCWarning(dcApplication()) << "Could not start network manager. Please make sure the networkmanager is available.";
        return;
    }

    // Note: give network-manager more time to start and get online status
    QTimer::singleShot(3000, this, &Core::postRun);
}

Core::Core(QObject *parent) :
    QObject(parent)
{
    m_networkManager = new NetworkManager(this);
    connect(m_networkManager, &NetworkManager::availableChanged, this, &Core::onNetworkManagerAvailableChanged);
    connect(m_networkManager, &NetworkManager::stateChanged, this, &Core::onNetworkManagerStateChanged);

    m_bluetoothServer = new BluetoothServer(m_networkManager);

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

void Core::evaluateNetworkManagerState(NetworkManager::NetworkManagerState state)
{
    if (m_mode != ModeOffline)
        return;

    // If we are still initializing, we don't need to react on the state changed
    if (m_initRunning)
        return;

    // Note: if the wireless device is in the access point mode, the bluetooth server should stop
    if (m_wirelessDevice && m_wirelessDevice->mode() == WirelessNetworkDevice::ModeAccessPoint) {
        stopService();
        return;
    }

    switch (state) {
    case NetworkManager::NetworkManagerStateConnectedGlobal:
        // We are online
        qCDebug(dcApplication()) << "Not advertising bluetooth because we are online and we are running in" << m_mode;
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
        qCDebug(dcApplication()) << "Ignoring" << state;
        break;
    }
}

void Core::startService()
{
    qCDebug(dcApplication()) << "Start the service...";
    if (!m_networkManager->available()) {
        qCWarning(dcApplication()) << "Could not start services. There is no network manager available.";
        return;
    }

    // Verify if we have a wireless network available
    if (!m_networkManager->wirelessAvailable()) {
        qCWarning(dcApplication()) << "Could not start services. There is no wireless device available.";
        return;
    }

    // Disable bluetooth on nymea in order to not crash with client connections
    m_nymeaService->enableBluetooth(false);

    // Start the bluetooth server for this wireless device
    m_bluetoothServer->setAdvertiseName(m_advertiseName);
    m_bluetoothServer->setModelName(m_platformName);
    m_bluetoothServer->setSoftwareVersion(VERSION_STRING);
    m_bluetoothServer->start();
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
    case ModeButton:
        // Enable button
        m_button = new GpioButton(m_buttonGpio, this);
        m_button->setLongPressedTimeout(2000);
        connect(m_button, &GpioButton::longPressed, this, &Core::onButtonLongPressed);
        if (!m_button->enable()) {
            qCCritical(dcApplication()) << "Could not not enable GPIO button for" << m_buttonGpio;
            m_button->deleteLater();
            m_button = nullptr;
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
        m_advertisingTimer->stop();

        switch (m_mode) {
        case ModeAlways:
            qCDebug(dcApplication()) << "Restart the bluetooth service because of \"always\" mode.";
            // Give some grace periode for bluez to clean up and restart the service again
            QTimer::singleShot(3000, this, &Core::startService);
            break;
        case ModeStart:
            // Enable bluetooth on nymea
            m_nymeaService->enableBluetooth(true);
            // We are done here. The bluetooth server was already running
            break;
        case ModeOffline:
            // Enable bluetooth on nymea
            m_nymeaService->enableBluetooth(true);
            evaluateNetworkManagerState(m_networkManager->state());
            break;
        case ModeOnce:
            if (m_networkManager->networkSettings()->connections().isEmpty()) {
                qCDebug(dcApplication()) << "Start the bluetooth service because of \"once\" mode and there is currenlty no network configured yet.";
                startService();
            } else {
                // Enable bluetooth on nymea
                m_nymeaService->enableBluetooth(true);
                qCDebug(dcApplication()) << "Not starting the bluetooth service because of \"once\" mode. There are" << m_networkManager->networkSettings()->connections().count() << "network configurations.";
            }
            break;
        case ModeButton:
            break;
        }
    }
}

void Core::onBluetoothServerConnectedChanged(bool connected)
{
    qCDebug(dcApplication()) << "Bluetooth client" << (connected ? "connected" : "disconnected");
    m_advertisingTimer->stop();

    if (!connected) {
        m_advertisingTimer->stop();
        m_bluetoothServer->stop();
    }
}

void Core::onNetworkManagerAvailableChanged(bool available)
{
    if (!available) {
        qCWarning(dcApplication()) << "Networkmanager is not available any more.";
        return;
    }

    qCDebug(dcApplication()) << "Networkmanager is now available.";

    if (m_initRunning) {
        qCDebug(dcApplication()) << "Init is still running...";
        return;
    }

    switch (m_mode) {
    case ModeAlways:
        qCDebug(dcApplication()) << "Start the bluetooth service because of \"always\" mode.";
        // Give some grace periode for networkmanager
        QTimer::singleShot(4000, this, &Core::startService);
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
    case ModeButton:
        break;
    }
}

void Core::onNetworkManagerStateChanged(NetworkManager::NetworkManagerState state)
{
    evaluateNetworkManagerState(state);
}

void Core::onButtonLongPressed()
{
    startService();
}

void Core::onNymeaServiceAvailableChanged(bool available)
{
    if (available)
        m_nymeaService->enableBluetooth(!m_bluetoothServer->running());
}
