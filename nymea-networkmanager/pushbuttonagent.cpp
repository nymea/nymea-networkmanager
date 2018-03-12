#include "pushbuttonagent.h"
#include "loggingcategories.h"

#include <QDBusMessage>
#include <QDBusObjectPath>

PushButtonAgent::PushButtonAgent(QObject *parent) :
    QObject(parent)
{

}

bool PushButtonAgent::init(QDBusConnection::BusType busType)
{
    QDBusConnection bus = busType == QDBusConnection::SessionBus ? QDBusConnection::sessionBus() : QDBusConnection::systemBus();

    bool result = bus.registerObject("/io/nymea/nymea-networkmanager/pushbutton", this, QDBusConnection::ExportScriptableContents);
    if (!result) {
        qCWarning(dcNymeaService()) << "PushButtonAgent: Error registering PushButton agent on D-Bus.";
        return false;
    }

    QDBusMessage message = QDBusMessage::createMethodCall("io.guh.nymead", "/io/guh/nymead/UserManager", QString(), "RegisterButtonAgent");
    message << qVariantFromValue(QDBusObjectPath("/io/nymea/nymea-networkmanager/pushbutton"));
    QDBusMessage reply = bus.call(message);
    if (!reply.errorName().isEmpty()) {
        qCWarning(dcNymeaService()) << "PushButtonAgent: Error registering PushButton agent:" << reply.errorMessage();
        return false;
    }
    qCDebug(dcNymeaService()) << "PushButton agent registered.";
    return true;
}

void PushButtonAgent::sendButtonPressed()
{
    qCDebug(dcNymeaService()) << "PushButtonAgent: Sending button pressed event.";
    emit PushButtonPressed();
}
