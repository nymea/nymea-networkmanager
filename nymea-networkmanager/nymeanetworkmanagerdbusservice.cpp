#include "nymeanetworkmanagerdbusservice.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(dcDBus, "DBus");

NymeaNetworkManagerDBusService::NymeaNetworkManagerDBusService(QDBusConnection::BusType busType, QObject *parent) : QObject(parent),
    m_connection(busType == QDBusConnection::SystemBus ? QDBusConnection::systemBus() : QDBusConnection::sessionBus())
{
    bool status = m_connection.registerService("io.nymea.networkmanager");
    if (!status) {
        qCWarning(dcDBus()) << "Failed to register D-Bus service.";
        return;
    }
    status = m_connection.registerObject("/io/nymea/networkmanager", "io.nymea.networkmanager", this, QDBusConnection::ExportScriptableSlots);
    if (!status) {
        qCWarning(dcDBus()) << "Failed to register D-Bus object.";
        return;
    }
    qCDebug(dcDBus()) << "Registered DBus interface";
}

void NymeaNetworkManagerDBusService::enableBluetoothServer()
{
    qCDebug(dcDBus()) << "Enable bluetooth server called";
    emit enableBluetoothServerCalled();
}
