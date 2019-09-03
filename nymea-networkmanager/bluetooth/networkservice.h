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

#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

#include "libnymea-networkmanager/networkmanager.h"

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

    explicit NetworkService(QLowEnergyService *service, QObject *parent = nullptr);
    ~NetworkService();

    QLowEnergyService *service();

    void setNetworkManagerAvailable(bool available);
    void setNetworkManagerState(const NetworkManager::NetworkManagerState &state);
    void setNetworkingEnabled(bool enabled);
    void setWirelessNetworkingEnabled(bool enabled);

    static QLowEnergyServiceData serviceData();
    static QByteArray getNetworkManagerStateByteArray(const NetworkManager::NetworkManagerState &state);

private:
    QLowEnergyService *m_service = nullptr;

    bool m_networkManagerAvailable = false;
    NetworkManager::NetworkManagerState m_state = NetworkManager::NetworkManagerStateUnknown;
    bool m_networkingEnabled = false;
    bool m_wirelessNetworkingEnabled = false;

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

};

#endif // NETWORKSERVICE_H
