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

#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

#include "networkmanager.h"

class NetworkService : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkServiceCommand)
    Q_ENUMS(NetworkServiceResponse)

public:
    enum NetworkServiceCommand {
        NetworkServiceCommandInvalid = -1,
        NetworkServiceCommandEnableNetworking   = 0x00,
        NetworkServiceCommandDisableNetworking  = 0x01,
        NetworkServiceCommandEnableWireless     = 0x02,
        NetworkServiceCommandDisableWireless    = 0x03
    };
    Q_ENUM(NetworkServiceCommand)

    enum NetworkServiceResponse {
        NetworkServiceResponseSuccess                      = 0x00,
        NetworkServiceResponseIvalidValue                  = 0x01,
        NetworkServiceResponseNetworkManagerNotAvailable   = 0x02,
        NetworkServiceResponseWirelessNotAvailable         = 0x03,
        NetworkServiceResponseUnknownError                 = 0x04,
    };
    Q_ENUM(NetworkServiceResponse)

    explicit NetworkService(QLowEnergyService *service, QObject *parent = 0);

    QLowEnergyService *service();

    static QLowEnergyServiceData serviceData();
    static QByteArray getNetworkManagerStateByteArray(const NetworkManager::NetworkManagerState &state);

private:
    QLowEnergyService *m_service;
    void sendResponse(const NetworkServiceResponse &response);

    NetworkServiceCommand verifyCommand(const QByteArray &commandData);

private slots:
    // Service
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(const QLowEnergyService::ServiceError &error);

    // Commands
    void processCommand(const NetworkServiceCommand &command);

public slots:
    bool onNetworkManagerStateChanged();
    bool onNetworkingEnabledChanged();
    bool onWirelessEnabledChanged();

};

#endif // NETWORKSERVICE_H
