/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of libnymea-networkmanager.                                 *
 *                                                                               *
 * libnymea-networkmanager is free software: you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License,                *
 * or (at your option) any later version.                                        *
 *                                                                               *
 * libnymea-networkmanager is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License along       *
 * with libnymea-networkmanager. If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NETWORKMANAGERUTILS_H
#define NETWORKMANAGERUTILS_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dcNetworkManager)

class NetworkManagerUtils
{
    Q_GADGET

public:
    static QString networkManagerServiceString();

    static QString networkManagerPathString();
    static QString settingsPathString();

    static QString deviceInterfaceString();
    static QString wirelessInterfaceString();
    static QString wiredInterfaceString();
    static QString accessPointInterfaceString();
    static QString settingsInterfaceString();
    static QString connectionsInterfaceString();

};

#endif // NETWORKMANAGERUTILS_H
