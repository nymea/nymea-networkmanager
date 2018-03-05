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

#include "networkservice.h"
#include "bluetoothuuids.h"
#include "loggingcategory.h"
#include "loopd.h"

#include <QLowEnergyDescriptorData>
#include <QLowEnergyCharacteristicData>

NetworkService::NetworkService(QLowEnergyService *service, QObject *parent) :
    QObject(parent),
    m_service(service)
{
    qCDebug(dcBluetoothServer()) << "Create NetworkService.";

    // Service
    connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic, QByteArray)), this, SLOT(characteristicChanged(QLowEnergyCharacteristic, QByteArray)));
    connect(m_service, SIGNAL(characteristicRead(QLowEnergyCharacteristic, QByteArray)), this, SLOT(characteristicChanged(QLowEnergyCharacteristic, QByteArray)));
    connect(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic, QByteArray)), this, SLOT(characteristicWritten(QLowEnergyCharacteristic, QByteArray)));
    connect(m_service, SIGNAL(descriptorWritten(QLowEnergyDescriptor, QByteArray)), this, SLOT(descriptorWritten(QLowEnergyDescriptor, QByteArray)));
    connect(m_service, SIGNAL(error(QLowEnergyService::ServiceError)), this, SLOT(serviceError(QLowEnergyService::ServiceError)));

    // NetworkManager
    connect(Loopd::instance()->networkManager(), &NetworkManager::stateChanged, this, &NetworkService::onNetworkManagerStateChanged);
    connect(Loopd::instance()->networkManager(), &NetworkManager::networkingEnabledChanged, this, &NetworkService::onNetworkingEnabledChanged);
    connect(Loopd::instance()->networkManager(), &NetworkManager::wirelessEnabledChanged, this, &NetworkService::onWirelessEnabledChanged);
}

QLowEnergyService *NetworkService::service()
{
    return m_service;
}

QLowEnergyServiceData NetworkService::serviceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(networkServiceUuid);

    QLowEnergyDescriptorData clientConfigDescriptorData(QBluetoothUuid::ClientCharacteristicConfiguration, QByteArray(2, 0));

    // Network manager status ef6d661-b8af-49e0-9eca-ab343513641c
    QLowEnergyCharacteristicData networkStatusData;
    networkStatusData.setUuid(networkStatusCharacteristicUuid);
    networkStatusData.setValue(QByteArray(1, 0));
    networkStatusData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Notify);
    networkStatusData.addDescriptor(clientConfigDescriptorData);
    networkStatusData.setValue(NetworkService::getNetworkManagerStateByteArray(Loopd::instance()->networkManager()->state()));
    serviceData.addCharacteristic(networkStatusData);

    // Network manager commander ef6d6612-b8af-49e0-9eca-ab343513641c
    QLowEnergyCharacteristicData networkCommanderCharacteristicData;
    networkCommanderCharacteristicData.setUuid(networkCommanderCharacteristicUuid);
    networkCommanderCharacteristicData.setProperties(QLowEnergyCharacteristic::Write);
    networkCommanderCharacteristicData.setValueLength(1, 1);
    serviceData.addCharacteristic(networkCommanderCharacteristicData);

    // Response characteristic ef6d6613-b8af-49e0-9eca-ab343513641c
    QLowEnergyCharacteristicData networkResponseCharacteristicData;
    networkResponseCharacteristicData.setUuid(networkResponseCharacteristicUuid);
    networkResponseCharacteristicData.setProperties(QLowEnergyCharacteristic::Notify);
    networkResponseCharacteristicData.addDescriptor(clientConfigDescriptorData);
    networkResponseCharacteristicData.setValueLength(1, 1);
    serviceData.addCharacteristic(networkResponseCharacteristicData);

    // Networking enabled ef6d6614-b8af-49e0-9eca-ab343513641c
    QLowEnergyCharacteristicData networkingEnabledStatusData;
    networkingEnabledStatusData.setUuid(networkingEnabledCharacteristicUuid);
    networkingEnabledStatusData.setValue(QByteArray(1, 0));
    networkingEnabledStatusData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Notify);
    networkingEnabledStatusData.setValue(QByteArray::number((int)Loopd::instance()->networkManager()->networkingEnabled()));
    serviceData.addCharacteristic(networkingEnabledStatusData);

    // Wireless enabled ef6d6615-b8af-49e0-9eca-ab343513641c
    QLowEnergyCharacteristicData wirelessEnabledStatusData;
    wirelessEnabledStatusData.setUuid(wirelessEnabledCharacteristicUuid);
    wirelessEnabledStatusData.setValue(QByteArray(1, 0));
    wirelessEnabledStatusData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Notify);
    wirelessEnabledStatusData.setValue(QByteArray::number((int)Loopd::instance()->networkManager()->wirelessEnabled()));
    serviceData.addCharacteristic(wirelessEnabledStatusData);

    return serviceData;
}

