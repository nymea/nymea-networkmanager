#include "networkmanagerutils.h"

Q_LOGGING_CATEGORY(dcNetworkManager, "NetworkManager")

QString NetworkManagerUtils::NetworkManagerUtils::networkManagerServiceString()
{
    return "org.freedesktop.NetworkManager";
}

QString NetworkManagerUtils::networkManagerPathString()
{
    return "/org/freedesktop/NetworkManager";
}

QString NetworkManagerUtils::NetworkManagerUtils::settingsPathString()
{
    return "/org/freedesktop/NetworkManager/Settings";
}

QString NetworkManagerUtils::NetworkManagerUtils::deviceInterfaceString()
{
    return "org.freedesktop.NetworkManager.Device";
}

QString NetworkManagerUtils::NetworkManagerUtils::wirelessInterfaceString()
{
    return "org.freedesktop.NetworkManager.Device.Wireless";
}

QString NetworkManagerUtils::NetworkManagerUtils::wiredInterfaceString()
{
    return "org.freedesktop.NetworkManager.Device.Wired";
}

QString NetworkManagerUtils::NetworkManagerUtils::accessPointInterfaceString()
{
    return "org.freedesktop.NetworkManager.AccessPoint";
}

QString NetworkManagerUtils::NetworkManagerUtils::settingsInterfaceString()
{
    return "org.freedesktop.NetworkManager.Settings";
}

QString NetworkManagerUtils::connectionsInterfaceString()
{
    return "org.freedesktop.NetworkManager.Settings.Connection";
}


