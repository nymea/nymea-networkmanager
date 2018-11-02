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

/*! \fn void WirelessNetworkDevice::bitRateChanged(const int &bitRate);
    This signal will be emitted when the \a bitRate of this \l{WirelessNetworkDevice} has changed.
*/

/*! \fn void WirelessNetworkDevice::stateChanged(const NetworkDeviceState &state);
    This signal will be emitted when the current \a state of this \l{WirelessNetworkDevice} has changed.

    \sa NetworkDeviceState
*/


#include "networkmanagerutils.h"
#include "wirelessnetworkdevice.h"

#include <QUuid>
#include <QDebug>
#include <QMetaEnum>

/*! Constructs a new \l{WirelessNetworkDevice} with the given dbus \a objectPath and \a parent. */
WirelessNetworkDevice::WirelessNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent) :
    NetworkDevice(objectPath, parent),
    m_activeAccessPoint(nullptr)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(dcNetworkManager()) << "WirelessNetworkDevice: System DBus not connected";
        return;
    }

    m_wirelessInterface = new QDBusInterface(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), systemBus, this);
    if (!m_wirelessInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "WirelessNetworkDevice: Invalid wireless dbus interface";
        return;
    }

    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), "AccessPointAdded", this, SLOT(accessPointAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), "AccessPointRemoved", this, SLOT(accessPointRemoved(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), "PropertiesChanged", this, SLOT(propertiesChanged(QVariantMap)));

    readAccessPoints();

    setMacAddress(m_wirelessInterface->property("HwAddress").toString());
    setMode(static_cast<Mode>(m_wirelessInterface->property("Mode").toUInt()));
    setBitrate(m_wirelessInterface->property("Bitrate").toInt());
    setActiveAccessPoint(qdbus_cast<QDBusObjectPath>(m_wirelessInterface->property("ActiveAccessPoint")));
}

/*! Returns the mac address of this \l{WirelessNetworkDevice}. */
QString WirelessNetworkDevice::macAddress() const
{
    return m_macAddress;
}

/*! Returns the bit rate [Mb/s] of this \l{WirelessNetworkDevice}. */
int WirelessNetworkDevice::bitRate() const
{
    return m_bitRate;
}

WirelessNetworkDevice::Mode WirelessNetworkDevice::mode() const
{
    return m_mode;
}

/*! Returns the current active \l{WirelessAccessPoint} of this \l{WirelessNetworkDevice}. */
WirelessAccessPoint *WirelessNetworkDevice::activeAccessPoint()
{
    return m_activeAccessPoint;
}

/*! Perform a wireless network scan on this \l{WirelessNetworkDevice}. */
void WirelessNetworkDevice::scanWirelessNetworks()
{
    qCDebug(dcNetworkManager()) << this << "Request scan";
    QDBusMessage query = m_wirelessInterface->call("RequestScan", QVariantMap());
    if (query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << "Scan error:" << query.errorName() << query.errorMessage();
        return;
    }
}

/*! Returns the list of all \l{WirelessAccessPoint}{WirelessAccessPoints} of this \l{WirelessNetworkDevice}. */
QList<WirelessAccessPoint *> WirelessNetworkDevice::accessPoints()
{
    return m_accessPointsTable.values();
}

/*! Returns the \l{WirelessAccessPoint} with the given \a ssid. If the \l{WirelessAccessPoint} could not be found, return nullptr. */
WirelessAccessPoint *WirelessNetworkDevice::getAccessPoint(const QString &ssid)
{
    foreach (WirelessAccessPoint *accessPoint, m_accessPointsTable.values()) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }
    return nullptr;
}

/*! Returns the \l{WirelessAccessPoint} with the given \a objectPath. If the \l{WirelessAccessPoint} could not be found, return nullptr. */
WirelessAccessPoint *WirelessNetworkDevice::getAccessPoint(const QDBusObjectPath &objectPath)
{
    return m_accessPointsTable.value(objectPath);
}

void WirelessNetworkDevice::readAccessPoints()
{
    QDBusMessage query = m_wirelessInterface->call("GetAccessPoints");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();
        return;
    }

    if (query.arguments().isEmpty())
        return;

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    argument.beginArray();
    while (!argument.atEnd()) {
        QDBusObjectPath accessPointObjectPath = qdbus_cast<QDBusObjectPath>(argument);
        accessPointAdded(accessPointObjectPath);
    }
    argument.endArray();
}

