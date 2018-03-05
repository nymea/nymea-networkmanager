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

#include "bluetoothserver.h"
#include "loggingcategory.h"
#include "loopd.h"
#include "bluetoothuuids.h"

#include <QJsonDocument>
#include <QCoreApplication>

BluetoothServer::BluetoothServer(const QString &machineId, QObject *parent) :
    QObject(parent),
    m_machineId(machineId)
{
    m_advertisingTimer = new QTimer(this);
    m_advertisingTimer->setInterval(60000);
    m_advertisingTimer->setSingleShot(true);

    connect(m_advertisingTimer, &QTimer::timeout, this, &BluetoothServer::onAdvertisingTimeout);
}

BluetoothServer::~BluetoothServer()
{
    qCDebug(dcLoopd()) << "Destroy bluetooth server.";
    if (m_controller)
        m_controller->stopAdvertising();

    if (m_localDevice)
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);

}

bool BluetoothServer::running() const
{
    return m_running;
}

bool BluetoothServer::connected() const
{
    return m_connected;
}

QLowEnergyServiceData BluetoothServer::deviceInformationServiceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::DeviceInformation);

    // Model number string 0x2a24
    QLowEnergyCharacteristicData modelNumberCharData;
    modelNumberCharData.setUuid(QBluetoothUuid::ModelNumberString);
    if (m_machineId.isEmpty()) {
        modelNumberCharData.setValue(QString("N.A.").toUtf8());
    } else {
        modelNumberCharData.setValue(m_machineId.toUtf8());
    }

    modelNumberCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(modelNumberCharData);

    // Firmware revision string 0x2a26
    QLowEnergyCharacteristicData firmwareRevisionCharData;
    firmwareRevisionCharData.setUuid(QBluetoothUuid::FirmwareRevisionString);
    firmwareRevisionCharData.setValue(QString("1.0.0").toUtf8());
    firmwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(firmwareRevisionCharData);

    // Hardware revision string 0x2a27
    QLowEnergyCharacteristicData hardwareRevisionCharData;
    hardwareRevisionCharData.setUuid(QBluetoothUuid::HardwareRevisionString);
    hardwareRevisionCharData.setValue(QString("1.0.0").toUtf8());
    hardwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(hardwareRevisionCharData);

#ifdef SNAPPY
    // Software revision string 0x2a28
    QLowEnergyCharacteristicData softwareRevisionCharData;
    softwareRevisionCharData.setUuid(QBluetoothUuid::SoftwareRevisionString);
    softwareRevisionCharData.setValue(qgetenv("SNAP_VERSION"));
    softwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(softwareRevisionCharData);
#else
    // Software revision string 0x2a28
    QLowEnergyCharacteristicData softwareRevisionCharData;
    softwareRevisionCharData.setUuid(QBluetoothUuid::SoftwareRevisionString);
    softwareRevisionCharData.setValue(QCoreApplication::applicationVersion().toUtf8());
    softwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(softwareRevisionCharData);
#endif

    // Manufacturer name string 0x2a29
    QLowEnergyCharacteristicData manufacturerNameCharData;
    manufacturerNameCharData.setUuid(QBluetoothUuid::ManufacturerNameString);
    manufacturerNameCharData.setValue(QString("guh GmbH").toUtf8());
    manufacturerNameCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(manufacturerNameCharData);

    return serviceData;
}

QLowEnergyServiceData BluetoothServer::genericAccessServiceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::GenericAccess);

    // Device name 0x2a00
    QLowEnergyCharacteristicData nameCharData;
    nameCharData.setUuid(QBluetoothUuid::DeviceName);
    nameCharData.setValue(QString("Loop-box").toUtf8());
    nameCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(nameCharData);

    // Appearance 0x2a01
    QLowEnergyCharacteristicData appearanceCharData;
    appearanceCharData.setUuid(QBluetoothUuid::Appearance);
    appearanceCharData.setValue(QByteArray(4, 0));
    appearanceCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(appearanceCharData);

    // Peripheral Privacy Flag 0x2a02
    QLowEnergyCharacteristicData privacyFlagCharData;
    privacyFlagCharData.setUuid(QBluetoothUuid::PeripheralPrivacyFlag);
    privacyFlagCharData.setValue(QByteArray(2, 0));
    privacyFlagCharData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Write);
    serviceData.addCharacteristic(privacyFlagCharData);

    // Reconnection Address 0x2a03
    QLowEnergyCharacteristicData reconnectionAddressCharData;
    reconnectionAddressCharData.setUuid(QBluetoothUuid::ReconnectionAddress);
    reconnectionAddressCharData.setValue(QByteArray());
    reconnectionAddressCharData.setProperties(QLowEnergyCharacteristic::Write);
    serviceData.addCharacteristic(reconnectionAddressCharData);

    return serviceData;
}

