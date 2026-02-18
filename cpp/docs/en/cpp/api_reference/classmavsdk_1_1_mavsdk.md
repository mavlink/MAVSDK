# mavsdk::Mavsdk Class Reference
`#include: mavsdk.h`

----


This is the main class of MAVSDK (a MAVLink API Library). 


It is used to discover vehicles and manage active connections.


An instance of this class must be created and kept alive in order to use the library. The instance can be destroyed after use in order to break connections and release all resources. 


## Data Structures


struct [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md)

struct [ConnectionError](structmavsdk_1_1_mavsdk_1_1_connection_error.md)

struct [MavlinkMessage](structmavsdk_1_1_mavsdk_1_1_mavlink_message.md)

## Public Types


Type | Description
--- | ---
[Handle](classmavsdk_1_1_handle.md)<> [ConnectionHandle](#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) | [Handle](classmavsdk_1_1_handle.md) type to remove a connection.
std::function< void([ConnectionError](structmavsdk_1_1_mavsdk_1_1_connection_error.md))> [ConnectionErrorCallback](#classmavsdk_1_1_mavsdk_1ac71d182538bee6fb3ab007edafac5d5f) | 
[Handle](classmavsdk_1_1_handle.md)< [ConnectionError](structmavsdk_1_1_mavsdk_1_1_connection_error.md) > [ConnectionErrorHandle](#classmavsdk_1_1_mavsdk_1aeb442a462d03662e4c152509fd0c203b) | [Handle](classmavsdk_1_1_handle.md) type to remove a connection error subscription.
std::function< void()> [NewSystemCallback](#classmavsdk_1_1_mavsdk_1a7a283c6a75e852a56be4c5862f8a3fab) | Callback type discover and timeout notifications.
[Handle](classmavsdk_1_1_handle.md)<> [NewSystemHandle](#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) | [Handle](classmavsdk_1_1_handle.md) type to unsubscribe from subscribe_on_new_system.
[Handle](classmavsdk_1_1_handle.md)< bool([MavlinkMessage](structmavsdk_1_1_mavsdk_1_1_mavlink_message.md))> [InterceptJsonHandle](#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) | [Handle](classmavsdk_1_1_handle.md) for intercepting messages.
std::function< bool([MavlinkMessage](structmavsdk_1_1_mavsdk_1_1_mavlink_message.md))> [InterceptJsonCallback](#classmavsdk_1_1_mavsdk_1a17923db3b1504e911487729114b68f48) | Callback type for intercepting messages.
std::function< void(const char *bytes, size_t length)> [RawBytesCallback](#classmavsdk_1_1_mavsdk_1acb5be9a1be97d251387ffe87ae8b9eb0) | Callback type for raw bytes subscriptions.
[Handle](classmavsdk_1_1_handle.md)< const char *, size_t > [RawBytesHandle](#classmavsdk_1_1_mavsdk_1ac766258f137aa3e8b0dabb5a66435ea1) | [Handle](classmavsdk_1_1_handle.md) type for raw bytes subscriptions.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Mavsdk](#classmavsdk_1_1_mavsdk_1ab10a870a7b8d1f8dae5c53c23c3fc596) ()=delete | Default constructor without configuration, no longer recommended.
&nbsp; | [Mavsdk](#classmavsdk_1_1_mavsdk_1a02b160aa2717f7064d2517e00065e6ac) ([Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) configuration) | Constructor with configuration.
&nbsp; | [~Mavsdk](#classmavsdk_1_1_mavsdk_1ac1549f715d6857711b9b9e364a4ca351) () | Destructor.
std::string | [version](#classmavsdk_1_1_mavsdk_1a8fdb97695762d06fd2bccfc6309943fa) () const | Returns the version of MAVSDK.
[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | [add_any_connection](#classmavsdk_1_1_mavsdk_1a405041a5043c610c86540de090626d97) (const std::string & connection_url, [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds Connection via URL.
std::pair< [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992), [ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) > | [add_any_connection_with_handle](#classmavsdk_1_1_mavsdk_1ad12297d30ec1bbe4750f7cf4efbbed57) (const std::string & connection_url, [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) forwarding_option=ForwardingOption::ForwardingOff) | Adds Connection via URL Additionally returns a handle to remove the connection later.
void | [remove_connection](#classmavsdk_1_1_mavsdk_1a23cf630bb123aa53b0e99c6bd83ad013) ([ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) handle) |
[ConnectionErrorHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1aeb442a462d03662e4c152509fd0c203b) | [subscribe_connection_errors](#classmavsdk_1_1_mavsdk_1a8f50fffac65a35857a88da2fd42f9c54) ([ConnectionErrorCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac71d182538bee6fb3ab007edafac5d5f) callback) |
void | [unsubscribe_connection_errors](#classmavsdk_1_1_mavsdk_1a377ec6517ee75981ceb2f26b30e59fbd) ([ConnectionErrorHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1aeb442a462d03662e4c152509fd0c203b) handle) |
std::vector< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > | [systems](#classmavsdk_1_1_mavsdk_1aca9c72b300d384341b00ff9ba2c6e5c5) () const | Get a vector of systems which have been discovered or set-up.
std::optional< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > | [first_autopilot](#classmavsdk_1_1_mavsdk_1aa1bcb865693dbd140478e75ce58699b7) (double timeout_s)const | Get the first autopilot that has been discovered.
void | [set_configuration](#classmavsdk_1_1_mavsdk_1acaeea86253493dc15b6540d2100a1b86) ([Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) configuration) | Set [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) of SDK.
void | [set_timeout_s](#classmavsdk_1_1_mavsdk_1a765f37b61462addcfd961e720585d2c6) (double timeout_s) | Set timeout of MAVLink transfers.
void | [set_heartbeat_timeout_s](#classmavsdk_1_1_mavsdk_1afbab63cf2a795e4ca7262836d5fe4b46) (double timeout_s) | Set heartbeat timeout.
double | [get_heartbeat_timeout_s](#classmavsdk_1_1_mavsdk_1a6179b858f74415251ef43da11bc6edbc) () const | Get heartbeat timeout.
void | [set_callback_executor](#classmavsdk_1_1_mavsdk_1a5de0ff39a51efe3b235fe022e6b58034) (std::function< void(std::function< void()>)> executor) | Set a custom callback executor.
[NewSystemHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) | [subscribe_on_new_system](#classmavsdk_1_1_mavsdk_1a5b7c958ad2e4529dc7b950ab26618575) (const [NewSystemCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a7a283c6a75e852a56be4c5862f8a3fab) & callback) | Get notification about a change in systems.
void | [unsubscribe_on_new_system](#classmavsdk_1_1_mavsdk_1ad7f77f1295a700ee73cccc345019c1ff) ([NewSystemHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ae0727f2bed9cbf276d161ada0a432b8c) handle) | unsubscribe from subscribe_on_new_system.
std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > | [server_component](#classmavsdk_1_1_mavsdk_1a693a2f665c35d6b01d6144819d353280) (unsigned instance=0) | Get server component with default type of [Mavsdk](classmavsdk_1_1_mavsdk.md) instance.
std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > | [server_component_by_type](#classmavsdk_1_1_mavsdk_1a547fc7e18434473ea3e0e51ab3305abb) ([ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) component_type, unsigned instance=0) | Get server component by a high level type.
std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > | [server_component_by_id](#classmavsdk_1_1_mavsdk_1adef7d0d7422bcddbda629a404fb33ae2) (uint8_t component_id) | Get server component by the low MAVLink component ID.
[InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) | [subscribe_incoming_messages_json](#classmavsdk_1_1_mavsdk_1a722fce6f1abe8337721dde710b5b40d7) (const [InterceptJsonCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a17923db3b1504e911487729114b68f48) & callback) | Intercept incoming messages as JSON.
void | [unsubscribe_incoming_messages_json](#classmavsdk_1_1_mavsdk_1a4be244c38939cb517c2061baf4d43386) ([InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) handle) | Unsubscribe from incoming messages as JSON.
[InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) | [subscribe_outgoing_messages_json](#classmavsdk_1_1_mavsdk_1a58f85b2f74a32404a8e975feefed8f47) (const [InterceptJsonCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a17923db3b1504e911487729114b68f48) & callback) | Intercept outgoing messages as JSON.
void | [unsubscribe_outgoing_messages_json](#classmavsdk_1_1_mavsdk_1aa3a490358db87cfed617cdad902bb753) ([InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) handle) | Unsubscribe from outgoing messages as JSON.
void | [intercept_incoming_messages_async](#classmavsdk_1_1_mavsdk_1ac80c8909958533131cbdbc61d061794f) (std::function< bool(mavlink_message_t &)> callback) | Intercept incoming messages.
void | [intercept_outgoing_messages_async](#classmavsdk_1_1_mavsdk_1a040ee5c1d41e71c0d63cf8f76d2db275) (std::function< bool(mavlink_message_t &)> callback) | Intercept outgoing messages.
void | [pass_received_raw_bytes](#classmavsdk_1_1_mavsdk_1a65329315ac07bae110839d9e054fbc05) (const char * bytes, size_t length) | Pass received raw MAVLink bytes.
[RawBytesHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac766258f137aa3e8b0dabb5a66435ea1) | [subscribe_raw_bytes_to_be_sent](#classmavsdk_1_1_mavsdk_1a116e9bab0efdf7ec90866107ef517b20) ([RawBytesCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1acb5be9a1be97d251387ffe87ae8b9eb0) callback) | Subscribe to raw bytes to be sent.
void | [unsubscribe_raw_bytes_to_be_sent](#classmavsdk_1_1_mavsdk_1af6ec813a9728f4258056fa1f5d399eb1) ([RawBytesHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac766258f137aa3e8b0dabb5a66435ea1) handle) | Unsubscribe from raw bytes to be sent.


## Constructor & Destructor Documentation


### Mavsdk() {#classmavsdk_1_1_mavsdk_1ab10a870a7b8d1f8dae5c53c23c3fc596}
```cpp
mavsdk::Mavsdk::Mavsdk()=delete
```


Default constructor without configuration, no longer recommended.

::: info
This has been removed because MAVSDK used to identify itself as a ground station by default which isn't always the safest choice. For instance, when MAVSDK is used on a companion computer (set as a ground station) it means that the appropriate failsafe doesn't trigger.
:::

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


### typedef ConnectionErrorCallback {#classmavsdk_1_1_mavsdk_1ac71d182538bee6fb3ab007edafac5d5f}

```cpp
using mavsdk::Mavsdk::ConnectionErrorCallback =  std::function<void(ConnectionError)>
```


Connection Error callback type

### typedef ConnectionErrorHandle {#classmavsdk_1_1_mavsdk_1aeb442a462d03662e4c152509fd0c203b}

```cpp
using mavsdk::Mavsdk::ConnectionErrorHandle =  Handle<ConnectionError>
```


[Handle](classmavsdk_1_1_handle.md) type to remove a connection error subscription.


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


### typedef InterceptJsonHandle {#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f}

```cpp
using mavsdk::Mavsdk::InterceptJsonHandle =  Handle<bool(MavlinkMessage)>
```


[Handle](classmavsdk_1_1_handle.md) for intercepting messages.


### typedef InterceptJsonCallback {#classmavsdk_1_1_mavsdk_1a17923db3b1504e911487729114b68f48}

```cpp
using mavsdk::Mavsdk::InterceptJsonCallback =  std::function<bool(MavlinkMessage)>
```


Callback type for intercepting messages.


### typedef RawBytesCallback {#classmavsdk_1_1_mavsdk_1acb5be9a1be97d251387ffe87ae8b9eb0}

```cpp
using mavsdk::Mavsdk::RawBytesCallback =  std::function<void(const char* bytes, size_t length)>
```


Callback type for raw bytes subscriptions.


### typedef RawBytesHandle {#classmavsdk_1_1_mavsdk_1ac766258f137aa3e8b0dabb5a66435ea1}

```cpp
using mavsdk::Mavsdk::RawBytesHandle =  Handle<const char*, size_t>
```


[Handle](classmavsdk_1_1_handle.md) type for raw bytes subscriptions.


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
<li><p>UDP in (server): udpin://our_ip:port</p>
</li>
<li><p>UDP out (client): udpout://remote_ip:port</p>
</li>
<li><p>TCP in (server): tcpin://our_ip:port</p>
</li>
<li><p>TCP out (client): tcpout://remote_ip:port</p>
</li>
<li><p>Serial: serial://dev_node:baudrate</p>
</li>
<li><p>Serial with flow control: serial_flowcontrol://dev_node:baudrate</p>
</li>
</ul>


For UDP in and TCP in (as server), our IP can be set to:
<ul>
<li><p>0.0.0.0: listen on all interfaces</p>
</li>
<li><p>127.0.0.1: listen on loopback (local) interface only</p>
</li>
<li><p>Our IP: (e.g. 192.168.1.12): listen only on the network interface with this IP.</p>
</li>
</ul>


For UDP out and TCP out, the IP needs to be set to the remote IP, where the MAVLink messages are to be sent to.

**Parameters**

* const std::string& **connection_url** - connection URL string.
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;[ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) - The result of adding the connection.

### add_any_connection_with_handle() {#classmavsdk_1_1_mavsdk_1ad12297d30ec1bbe4750f7cf4efbbed57}
```cpp
std::pair< ConnectionResult, ConnectionHandle > mavsdk::Mavsdk::add_any_connection_with_handle(const std::string &connection_url, ForwardingOption forwarding_option=ForwardingOption::ForwardingOff)
```


Adds Connection via URL Additionally returns a handle to remove the connection later.

Supports connection: Serial, TCP or UDP. Connection URL format should be:


<ul>
<li><p>UDP in (server): udpin://our_ip:port</p>
</li>
<li><p>UDP out (client): udpout://remote_ip:port</p>
</li>
<li><p>TCP in (server): tcpin://our_ip:port</p>
</li>
<li><p>TCP out (client): tcpout://remote_ip:port</p>
</li>
<li><p>Serial: serial://dev_node:baudrate</p>
</li>
<li><p>Serial with flow control: serial_flowcontrol://dev_node:baudrate</p>
</li>
</ul>


For UDP in and TCP in (as server), our IP can be set to:
<ul>
<li><p>0.0.0.0: listen on all interfaces</p>
</li>
<li><p>127.0.0.1: listen on loopback (local) interface only</p>
</li>
<li><p>Our IP: (e.g. 192.168.1.12): listen only on the network interface with this IP.</p>
</li>
</ul>


For UDP out and TCP out, the IP needs to be set to the remote IP, where the MAVLink messages are to be sent to.

**Parameters**

* const std::string& **connection_url** - connection URL string.
* [ForwardingOption](namespacemavsdk.md#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) **forwarding_option** - message forwarding option (when multiple interfaces are used).

**Returns**

&emsp;std::pair< [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992), [ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) > - A pair containing the result of adding the connection as well as a handle to remove it later.

### remove_connection() {#classmavsdk_1_1_mavsdk_1a23cf630bb123aa53b0e99c6bd83ad013}
```cpp
void mavsdk::Mavsdk::remove_connection(ConnectionHandle handle)
```


Remove connection again.

**Parameters**

* [ConnectionHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a1b16edeae47af0815b3267c9075f6a8f) **handle** - [Handle](classmavsdk_1_1_handle.md) returned when connection was added.

### subscribe_connection_errors() {#classmavsdk_1_1_mavsdk_1a8f50fffac65a35857a88da2fd42f9c54}
```cpp
ConnectionErrorHandle mavsdk::Mavsdk::subscribe_connection_errors(ConnectionErrorCallback callback)
```


Subscribe to connection errors.


This will trigger when messages fail to be sent which can help diagnosing network interfaces or serial devices disappearing.


Usually, an error will require to remove a connection and add it fresh.

**Parameters**

* [ConnectionErrorCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac71d182538bee6fb3ab007edafac5d5f) **callback** - Callback to subscribe.

**Returns**

&emsp;[ConnectionErrorHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1aeb442a462d03662e4c152509fd0c203b) - [Handle](classmavsdk_1_1_handle.md) to unsubscribe again.

### unsubscribe_connection_errors() {#classmavsdk_1_1_mavsdk_1a377ec6517ee75981ceb2f26b30e59fbd}
```cpp
void mavsdk::Mavsdk::unsubscribe_connection_errors(ConnectionErrorHandle handle)
```


Unsubscribe from connection errors.

**Parameters**

* [ConnectionErrorHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1aeb442a462d03662e4c152509fd0c203b) **handle** - [Handle](classmavsdk_1_1_handle.md) to unsubscribe.

### systems() {#classmavsdk_1_1_mavsdk_1aca9c72b300d384341b00ff9ba2c6e5c5}
```cpp
std::vector< std::shared_ptr< System > > mavsdk::Mavsdk::systems() const
```


Get a vector of systems which have been discovered or set-up.


**Returns**

&emsp;std::vector< std::shared_ptr< [System](classmavsdk_1_1_system.md) > > - The vector of systems which are available.

### first_autopilot() {#classmavsdk_1_1_mavsdk_1aa1bcb865693dbd140478e75ce58699b7}
```cpp
std::optional< std::shared_ptr< System > > mavsdk::Mavsdk::first_autopilot(double timeout_s) const
```


Get the first autopilot that has been discovered.

::: info
This requires a MAVLink component with component ID 1 sending heartbeats.
:::

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

### set_heartbeat_timeout_s() {#classmavsdk_1_1_mavsdk_1afbab63cf2a795e4ca7262836d5fe4b46}
```cpp
void mavsdk::Mavsdk::set_heartbeat_timeout_s(double timeout_s)
```


Set heartbeat timeout.

The default heartbeat timeout is 3 seconds. If no heartbeat is received within this time, the system is considered disconnected.

**Parameters**

* double **timeout_s** - Timeout in seconds.

### get_heartbeat_timeout_s() {#classmavsdk_1_1_mavsdk_1a6179b858f74415251ef43da11bc6edbc}
```cpp
double mavsdk::Mavsdk::get_heartbeat_timeout_s() const
```


Get heartbeat timeout.


**Returns**

&emsp;double - Timeout in seconds.

### set_callback_executor() {#classmavsdk_1_1_mavsdk_1a5de0ff39a51efe3b235fe022e6b58034}
```cpp
void mavsdk::Mavsdk::set_callback_executor(std::function< void(std::function< void()>)> executor)
```


Set a custom callback executor.

By default, MAVSDK runs all user callbacks on an internal thread. Setting a custom executor replaces this: the executor function is called for each pending callback, and the internal callback thread is stopped.


The executor is called from MAVSDK's internal work thread, so it must be fast (e.g., just post/queue the callback for later execution).


This is useful for integrating with an existing event loop or ensuring callbacks run on a specific thread.


::: info
When a custom executor is set, blocking/synchronous APIs (e.g., [first_autopilot()](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1aa1bcb865693dbd140478e75ce58699b7), or any sync plugin method) must not be called from the thread that drains the executor queue, as they internally wait for a callback that the blocked thread would need to process. Use async APIs and drain callbacks in your event loop instead, or call sync APIs from a separate thread.
:::

**Parameters**

* std::function< void(std::function< void()>)> **executor** - Function that will be called with each callback to execute. Pass nullptr/empty to revert to the default internal thread.

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

### server_component() {#classmavsdk_1_1_mavsdk_1a693a2f665c35d6b01d6144819d353280}
```cpp
std::shared_ptr< ServerComponent > mavsdk::Mavsdk::server_component(unsigned instance=0)
```


Get server component with default type of [Mavsdk](classmavsdk_1_1_mavsdk.md) instance.


**Parameters**

* unsigned **instance** - 

**Returns**

&emsp;std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > - A valid shared pointer to a server component if it was successful, an empty pointer otherwise.

### server_component_by_type() {#classmavsdk_1_1_mavsdk_1a547fc7e18434473ea3e0e51ab3305abb}
```cpp
std::shared_ptr< ServerComponent > mavsdk::Mavsdk::server_component_by_type(ComponentType component_type, unsigned instance=0)
```


Get server component by a high level type.

This represents a server component of the MAVSDK instance.

**Parameters**

* [ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) **component_type** - The high level type of the component.
* unsigned **instance** - The instance of the component if there are multiple, starting at 0.

**Returns**

&emsp;std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > - A valid shared pointer to a server component if it was successful, an empty pointer otherwise.

### server_component_by_id() {#classmavsdk_1_1_mavsdk_1adef7d0d7422bcddbda629a404fb33ae2}
```cpp
std::shared_ptr< ServerComponent > mavsdk::Mavsdk::server_component_by_id(uint8_t component_id)
```


Get server component by the low MAVLink component ID.

This represents a server component of the MAVSDK instance.

**Parameters**

* uint8_t **component_id** - MAVLink component ID to use

**Returns**

&emsp;std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > - A valid shared pointer to a server component if it was successful, an empty pointer otherwise.

### subscribe_incoming_messages_json() {#classmavsdk_1_1_mavsdk_1a722fce6f1abe8337721dde710b5b40d7}
```cpp
InterceptJsonHandle mavsdk::Mavsdk::subscribe_incoming_messages_json(const InterceptJsonCallback &callback)
```


Intercept incoming messages as JSON.

This is a hook that allows to read any messages arriving via the in JSON format.

**Parameters**

* const [InterceptJsonCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a17923db3b1504e911487729114b68f48)& **callback** - Callback to be called for each incoming message. To drop a message, return 'false' from the callback.

**Returns**

&emsp;[InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) - 

### unsubscribe_incoming_messages_json() {#classmavsdk_1_1_mavsdk_1a4be244c38939cb517c2061baf4d43386}
```cpp
void mavsdk::Mavsdk::unsubscribe_incoming_messages_json(InterceptJsonHandle handle)
```


Unsubscribe from incoming messages as JSON.


**Parameters**

* [InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) **handle** - 

### subscribe_outgoing_messages_json() {#classmavsdk_1_1_mavsdk_1a58f85b2f74a32404a8e975feefed8f47}
```cpp
InterceptJsonHandle mavsdk::Mavsdk::subscribe_outgoing_messages_json(const InterceptJsonCallback &callback)
```


Intercept outgoing messages as JSON.

3 


This is a hook that allows to read any messages arriving via the in JSON format.

**Parameters**

* const [InterceptJsonCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a17923db3b1504e911487729114b68f48)& **callback** - Callback to be called for each outgoing message. To drop a message, return 'false' from the callback.

**Returns**

&emsp;[InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) - 

### unsubscribe_outgoing_messages_json() {#classmavsdk_1_1_mavsdk_1aa3a490358db87cfed617cdad902bb753}
```cpp
void mavsdk::Mavsdk::unsubscribe_outgoing_messages_json(InterceptJsonHandle handle)
```


Unsubscribe from outgoing messages as JSON.


**Parameters**

* [InterceptJsonHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1a3b40ae4fd8af4c4419b61f0ad955812f) **handle** - 

### intercept_incoming_messages_async() {#classmavsdk_1_1_mavsdk_1ac80c8909958533131cbdbc61d061794f}
```cpp
void mavsdk::Mavsdk::intercept_incoming_messages_async(std::function< bool(mavlink_message_t &)> callback)
```


Intercept incoming messages.

This is a hook which allows to change or drop MAVLink messages as they are received before they get forwarded any subscribers.


::: info
This functionality is provided primarily for testing in order to simulate packet drops or actors not adhering to the MAVLink protocols.
:::

**Parameters**

* std::function< bool(mavlink_message_t &)> **callback** - Callback to be called for each incoming message. To drop a message, return 'false' from the callback.

### intercept_outgoing_messages_async() {#classmavsdk_1_1_mavsdk_1a040ee5c1d41e71c0d63cf8f76d2db275}
```cpp
void mavsdk::Mavsdk::intercept_outgoing_messages_async(std::function< bool(mavlink_message_t &)> callback)
```


Intercept outgoing messages.

This is a hook which allows to change or drop MAVLink messages before they are sent.


::: info
This functionality is provided primarily for testing in order to simulate packet drops or actors not adhering to the MAVLink protocols.
:::

**Parameters**

* std::function< bool(mavlink_message_t &)> **callback** - Callback to be called for each outgoing message. To drop a message, return 'false' from the callback.

### pass_received_raw_bytes() {#classmavsdk_1_1_mavsdk_1a65329315ac07bae110839d9e054fbc05}
```cpp
void mavsdk::Mavsdk::pass_received_raw_bytes(const char *bytes, size_t length)
```


Pass received raw MAVLink bytes.

This allows passing raw MAVLink message bytes into MAVSDK to be processed. The bytes can contain one or more MAVLink messages.


::: info
Before using this, run add_any_connection("raw://")
:::

This goes together with subscribe_raw_bytes_to_be_sent.

**Parameters**

* const char\* **bytes** - Pointer to raw MAVLink message bytes.
* size_t **length** - Number of bytes to send.

### subscribe_raw_bytes_to_be_sent() {#classmavsdk_1_1_mavsdk_1a116e9bab0efdf7ec90866107ef517b20}
```cpp
RawBytesHandle mavsdk::Mavsdk::subscribe_raw_bytes_to_be_sent(RawBytesCallback callback)
```


Subscribe to raw bytes to be sent.

This allows getting MAVLink bytes that need to be sent out.


::: info
Before using this, run add_any_connection("raw://")
:::

This goes together with pass_received_raw_bytes. The bytes contain one mavlink message at a time.

**Parameters**

* [RawBytesCallback](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1acb5be9a1be97d251387ffe87ae8b9eb0) **callback** - Callback to be called with outgoing raw bytes.

**Returns**

&emsp;[RawBytesHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac766258f137aa3e8b0dabb5a66435ea1) - [Handle](classmavsdk_1_1_handle.md) to unsubscribe again.

### unsubscribe_raw_bytes_to_be_sent() {#classmavsdk_1_1_mavsdk_1af6ec813a9728f4258056fa1f5d399eb1}
```cpp
void mavsdk::Mavsdk::unsubscribe_raw_bytes_to_be_sent(RawBytesHandle handle)
```


Unsubscribe from raw bytes to be sent.


**Parameters**

* [RawBytesHandle](classmavsdk_1_1_mavsdk.md#classmavsdk_1_1_mavsdk_1ac766258f137aa3e8b0dabb5a66435ea1) **handle** - [Handle](classmavsdk_1_1_handle.md) from subscribe_raw_bytes_to_be_sent.