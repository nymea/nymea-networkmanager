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

#ifndef WIRELESSSERVICE_H
#define WIRELESSSERVICE_H

#include <QObject>
#include <QVariantMap>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

#include "networkmanager/wirelessnetworkdevice.h"
#include "networkmanager/wirelessaccesspoint.h"

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
    QLowEnergyService *service();

    static QLowEnergyServiceData serviceData();

private:
    QLowEnergyService *m_service;
    WirelessNetworkDevice *m_device;

    bool m_readingInputData;
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

    // Wireless network device
    void onWirelessDeviceBitRateChanged(const int &bitRate);
    void onWirelessDeviceStateChanged(const NetworkDevice::NetworkDeviceState &state);
};

#endif // WIRELESSSERVICE_H
