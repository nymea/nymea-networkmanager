/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 * Copyright (C) 2016 Simon Stürz <simon.stuerz@guh.io>                          *
 *                                                                               *
 * This file is part of loopd.                                                   *
 *                                                                               *
 * Loopd can not be copied and/or distributed without the express                *
 * permission of guh GmbH.                                                       *
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
