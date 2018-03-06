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

#ifndef BLUETOOTHUUIDS_H
#define BLUETOOTHUUIDS_H

#include <QBluetoothUuid>

static QBluetoothUuid networkServiceUuid =                  QBluetoothUuid(QUuid("ef6d6610-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkStatusCharacteristicUuid =     QBluetoothUuid(QUuid("ef6d6611-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkCommanderCharacteristicUuid =  QBluetoothUuid(QUuid("ef6d6612-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkResponseCharacteristicUuid =   QBluetoothUuid(QUuid("ef6d6613-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkingEnabledCharacteristicUuid = QBluetoothUuid(QUuid("ef6d6614-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid wirelessEnabledCharacteristicUuid =   QBluetoothUuid(QUuid("ef6d6615-b8af-49e0-9eca-ab343513641c"));

static QBluetoothUuid wirelessServiceUuid =                 QBluetoothUuid(QUuid("e081fec0-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wirelessCommanderCharacteristicUuid = QBluetoothUuid(QUuid("e081fec1-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wirelessResponseCharacteristicUuid =  QBluetoothUuid(QUuid("e081fec2-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wirelessStateCharacteristicUuid =     QBluetoothUuid(QUuid("e081fec3-f757-4449-b9c9-bfa83133f7fc"));

static QBluetoothUuid systemServiceUuid =                 QBluetoothUuid(QUuid("e081fed0-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemCommanderCharacteristicUuid = QBluetoothUuid(QUuid("e081fed1-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemResponseCharacteristicUuid =  QBluetoothUuid(QUuid("e081fed2-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemUpdateCharacteristicUuid =    QBluetoothUuid(QUuid("e081fed3-f757-4449-b9c9-bfa83133f7fc"));


#endif // BLUETOOTHUUIDS_H
