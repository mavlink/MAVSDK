# mavsdk::Mavsdk Class Reference
`#include: mavsdk.h`

----


This is the main class of MAVSDK (a MAVLink API Library). 


It is used to discover vehicles and manage active connections.


An instance of this class must be created (first) in order to use the library. The instance must be destroyed after use in order to break connections and release all resources. 


## Data Structures


struct [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md)

## Public Types


Type | Description
--- | ---
enum [ComponentType](#classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746) | ComponentType of configurations, used for automatic ID setting.
[Handle](classmavsdk_1_1_handle.md)<> [ConnectionHandle](#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) | [Handle](classmavsdk_1_1_handle.md) type to remove a connection.
std::function< void()> [NewSystemCallback](#classmavsdk_1_1_mavsdk_1a7a283c6a75e852a56be4c5862f8a3fab) | Callback type discover and timeout notifications.
[Handle](classmavsdk_1_1_handle.md)<> [NewSystemHandle](#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) | [Handle](classmavsdk_1_1_handle.md) type to unsubscribe from subscribe_on_new_system.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Mavsdk](#classmavsdk_1_1_mavsdk_1ab10a870a7b8d1f8dae5c53c23c3fc596) ()=delete | Default constructor without configuration, no longer recommended.
&nbsp; | [Mavsdk](#classmavsdk_1_1_mavsdk_1a02b160aa2717f7064d2517e00065e6ac) ([Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) configuration) | Constructor with configuration.
&nbsp; | [~Mavsdk](#classmavsdk_1_1_mavsdk_1ac1549f715d6857711b9b9e364a4ca351) () | Destructor.
std::string | [version](#classmavsdk_1_1_mavsdk_1a8fdb97695762d06fd2bccfc6309943fa) () const | Returns the version of MAVSDK.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [add_any_connection](#classmavsdk_1_1_mavsdk_1a405041a5043c610c86540de090626d97) (const std::string & connection_url, [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds Connection via URL.
std::pair< [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992), [ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) > | [add_any_connection_with_handle](#classmavsdk_1_1_mavsdk_1ad1dc8afaafea8f7e900c990552e6fdfb) (const std::string & connection_url, [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds Connection via URL Additionally returns a handle to remove the connection later.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [add_udp_connection](#classmavsdk_1_1_mavsdk_1aa43dfb00d5118d26ae5aabd0f9ba56b2) (int local_port=[DEFAULT_UDP_PORT](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1affddcc7c7849ed86a0c7dab1166e657a), [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds a UDP connection to the specified port number.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [add_udp_connection](#classmavsdk_1_1_mavsdk_1a98fd1c01bd366b27084810875a1b94c1) (const std::string & local_ip, int local_port=[DEFAULT_UDP_PORT](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1affddcc7c7849ed86a0c7dab1166e657a), [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds a UDP connection to the specified port number and local interface.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [setup_udp_remote](#classmavsdk_1_1_mavsdk_1adb2a69282a5d3766fd6251662c28616d) (const std::string & remote_ip, int remote_port, [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Sets up instance to send heartbeats to the specified remote interface and port number.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [add_tcp_connection](#classmavsdk_1_1_mavsdk_1a50ecebb9ee710b9a044d2d5eb57645e4) (const std::string & remote_ip, int remote_port=[DEFAULT_TCP_REMOTE_PORT](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a52a6a9e0acd6c0ade566208d253427bd), [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds a TCP connection with a specific IP address and port number.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [add_serial_connection](#classmavsdk_1_1_mavsdk_1a669ddeec7af571fdbde9f31e343d50ac) (const std::string & dev_path, int baudrate=[DEFAULT_SERIAL_BAUDRATE](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a870d15142914f1db564c12f385d5489b), bool flow_control=false, [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds a serial connection with a specific port (COM or UART dev node) and baudrate as specified.
void | [remove_connection](#classmavsdk_1_1_mavsdk_1a23cf630bb123aa53b0e99c6bd83ad013) ([ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) handle) |
std::vector< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > | [systems](#classmavsdk_1_1_mavsdk_1a0d0bc4cdab14d96877b52baec5113fa8) () const | Get a vector of systems which have been discovered or set-up.
std::optional< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > | [first_autopilot](#classmavsdk_1_1_mavsdk_1a30b5c588643d19f1cfa9a7be47acb52c) (double timeout_s)const | Get the first autopilot that has been discovered.
void | [set_configuration](#classmavsdk_1_1_mavsdk_1acaeea86253493dc15b6540d2100a1b86) ([Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) configuration) | Set [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) of SDK.
void | [set_timeout_s](#classmavsdk_1_1_mavsdk_1a765f37b61462addcfd961e720585d2c6) (double timeout_s) | Set timeout of MAVLink transfers.
void | [set_system_status](#classmavsdk_1_1_mavsdk_1a693eac9fce2e9a330a3b9213ac9faae4) (uint8_t system_status) | Set system status of this MAVLink entity.
[NewSystemHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) | [subscribe_on_new_system](#classmavsdk_1_1_mavsdk_1a5b7c958ad2e4529dc7b950ab26618575) (const [NewSystemCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a7a283c6a75e852a56be4c5862f8a3fab) & callback) | Get notification about a change in systems.
void | [unsubscribe_on_new_system](#classmavsdk_1_1_mavsdk_1ad7f77f1295a700ee73cccc345019c1ff) ([NewSystemHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) handle) | unsubscribe from subscribe_on_new_system.
std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > | [server_component](#classmavsdk_1_1_mavsdk_1a5ad97e4af7de0f253cca8af7e76b68b6) (unsigned instance=0) | Get server component with default type of [Mavsdk](classmavsdk_1_1_mavsdk.md) instance.
std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > | [server_component_by_type](#classmavsdk_1_1_mavsdk_1a669a7ec5ed40dec9fde7e7a080fa7f2d) ([ComponentType](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746) component_type, unsigned instance=0) | Get server component by a high level type.
std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > | [server_component_by_id](#classmavsdk_1_1_mavsdk_1a117e6dc31b9124b6d535aa363283f8d7) (uint8_t component_id) | Get server component by the low MAVLink component ID.
void | [intercept_incoming_messages_async](#classmavsdk_1_1_mavsdk_1ac80c8909958533131cbdbc61d061794f) (std::function< bool(mavlink_message_t &)> callback) | Intercept incoming messages.
void | [intercept_outgoing_messages_async](#classmavsdk_1_1_mavsdk_1a040ee5c1d41e71c0d63cf8f76d2db275) (std::function< bool(mavlink_message_t &)> callback) | Intercept outgoing messages.

## Static Public Attributes


static constexpr auto [DEFAULT_UDP_BIND_IP](#classmavsdk_1_1_mavsdk_1ac46b2c27d9c428ec46092f10774482fa) = "0.0.0.0" - Default UDP bind IP (accepts any incoming connections).


static constexpr int [DEFAULT_UDP_PORT](#classmavsdk_1_1_mavsdk_1affddcc7c7849ed86a0c7dab1166e657a) = 14540 - Default UDP bind port (same port as used by MAVROS).


static constexpr auto [DEFAULT_TCP_REMOTE_IP](#classmavsdk_1_1_mavsdk_1a0154aac9d933fa212a50dc687816fbad) = "127.0.0.1" - Default TCP remote IP (localhost).


static constexpr int [DEFAULT_TCP_REMOTE_PORT](#classmavsdk_1_1_mavsdk_1a52a6a9e0acd6c0ade566208d253427bd) = 5760 - Default TCP remote port.


static constexpr int [DEFAULT_SERIAL_BAUDRATE](#classmavsdk_1_1_mavsdk_1a870d15142914f1db564c12f385d5489b) = 57600 - Default serial baudrate.


static constexpr double [DEFAULT_TIMEOUT_S](#classmavsdk_1_1_mavsdk_1a74f7b4d32d9551bb9c11ce8668f634a6) = 0.5 - Default internal timeout in seconds.


## Constructor & Destructor Documentation


### Mavsdk() {#classmavsdk_1_1_mavsdk_1ab10a870a7b8d1f8dae5c53c23c3fc596}
```cpp
mavsdk::Mavsdk::Mavsdk()=delete
```


Default constructor without configuration, no longer recommended.

> **Note** This has been removed because MAVSDK used to identify itself as a ground station by default which isn't always the safest choice. For instance, when MAVSDK is used on a companion computer (set as a ground station) it means that the appropriate failsafe doesn't trigger.

### Mavsdk() {#classmavsdk_1_1_mavsdk_1a02b160aa2717f7064d2517e00065e6ac}
```cpp
mavsdk::Mavsdk::Mavsdk(Configuration configuration)
```


Constructor with configuration.


**Parameters**

* [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) **configuration** - [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) to use in MAVSDK instance.

### ~Mavsdk() {#classmavsdk_1_1_mavsdk_1ac1549f715d6857711b9b9e364a4ca351}
```cpp
mavsdk::Mavsdk::~Mavsdk()
```


Destructor.

Disconnects all connected vehicles and releases all resources.

## Member Typdef Documentation


### typedef ConnectionHandle {#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f}

```cpp
using mavsdk::Mavsdk::ConnectionHandle =  Handle<>
```


[Handle](classmavsdk_1_1_handle.md) type to remove a connection.


### typedef NewSystemCallback {#classmavsdk_1_1_mavsdk_1a7a283c6a75e852a56be4c5862f8a3fab}

```cpp
using mavsdk::Mavsdk::NewSystemCallback =  std::function<void()>
```


Callback type discover and timeout notifications.


### typedef NewSystemHandle {#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c}

```cpp
using mavsdk::Mavsdk::NewSystemHandle =  Handle<>
```


[Handle](classmavsdk_1_1_handle.md) type to unsubscribe from subscribe_on_new_system.


## Member Enumeration Documentation


### enum ComponentType {#classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746}


ComponentType of configurations, used for automatic ID setting.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746a6ca1d2b081cc474f42cb95e3d04e6e68"></span> `Autopilot` | SDK is used as an autopilot. 
<span id="classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746af64f82089eddc6133add8c55c65d6687"></span> `GroundStation` | SDK is used as a ground station. 
<span id="classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746a8f2f82e1a7aa48819e9530d5c4977477"></span> `CompanionComputer` | SDK is used as a companion computer on board the MAV. 
<span id="classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746a967d35e40f3f95b1f538bd248640bf3b"></span> `Camera` |  SDK is used as a camera.
<span id="classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746a90589c47f06eb971d548591f23c285af"></span> `Custom` |  SDK is used in a custom configuration, no automatic ID will be provided.

## Member Function Documentation


### version() {#classmavsdk_1_1_mavsdk_1a8fdb97695762d06fd2bccfc6309943fa}
```cpp
std::string mavsdk::Mavsdk::version() const
```


Returns the version of MAVSDK.

Note, you're not supposed to request the version too many times.

**Returns**

&emsp;std::string - A string containing the version.

### add_any_connection() {#classmavsdk_1_1_mavsdk_1a405041a5043c610c86540de090626d97}
```cpp
ConnectionResult mavsdk::Mavsdk::add_any_connection(const std::string &connection_url, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds Connection via URL.

Supports connection: Serial, TCP or UDP. Connection URL format should be:
<ul>
<li><p>UDP: udp://[host][:bind_port]</p>
</li>
<li><p>TCP: tcp://[host][:remote_port]</p>
</li>
<li><p>Serial: serial://dev_node[:baudrate]</p>
</li>
</ul>


For UDP, the host can be set to either:
<ul>
<li><p>zero IP: 0.0.0.0 -> behave like a server and listen for heartbeats.</p>
</li>
<li><p>some IP: 192.168.1.12 -> behave like a client, initiate connection and start sending heartbeats.</p>
</li>
</ul>

**Parameters**

* const std::string& **connection_url** - connection URL string.
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of adding the connection.

### add_any_connection_with_handle() {#classmavsdk_1_1_mavsdk_1ad1dc8afaafea8f7e900c990552e6fdfb}
```cpp
std::pair<ConnectionResult, ConnectionHandle> mavsdk::Mavsdk::add_any_connection_with_handle(const std::string &connection_url, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds Connection via URL Additionally returns a handle to remove the connection later.

Supports connection: Serial, TCP or UDP. Connection URL format should be:
<ul>
<li><p>UDP: udp://[host][:bind_port]</p>
</li>
<li><p>TCP: tcp://[host][:remote_port]</p>
</li>
<li><p>Serial: serial://dev_node[:baudrate]</p>
</li>
</ul>


For UDP, the host can be set to either:
<ul>
<li><p>zero IP: 0.0.0.0 -> behave like a server and listen for heartbeats.</p>
</li>
<li><p>some IP: 192.168.1.12 -> behave like a client, initiate connection and start sending heartbeats.</p>
</li>
</ul>

**Parameters**

* const std::string& **connection_url** - connection URL string.
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;std::pair< [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992), [ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) > - A pair containing the result of adding the connection as well as a handle to remove it later.

### add_udp_connection() {#classmavsdk_1_1_mavsdk_1aa43dfb00d5118d26ae5aabd0f9ba56b2}
```cpp
ConnectionResult mavsdk::Mavsdk::add_udp_connection(int local_port=DEFAULT_UDP_PORT, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds a UDP connection to the specified port number.

Any incoming connections are accepted (0.0.0.0).

**Parameters**

* int **local_port** - The local UDP port to listen to (defaults to 14540, the same as MAVROS).
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of adding the connection.

### add_udp_connection() {#classmavsdk_1_1_mavsdk_1a98fd1c01bd366b27084810875a1b94c1}
```cpp
ConnectionResult mavsdk::Mavsdk::add_udp_connection(const std::string &local_ip, int local_port=DEFAULT_UDP_PORT, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds a UDP connection to the specified port number and local interface.

To accept only local connections of the machine, use 127.0.0.1. For any incoming connections, use 0.0.0.0.

**Parameters**

* const std::string& **local_ip** - The local UDP IP address to listen to.
* int **local_port** - The local UDP port to listen to (defaults to 14540, the same as MAVROS).
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of adding the connection.

### setup_udp_remote() {#classmavsdk_1_1_mavsdk_1adb2a69282a5d3766fd6251662c28616d}
```cpp
ConnectionResult mavsdk::Mavsdk::setup_udp_remote(const std::string &remote_ip, int remote_port, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Sets up instance to send heartbeats to the specified remote interface and port number.


**Parameters**

* const std::string& **remote_ip** - The remote UDP IP address to report to.
* int **remote_port** - The local UDP port to report to.
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of operation.

### add_tcp_connection() {#classmavsdk_1_1_mavsdk_1a50ecebb9ee710b9a044d2d5eb57645e4}
```cpp
ConnectionResult mavsdk::Mavsdk::add_tcp_connection(const std::string &remote_ip, int remote_port=DEFAULT_TCP_REMOTE_PORT, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds a TCP connection with a specific IP address and port number.


**Parameters**

* const std::string& **remote_ip** - Remote IP address to connect to.
* int **remote_port** - The TCP port to connect to (defaults to 5760).
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of adding the connection.

### add_serial_connection() {#classmavsdk_1_1_mavsdk_1a669ddeec7af571fdbde9f31e343d50ac}
```cpp
ConnectionResult mavsdk::Mavsdk::add_serial_connection(const std::string &dev_path, int baudrate=DEFAULT_SERIAL_BAUDRATE, bool flow_control=false, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds a serial connection with a specific port (COM or UART dev node) and baudrate as specified.


**Parameters**

* const std::string& **dev_path** - COM or UART dev node name/path (e.g. "/dev/ttyS0", or "COM3" on Windows).
* int **baudrate** - Baudrate of the serial port (defaults to 57600).
* bool **flow_control** - enable/disable flow control.
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of adding the connection.

### remove_connection() {#classmavsdk_1_1_mavsdk_1a23cf630bb123aa53b0e99c6bd83ad013}
```cpp
void mavsdk::Mavsdk::remove_connection(ConnectionHandle handle)
```


Remove connection again.

**Parameters**

* [ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) **handle** - [Handle](classmavsdk_1_1_handle.md) returned when connection was added.

### systems() {#classmavsdk_1_1_mavsdk_1a0d0bc4cdab14d96877b52baec5113fa8}
```cpp
std::vector<std::shared_ptr<System> > mavsdk::Mavsdk::systems() const
```


Get a vector of systems which have been discovered or set-up.


**Returns**

&emsp;std::vector< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > - The vector of systems which are available.

### first_autopilot() {#classmavsdk_1_1_mavsdk_1a30b5c588643d19f1cfa9a7be47acb52c}
```cpp
std::optional<std::shared_ptr<System> > mavsdk::Mavsdk::first_autopilot(double timeout_s) const
```


Get the first autopilot that has been discovered.

> **Note** This requires a MAVLink component with component ID 1 sending heartbeats.

**Parameters**

* double **timeout_s** - A timeout in seconds. A timeout of 0 will not wait and return immediately. A negative timeout will wait forever.

**Returns**

&emsp;std::optional< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > - A system or nothing if nothing was discovered within the timeout.

### set_configuration() {#classmavsdk_1_1_mavsdk_1acaeea86253493dc15b6540d2100a1b86}
```cpp
void mavsdk::Mavsdk::set_configuration(Configuration configuration)
```


Set [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) of SDK.

The default configuration is `Configuration::GroundStation` The configuration is used in order to set the MAVLink system ID, the component ID, as well as the MAV_TYPE accordingly.

**Parameters**

* [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) **configuration** - [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) chosen.

### set_timeout_s() {#classmavsdk_1_1_mavsdk_1a765f37b61462addcfd961e720585d2c6}
```cpp
void mavsdk::Mavsdk::set_timeout_s(double timeout_s)
```


Set timeout of MAVLink transfers.

The default timeout used is generally DEFAULT_SERIAL_BAUDRATE (0.5 seconds) seconds. If MAVSDK is used on the same host this timeout can be reduced, while if MAVSDK has to communicate over links with high latency it might need to be increased to prevent timeouts.

**Parameters**

* double **timeout_s** - 

### set_system_status() {#classmavsdk_1_1_mavsdk_1a693eac9fce2e9a330a3b9213ac9faae4}
```cpp
void mavsdk::Mavsdk::set_system_status(uint8_t system_status)
```


Set system status of this MAVLink entity.

The default system status is MAV_STATE_UNINIT.

**Parameters**

* uint8_t **system_status** - system status.

### subscribe_on_new_system() {#classmavsdk_1_1_mavsdk_1a5b7c958ad2e4529dc7b950ab26618575}
```cpp
NewSystemHandle mavsdk::Mavsdk::subscribe_on_new_system(const NewSystemCallback &callback)
```


Get notification about a change in systems.

This gets called whenever a system is added.

**Parameters**

* const [NewSystemCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a7a283c6a75e852a56be4c5862f8a3fab)& **callback** - Callback to subscribe.

**Returns**

&emsp;[NewSystemHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) - A handle to unsubscribe again.

### unsubscribe_on_new_system() {#classmavsdk_1_1_mavsdk_1ad7f77f1295a700ee73cccc345019c1ff}
```cpp
void mavsdk::Mavsdk::unsubscribe_on_new_system(NewSystemHandle handle)
```


unsubscribe from subscribe_on_new_system.


**Parameters**

* [NewSystemHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) **handle** - [Handle](classmavsdk_1_1_handle.md) received on subscription.

### server_component() {#classmavsdk_1_1_mavsdk_1a5ad97e4af7de0f253cca8af7e76b68b6}
```cpp
std::shared_ptr<ServerComponent> mavsdk::Mavsdk::server_component(unsigned instance=0)
```


Get server component with default type of [Mavsdk](classmavsdk_1_1_mavsdk.md) instance.


**Parameters**

* unsigned **instance** - 

**Returns**

&emsp;std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > - A valid shared pointer to a server component if it was successful, an empty pointer otherwise.

### server_component_by_type() {#classmavsdk_1_1_mavsdk_1a669a7ec5ed40dec9fde7e7a080fa7f2d}
```cpp
std::shared_ptr<ServerComponent> mavsdk::Mavsdk::server_component_by_type(ComponentType component_type, unsigned instance=0)
```


Get server component by a high level type.

This represents a server component of the MAVSDK instance.

**Parameters**

* [ComponentType](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac9e9d48bbf840dad8705323b224b1746) **component_type** - The high level type of the component.
* unsigned **instance** - The instance of the component if there are multiple, starting at 0.

**Returns**

&emsp;std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > - A valid shared pointer to a server component if it was successful, an empty pointer otherwise.

### server_component_by_id() {#classmavsdk_1_1_mavsdk_1a117e6dc31b9124b6d535aa363283f8d7}
```cpp
std::shared_ptr<ServerComponent> mavsdk::Mavsdk::server_component_by_id(uint8_t component_id)
```


Get server component by the low MAVLink component ID.

This represents a server component of the MAVSDK instance.

**Parameters**

* uint8_t **component_id** - MAVLink component ID to use

**Returns**

&emsp;std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > - A valid shared pointer to a server component if it was successful, an empty pointer otherwise.

### intercept_incoming_messages_async() {#classmavsdk_1_1_mavsdk_1ac80c8909958533131cbdbc61d061794f}
```cpp
void mavsdk::Mavsdk::intercept_incoming_messages_async(std::function< bool(mavlink_message_t &)> callback)
```


Intercept incoming messages.

This is a hook which allows to change or drop MAVLink messages as they are received before they get forwarded any subscribers.


> **Note** This functionality is provided primarily for testing in order to simulate packet drops or actors not adhering to the MAVLink protocols.

**Parameters**

* std::function< bool(mavlink_message_t &)> **callback** - Callback to be called for each incoming message. To drop a message, return 'false' from the callback.

### intercept_outgoing_messages_async() {#classmavsdk_1_1_mavsdk_1a040ee5c1d41e71c0d63cf8f76d2db275}
```cpp
void mavsdk::Mavsdk::intercept_outgoing_messages_async(std::function< bool(mavlink_message_t &)> callback)
```


Intercept outgoing messages.

This is a hook which allows to change or drop MAVLink messages before they are sent.


> **Note** This functionality is provided primarily for testing in order to simulate packet drops or actors not adhering to the MAVLink protocols.

**Parameters**

* std::function< bool(mavlink_message_t &)> **callback** - Callback to be called for each outgoing message. To drop a message, return 'false' from the callback.

## Field Documentation


### DEFAULT_UDP_BIND_IP {#classmavsdk_1_1_mavsdk_1ac46b2c27d9c428ec46092f10774482fa}

```cpp
constexpr auto mavsdk::Mavsdk::DEFAULT_UDP_BIND_IP = "0.0.0.0"
```


Default UDP bind IP (accepts any incoming connections).


### DEFAULT_UDP_PORT {#classmavsdk_1_1_mavsdk_1affddcc7c7849ed86a0c7dab1166e657a}

```cpp
constexpr int mavsdk::Mavsdk::DEFAULT_UDP_PORT = 14540
```


Default UDP bind port (same port as used by MAVROS).


### DEFAULT_TCP_REMOTE_IP {#classmavsdk_1_1_mavsdk_1a0154aac9d933fa212a50dc687816fbad}

```cpp
constexpr auto mavsdk::Mavsdk::DEFAULT_TCP_REMOTE_IP = "127.0.0.1"
```


Default TCP remote IP (localhost).


### DEFAULT_TCP_REMOTE_PORT {#classmavsdk_1_1_mavsdk_1a52a6a9e0acd6c0ade566208d253427bd}

```cpp
constexpr int mavsdk::Mavsdk::DEFAULT_TCP_REMOTE_PORT = 5760
```


Default TCP remote port.


### DEFAULT_SERIAL_BAUDRATE {#classmavsdk_1_1_mavsdk_1a870d15142914f1db564c12f385d5489b}

```cpp
constexpr int mavsdk::Mavsdk::DEFAULT_SERIAL_BAUDRATE = 57600
```


Default serial baudrate.


### DEFAULT_TIMEOUT_S {#classmavsdk_1_1_mavsdk_1a74f7b4d32d9551bb9c11ce8668f634a6}

```cpp
constexpr double mavsdk::Mavsdk::DEFAULT_TIMEOUT_S = 0.5
```


Default internal timeout in seconds.

