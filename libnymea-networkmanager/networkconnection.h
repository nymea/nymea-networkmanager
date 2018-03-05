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

#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H

#include <QUuid>
#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

typedef QMap<QString, QVariantMap> ConnectionSettings;

class NetworkConnection : public QObject
{
    Q_OBJECT
public:
    explicit NetworkConnection(const QDBusObjectPath &objectPath, QObject *parent = 0);

    void deleteConnection();

    QDBusObjectPath objectPath() const;
    ConnectionSettings connectionSettings() const;

    QString id() const;
    QString name() const;
    QString type() const;
    QUuid uuid() const;
    QString interfaceName() const;
    bool autoconnect() const;
    QDateTime timeStamp() const;

private:
    QDBusObjectPath m_objectPath;
    QDBusInterface *m_connectionInterface;

    ConnectionSettings m_connectionSettings;

};

QDebug operator<<(QDebug debug, NetworkConnection *networkConnection);

#endif // NETWORKCONNECTION_H