QLowEnergyServiceData BluetoothServer::genericAttributeServiceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::GenericAttribute);

    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid::ServiceChanged);
    charData.setProperties(QLowEnergyCharacteristic::Indicate);

    serviceData.addCharacteristic(charData);

    return serviceData;
}

void BluetoothServer::setRunning(const bool &running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(m_running);
}

void BluetoothServer::setConnected(const bool &connected)
{
    if (m_connected == connected)
        return;

    m_connected = connected;
    emit connectedChanged(m_connected);
}

void BluetoothServer::onHostModeStateChanged(const QBluetoothLocalDevice::HostMode mode)
{
    switch (mode) {
    case QBluetoothLocalDevice::HostConnectable:
        qCDebug(dcBluetoothServer()) << "Bluetooth host in connectable mode.";
        break;
    case QBluetoothLocalDevice::HostDiscoverable:
        qCDebug(dcBluetoothServer()) << "Bluetooth host in discoverable mode.";
        break;
    case QBluetoothLocalDevice::HostPoweredOff:
        qCDebug(dcBluetoothServer()) << "Bluetooth host in power off mode.";
        break;
    case QBluetoothLocalDevice::HostDiscoverableLimitedInquiry:
        qCDebug(dcBluetoothServer()) << "Bluetooth host in discoverable limited inquiry mode.";
        break;
    default:
        break;
    }
}

void BluetoothServer::onDeviceConnected(const QBluetoothAddress &address)
{
    qCDebug(dcBluetoothServer()) << "Device connected" << address.toString();
}

void BluetoothServer::onDeviceDisconnected(const QBluetoothAddress &address)
{
    qCDebug(dcBluetoothServer()) << "Device disconnected" << address.toString();
}

void BluetoothServer::onError(const QLowEnergyController::Error &error)
{
    qCWarning(dcBluetoothServer()) << "Bluetooth error occured:" << error << m_controller->errorString();
    if (error == QLowEnergyController::AdvertisingError) {
        qCWarning(dcBluetoothServer()) << "Advertising error: enable error mode";
        Loopd::instance()->hardwareManager()->ledController()->setState(LedController::LedStateBlinkingFast);
    }
}

void BluetoothServer::onConnected()
{
    qCDebug(dcBluetoothServer()) << "Client connected" << m_controller->remoteName() << m_controller->remoteAddress();
    setConnected(true);
}

void BluetoothServer::onDisconnected()
{
    qCDebug(dcBluetoothServer()) << "Client disconnected";
    setConnected(false);
    stop();
}

void BluetoothServer::onControllerStateChanged(const QLowEnergyController::ControllerState &state)
{
    switch (state) {
    case QLowEnergyController::UnconnectedState:
        qCDebug(dcBluetoothServer()) << "Controller state disonnected.";
        setConnected(false);
        break;
    case QLowEnergyController::ConnectingState:
        qCDebug(dcBluetoothServer()) << "Controller state connecting...";
        setConnected(false);
        m_advertisingTimer->stop();
        break;
    case QLowEnergyController::ConnectedState:
        qCDebug(dcBluetoothServer()) << "Controller state connected." << m_controller->remoteName() << m_controller->remoteAddress();
        setConnected(true);
        m_advertisingTimer->stop();
        break;
    case QLowEnergyController::DiscoveringState:
        qCDebug(dcBluetoothServer()) << "Controller state discovering...";
        break;
    case QLowEnergyController::DiscoveredState:
        qCDebug(dcBluetoothServer()) << "Controller state discovered.";
        break;
    case QLowEnergyController::ClosingState:
        qCDebug(dcBluetoothServer()) << "Controller state closing...";
        break;
    case QLowEnergyController::AdvertisingState:
        qCDebug(dcBluetoothServer()) << "Controller state advertising...";
        setRunning(true);
        break;
    default:
        break;
    }
}

void BluetoothServer::onAdvertisingTimeout()
{
    qCDebug(dcBluetoothServer()) << "Advertising timeout.";
    stop();
}

void BluetoothServer::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "Service characteristic changed" << characteristic.uuid() << value;
}

void BluetoothServer::characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "Service characteristic read" << characteristic.uuid() << value;
}

void BluetoothServer::characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "Service characteristic written" << characteristic.uuid() << value;
}

void BluetoothServer::descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "Descriptor read" << descriptor.uuid() << value;
}

void BluetoothServer::descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "Descriptor written" << descriptor.uuid() << value;
}

void BluetoothServer::serviceError(const QLowEnergyService::ServiceError &error)
{
    QString errorString;
    switch (error) {
    case QLowEnergyService::NoError:
        errorString = "No error";
        break;
    case QLowEnergyService::OperationError:
        errorString = "Operation error";
        break;
    case QLowEnergyService::CharacteristicReadError:
        errorString = "Characteristic read error";
        break;
    case QLowEnergyService::CharacteristicWriteError:
        errorString = "Characteristic write error";
        break;
    case QLowEnergyService::DescriptorReadError:
        errorString = "Descriptor read error";
        break;
    case QLowEnergyService::DescriptorWriteError:
        errorString = "Descriptor write error";
        break;
    case QLowEnergyService::UnknownError:
        errorString = "Unknown error";
        break;
    default:
        errorString = "Unhandled error";
        break;
    }

    qCWarning(dcBluetoothServer()) << "Service error:" << errorString;
}