void WirelessNetworkDevice::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}

void WirelessNetworkDevice::setMode(WirelessNetworkDevice::Mode mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(m_mode);
}

void WirelessNetworkDevice::setBitrate(int bitRate)
{
    if (m_bitRate != bitRate / 1000) {
        m_bitRate = bitRate / 1000;
        emit bitRateChanged(m_bitRate);
        emit deviceChanged();
    }
}

void WirelessNetworkDevice::setActiveAccessPoint(const QDBusObjectPath &activeAccessPointObjectPath)
{
    if (m_activeAccessPointObjectPath != activeAccessPointObjectPath) {
        m_activeAccessPointObjectPath = activeAccessPointObjectPath;
        if (m_accessPointsTable.contains(m_activeAccessPointObjectPath)) {
            if (m_activeAccessPoint)
                disconnect(m_activeAccessPoint, &WirelessAccessPoint::signalStrengthChanged, this, &WirelessNetworkDevice::deviceChanged);

            // Set new access point object
            m_activeAccessPoint = m_accessPointsTable.value(activeAccessPointObjectPath);
            // Update the device when the signalstrength changed
            connect(m_activeAccessPoint, &WirelessAccessPoint::signalStrengthChanged, this, &WirelessNetworkDevice::deviceChanged);
        } else {
            m_activeAccessPoint = nullptr;
        }
        emit deviceChanged();
    }
}

void WirelessNetworkDevice::accessPointAdded(const QDBusObjectPath &objectPath)
{
    QDBusInterface accessPointInterface(NetworkManagerUtils::networkManagerServiceString(), objectPath.path(), NetworkManagerUtils::accessPointInterfaceString(), QDBusConnection::systemBus());
    if (!accessPointInterface.isValid()) {
        qCWarning(dcNetworkManager()) << "WirelessNetworkDevice: Invalid access point dbus interface";
        return;
    }

    if (m_accessPointsTable.keys().contains(objectPath)) {
        qCWarning(dcNetworkManager()) << "WirelessNetworkDevice: Access point already added" << objectPath.path();
        return;
    }

    WirelessAccessPoint *accessPoint = new WirelessAccessPoint(objectPath, this);
    //qCDebug(dcNetworkManager()) << "WirelessNetworkDevice: [+]" << accessPoint;
    m_accessPointsTable.insert(objectPath, accessPoint);
}

void WirelessNetworkDevice::accessPointRemoved(const QDBusObjectPath &objectPath)
{
    if (!m_accessPointsTable.keys().contains(objectPath))
        return;

    WirelessAccessPoint *accessPoint = m_accessPointsTable.take(objectPath);
    if (accessPoint == m_activeAccessPoint)
        m_activeAccessPoint = nullptr;

    //qCDebug(dcNetworkManager()) << "WirelessNetworkDevice: [-]" << accessPoint;
    accessPoint->deleteLater();
}

void WirelessNetworkDevice::propertiesChanged(const QVariantMap &properties)
{
    //qCDebug(dcNetworkManager()) << "WirelessNetworkDevice: Property changed" << properties;

    if (properties.contains("Bitrate"))
        setBitrate(properties.value("Bitrate").toInt());

    if (properties.contains("ActiveAccessPoint"))
        setActiveAccessPoint(qdbus_cast<QDBusObjectPath>(properties.value("ActiveAccessPoint")));

    if (properties.contains("Mode"))
        setMode(static_cast<Mode>(m_wirelessInterface->property("Mode").toUInt()));

}

/*! Writes the given \a device to the given to \a debug. \sa WirelessNetworkDevice, */
QDebug operator<<(QDebug debug, WirelessNetworkDevice *device)
{
    debug.nospace() << "WirelessNetworkDevice(" << device->interface() << ", ";
    debug.nospace() << device->macAddress() <<  ", ";
    debug.nospace() << device->mode() <<  ", ";
    debug.nospace() << device->bitRate() <<  " [Mb/s], ";
    debug.nospace() << device->deviceStateString() <<  ") ";
    return debug;
}