QByteArray NetworkService::getNetworkManagerStateByteArray(const NetworkManager::NetworkManagerState &state)
{
    QByteArray networkManagerState;
    switch (state) {
    case NetworkManager::NetworkManagerStateUnknown:
        networkManagerState = QByteArray::fromHex("00");
        break;
    case NetworkManager::NetworkManagerStateAsleep:
        networkManagerState = QByteArray::fromHex("01");
        break;
    case NetworkManager::NetworkManagerStateDisconnected:
        networkManagerState = QByteArray::fromHex("02");
        break;
    case NetworkManager::NetworkManagerStateDisconnecting:
        networkManagerState = QByteArray::fromHex("03");
        break;
    case NetworkManager::NetworkManagerStateConnecting:
        networkManagerState = QByteArray::fromHex("04");
        break;
    case NetworkManager::NetworkManagerStateConnectedLocal:
        networkManagerState = QByteArray::fromHex("05");
        break;
    case NetworkManager::NetworkManagerStateConnectedSite:
        networkManagerState = QByteArray::fromHex("06");
        break;
    case NetworkManager::NetworkManagerStateConnectedGlobal:
        networkManagerState = QByteArray::fromHex("07");
        break;
    }

    return networkManagerState;
}

void NetworkService::sendResponse(const NetworkService::NetworkServiceResponse &response)
{
    if (!m_service) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not send response. Service not valid";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_service->characteristic(networkResponseCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not send response. Characteristic not valid";
        return;
    }

    switch (response) {
    case NetworkServiceResponseSuccess:
        m_service->writeCharacteristic(characteristic, QByteArray::fromHex("00"));
        break;
    case NetworkServiceResponseIvalidValue:
        m_service->writeCharacteristic(characteristic, QByteArray::fromHex("01"));
        break;
    case NetworkServiceResponseNetworkManagerNotAvailable:
        m_service->writeCharacteristic(characteristic, QByteArray::fromHex("02"));
        break;
    case NetworkServiceResponseWirelessNotAvailable:
        m_service->writeCharacteristic(characteristic, QByteArray::fromHex("03"));
        break;
    default:
        // Unknown error
        m_service->writeCharacteristic(characteristic, QByteArray::fromHex("04"));
        break;
    }
}

NetworkService::NetworkServiceCommand NetworkService::verifyCommand(const QByteArray &commandData)
{
    if (commandData.length() != 1)
        return NetworkServiceCommandInvalid;

    uint commandInteger = commandData.toHex().toUInt(0, 16);
    switch (commandInteger) {
    case NetworkServiceCommandEnableNetworking:
        return NetworkServiceCommandEnableNetworking;
    case NetworkServiceCommandDisableNetworking:
        return NetworkServiceCommandDisableNetworking;
    case NetworkServiceCommandEnableWireless:
        return NetworkServiceCommandEnableWireless;
    case NetworkServiceCommandDisableWireless:
        return NetworkServiceCommandDisableWireless;
    default:
        break;
    }

    return NetworkServiceCommandInvalid;
}

void NetworkService::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if (characteristic.uuid() == networkCommanderCharacteristicUuid) {

        NetworkServiceCommand command = verifyCommand(value);
        if (command == NetworkServiceCommandInvalid) {
            qCWarning(dcBluetoothServer()) << "NetworkService: received invalid command" << command;
            sendResponse(NetworkServiceResponseIvalidValue);
            return;
        }

        if (!Loopd::instance()->networkManager()->available()) {
            qCWarning(dcBluetoothServer()) << "NetworkService: Networkmanager not available";
            sendResponse(NetworkServiceResponseNetworkManagerNotAvailable);
            return;
        }

        processCommand(command);

        sendResponse(NetworkServiceResponseSuccess);
        return;
    }

    qCDebug(dcBluetoothServer()) << "NetworkService: Characteristic changed" << characteristic.uuid().toString() << value;
}

