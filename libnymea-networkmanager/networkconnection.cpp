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

#include "networkconnection.h"
#include "dbus-interfaces.h"

#include <QDebug>
#include <QJsonDocument>

/*! Constructs a new \l{NetworkConnection} with the given dbus \a objectPath and \a parent. */
NetworkConnection::NetworkConnection(const QDBusObjectPath &objectPath, QObject *parent) :
    QObject(parent),
    m_objectPath(objectPath)
{
    qDBusRegisterMetaType<ConnectionSettings>();

    m_connectionInterface = new QDBusInterface(networkManagerServiceString, m_objectPath.path(), connectionsInterfaceString, QDBusConnection::systemBus(), this);
    if(!m_connectionInterface->isValid()) {
        qWarning() << "Invalid connection dbus interface";
        return;
    }

    QDBusMessage query = m_connectionInterface->call("GetSettings");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qWarning() << query.errorName() << query.errorMessage();
        return;
    }

    if (query.arguments().isEmpty())
        return;

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    m_connectionSettings = qdbus_cast<ConnectionSettings>(argument);
}

/*! Delets this \l{NetworkConnection} in the \l{NetworkManager}. */
void NetworkConnection::deleteConnection()
{
    QDBusMessage query = m_connectionInterface->call("Delete");
    if(query.type() != QDBusMessage::ReplyMessage)
        qWarning() << query.errorName() << query.errorMessage();

}

/*! Returns the dbus object path of this \l{NetworkConnection}. */
QDBusObjectPath NetworkConnection::objectPath() const
{
    return m_objectPath;
}

/*! Returns the connection settings of this \l{NetworkConnection}. */
ConnectionSettings NetworkConnection::connectionSettings() const
{
    return m_connectionSettings;
}

/*! Returns the id of this \l{NetworkConnection}. */
QString NetworkConnection::id() const
{
    return m_connectionSettings.value("connection").value("id").toString();
}

/*! Returns the name of this \l{NetworkConnection}. */
QString NetworkConnection::name() const
{
    return m_connectionSettings.value("connection").value("name").toString();
}

/*! Returns the type of this \l{NetworkConnection}. */
QString NetworkConnection::type() const
{
    return m_connectionSettings.value("connection").value("type").toString();
}

/*! Returns the uuid of this \l{NetworkConnection}. */
QUuid NetworkConnection::uuid() const
{
    return m_connectionSettings.value("connection").value("uuid").toUuid();
}

/*! Returns the interface name of this \l{NetworkConnection}. */
QString NetworkConnection::interfaceName() const
{
    return m_connectionSettings.value("connection").value("interface-name").toString();
}

/*! Returns true if this \l{NetworkConnection} will autoconnect if available. */
bool NetworkConnection::autoconnect() const
{
    return m_connectionSettings.value("connection").value("autoconnect").toBool();
}

/*! Returns the timestamp of this \l{NetworkConnection} from the last connection. */
QDateTime NetworkConnection::timeStamp() const
{
    return QDateTime::fromTime_t(m_connectionSettings.value("connection").value("timestamp").toUInt());
}

QDebug operator<<(QDebug debug, NetworkConnection *networkConnection)
{
    debug.nospace() << "NetworkConnection(" << networkConnection->id() << ", ";
    debug.nospace() << networkConnection->uuid().toString() << ", ";
    debug.nospace() << networkConnection->interfaceName() << ", ";
    debug.nospace() << networkConnection->type() << ", ";
    debug.nospace() << networkConnection->timeStamp().toString("dd.MM.yyyy hh:mm") << ") ";
    return debug;
}
