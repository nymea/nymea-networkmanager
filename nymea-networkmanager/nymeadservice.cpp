/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

NymeadService::~NymeadService()
{
    // Note: re-enable bluetooth hardware resource on nymea
    enableBluetooth(true);
}

bool NymeadService::available() const
{
    return m_available;
}

void NymeadService::enableBluetooth(const bool &enable)
{
    if (!m_nymeadHardwareBluetoothInterface) {
        qCWarning(dcNymeaService()) << "Could not enable/disable bluetooth hardware resource. D-Bus interface not available.";
        return;
    }

    qCDebug(dcNymeaService()) << "Request nymea to" << (enable ? "enable" : "disable") << "bluetooth resources";

    QDBusMessage query = m_nymeadHardwareBluetoothInterface->call("EnableBluetooth", enable);
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

    m_nymeadHardwareBluetoothInterface = new QDBusInterface("io.guh.nymead", "/io/guh/nymead/HardwareManager/BluetoothLEManager", "io.guh.nymead", QDBusConnection::systemBus(), this);
    if (!m_nymeadHardwareBluetoothInterface->isValid()) {
        qCWarning(dcNymeaService()) << "Invalid D-Bus HardwareManager BluetoothLE interface.";
        m_nymeadHardwareBluetoothInterface->deleteLater();
        m_nymeadHardwareBluetoothInterface = nullptr;
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

    if (m_nymeadHardwareBluetoothInterface) {
        m_nymeadHardwareBluetoothInterface->deleteLater();
        m_nymeadHardwareBluetoothInterface = nullptr;
    }

    setAvailable(false);
}
