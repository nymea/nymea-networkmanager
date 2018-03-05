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

#include "networksettings.h"
#include "dbus-interfaces.h"

#include <QDebug>

/*! Constructs a new \l{NetworkSettings} object with the given \a parent. */
NetworkSettings::NetworkSettings(QObject *parent) : QObject(parent)
{
    m_settingsInterface = new QDBusInterface(networkManagerServiceString, settingsPathString, settingsInterfaceString, QDBusConnection::systemBus(), this);
    if(!m_settingsInterface->isValid()) {
        qWarning() << "Invalid DBus network settings interface";
        return;
    }

    loadConnections();

    QDBusConnection::systemBus().connect(networkManagerServiceString, settingsPathString, settingsInterfaceString, "NewConnection", this, SLOT(connectionAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(networkManagerServiceString, settingsPathString, settingsInterfaceString, "ConnectionRemoved", this, SLOT(connectionRemoved(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(networkManagerServiceString, settingsPathString, settingsInterfaceString, "PropertiesChanged", this, SLOT(propertiesChanged(QVariantMap)));
}

/*! Add the given \a settings to this \l{NetworkSettings}. Returns the dbus object path from the new settings. */
QDBusObjectPath NetworkSettings::addConnection(const ConnectionSettings &settings)
{
    QDBusMessage query = m_settingsInterface->call("AddConnection", QVariant::fromValue(settings));
    if(query.type() != QDBusMessage::ReplyMessage) {
        qWarning() << query.errorName() << query.errorMessage();
        return QDBusObjectPath();
    }

    if (query.arguments().isEmpty())
        return QDBusObjectPath();

    return query.arguments().at(0).value<QDBusObjectPath>();
}

/*! Returns the list of current \l{NetworkConnection}{NetworkConnections} from this \l{NetworkSettings}. */
QList<NetworkConnection *> NetworkSettings::connections() const
{
    return m_connections.values();
}

void NetworkSettings::loadConnections()
{
    QDBusMessage query = m_settingsInterface->call("ListConnections");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qWarning() << query.errorName() << query.errorMessage();
        return;
    }

    if (query.arguments().isEmpty())
        return;

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    argument.beginArray();
    while(!argument.atEnd()) {
        QDBusObjectPath objectPath = qdbus_cast<QDBusObjectPath>(argument);
        connectionAdded(objectPath);
    }
    argument.endArray();

}

void NetworkSettings::connectionAdded(const QDBusObjectPath &objectPath)
{
    NetworkConnection *connection = new NetworkConnection(objectPath, this);
    m_connections.insert(objectPath, connection);

    qDebug() << "Settings: [+]" << connection;
}

void NetworkSettings::connectionRemoved(const QDBusObjectPath &objectPath)
{
    NetworkConnection *connection = m_connections.take(objectPath);
    qDebug() << "Settings: [-]" << connection;
    connection->deleteLater();
}

void NetworkSettings::propertiesChanged(const QVariantMap &properties)
{
    Q_UNUSED(properties);
    //qDebug() << "Settins: properties changed" << properties;
}
