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
