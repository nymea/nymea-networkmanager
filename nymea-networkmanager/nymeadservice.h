// SPDX-License-Identifier: GPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-networkmanager.
*
* nymea-networkmanager is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* nymea-networkmanager is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with nymea-networkmanager. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NYMEADSERVICE_H
#define NYMEADSERVICE_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusServiceWatcher>

#include "pushbuttonagent.h"

class NymeadService : public QObject
{
    Q_OBJECT
public:
    explicit NymeadService(bool pushbuttonEnabled, QObject *parent = nullptr);
    ~NymeadService();
    bool available() const;

    void enableBluetooth(bool enable);
    void pushButtonPressed();

private:
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    PushButtonAgent *m_pushButtonAgent = nullptr;

    QDBusInterface *m_nymeadHardwareInterface = nullptr;
    QDBusInterface *m_nymeadHardwareBluetoothInterface = nullptr;

    bool m_pushbuttonEnabled = false;
    bool m_available = false;

    void setAvailable(const bool &available);

    bool init();

signals:
    void availableChanged(const bool &available);

private slots:
    void serviceRegistered(const QString &serviceName);
    void serviceUnregistered(const QString &serviceName);

};

#endif // NYMEADSERVICE_H
