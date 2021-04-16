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

#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "nymeadservice.h"
#include "nymea-gpio/gpiobutton.h"
#include "bluetooth/bluetoothserver.h"
#include "networkmanager.h"
#include "bluetooth/bluetoothserver.h"

Q_DECLARE_LOGGING_CATEGORY(dcApplication)

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    enum Mode {
        ModeAlways,
        ModeOffline,
        ModeOnce,
        ModeStart,
        ModeButton
    };
    Q_ENUM(Mode)

    NetworkManager *networkManager() const;
    BluetoothServer *bluetoothServer() const;
    NymeadService *nymeaService() const;

    Mode mode() const;
    void setMode(Mode mode);

    QString advertiseName() const;
    void setAdvertiseName(const QString &name, bool forceFullName = false);

    QString platformName() const;
    void setPlatformName(const QString &name);

    int advertisingTimeout() const;
    void setAdvertisingTimeout(int advertisingTimeout);

    void addGPioButton(int buttonGpio);
    void enableDBusInterface(QDBusConnection::BusType busType);

    void run();

private:
    NetworkManager *m_networkManager = nullptr;
    BluetoothServer *m_bluetoothServer = nullptr;
    NymeadService *m_nymeaService = nullptr;
    WirelessNetworkDevice *m_wirelessDevice = nullptr;
    QList<GpioButton*> m_buttons;

    QTimer *m_advertisingTimer = nullptr;

    Mode m_mode = ModeOffline;
    QString m_advertiseName;
    bool m_forceFullName = false;
    QString m_platformName;
    int m_advertisingTimeout = 60;

    void evaluateNetworkManagerState(NetworkManager::NetworkManagerState state);

private slots:
    void startService();
    void stopService();

    void onAdvertisingTimeout();

    void onBluetoothServerRunningChanged(bool running);
    void onBluetoothServerConnectedChanged(bool connected);

    void onNetworkManagerAvailableChanged(bool available);
    void onNetworkManagerStateChanged(NetworkManager::NetworkManagerState state);

    void onNymeaServiceAvailableChanged(bool available);

};

#endif // CORE_H
