/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of libnymea-networkmanager.                                 *
 *                                                                               *
 * libnymea-networkmanager is free software: you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License,                *
 * or (at your option) any later version.                                        *
 *                                                                               *
 * libnymea-networkmanager is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License along       *
 * with libnymea-networkmanager. If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef WIRELESSNETWORKMANAGER_H
#define WIRELESSNETWORKMANAGER_H

#include <QObject>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDBusArgument>

#include "networkdevice.h"
#include "wirelessaccesspoint.h"

class WirelessNetworkDevice : public NetworkDevice
{
    Q_OBJECT
public:

    explicit WirelessNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent = 0);

    // Properties
    QString macAddress() const;
    int bitRate() const;
    WirelessAccessPoint *activeAccessPoint();

    // Accesspoints
    QList<WirelessAccessPoint *> accessPoints();
    WirelessAccessPoint *getAccessPoint(const QString &ssid);
    WirelessAccessPoint *getAccessPoint(const QDBusObjectPath &objectPath);

    // Methods
    void scanWirelessNetworks();

private:
    QDBusInterface *m_wirelessInterface;

    QString m_macAddress;
    int m_bitRate;
    WirelessAccessPoint *m_activeAccessPoint;
    QDBusObjectPath m_activeAccessPointObjectPath;

    QHash<QDBusObjectPath, WirelessAccessPoint *> m_accessPointsTable;

    void readAccessPoints();

    void setMacAddress(const QString &macAddress);
    void setBitrate(const int &bitRate);
    void setActiveAccessPoint(const QDBusObjectPath &activeAccessPointObjectPath);

private slots:
    void accessPointAdded(const QDBusObjectPath &objectPath);
    void accessPointRemoved(const QDBusObjectPath &objectPath);
    void propertiesChanged(const QVariantMap &properties);

signals:
    void bitRateChanged(const int &bitRate);

};

QDebug operator<<(QDebug debug, WirelessNetworkDevice *device);

#endif // WIRELESSNETWORKMANAGER_H
