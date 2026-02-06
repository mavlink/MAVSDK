# mavsdk::MavlinkDirect Class Reference
`#include: mavlink_direct.h`

----


Enable direct MAVLink communication using libmav. 


## Data Structures


struct [MavlinkMessage](structmavsdk_1_1_mavlink_direct_1_1_mavlink_message.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864) | Possible results returned for action requests.
std::function< void([Result](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864))> [ResultCallback](#classmavsdk_1_1_mavlink_direct_1ae166c4e87888a21c8e3a6a5858628cb4) | Callback type for asynchronous [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) calls.
std::function< void([MavlinkMessage](structmavsdk_1_1_mavlink_direct_1_1_mavlink_message.md))> [MessageCallback](#classmavsdk_1_1_mavlink_direct_1a20acd4068f45cbde53afc55eb292692d) | Callback type for subscribe_message.
[Handle](classmavsdk_1_1_handle.md)< [MavlinkMessage](structmavsdk_1_1_mavlink_direct_1_1_mavlink_message.md) > [MessageHandle](#classmavsdk_1_1_mavlink_direct_1aa4a3a6b305c89ef6a0cde3cb47b5c6a3) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_message.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [MavlinkDirect](#classmavsdk_1_1_mavlink_direct_1aa52aa19602ee54074c6a9285ae7aae6c) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [MavlinkDirect](#classmavsdk_1_1_mavlink_direct_1a4b968c477c05001538a84816ffe531fb) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~MavlinkDirect](#classmavsdk_1_1_mavlink_direct_1a3cb749215b130d7e829b54cefdfe3b75) () override | Destructor (internal use only).
&nbsp; | [MavlinkDirect](#classmavsdk_1_1_mavlink_direct_1a558285b9e08cfd038de9d5dc3b9111bb) (const [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864) | [send_message](#classmavsdk_1_1_mavlink_direct_1ac81f7a568b426575eb2d12116e5faeef) ([MavlinkMessage](structmavsdk_1_1_mavlink_direct_1_1_mavlink_message.md) message)const | Send a MAVLink message directly to the system.
[MessageHandle](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1aa4a3a6b305c89ef6a0cde3cb47b5c6a3) | [subscribe_message](#classmavsdk_1_1_mavlink_direct_1a0fd42307f0b5e2ad427437c7a9309c9a) (std::string message_name, const [MessageCallback](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a20acd4068f45cbde53afc55eb292692d) & callback) | Subscribe to incoming MAVLink messages.
void | [unsubscribe_message](#classmavsdk_1_1_mavlink_direct_1a2ae1167e995caf0b7b8a5193cf5dbf52) ([MessageHandle](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1aa4a3a6b305c89ef6a0cde3cb47b5c6a3) handle) | Unsubscribe from subscribe_message.
[Result](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864) | [load_custom_xml](#classmavsdk_1_1_mavlink_direct_1a9deb1c72b084d1b2369a133c0260bd46) (std::string xml_content)const | Load custom MAVLink message definitions from XML.
const [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) & | [operator=](#classmavsdk_1_1_mavlink_direct_1a909f97b32d832d00488c695940760b63) (const [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### MavlinkDirect() {#classmavsdk_1_1_mavlink_direct_1aa52aa19602ee54074c6a9285ae7aae6c}
```cpp
mavsdk::MavlinkDirect::MavlinkDirect(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mavlink_direct = MavlinkDirect(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### MavlinkDirect() {#classmavsdk_1_1_mavlink_direct_1a4b968c477c05001538a84816ffe531fb}
```cpp
mavsdk::MavlinkDirect::MavlinkDirect(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mavlink_direct = MavlinkDirect(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~MavlinkDirect() {#classmavsdk_1_1_mavlink_direct_1a3cb749215b130d7e829b54cefdfe3b75}
```cpp
mavsdk::MavlinkDirect::~MavlinkDirect() override
```


Destructor (internal use only).


### MavlinkDirect() {#classmavsdk_1_1_mavlink_direct_1a558285b9e08cfd038de9d5dc3b9111bb}
```cpp
mavsdk::MavlinkDirect::MavlinkDirect(const MavlinkDirect &other)
```


Copy constructor.


**Parameters**

* const [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_mavlink_direct_1ae166c4e87888a21c8e3a6a5858628cb4}

```cpp
using mavsdk::MavlinkDirect::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) calls.


### typedef MessageCallback {#classmavsdk_1_1_mavlink_direct_1a20acd4068f45cbde53afc55eb292692d}

```cpp
using mavsdk::MavlinkDirect::MessageCallback =  std::function<void(MavlinkMessage)>
```


Callback type for subscribe_message.


### typedef MessageHandle {#classmavsdk_1_1_mavlink_direct_1aa4a3a6b305c89ef6a0cde3cb47b5c6a3}

```cpp
using mavsdk::MavlinkDirect::MessageHandle =  Handle<MavlinkMessage>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_message.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864}


Possible results returned for action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864ad5e88abe375e264803a6f6b436e769e6"></span> `InvalidMessage` | Invalid MAVLink message. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864a55f2db628c561fd08618598c69b3278d"></span> `InvalidField` | Invalid field name or value. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 

## Member Function Documentation


### send_message() {#classmavsdk_1_1_mavlink_direct_1ac81f7a568b426575eb2d12116e5faeef}
```cpp
Result mavsdk::MavlinkDirect::send_message(MavlinkMessage message) const
```


Send a MAVLink message directly to the system.

This allows sending any MAVLink message with full control over the message content.


This function is blocking.

**Parameters**

* [MavlinkMessage](structmavsdk_1_1_mavlink_direct_1_1_mavlink_message.md) **message** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864) - Result of request.

### subscribe_message() {#classmavsdk_1_1_mavlink_direct_1a0fd42307f0b5e2ad427437c7a9309c9a}
```cpp
MessageHandle mavsdk::MavlinkDirect::subscribe_message(std::string message_name, const MessageCallback &callback)
```


Subscribe to incoming MAVLink messages.

This provides direct access to incoming MAVLink messages. Use an empty string in message_name to subscribe to all messages, or specify a message name (e.g., "HEARTBEAT") to filter for specific message types.

**Parameters**

* std::string **message_name** - 
* const [MessageCallback](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a20acd4068f45cbde53afc55eb292692d)& **callback** - 

**Returns**

&emsp;[MessageHandle](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1aa4a3a6b305c89ef6a0cde3cb47b5c6a3) - 

### unsubscribe_message() {#classmavsdk_1_1_mavlink_direct_1a2ae1167e995caf0b7b8a5193cf5dbf52}
```cpp
void mavsdk::MavlinkDirect::unsubscribe_message(MessageHandle handle)
```


Unsubscribe from subscribe_message.


**Parameters**

* [MessageHandle](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1aa4a3a6b305c89ef6a0cde3cb47b5c6a3) **handle** - 

### load_custom_xml() {#classmavsdk_1_1_mavlink_direct_1a9deb1c72b084d1b2369a133c0260bd46}
```cpp
Result mavsdk::MavlinkDirect::load_custom_xml(std::string xml_content) const
```


Load custom MAVLink message definitions from XML.

This allows loading custom MAVLink message definitions at runtime, extending the available message types beyond the built-in definitions.


This function is blocking.

**Parameters**

* std::string **xml_content** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_direct.md#classmavsdk_1_1_mavlink_direct_1a63864e376328d1fe2ef7d667aa61f864) - Result of request.

### operator=() {#classmavsdk_1_1_mavlink_direct_1a909f97b32d832d00488c695940760b63}
```cpp
const MavlinkDirect & mavsdk::MavlinkDirect::operator=(const MavlinkDirect &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md)&  - 

**Returns**

&emsp;const [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) & - 