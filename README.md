# nymea-networkmanager

This daemon allows to set up the wireless network using a bluetooth LE connection. The daemon will automatically start a bluetooth low energy server 
if the system is currently not connected to any network. Once the system is connected, the daemon will shutdown the bluetooth server. 

# Build

First you have to install build dependencies:

> Note: the `libnymea-networkmanager-dev` package can be installed from the [nymea repository](https://nymea.io/en/wiki/nymea/master/install/debian) or built 
and installed from [source](https://github.com/nymea/libnymea-networkmanager).


> Note: the `libnymea-gpio-dev` package can be installed from the [nymea repository](https://nymea.io/en/wiki/nymea/master/install/debian) or built
and installed from [source](https://github.com/nymea/nymea-gpio).

> Note: a `Qt` verion `>=` `5.7.0` is required in order to work and bluez version `>=` `5.48`.


    $ sudo apt update
    $ sudo apt install qt5-default qtbase5-dev qtbase5-dev-tools libqt5bluetooth5 qtconnectivity5-dev libnymea-networkmanager-dev libnymea-gpio-dev git

Clone the source code and change into the source directory

    $ git clone https://github.com/guh/nymea-networkmanager.git
    $ cd nymea-networkmanager

Create the build directory

    $ mkdir build
    $ cd build

And finally build the daemon and library

    $ qmake ..
    $ make -j$(nproc)

You can run the daemon directly with following command

    $ sudo ./nymea-networkmanager/nymea-networkmanager


## Building the debian packages

In order to build a debian package you can do following:

    $ sudo apt install debhelper dh-systemd
    
    $ mkdir nymea-networkmanager
    $ cd nymea-networkmanager
    $ git clone https://github.com/guh/nymea-networkmanager.git
    $ cd nymea-networkmanager
    $ dpkg-buildpackage -us -uc -tc
    $ cd ..

    $ ls -l *.deb

# Config file

nymea-networkmanager will search for a config file in the following locations (in this order):

    ~/.config/nymea/nymea-networkmanager.conf
    /etc/nymea/nymea-networkmanager.conf

If such a config file is found, it reads values from there. There is a example config in
this repository and it will be installed to /etc/nymea-networkmanager.conf with the dpkg package.

> Note: Command line parameters will have higher priority than entries in the configuration file.

# Command line parameters

    $ nymea-networkmanager --help
    Usage: ./nymea-networkmanager [options]
    
    This daemon allows to configure a wifi network using a bluetooth low energy connection.
    
    Copyright © 2018-2019 Simon Stürz <simon.stuerz@nymea.io>
    
    Modes: 
      - offline  This mode starts the bluetooth server once the device is offline
                 and not connected to any LAN network.
      - once     This mode starts the bluetooth server only if no network configuration exists.
                 Once a network connection exists the server will never start again.
      - button   This mode enables the bluetooth server when a GPIO button has been pressed for
                 the configured timeout periode.
      - always   This mode enables the bluetooth server as long the application is running.
      - start    This mode starts the bluetooth server for 3 minutes on start and shuts down after a connection.
    
    
    
    Options:
      -h, --help                   Displays this help.
      -v, --version                Displays version information.
      -d, --debug                  Enable more debug output.
      -a, --advertise-name <NAME>  The name of the bluetooth server. Default "BT
                                   WLAN setup".
      -p, --platform-name <NAME>   The name of the platform this daemon is running.
                                   Default "nymea-box".
      -g, --gpio <GPIO>            The GPIO sysfs number for the button GPIO. This
                                   parameter is only needed for the "button" mode.
      -t, --timeout <SECONDS>      The timeout of the bluetooth server. Minimum
                                   value is 10. Default "60".
      -m, --mode <MODE>            Run the daemon in a specific mode (offline,
                                   once, always, button, start). Default is
                                   "offline".
        
        

# Bluetooth GATT profile
-------------------------------------------

In order to connect to nymea-networkmanager using bluetooth low energy, once has to perform a bluetooth discovery, filter for all low energy 
devices and connect to the device with the name `nymea`. The remote address type for connecting to `nymea-networkmanager` is `public`.


## Notifications

In order to enable/disable the notification for a characteristic with the `notify` flag, a client has to write the value `0x0100` for 
enabling and `0x0000` for disabling to the descriptor `0x2902` of the corresponding characteristic.

## Services:

### Overview

| Name               | Service UUID                           | Description
| ------------------ | -------------------------------------- | ----------------------------------------------------
| Generic Access     | `00001800-0000-1000-8000-00805f9b34fb` | The service contains generic information about the device
| Generic Attribute  | `00001801-0000-1000-8000-00805f9b34fb` | Default service for Bluetooth LE GATT devices
| Device information | `0000180a-0000-1000-8000-00805f9b34fb` | The service contains information about the device and manufacturer
| Wireless service   | `e081fec0-f757-4449-b9c9-bfa83133f7fc` | The wifi service for managing the wireless network
| Network service    | `ef6d6610-b8af-49e0-9eca-ab343513641c` | The network service for managing the network


> **S** = Service; **C** = Characteristic; **D** = Descriptor

> **W** = Write; **R** = Read; **N** = Notify


### **S**: Generic Access

> Default service for Bluetooth LE GATT devices. More information can be 
found [here](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.generic_access.xml).

### **S**: Generic Attribute

> Default service for Bluetooth LE GATT devices. More information can be found [here](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.generic_attribute.xml).

### **S**: Device Information

> Default service for Bluetooth LE GATT devices.  More information can be found [here](https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.device_information.xml).

### **S**: Wireless service `e081fec0-f757-4449-b9c9-bfa83133f7fc`

The *Wireless Service* allows a client to configure and monitor a wireless network connection. The connection can be controlled with the *Wireless commander* characteristic. Each command sent will generate a respone on the *Comander response* characteristic containing the error code for the command. The *Wireless connection status* characteristic informs the client about the current connection status of the wireless device.

**Characteristic overview**

| Name                       | Characterisitc UUID                    | Flag   | Description
| -------------------------- | -------------------------------------- | ------ | -------------------------------------------------------
| Wireless commander         | `e081fec1-f757-4449-b9c9-bfa83133f7fc` | **W**  | Controll what the wifi manager should do.
| Commander response         | `e081fec2-f757-4449-b9c9-bfa83133f7fc` | **N**  | This characteristic will be used to inform about the command result (error reporting).
| Wireless connection status | `e081fec3-f757-4449-b9c9-bfa83133f7fc` | **RN** | Informs about the current wireless connection status.
| Wireless mode              | `e081fec4-f757-4449-b9c9-bfa83133f7fc` | **RN** | Informs about the current mode of the wireless device.


**Characteristic details**

- **C**: *Wireless commander* (W) `e081fec1-f757-4449-b9c9-bfa83133f7fc`

    - *Description*: Controll characteristic for the wireless manager. Each command sent to this characteristic will create a response report on the *Commander response* characterisitc.
    - *Range*: [0-20] Byte, UTF-8, JSON
    - *Possible values*:


In following example you can find the basic structure of a command and a response. The command can be sent to this *Wireless commander* characteristic, the response will be notified on the *Commander response* characteristic. The JSON object containing the command map has to be formated compact and must end with the '\n' character. If a data package is longer than the allowed 20 Bytes, the data must be splitted into 20 Byte packages and sent in the correct order. The end of a JSON data stream will be recongnized, once the '\n' character will be found at the end of a package. The *Commander response* characteristic uses the same mechanism.


- Request

                  {
                      "c": 0,           // Integer: Command: describing the method called
                      "p": { }          // The parameters of the method. If the method does not have any parameters you can skip this.
                  }


- Response

                  {
                      "c": 0,               // Integer: Command: describing the method called
                      "r": 0,               // Integer: Response error code. See list of error codes.
                      "p": Object or Array  // Object or Array. This value is optional and depends on the requested command.
                  }

- List of response error code:

| Value  | Name                       | Description
| ------ | -------------------------- | ----------------------------------------------------
| `0`    | Success                    | The command has been executed successfully.
| `1`    | InvalidCommand             | The commander received an invalid command.
| `2`    | InvalidParameter           | The commander received an invalid parameter.
| `3`    | NetworkManagerNotAvailable | Network manager is currently not available (`network-manager` daemon or dbus not running).
| `4`    | WirelessNotAvailable       | Wireless currently not available. There is no wireless adapter in the system.
| `5`    | NetworkingDisabled         | Networking is not enabled. One can enable it in the *Network service*.
| `6`    | WirelessDisabled           | Wireless networking is not enabled. One can enable it in the *Network service*.
| `7`    | Unknown                    | An unknown error happend.


#### Methods

| Value  | Name              | Description
| ------ | ----------------- | ----------------------------------------------------
| `0`    | GetNetworks       | Get the current wifi network list on the "Wireless data stream".
| `1`    | Connect           | Connect to the network with the given ssid and password in **C** `e081fec4` and **C** `e081fec5`. If the network is open, set the password characteristic to an empty string.
| `2`    | ConnectHidden     | Connect to the hidden network using the given ssid and password in **C** `e081fec4` and **C** `e081fec5`.
| `3`    | Disconnect        | Disconnect from current wireless network.
| `4`    | Scan              | Perform a wireless accesspoint scan.
| `5`    | GetConnection     | Get the current connection information on the "Wireless data stream"
| `6`    | StartAccessPoint  | Start a wireless access point.


##### - GetNetworks (0)

- Request

                  {
                      "c": 0            // Command: GetNetworks
                  }

- Response

                  {
                      "c": 0,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                      "p": [
                          {
                              "e": "My network",               // String: ESSID: the name of the wifi network
                              "m": "AA:BB:CC:DD:EE",           // String: The MAC address of the wifi access point
                              "s": 86,                         // Integer: Signal strength [0-100] %
                              "p": 0                           // Integer: Protected [0,1]; 0 - open, 1 - protected 
                          }
                     ]
                  }


##### - Connect (1)

- Request

                  {
                      "c": 1,                    // Command: Connect
                      "p": {
                          "e": "Wifi SSID",      // The SSID of the wifi access point you want to connect to
                          "p": "Wifi password"   // The password of the access point you want to connect to
                      }
                  }

- Response

                  {
                      "c": 1,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                  }


##### - ConnectHidden (2)

- Request

                  {
                      "c": 2,                    // Command: ConnectHidden
                      "p": {
                          "e": "Wifi SSID",      // The SSID of the wifi access point you want to connect to
                          "p": "Wifi password"   // The password of the access point you want to connect to

                      }
                  }

- Response

                  {
                      "c": 2,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                  }


##### - Disconnect (3)

- Request

                  {
                      "c": 3            // Command: Disconnect
                  }

- Response

                  {
                      "c": 3,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                  }


##### - Scan (4)

- Request

                  {
                      "c": 4            // Command: Scan
                  }

- Response

                  {
                      "c": 4,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                  }


##### - GetConnection (5)

- Request

                  {
                      "c": 5            // Command: GetConnection
                  }

- Response

                  {
                      "c": 5,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                      "p": {
                          "e": "My network",               // String: ESSID: the name of the current wifi network
                          "m": "AA:BB:CC:DD:EE",           // String: The MAC address of the current wifi access point
                          "s": 86,                         // Integer: Signal strength [0-100] %
                          "p": 0,                          // Integer: Protected [0,1]; 0 - open, 1 - protected 
                          "i": "192.168.0.12"              // String: The IP address of the current wifi connection
                      }
                  }


##### - StartAccessPoint (6)

- Request

                  {
                      "c": 6,                    // Command: StartAccessPoint
                      "p": {
                          "e": "Access Point SSID",               // The SSID of the wifi access point you want to create
                          "p": "Wireless access point password"   // The password of the access point you want to create
                      }
                  }


- Response

                  {
                      "c": 6,               // Integer: Command: describing the method called
                      "r": 0                // Integer: Response error code. See list of response error codes.
                  }




- **C**: *Commander response* (N) `e081fec2-f757-4449-b9c9-bfa83133f7fc`

    - *Description*: Sends a JSON object in 20 Byte packages. The data stream is finished once the \n charater received at the end of a package.
    - *Range*: [0-20] Byte, UTF-8, JSON
    - *Possible values*: See "methods - response" for more details

    The JSON object containing the response map has to be formated compact and must end with the '\n' character. If a data package is longer than the allowed 20 Bytes, the data must be splitted into 20 Byte packages and sent in the correct order. The end of a JSON data stream will be recongnized, once the '\n' character will be found at the end of a package. The *Commander* characteristic uses the same mechanism.



- **C**: *Wireless connection status* (RN) `e081fec3-f757-4449-b9c9-bfa83133f7fc`

    - *Description*: This characteristic represents the current state of the wireless adapter.
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

| Value  | Name         | Description
| ------ | ------------ | ----------------------------------------------------
| `0x00` | Unknown      | The device state is unknown.
| `0x01` | Unmanaged    | The device is recognized, but not managed by NetworkManager.
| `0x02` | Unavailable  | The device is managed by NetworkManager, but is not available for use (i.e. Wireless switched off, missing firmware).
| `0x03` | Disconnected | The device can be activated, but is currently idle and not connected to a network.
| `0x04` | Prepare      | The device is preparing the connection to the network.
| `0x05` | Config       | The device is connecting to the requested network (Associating with the WiFi accesspoint).
| `0x06` | NeedAuth     | The device requires more information to continue connecting to the requested network.
| `0x07` | IpConfig     | The device is requesting IPv4 and/or IPv6 addresses and routing information from the network.
| `0x08` | IpCheck      | The device is checking whether further action is required for the requested network connection. This may include checking whether only local network access is available, whether a captive portal is blocking access to the Internet.
| `0x09` | Secondaries  | The device is waiting for a secondary connection (like a VPN) which must activated before the device can be activated.
| `0x0A` | Activated    | The device has a network connection, either local or global.
| `0x0B` | Deactivating | A disconnection from the current network connection was requested, and the device is cleaning up resources used for that connection. The network connection may still be valid.
| `0x0C` | Failed       | The device failed to connect to the requested network and is cleaning up the connection request.


- **C**: *Wireless mode* (RN) `e081fec4-f757-4449-b9c9-bfa83133f7fc`

    - *Description*: This characteristic represents the current state of the wireless adapter.
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

| Value  | Name            | Description
| ------ | --------------- | ----------------------------------------------------
| `0x00` | Unknown         | The device mode is unknown.
| `0x01` | Adhoc           | For both devices and access point objects, indicates the object is part of an Ad-Hoc 802.11 network without a central coordinating access point.
| `0x02` | Infrastructure  | The device or access point is in infrastructure mode. For devices, this indicates the device is an 802.11 client/station.
| `0x03` | AccessPoint     | The device is an access point/hotspot.


### **S**: Network service `ef6d6610-b8af-49e0-9eca-ab343513641c`

This service allows to monitor and configure the `network-manager` daemon running on the system.

**Characteristic overview**

| Name                          | Characteristic UUID                    | Flag   |  Description
| ----------------------------- | -------------------------------------- | ------ | --------------------------------------------------------
| Network status                | `ef6d6611-b8af-49e0-9eca-ab343513641c` | **RN** | Represents the current network manager state.
| Network commander             | `ef6d6612-b8af-49e0-9eca-ab343513641c` | **W**  | Controll what the network manager should do.
| Commander response            | `ef6d6613-b8af-49e0-9eca-ab343513641c` | **N**  | This characteristic will be used to inform about the command result (error reporting).
| Networking enabled            | `ef6d6614-b8af-49e0-9eca-ab343513641c` | **RN** | This characteristic indicates if the networking in the `network-manager` is enabled.
| Wireless enabled              | `ef6d6615-b8af-49e0-9eca-ab343513641c` | **RN** | This characteristic indicates if the wireless networking in the `network-manager` is enabled.


**Characteristic details**

- **C**: *Network status* (RN) `ef6d6611-b8af-49e0-9eca-ab343513641c`

    - *Description*: Represents the current network manager state.
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

| Value  | Name            | Description
| ------ | --------------- | ----------------------------------------------------
| `0x00` | Unknown         | The networking state is unknown.
| `0x01` | Asleep          | Networking is not enabled.
| `0x02` | Disconnected    | There is no active network connection.
| `0x03` | Disconnecting   | Network connections are being cleaned up.
| `0x04` | Connecting      | A network connection is being started.
| `0x05` | Local           | There is only local IPv4 and/or IPv6 connectivity
| `0x06` | ConnectedSite   | There is only site-wide IPv4 and/or IPv6 connectivity
| `0x07` | ConnectedGlobal | There is global IPv4 and/or IPv6 Internet connectivity


- **C**: *Network commander* (W) `ef6d6612-b8af-49e0-9eca-ab343513641c`

    - *Description*: Controll what the network manager should do.
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

| Value  | Name                | Description
| ------ | ------------------- | ----------------------------------------------------
| `0x00` | EnableNetworking    | Enable networking on the system.
| `0x01` | DisableNetworking   | Disable networking on the system.
| `0x02` | EnableWireless      | Enable wireless networking on the system.
| `0x03` | DisableWireless     | Disable wireless networking on the system.


- **C**: *Commander response* (N) `ef6d6613-b8af-49e0-9eca-ab343513641c`

    - *Description*: This characteristic will be used to inform about the command result (error reporting).
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

| Value  | Name                       | Description
| ------ | -------------------------- | ----------------------------------------------------
| `0x00` | Success                    | The command has been executed successfully.
| `0x01` | InvalidValue               | The commander received an invalid command.
| `0x02` | NetworkManagerNotAvailable | Network manager is currently not available (`network-manager` daemon or dbus not running).
| `0x03` | WirelessNotAvailable       | Wireless currently not available. There is no wireless adapter in the system.
| `0x04` | Unknown                    | An unknown error happend.


- **C**: *Networking enabled* (RN) `ef6d6614-b8af-49e0-9eca-ab343513641c`

    - *Description*: This characteristic indicates if the networking in the `network-manager` is enabled.
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

        - `0x00` : Disabled
        - `0x01` : Enabled

- **C**: *Wireless enabled* (RN) `ef6d6615-b8af-49e0-9eca-ab343513641c`

    - *Description*: This characteristic indicates if the wireless networking in the `network-manager` is enabled.
    - *Range*: 1 Byte, Hex value
    - *Possible values*:

        - `0x00` : Disabled
        - `0x01` : Enabled


