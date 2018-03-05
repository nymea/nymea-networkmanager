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

#ifndef DBUSINTERFACES_H
#define DBUSINTERFACES_H

#include <QString>
static const QString networkManagerServiceString("org.freedesktop.NetworkManager");

static const QString networkManagerPathString("/org/freedesktop/NetworkManager");
static const QString settingsPathString("/org/freedesktop/NetworkManager/Settings");

static const QString deviceInterfaceString("org.freedesktop.NetworkManager.Device");
static const QString wirelessInterfaceString("org.freedesktop.NetworkManager.Device.Wireless");
static const QString wiredInterfaceString("org.freedesktop.NetworkManager.Device.Wired");
static const QString accessPointInterfaceString("org.freedesktop.NetworkManager.AccessPoint");
static const QString settingsInterfaceString("org.freedesktop.NetworkManager.Settings");
static const QString connectionsInterfaceString("org.freedesktop.NetworkManager.Settings.Connection");

#endif // DBUSINTERFACES_H
