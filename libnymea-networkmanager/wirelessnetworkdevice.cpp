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

/*! \fn void WirelessNetworkDevice::bitRateChanged(const int &bitRate);
    This signal will be emitted when the \a bitRate of this \l{WirelessNetworkDevice} has changed.
*/

/*! \fn void WirelessNetworkDevice::stateChanged(const NetworkDeviceState &state);
    This signal will be emitted when the current \a state of this \l{WirelessNetworkDevice} has changed.

    \sa NetworkDeviceState
*/


#include "dbus-interfaces.h"
#include "wirelessnetworkdevice.h"

#include <QUuid>
#include <QDebug>
#include <QMetaEnum>

/*! Constructs a new \l{WirelessNetworkDevice} with the given dbus \a objectPath and \a parent. */
WirelessNetworkDevice::WirelessNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent) :
    NetworkDevice(objectPath, parent),
    m_activeAccessPoint(Q_NULLPTR)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qWarning() << "WirelessNetworkDevice: System DBus not connected";
        return;
    }

    m_wirelessInterface = new QDBusInterface(networkManagerServiceString, this->objectPath().path(), wirelessInterfaceString, systemBus, this);
    if (!m_wirelessInterface->isValid()) {
        qWarning() << "WirelessNetworkDevice: Invalid wireless dbus interface";
        return;
    }

    QDBusConnection::systemBus().connect(networkManagerServiceString, this->objectPath().path(), wirelessInterfaceString, "AccessPointAdded", this, SLOT(accessPointAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(networkManagerServiceString, this->objectPath().path(), wirelessInterfaceString, "AccessPointRemoved", this, SLOT(accessPointRemoved(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(networkManagerServiceString, this->objectPath().path(), wirelessInterfaceString, "PropertiesChanged", this, SLOT(propertiesChanged(QVariantMap)));

    readAccessPoints();

    setMacAddress(m_wirelessInterface->property("HwAddress").toString());
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

/*! Returns the current active \l{WirelessAccessPoint} of this \l{WirelessNetworkDevice}. */
WirelessAccessPoint *WirelessNetworkDevice::activeAccessPoint()
{
    return m_activeAccessPoint;
}

/*! Perform a wireless network scan on this \l{WirelessNetworkDevice}. */
void WirelessNetworkDevice::scanWirelessNetworks()
{
    qDebug() << this << "Request scan";
    QDBusMessage query = m_wirelessInterface->call("RequestScan", QVariantMap());
    if (query.type() != QDBusMessage::ReplyMessage) {
        qWarning() << "Scan error:" << query.errorName() << query.errorMessage();
        return;
    }
}

/*! Returns the list of all \l{WirelessAccessPoint}{WirelessAccessPoints} of this \l{WirelessNetworkDevice}. */
QList<WirelessAccessPoint *> WirelessNetworkDevice::accessPoints()
{
    return m_accessPointsTable.values();
}

/*! Returns the \l{WirelessAccessPoint} with the given \a ssid. If the \l{WirelessAccessPoint} could not be found, return Q_NULLPTR. */
WirelessAccessPoint *WirelessNetworkDevice::getAccessPoint(const QString &ssid)
{
    foreach (WirelessAccessPoint *accessPoint, m_accessPointsTable.values()) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }
    return Q_NULLPTR;
}

/*! Returns the \l{WirelessAccessPoint} with the given \a objectPath. If the \l{WirelessAccessPoint} could not be found, return Q_NULLPTR. */
WirelessAccessPoint *WirelessNetworkDevice::getAccessPoint(const QDBusObjectPath &objectPath)
{
    return m_accessPointsTable.value(objectPath);
}

void WirelessNetworkDevice::readAccessPoints()
{
    QDBusMessage query = m_wirelessInterface->call("GetAccessPoints");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qWarning() << query.errorName() << query.errorMessage();
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

void WirelessNetworkDevice::setBitrate(const int &bitRate)
{
    if (m_bitRate != bitRate / 1000) {
        m_bitRate = bitRate / 1000;
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
            m_activeAccessPoint = Q_NULLPTR;
        }
        emit deviceChanged();
    }
}

void WirelessNetworkDevice::accessPointAdded(const QDBusObjectPath &objectPath)
{
    QDBusInterface accessPointInterface(networkManagerServiceString, objectPath.path(), accessPointInterfaceString, QDBusConnection::systemBus());
    if (!accessPointInterface.isValid()) {
        qWarning() << "WirelessNetworkDevice: Invalid access point dbus interface";
        return;
    }

    if (m_accessPointsTable.keys().contains(objectPath)) {
        qWarning() << "WirelessNetworkDevice: Access point already added" << objectPath.path();
        return;
    }

    WirelessAccessPoint *accessPoint = new WirelessAccessPoint(objectPath, this);
    //qDebug() << "WirelessNetworkDevice: [+]" << accessPoint;
    m_accessPointsTable.insert(objectPath, accessPoint);
}

void WirelessNetworkDevice::accessPointRemoved(const QDBusObjectPath &objectPath)
{
    if (!m_accessPointsTable.keys().contains(objectPath))
        return;

    WirelessAccessPoint *accessPoint = m_accessPointsTable.take(objectPath);
    if (accessPoint == m_activeAccessPoint)
        m_activeAccessPoint = Q_NULLPTR;

    //qDebug() << "WirelessNetworkDevice: [-]" << accessPoint;
    accessPoint->deleteLater();
}

void WirelessNetworkDevice::propertiesChanged(const QVariantMap &properties)
{
    //qDebug() << "WirelessNetworkDevice: Property changed" << properties;

    if (properties.contains("Bitrate"))
        setBitrate(properties.value("Bitrate").toInt());

    if (properties.contains("ActiveAccessPoint"))
        setActiveAccessPoint(qdbus_cast<QDBusObjectPath>(properties.value("ActiveAccessPoint")));
}

/*! Writes the given \a device to the given to \a debug. \sa WirelessNetworkDevice, */
QDebug operator<<(QDebug debug, WirelessNetworkDevice *device)
{
    debug.nospace() << "WirelessNetworkDevice(" << device->interface() << ", ";
    debug.nospace() << device->macAddress() <<  ", ";
    debug.nospace() << device->bitRate() <<  " [Mb/s], ";
    debug.nospace() << device->deviceStateString() <<  ") ";
    return debug;
}