void NetworkService::characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "NetworkService: Characteristic read" << characteristic.uuid().toString() << value;
}

void NetworkService::characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "NetworkService: Characteristic written" << characteristic.uuid().toString() << value;
}

void NetworkService::descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "NetworkService: Descriptor read" << descriptor.uuid().toString() << value;
}

void NetworkService::descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcBluetoothServer()) << "NetworkService: Descriptor written" << descriptor.uuid().toString() << value;
}

void NetworkService::serviceError(const QLowEnergyService::ServiceError &error)
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

    qCWarning(dcBluetoothServer()) << "NetworkService: Error:" << errorString;
}

void NetworkService::processCommand(const NetworkServiceCommand &command)
{
    switch (command) {
    case NetworkServiceCommandEnableNetworking:
        qCDebug(dcBluetoothServer()) << "NetworkService: received \"Enable networking\" command";
        Loopd::instance()->networkManager()->enableNetworking(true);
        break;
    case NetworkServiceCommandDisableNetworking:
        qCDebug(dcBluetoothServer()) << "NetworkService: received \"Disable networking\" command";
        Loopd::instance()->networkManager()->enableNetworking(false);
        break;
    case NetworkServiceCommandEnableWireless:
        qCDebug(dcBluetoothServer()) << "NetworkService: received \"Enable wireless networking\" command";
        Loopd::instance()->networkManager()->enableWireless(true);
        break;
    case NetworkServiceCommandDisableWireless:
        qCDebug(dcBluetoothServer()) << "NetworkService: received \"Disable wireless networking\" command";
        Loopd::instance()->networkManager()->enableWireless(false);
        break;
    default:
        qCWarning(dcBluetoothServer()) << "NetworkService: Unhandled command" << command;
        sendResponse(NetworkServiceResponseIvalidValue);
        break;
    }
}

bool NetworkService::onNetworkManagerStateChanged()
{
    if (!m_service) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not updatet network manager status. Service not valid";
        return false;
    }

    QLowEnergyCharacteristic characteristic = m_service->characteristic(networkStatusCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not update network manager status. Characteristic not valid";
        return false;
    }

    qCDebug(dcBluetoothServer()) << "NetworkService: Notify state changed" << NetworkService::getNetworkManagerStateByteArray(Loopd::instance()->networkManager()->state());
    m_service->writeCharacteristic(characteristic, NetworkService::getNetworkManagerStateByteArray(Loopd::instance()->networkManager()->state()));
    return true;
}

bool NetworkService::onNetworkingEnabledChanged()
{
    if (!m_service) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not set networking enabled. Service not valid";
        return false;
    }

    QLowEnergyCharacteristic characteristic = m_service->characteristic(networkingEnabledCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not set networking enabled. Characteristic not valid";
        return false;
    }

    qCDebug(dcBluetoothServer()) << "NetworkService: Notify networking enabled changed:" << (Loopd::instance()->networkManager()->networkingEnabled() ? "enabled" : "disabled");
    m_service->writeCharacteristic(characteristic, Loopd::instance()->networkManager()->networkingEnabled() ? QByteArray::fromHex("01") : QByteArray::fromHex("00"));
    return true;
}

bool NetworkService::onWirelessEnabledChanged()
{
    if (!m_service) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not set wireless enabled. Service not valid";
        return false;
    }

    QLowEnergyCharacteristic characteristic = m_service->characteristic(wirelessEnabledCharacteristicUuid);
    if (!characteristic.isValid()) {
        qCWarning(dcBluetoothServer()) << "NetworkService: Could not set wireless enabled. Characteristic not valid";
        return false;
    }

    qCDebug(dcBluetoothServer()) << "NetworkService: Notify wireless networking enabled changed:" << (Loopd::instance()->networkManager()->wirelessEnabled() ? "enabled" : "disabled");
    m_service->writeCharacteristic(characteristic, Loopd::instance()->networkManager()->wirelessEnabled() ? QByteArray::fromHex("01") : QByteArray::fromHex("00"));
    return true;
}
