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

#ifndef WIREDNETWORKDEVICE_H
#define WIREDNETWORKDEVICE_H

#include <QObject>
#include <QDBusObjectPath>

#include "networkdevice.h"

class WiredNetworkDevice : public NetworkDevice
{
    Q_OBJECT
public:
    explicit WiredNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent = 0);

    QString macAddress() const;
    int bitRate() const;
    bool pluggedIn() const;

private:
    QDBusInterface *m_wiredInterface;

    QString m_macAddress;
    int m_bitRate;
    bool m_pluggedIn;

    void setMacAddress(const QString &macAddress);
    void setBitRate(const int &bitRate);
    void setPluggedIn(const bool &pluggedIn);

private slots:
    void propertiesChanged(const QVariantMap &properties);

};

QDebug operator<<(QDebug debug, WiredNetworkDevice *networkDevice);

#endif // WIREDNETWORKDEVICE_H
