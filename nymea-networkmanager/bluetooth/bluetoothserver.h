/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2016 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of loopd.                                                   *
 *                                                                               *
 * Loopd can not be copied and/or distributed without the express                *
 * permission of guh GmbH.                                                       *
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
    SystemService *m_systemService = nullptr;

    QTimer *m_advertisingTimer = nullptr;

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

    void onAdvertisingTimeout();

    // Services
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(const QLowEnergyService::ServiceError &error);

public slots:
    void start();
    void stop();

};

#endif // BLUETOOTHSERVER_H
