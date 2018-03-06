/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of loopd.                                                   *
 *                                                                               *
 * Loopd can not be copied and/or distributed without the express                *
 * permission of guh GmbH.                                                       *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "wirednetworkdevice.h"

#include <QDebug>

/*! Constructs a new \l{WiredNetworkDevice} with the given dbus \a objectPath and \a parent. */
WiredNetworkDevice::WiredNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent) :
    NetworkDevice(objectPath, parent)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(dcNetworkManager()) << "WiredNetworkDevice: System DBus not connected";
        return;
    }

    m_wiredInterface = new QDBusInterface(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wiredInterfaceString(), systemBus, this);
    if(!m_wiredInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "WiredNetworkDevice: Invalid wired dbus interface";
        return;
    }

    setMacAddress(m_wiredInterface->property("HwAddress").toString());
    setBitRate(m_wiredInterface->property("Bitrate").toInt());
    setPluggedIn(m_wiredInterface->property("Carrier").toBool());

    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wiredInterfaceString(), "PropertiesChanged", this, SLOT(propertiesChanged(QVariantMap)));
}

/*! Returns the mac address of this \l{WiredNetworkDevice}. */
QString WiredNetworkDevice::macAddress() const
{
    return m_macAddress;
}

/*! Returns the current bit rate [Mb/s] of this \l{WiredNetworkDevice}. */
int WiredNetworkDevice::bitRate() const
{
    return m_bitRate;
}

/*! Returns true if this \l{WiredNetworkDevice} has a cable plugged in. */
bool WiredNetworkDevice::pluggedIn() const
{
    return m_pluggedIn;
}

void WiredNetworkDevice::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}

void WiredNetworkDevice::setBitRate(const int &bitRate)
{
    m_bitRate = bitRate;
}

void WiredNetworkDevice::setPluggedIn(const bool &pluggedIn)
{
    m_pluggedIn = pluggedIn;
}

void WiredNetworkDevice::propertiesChanged(const QVariantMap &properties)
{
    if (properties.contains("Carrier"))
        setPluggedIn(properties.value("Carrier").toBool());

}

QDebug operator<<(QDebug debug, WiredNetworkDevice *networkDevice)
{
    debug.nospace() << "WiredNetworkDevice(" << networkDevice->interface() << ", ";
    debug.nospace() << networkDevice->macAddress() <<  ", ";
    debug.nospace() << networkDevice->bitRate() <<  " [Mb/s], ";
    debug.nospace() << networkDevice->pluggedIn() <<  ", ";
    debug.nospace() << networkDevice->deviceStateString() <<  ") ";
    return debug;
}
