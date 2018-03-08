/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of nymea-networkmanager.                                    *
 *                                                                               *
 * nymea-networkmanager is free software: you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License,                *
 * or (at your option) any later version.                                        *
 *                                                                               *
 * nymea-networkmanager is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License along       *
 * with nymea-networkmanager. If not, see <http://www.gnu.org/licenses/>.        *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "networkmanager.h"
#include "nymeadservice.h"
#include "bluetooth/bluetoothserver.h"

class Core : public QObject
{
    Q_OBJECT
public:
    static Core* instance();
    void destroy();

    NetworkManager *networkManager() const;

private:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    static Core *s_instance;

    NetworkManager *m_networkManager = nullptr;
    BluetoothServer *m_bluetoothServer = nullptr;
    NymeadService *m_nymeaService = nullptr;

private slots:
    void onNetworkManagerAvailableChanged(const bool &available);

};

#endif // CORE_H
