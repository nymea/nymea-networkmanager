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

#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QObject>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

#include "networkconnection.h"

class NetworkConnection;

class NetworkSettings : public QObject
{
    Q_OBJECT
public:
    explicit NetworkSettings(QObject *parent = 0);

    QDBusObjectPath addConnection(const ConnectionSettings &settings);
    QList<NetworkConnection *> connections() const;

private:
    QDBusInterface *m_settingsInterface;
    QHash<QDBusObjectPath, NetworkConnection *> m_connections;

    void loadConnections();

signals:

private slots:
    void connectionAdded(const QDBusObjectPath &objectPath);
    void connectionRemoved(const QDBusObjectPath &objectPath);
    void propertiesChanged(const QVariantMap &properties);

};

#endif // NETWORKSETTINGS_H
