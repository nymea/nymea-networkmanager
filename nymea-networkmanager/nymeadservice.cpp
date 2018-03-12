#include "nymeadservice.h"
#include "loggingcategories.h"

NymeadService::NymeadService(bool pushbuttonEnabled, QObject *parent) :
    QObject(parent),
    m_pushbuttonEnabled(pushbuttonEnabled)
{
    // Check DBus connection
    if (!QDBusConnection::systemBus().isConnected()) {
        qCWarning(dcNymeaService()) << "System DBus not connected.";
        return;
    }

    // Get notification when nymead appears/disappears on DBus
    m_serviceWatcher = new QDBusServiceWatcher("io.guh.nymead", QDBusConnection::systemBus(), QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration, this);
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &NymeadService::serviceRegistered);
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &NymeadService::serviceUnregistered);

    if (init()) {
        qCDebug(dcNymeaService()) << "Initialized nymea D-Bus services successfully";
    } else {
        qCWarning(dcNymeaService()) << "Could not init nymea D-Bus services";
        setAvailable(false);
    }
}

bool NymeadService::available() const
{
    return m_available;
}

void NymeadService::enableBluetooth(const bool &enable)
{
    if (!m_nymeadHardwareInterface) {
        qCWarning(dcNymeaService()) << "Could not enable/disable bluetooth hardware resource. D-Bus interface not available.";
        return;
    }

    qCDebug(dcNymeaService()) << "Request nymea to" << (enable ? "enable" : "disable") << "bluetooth resources";

    QDBusMessage query = m_nymeadHardwareInterface->call("EnableBluetooth", enable);
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNymeaService()) << "Could not enable/disable bluetooth on dbus:" << query.errorName() << query.errorMessage();
        return;
    }
}

void NymeadService::pushButtonPressed()
{
    if (!m_pushButtonAgent) {
        qCWarning(dcNymeaService()) << "Could not press pushbutton. Pushbutton agent not available.";
        return;
    }

    qCDebug(dcNymeaService()) << "Pushbutton pressed. Send to nymead";
    m_pushButtonAgent->sendButtonPressed();
}

void NymeadService::setAvailable(const bool &available)
{
    if (m_available == available)
        return;

    if (available) {
        qCDebug(dcNymeaService())  << "Service is now available.";
    } else {
        qCWarning(dcNymeaService())  << "Service is not available any more.";
    }

    m_available = available;
    emit availableChanged(m_available);
}

bool NymeadService::init()
{
    if (m_pushbuttonEnabled) {
        m_pushButtonAgent = new PushButtonAgent(this);
        if (!m_pushButtonAgent->init()) {
            qCWarning(dcNymeaService()) << "Could not init D-Bus push button agent.";
            m_pushButtonAgent->deleteLater();
            m_pushButtonAgent = nullptr;
            return false;
        }
    }

    m_nymeadHardwareInterface = new QDBusInterface("io.guh.nymead", "/io/guh/nymead/HardwareManager", "io.guh.nymead", QDBusConnection::systemBus(), this);
    if (!m_nymeadHardwareInterface->isValid()) {
        qCWarning(dcNymeaService()) << "Invalid D-Bus HardwareManager interface.";
        m_nymeadHardwareInterface->deleteLater();
        m_nymeadHardwareInterface = nullptr;
        return false;
    }

    setAvailable(true);
    return true;
}

void NymeadService::serviceRegistered(const QString &serviceName)
{
    qCDebug(dcNymeaService()) << "Service registered" << serviceName;
    if (!m_available) {
        init();
    }
}

void NymeadService::serviceUnregistered(const QString &serviceName)
{
    qCDebug(dcNymeaService()) << "Service unregistered" << serviceName;

    if (m_pushButtonAgent) {
        m_pushButtonAgent->deleteLater();
        m_pushButtonAgent = nullptr;
    }

    if (m_nymeadHardwareInterface) {
        m_nymeadHardwareInterface->deleteLater();
        m_nymeadHardwareInterface = nullptr;
    }

    setAvailable(false);
}
