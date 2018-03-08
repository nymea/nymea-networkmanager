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

#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H

#include <QTimer>
#include <QObject>
#include <QPointer>
#include <QLowEnergyHandle>
#include <QLowEnergyService>
#include <QBluetoothLocalDevice>
#include <QLowEnergyController>
#include <QLowEnergyServiceData>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyDescriptorData>
#include <QLowEnergyAdvertisingData>
#include <QLowEnergyCharacteristicData>
#include <QLowEnergyConnectionParameters>
#include <QLowEnergyAdvertisingParameters>

#include "networkservice.h"
#include "wirelessservice.h"
#include "systemservice.h"
#include "networkmanager.h"

class BluetoothServer : public QObject
{
    Q_OBJECT
public:

    explicit BluetoothServer(const QString &machineId, QObject *parent = 0);
    ~BluetoothServer();

    bool running() const;
    bool connected() const;

private:
    QString m_machineId;
    QBluetoothLocalDevice *m_localDevice = nullptr;
    QLowEnergyController *m_controller = nullptr;

    QLowEnergyService *m_deviceInfoService = nullptr;
    QLowEnergyService *m_genericAccessService = nullptr;
    QLowEnergyService *m_genericAttributeService = nullptr;

    NetworkService *m_networkService = nullptr;
    WirelessService *m_wirelessService = nullptr;

    bool m_running = false;
    bool m_connected = false;

    QLowEnergyServiceData deviceInformationServiceData();
    QLowEnergyServiceData genericAccessServiceData();
    QLowEnergyServiceData genericAttributeServiceData();

    void setRunning(const bool &running);
    void setConnected(const bool &connected);

signals:
    void runningChanged(const bool &running);
    void connectedChanged(const bool &connected);

private slots:
    // Local bluetooth device
    void onHostModeStateChanged(const QBluetoothLocalDevice::HostMode mode);
    void onDeviceConnected(const QBluetoothAddress &address);
    void onDeviceDisconnected(const QBluetoothAddress &address);
    void onError(const QLowEnergyController::Error &error);

    // Bluetooth controller
    void onConnected();
    void onDisconnected();
    void onControllerStateChanged(const QLowEnergyController::ControllerState &state);

    // Services
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(const QLowEnergyService::ServiceError &error);

public slots:
    void start(WirelessNetworkDevice *wirelessDevice);
    void stop();

    // Network manager
    void onNetworkManagerAvailableChanged(bool available);
    void onNetworkingEnabledChanged(bool enabled);
    void onWirelessNetworkingEnabledChanged(bool enabled);
    void onNetworkManagerStateChanged(const NetworkManager::NetworkManagerState &state);

};

#endif // BLUETOOTHSERVER_H
