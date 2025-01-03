# Connecting to Systems (Vehicles)

MAVSDK allows you to connect to multiple vehicles/systems attached to the local WiFi network and/or via serial ports.

In order to detect systems you must first specify the communication ports that MAVSDK will monitor for new systems.
Once monitoring a port, MAVSDK will automatically detect connected vehicles, add them to its collection, and notify registered users of connection and disconnection events.

## Monitoring a Port

Specify the port(s) to watch using one of the (synchronous) connection method: [add_any_connection()](../api_reference/classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a405041a5043c610c86540de090626d97). The method returns immediately with a [ConnectionResult](../api_reference/namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) indicating whether it succeeded.

The connection details are specified using the string formats shown below:

Connection | URL Format
--- | ---
UDP | `udp://[host][:port]`
TCP | `tcp://[host][:port]`
Serial | `serial://[path][:baudrate]`

### Connecting over serial

To add a serial connection (e.g. over USB, FTDI, or an SiK radio), you specify the serial port and the baudrate like this:

**On Linux:**

```cpp
Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
ConnectionResult connection_result = mavsdk.add_any_connection("serial:///dev/serial/by-id/usb-FTDI_FT232R_USB_UART_XXXXXXXX-if00-port0:57600");
if (connection_result != ConnectionResult::Success) {
    std::cout << "Adding connection failed: " << connection_result << '\n';
    return;
}
```

**On Windows:**

```cpp
Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
ConnectionResult connection_result = mavsdk.add_any_connection("serial://COM3:57600");
if (connection_result != ConnectionResult::Success) {
    std::cout << "Adding connection failed: " << connection_result << '\n';
    return;
}
```

### Connecting over UDP

When connecting over UDP, there are two setups to distinguish: server and client mode.

#### Behave like a server

This is the default connection mode for a ground station listening to a system/vehicle (this is the same as [add_udp_connection()](../api_reference/classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1aa43dfb00d5118d26ae5aabd0f9ba56b2)).

In the server mode, we listen on an local networking interface (`INADDR_ANY`/`0.0.0.0`) on the set port and wait for any heartbeats arriving.
This means that the drone has to send the UDP packets to that local IP, or broadcast them on the network.

The code snippet below shows how to set up a connection in server mode and listen on the "SDK port 14540":

```cpp
Mavsdk mavsdk;
ConnectionResult connection_result = mavsdk.add_any_connection("udp://:14540");
if (connection_result != ConnectionResult::Success) {
    std::cout << "Adding connection failed: " << connection_result << '\n';
    return;
}
```

> **Note** The connection string used above (`udp://:14540`) is to the [standard PX4 UDP port](https://docs.px4.io/master/en/simulation/#default-px4-mavlink-udp-ports) for off-board APIs (14540).
  This is the normal/most common way for offboard APIs to connect to PX4 over WiFi.
  The standard way to talk to a ground station (e.g. QGC is on port 14550).

#### Behave like a client

If MAVSDK is used on the vehicle or companion side, it sometimes has to actually initiate the connection, rather than listen for it.
In this case the IP and port, of where it should connect to, has to be set (this is the same as [setup_udp_remote()](../api_reference/classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1adb2a69282a5d3766fd6251662c28616d)). 

E.g. to connect to a ground station on 192.168.1.12, you would do:

```cpp
Mavsdk mavsdk;
ConnectionResult connection_result = mavsdk.add_any_connection("udp://192.168.1.12:14550");
if (connection_result != ConnectionResult::Success) {
    std::cout << "Adding connection failed: " << connection_result << '\n';
    return;
}
```

E.g. to listen locally to a mavlink-router endpoint at 14551, you would do:


```cpp
Mavsdk mavsdk;
ConnectionResult connection_result = mavsdk.add_any_connection("udp://:14551");
if (connection_result != ConnectionResult::Success) {
    std::cout << "Adding connection failed: " << connection_result << '\n';
    return;
}
```

### Connecting over TCP

For TCP connections, only the client connection is currently implemented.
This is the same as [add_tcp_connection()](../api_reference/classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a91c7a70c6e8ffa43844f2ce04f2696f0).

```cpp
Mavsdk mavsdk;
ConnectionResult connection_result = mavsdk.add_any_connection("tcp://192.168.1.12:14550");
if (connection_result != ConnectionResult::Success) {
    std::cout << "Adding connection failed: " << connection_result << '\n';
    return;
}
```

### Register for System-Detection Notifications

MAVSDK monitors any added communication ports for new systems.
Clients can register for notification when new systems are discovered using [subscribe_on_new_system()](../api_reference/classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a332ae41c7de84d5cfab11fb3e2e65522).

The code fragment below shows how to register a callback (in this case the callback is a lambda function that just prints if a new vehicle has been discovered.

```cpp
Mavsdk mavsdk;
... //add ports
mavsdk.register_on_new_system([]() {
    std::cout << "Discovered new system\n";
});
```

## Iterating all Systems

You can iterate all systems that have been detected by `Mavsdk` (since it was started, over all communication ports) using the [systems()](../api_reference/classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a0d0bc4cdab14d96877b52baec5113fa8) method.
This returns a vector of shared pointers to systems.

The following code fragment shows how to iterate through the systems and checking their MAVLink system ID, whether they are connected, and what components they have:

```cpp
//Iterate through detected systems
for (auto system : mavsdk.systems()) {
    std::cout << "Found system with MAVLink system ID: " << static_cast<int>(system->get_system_id())
              << ", connected: " << (system->is_connected() ? "yes" : "no")
              << ", has autopilot: " << (system->has_autopilot() ? "yes" : "no") << '\n';
}
```

## Accessing Systems

To access a certain system, pick the one from the vector that you require, or use the first one if only one system is assumed:


```cpp
Mavsdk mavsdk;
mavsdk.add_udp_connection();
// Wait for the system to connect via heartbeat
while (mavsdk.systems().size() == 0) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
// System got discovered.
std::shared_ptr<System> system = mavsdk.systems()[0];
```

The `System` is used by the MAVSDK plugin classes to query and control the vehicle.
For more information see [Using Plugins](../guide/using_plugins.md) (and the other guide topics).

## Forwarding MAVLink between connections

It is possible to add multiple connections to MAVSDK and forward all MAVLink messages. To do so, each connection which should forward messages needs to have forwarding set to on.

To forward bi-directional from UDP to serial and serial to UDP, you would set both connections forwarding:

```cpp
Mavsdk mavsdk;
mavsdk.add_any_connection("udp://:14540", ForwardingOption::ForwardingOn);
mavsdk.add_any_connection("serial:///dev/serial/by-id/usb-FTDI_FT232R_USB_UART_XXXXXXXX-if00-port0:57600", ForwardingOption::ForwardingOn);
```

To forward only in one direction, e.g to send messages arriving on serial over UDP, you would only set up forwarding for the UDP connection:

```cpp
Mavsdk mavsdk;
mavsdk.add_any_connection("udp://:14540", ForwardingOption::ForwardingOn);
mavsdk.add_any_connection("serial:///dev/serial/by-id/usb-FTDI_FT232R_USB_UART_XXXXXXXX-if00-port0:57600", `ForwardingOption::ForwardingOff`);
```

Note that the default function overload is `ForwardingOption::ForwardingOff`.
