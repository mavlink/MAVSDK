# mavsdk::MavlinkDirectServer Class Reference
`#include: mavlink_direct_server.hpp`

----


Enable direct MAVLink communication using libmav from a server component. 


Unlike [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md), this does not require a discovered system: it sends from the local server component, which makes it suitable for broadcasting messages (e.g. as an autopilot or custom component) without a connected counterpart. 


## Data Structures


struct [MavlinkMessage](structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960) | Possible results returned for action requests.
std::function< void([Result](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960))> [ResultCallback](#classmavsdk_1_1_mavlink_direct_server_1a2dc536637ada21cfa613c323ebe9ea85) | Callback type for asynchronous [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md) calls.
std::function< void([MavlinkMessage](structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message.md))> [MessageCallback](#classmavsdk_1_1_mavlink_direct_server_1ab4984454989841e5205f77f0b3c7b51e) | Callback type for subscribe_message.
[Handle](classmavsdk_1_1_handle.md)< [MavlinkMessage](structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message.md) > [MessageHandle](#classmavsdk_1_1_mavlink_direct_server_1aca946e5536d779d1d6c0604388a8357f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_message.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [MavlinkDirectServer](#classmavsdk_1_1_mavlink_direct_server_1a466864bb7130a52d099e29cb820987a3) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~MavlinkDirectServer](#classmavsdk_1_1_mavlink_direct_server_1af945aec68df1bf504e2110d42715e17b) () override | Destructor (internal use only).
&nbsp; | [MavlinkDirectServer](#classmavsdk_1_1_mavlink_direct_server_1a813d207b34b938a1072652117844149a) (const [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960) | [send_message](#classmavsdk_1_1_mavlink_direct_server_1abd7cd4380f2e07588e5f180b73cb23f4) ([MavlinkMessage](structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message.md) message)const | Send a MAVLink message directly.
[MessageHandle](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aca946e5536d779d1d6c0604388a8357f) | [subscribe_message](#classmavsdk_1_1_mavlink_direct_server_1a72f9b9439c17c117b3846ad9fc4b2588) (std::string message_name, const [MessageCallback](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1ab4984454989841e5205f77f0b3c7b51e) & callback) | Subscribe to incoming MAVLink messages.
void | [unsubscribe_message](#classmavsdk_1_1_mavlink_direct_server_1a377e2038cef335e30fa387b8f06621fa) ([MessageHandle](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aca946e5536d779d1d6c0604388a8357f) handle) | Unsubscribe from subscribe_message.
[Result](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960) | [load_custom_xml](#classmavsdk_1_1_mavlink_direct_server_1a6eb99920e7b5a45fe3e015f1f4849f6e) (std::string xml_content)const | Load custom MAVLink message definitions from XML.
const [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md) & | [operator=](#classmavsdk_1_1_mavlink_direct_server_1aeadf564e850b8fea4dddb47de9d335e3) (const [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### MavlinkDirectServer() {#classmavsdk_1_1_mavlink_direct_server_1a466864bb7130a52d099e29cb820987a3}
```cpp
mavsdk::MavlinkDirectServer::MavlinkDirectServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto mavlink_direct_server = MavlinkDirectServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~MavlinkDirectServer() {#classmavsdk_1_1_mavlink_direct_server_1af945aec68df1bf504e2110d42715e17b}
```cpp
mavsdk::MavlinkDirectServer::~MavlinkDirectServer() override
```


Destructor (internal use only).


### MavlinkDirectServer() {#classmavsdk_1_1_mavlink_direct_server_1a813d207b34b938a1072652117844149a}
```cpp
mavsdk::MavlinkDirectServer::MavlinkDirectServer(const MavlinkDirectServer &other)
```


Copy constructor.


**Parameters**

* const [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_mavlink_direct_server_1a2dc536637ada21cfa613c323ebe9ea85}

```cpp
using mavsdk::MavlinkDirectServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md) calls.


### typedef MessageCallback {#classmavsdk_1_1_mavlink_direct_server_1ab4984454989841e5205f77f0b3c7b51e}

```cpp
using mavsdk::MavlinkDirectServer::MessageCallback =  std::function<void(MavlinkMessage)>
```


Callback type for subscribe_message.


### typedef MessageHandle {#classmavsdk_1_1_mavlink_direct_server_1aca946e5536d779d1d6c0604388a8357f}

```cpp
using mavsdk::MavlinkDirectServer::MessageHandle =  Handle<MavlinkMessage>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_message.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960}


Possible results returned for action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960ad5e88abe375e264803a6f6b436e769e6"></span> `InvalidMessage` | Invalid MAVLink message. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960a55f2db628c561fd08618598c69b3278d"></span> `InvalidField` | Invalid field name or value. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 

## Member Function Documentation


### send_message() {#classmavsdk_1_1_mavlink_direct_server_1abd7cd4380f2e07588e5f180b73cb23f4}
```cpp
Result mavsdk::MavlinkDirectServer::send_message(MavlinkMessage message) const
```


Send a MAVLink message directly.

This allows sending any MAVLink message with full control over the message content. Leave target_system_id and target_component_id at 0 to broadcast.


This function is blocking.

**Parameters**

* [MavlinkMessage](structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message.md) **message** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960) - Result of request.

### subscribe_message() {#classmavsdk_1_1_mavlink_direct_server_1a72f9b9439c17c117b3846ad9fc4b2588}
```cpp
MessageHandle mavsdk::MavlinkDirectServer::subscribe_message(std::string message_name, const MessageCallback &callback)
```


Subscribe to incoming MAVLink messages.

This provides direct access to incoming MAVLink messages. Use an empty string in message_name to subscribe to all messages, or specify a message name (e.g., "HEARTBEAT") to filter for specific message types. Unlike the [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) (client) plugin, this is not scoped to a single system: it receives matching messages from all systems.

**Parameters**

* std::string **message_name** - 
* const [MessageCallback](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1ab4984454989841e5205f77f0b3c7b51e)& **callback** - 

**Returns**

&emsp;[MessageHandle](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aca946e5536d779d1d6c0604388a8357f) - 

### unsubscribe_message() {#classmavsdk_1_1_mavlink_direct_server_1a377e2038cef335e30fa387b8f06621fa}
```cpp
void mavsdk::MavlinkDirectServer::unsubscribe_message(MessageHandle handle)
```


Unsubscribe from subscribe_message.


**Parameters**

* [MessageHandle](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aca946e5536d779d1d6c0604388a8357f) **handle** - 

### load_custom_xml() {#classmavsdk_1_1_mavlink_direct_server_1a6eb99920e7b5a45fe3e015f1f4849f6e}
```cpp
Result mavsdk::MavlinkDirectServer::load_custom_xml(std::string xml_content) const
```


Load custom MAVLink message definitions from XML.

This allows loading custom MAVLink message definitions at runtime, extending the available message types beyond the built-in definitions.


This function is blocking.

**Parameters**

* std::string **xml_content** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_direct_server.md#classmavsdk_1_1_mavlink_direct_server_1aa816208dcb9a5634a1f42298aea17960) - Result of request.

### operator=() {#classmavsdk_1_1_mavlink_direct_server_1aeadf564e850b8fea4dddb47de9d335e3}
```cpp
const MavlinkDirectServer & mavsdk::MavlinkDirectServer::operator=(const MavlinkDirectServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md)&  - 

**Returns**

&emsp;const [MavlinkDirectServer](classmavsdk_1_1_mavlink_direct_server.md) & - 