void BluetoothServer::start()
{
    // Check if a user is connected
    if (connected()) {
        qCDebug(dcBluetoothServer()) << "User is connected. Doing nothing.";
        return;
    }

    if (running()) {
        qCDebug(dcBluetoothServer()) << "Already running. Restart advertise timer." << Loopd::instance()->advertisingTimeout() << "[s].";
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        m_advertisingTimer->stop();
        m_advertisingTimer->setSingleShot(true);
        m_advertisingTimer->setInterval(Loopd::instance()->advertisingTimeout() * 1000);
        m_advertisingTimer->start();
        return;
    }

    qCDebug(dcBluetoothServer()) << "-------------------------------------";
    qCDebug(dcBluetoothServer()) << "Starting bluetooth server for" << Loopd::instance()->advertisingTimeout() << "[s].";
    m_advertisingTimer->stop();
    m_advertisingTimer->setSingleShot(true);
    m_advertisingTimer->setInterval(Loopd::instance()->advertisingTimeout() * 1000);
    qCDebug(dcBluetoothServer()) << "-------------------------------------";

    // Local bluetooth device
    m_localDevice = new QBluetoothLocalDevice(this);
    if (!m_localDevice->isValid()) {
        qCCritical(dcBluetoothServer()) << "Local bluetooth device is not valid.";
        m_advertisingTimer->stop();
        delete m_localDevice;
        m_localDevice = nullptr;
        return;
    }

    connect(m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BluetoothServer::onHostModeStateChanged);
    connect(m_localDevice, &QBluetoothLocalDevice::deviceConnected, this, &BluetoothServer::onDeviceConnected);
    connect(m_localDevice, &QBluetoothLocalDevice::deviceDisconnected, this, &BluetoothServer::onDeviceDisconnected);

    qCDebug(dcBluetoothServer()) << "Local device" << m_localDevice->name() << m_localDevice->address().toString();
    m_localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    m_localDevice->powerOn();


    // Bluetooth low energy periperal controller
    m_controller = QLowEnergyController::createPeripheral(this);
    connect(m_controller, &QLowEnergyController::stateChanged, this, &BluetoothServer::onControllerStateChanged);
    connect(m_controller, &QLowEnergyController::connected, this, &BluetoothServer::onConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BluetoothServer::onDisconnected);
    connect(m_controller, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(onError(QLowEnergyController::Error)));

    // Note: https://www.bluetooth.com/specifications/gatt/services
    m_deviceInfoService = m_controller->addService(deviceInformationServiceData(), m_controller);
    m_genericAccessService = m_controller->addService(genericAccessServiceData(), m_controller);
    m_genericAttributeService = m_controller->addService(genericAttributeServiceData(), m_controller);

    // Create services
    m_networkService = new NetworkService(m_controller->addService(NetworkService::serviceData(), m_controller), m_controller);
    m_wirelessService = new WirelessService(m_controller->addService(WirelessService::serviceData(), m_controller), m_controller);
    m_systemService = new SystemService(m_controller->addService(SystemService::serviceData(), m_controller), m_controller);

    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setIncludePowerLevel(true);
    advertisingData.setLocalName("Loop-box");
    // TODO: set guh manufacturer SIG data


    // Note: start advertising in 100 ms interval, this makes the device better discoverable on certain phones
    QLowEnergyAdvertisingParameters advertisingParameters;
    advertisingParameters.setInterval(100,100);

    qCDebug(dcBluetoothServer()) << "Start advertising loopd" << m_localDevice->address().toString();
    m_controller->startAdvertising(advertisingParameters, advertisingData, advertisingData);

    // Start the advertising timer
    m_advertisingTimer->start();
    setRunning(true);
}

void BluetoothServer::stop()
{
    if (connected() && m_controller) {
        m_controller->disconnectFromDevice();
        return;
    }

    qCDebug(dcBluetoothServer()) << "-------------------------------------";
    qCDebug(dcBluetoothServer()) << "Stopping bluetooth server.";
    qCDebug(dcBluetoothServer()) << "-------------------------------------";

    if (m_controller) {
        qCDebug(dcBluetoothServer()) << "Stop advertising.";
        m_controller->stopAdvertising();
        delete m_controller;
        m_controller = nullptr;
    }

    if (m_localDevice) {
        qCDebug(dcBluetoothServer()) << "Set host mode to connectable.";
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);
        delete m_localDevice;
        m_localDevice = nullptr;
    }

    setConnected(false);
    setRunning(false);
}

