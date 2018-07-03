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

#ifndef WIRELESSSERVICE_H
#define WIRELESSSERVICE_H

#include <QObject>
#include <QVariantMap>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

#include "wirelessnetworkdevice.h"
#include "wirelessaccesspoint.h"

class WirelessService : public QObject
{
    Q_OBJECT
    Q_ENUMS(WirelessServiceCommand)
    Q_ENUMS(WirelessServiceResponse)

public:

    enum WirelessServiceCommand {
        WirelessServiceCommandInvalid = -1,
        WirelessServiceCommandGetNetworks           = 0x00,
        WirelessServiceCommandConnect               = 0x01,
        WirelessServiceCommandConnectHidden         = 0x02,
        WirelessServiceCommandDisconnect            = 0x03,
        WirelessServiceCommandScan                  = 0x04,
        WirelessServiceCommandGetCurrentConnection  = 0x05
    };
    Q_ENUM(WirelessServiceCommand)

    enum WirelessServiceResponse {
        WirelessServiceResponseSuccess                     = 0x00,
        WirelessServiceResponseIvalidCommand               = 0x01,
        WirelessServiceResponseIvalidParameters            = 0x02,
        WirelessServiceResponseNetworkManagerNotAvailable  = 0x03,
        WirelessServiceResponseWirelessNotAvailable        = 0x04,
        WirelessServiceResponseWirelessNotEnabled          = 0x05,
        WirelessServiceResponseNetworkingNotEnabled        = 0x06,
        WirelessServiceResponseUnknownError                = 0x07
    };
    Q_ENUM(WirelessServiceResponse)

    explicit WirelessService(QLowEnergyService *service, QObject *parent = 0);
    ~WirelessService();

    QLowEnergyService *service();

    static QLowEnergyServiceData serviceData();

private:
    QLowEnergyService *m_service = nullptr;

    bool m_readingInputData = false;
    QByteArray m_inputDataStream;

    WirelessServiceResponse checkWirelessErrors();

    // Note: static to be available in serviceData
    static QByteArray getWirelessNetworkDeviceState(const NetworkDevice::NetworkDeviceState &state);

    void streamData(const QVariantMap &responseMap);

    QVariantMap createResponse(const WirelessServiceCommand &command, const WirelessServiceResponse &responseCode = WirelessServiceResponseSuccess);

    // Methods
    void commandGetNetworks(const QVariantMap &request);
    void commandConnect(const QVariantMap &request);
    void commandConnectHidden(const QVariantMap &request);
    void commandDisconnect(const QVariantMap &request);
    void commandScan(const QVariantMap &request);
    void commandGetCurrentConnection(const QVariantMap &request);


private slots:
    // Service
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(const QLowEnergyService::ServiceError &error);

    // Commands
    void processCommand(const QVariantMap &request);

public slots:
    // Wireless network device
    void onWirelessDeviceBitRateChanged(const int &bitRate);
    void onWirelessDeviceStateChanged(const NetworkDevice::NetworkDeviceState &state);
};

#endif // WIRELESSSERVICE_H
