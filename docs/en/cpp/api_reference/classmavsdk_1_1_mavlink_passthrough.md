# mavsdk::MavlinkPassthrough Class Reference
`#include: mavlink_passthrough.h`

----


The [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md) class provides direct MAVLink access. 


"With great power comes great responsibility." - This plugin allows you to send and receive MAVLink messages. There is no checking or safe-guards, you're on your own, and you have been warned.


::: warning
[MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md) will get deprecated in MAVSDK v4 and will only be available built from source. Use [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) instead. [MavlinkDirect](classmavsdk_1_1_mavlink_direct.md) provides enhanced functionality including:
<ul>
<li><p>Runtime message parsing with JSON field representation</p>
</li>
<li><p>Custom message support via XML loading</p>
</li>
<li><p>Better language wrapper integration </p>
</li>
</ul>
:::


## Data Structures


struct [CommandInt](structmavsdk_1_1_mavlink_passthrough_1_1_command_int.md)

struct [CommandLong](structmavsdk_1_1_mavlink_passthrough_1_1_command_long.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) | Possible results returned for requests.
std::function< void(const mavlink_message_t &)> [MessageCallback](#classmavsdk_1_1_mavlink_passthrough_1a97f94c54e84fcce94d922fd7f4e3d231) | Callback type for message subscriptions.
[Handle](classmavsdk_1_1_handle.md)< const mavlink_message_t & > [MessageHandle](#classmavsdk_1_1_mavlink_passthrough_1a2e283239c4429eaeb33deb5821833066) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_message_async.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [MavlinkPassthrough](#classmavsdk_1_1_mavlink_passthrough_1ad17fc20d2b7c5c6996e0841aaabccb56) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [MavlinkPassthrough](#classmavsdk_1_1_mavlink_passthrough_1a9d1fdd3c4aeab8e221ac59612944c299) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~MavlinkPassthrough](#classmavsdk_1_1_mavlink_passthrough_1a890faef9ad80c3e79e0b785fd07106c8) () | Destructor (internal use only).
&nbsp; | [MavlinkPassthrough](#classmavsdk_1_1_mavlink_passthrough_1ae4b30f9c2c5e938ab965729e27f50ce5) (const [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md) &)=delete | Copy Constructor (object is not copyable).
DEPRECATED [Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) | [send_message](#classmavsdk_1_1_mavlink_passthrough_1a9bbd09d34f7ae1b6e27bcd5c3d4ba667) (mavlink_message_t & message) | Send message (deprecated).
[Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) | [queue_message](#classmavsdk_1_1_mavlink_passthrough_1aa95592bc4da7d56d444b8b5cb5bce814) (std::function< mavlink_message_t([MavlinkAddress](struct_mavlink_address.md) mavlink_address, uint8_t channel)> fun) | Send message by queueing it.
[Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) | [send_command_long](#classmavsdk_1_1_mavlink_passthrough_1a7e258aa16b92195e5329e861fb18f8e9) (const [CommandLong](structmavsdk_1_1_mavlink_passthrough_1_1_command_long.md) & command) | Send a MAVLink command_long.
[Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) | [send_command_int](#classmavsdk_1_1_mavlink_passthrough_1a654d67a3b136509c76e2ac804a656ada) (const [CommandInt](structmavsdk_1_1_mavlink_passthrough_1_1_command_int.md) & command) | Send a MAVLink command_long.
mavlink_message_t | [make_command_ack_message](#classmavsdk_1_1_mavlink_passthrough_1aa08cb5d2f9aed795367cf7e05d58bdcc) (const uint8_t target_sysid, const uint8_t target_compid, const uint16_t command, MAV_RESULT result) | Create a command_ack.
std::pair< [Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793), int32_t > | [get_param_int](#classmavsdk_1_1_mavlink_passthrough_1ad692f8b619f317151831bc53b4f6c168) (const std::string & name, std::optional< uint8_t > maybe_component_id, bool extended) | Request param (int).
std::pair< [Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793), float > | [get_param_float](#classmavsdk_1_1_mavlink_passthrough_1a347f3921a786865c7bb62817b706c359) (const std::string & name, std::optional< uint8_t > maybe_component_id, bool extended) | Request param (float).
[MessageHandle](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a2e283239c4429eaeb33deb5821833066) | [subscribe_message](#classmavsdk_1_1_mavlink_passthrough_1ae2ab2426bc0d844e931c266aa26c3607) (uint16_t message_id, const [MessageCallback](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a97f94c54e84fcce94d922fd7f4e3d231) & callback) | Subscribe to messages using message ID.
void | [unsubscribe_message](#classmavsdk_1_1_mavlink_passthrough_1af4e722ae613e799b60020c30193656be) (uint16_t message_id, [MessageHandle](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a2e283239c4429eaeb33deb5821833066) handle) | Unsubscribe from subscribe_message.
uint8_t | [get_our_sysid](#classmavsdk_1_1_mavlink_passthrough_1a985b269c1b78ec3e4e9d9468e46e19be) () const | Get our own system ID.
uint8_t | [get_our_compid](#classmavsdk_1_1_mavlink_passthrough_1a85ddd016ab35d5f3f487b1362723d3cf) () const | Get our own component ID.
uint8_t | [get_target_sysid](#classmavsdk_1_1_mavlink_passthrough_1a2867d1f37649d62e757bbac0a73b3ebd) () const | Get system ID of target.
uint8_t | [get_target_compid](#classmavsdk_1_1_mavlink_passthrough_1a22ecab3905237a2f227f77bbab9afd17) () const | Get target component ID.
const [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md) & | [operator=](#classmavsdk_1_1_mavlink_passthrough_1a39b74b37094511cc5bc910a2233d024e) (const [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### MavlinkPassthrough() {#classmavsdk_1_1_mavlink_passthrough_1ad17fc20d2b7c5c6996e0841aaabccb56}
```cpp
mavsdk::MavlinkPassthrough::MavlinkPassthrough(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mavlink_passthrough = MavlinkPassthrough(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### MavlinkPassthrough() {#classmavsdk_1_1_mavlink_passthrough_1a9d1fdd3c4aeab8e221ac59612944c299}
```cpp
mavsdk::MavlinkPassthrough::MavlinkPassthrough(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mavlink_passthrough = MavlinkPassthrough(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~MavlinkPassthrough() {#classmavsdk_1_1_mavlink_passthrough_1a890faef9ad80c3e79e0b785fd07106c8}
```cpp
mavsdk::MavlinkPassthrough::~MavlinkPassthrough()
```


Destructor (internal use only).


### MavlinkPassthrough() {#classmavsdk_1_1_mavlink_passthrough_1ae4b30f9c2c5e938ab965729e27f50ce5}
```cpp
mavsdk::MavlinkPassthrough::MavlinkPassthrough(const MavlinkPassthrough &)=delete
```


Copy Constructor (object is not copyable).


**Parameters**

* const [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md)&  - 

## Member Typdef Documentation


### typedef MessageCallback {#classmavsdk_1_1_mavlink_passthrough_1a97f94c54e84fcce94d922fd7f4e3d231}

```cpp
using mavsdk::MavlinkPassthrough::MessageCallback =  std::function<void(const mavlink_message_t&)>
```


Callback type for message subscriptions.


### typedef MessageHandle {#classmavsdk_1_1_mavlink_passthrough_1a2e283239c4429eaeb33deb5821833066}

```cpp
using mavsdk::MavlinkPassthrough::MessageHandle =  Handle<const mavlink_message_t&>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_message_async.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793}


Possible results returned for requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown error. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Success. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793aaa075662ac89e0922d349f55d81d7382"></span> `CommandNoSystem` | [System](classmavsdk_1_1_system.md) not available. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a236fe685992938e92191d04e69a77762"></span> `CommandBusy` | [System](classmavsdk_1_1_system.md) is busy. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command has been denied. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a711552fb22e889145c9769bb9ab88fe5"></span> `CommandUnsupported` | Command is not supported. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a309bc1cd528bc8f9a548616ae0a13a17"></span> `CommandTimeout` | A timeout happened. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a3ecbc4a65a301906f20d9136f1e28a54"></span> `CommandTemporarilyRejected` | Command has been rejected for now. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a205ab53602b765b27a17d0aa2b118986"></span> `CommandFailed` | Command has failed. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793aa5b149d9ae2ac3629f0cefb36bbacd1e"></span> `ParamWrongType` | Wrong type for requested param. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793aa2b5cfc4e45ca036892b3dadc483e655"></span> `ParamNameTooLong` | [Param](classmavsdk_1_1_param.md) name too long. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a1fc93bc695e2e3e1903029eb77228234"></span> `ParamValueTooLong` | [Param](classmavsdk_1_1_param.md) value too long. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793a2f49c4dd8da12dcbec351df940ba3a6f"></span> `ParamNotFound` | [Param](classmavsdk_1_1_param.md) not found. 
<span id="classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793aa6201b129c022880743d84f6e579ea8c"></span> `ParamValueUnsupported` | [Param](classmavsdk_1_1_param.md) value unsupported. 

## Member Function Documentation


### send_message() {#classmavsdk_1_1_mavlink_passthrough_1a9bbd09d34f7ae1b6e27bcd5c3d4ba667}
```cpp
DEPRECATED Result mavsdk::MavlinkPassthrough::send_message(mavlink_message_t &message)
```


Send message (deprecated).

::: info
This interface is deprecated. Instead the method [queue_message()](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1aa95592bc4da7d56d444b8b5cb5bce814) should be used.
:::

**Parameters**

* mavlink_message_t& **message** - 

**Returns**

&emsp;DEPRECATED [Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) - result of the request.

### queue_message() {#classmavsdk_1_1_mavlink_passthrough_1aa95592bc4da7d56d444b8b5cb5bce814}
```cpp
Result mavsdk::MavlinkPassthrough::queue_message(std::function< mavlink_message_t(MavlinkAddress mavlink_address, uint8_t channel)> fun)
```


Send message by queueing it.

::: info
This interface replaces the previous send_message method.
:::

The interface changed in order to prevent accessing the internal MAVLink status from different threads and to make sure the seq numbers are not unique to[Mavsdk](classmavsdk_1_1_mavsdk.md) instances and server components.

**Parameters**

* std::function< mavlink_message_t([MavlinkAddress](struct_mavlink_address.md) mavlink_address, uint8_t channel)> **fun** - Function which is (immediately) executed to send a message. It is passed the mavlink_address and channel, both data required to send a message using mavlink_message_xx_pack_chan().

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) - result of request

### send_command_long() {#classmavsdk_1_1_mavlink_passthrough_1a7e258aa16b92195e5329e861fb18f8e9}
```cpp
Result mavsdk::MavlinkPassthrough::send_command_long(const CommandLong &command)
```


Send a MAVLink command_long.


**Parameters**

* const [CommandLong](structmavsdk_1_1_mavlink_passthrough_1_1_command_long.md)& **command** - Command to send.

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) - result of the request.

### send_command_int() {#classmavsdk_1_1_mavlink_passthrough_1a654d67a3b136509c76e2ac804a656ada}
```cpp
Result mavsdk::MavlinkPassthrough::send_command_int(const CommandInt &command)
```


Send a MAVLink command_long.


**Parameters**

* const [CommandInt](structmavsdk_1_1_mavlink_passthrough_1_1_command_int.md)& **command** - Command to send.

**Returns**

&emsp;[Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793) - result of the request.

### make_command_ack_message() {#classmavsdk_1_1_mavlink_passthrough_1aa08cb5d2f9aed795367cf7e05d58bdcc}
```cpp
mavlink_message_t mavsdk::MavlinkPassthrough::make_command_ack_message(const uint8_t target_sysid, const uint8_t target_compid, const uint16_t command, MAV_RESULT result)
```


Create a command_ack.


**Parameters**

* const uint8_t **target_sysid** - Target system ID where to send command_ack to.
* const uint8_t **target_compid** - Target component ID where to send command_ack to.
* const uint16_t **command** - Command to respond to.
* MAV_RESULT **result** - Result of command.

**Returns**

&emsp;mavlink_message_t - message to send.

### get_param_int() {#classmavsdk_1_1_mavlink_passthrough_1ad692f8b619f317151831bc53b4f6c168}
```cpp
std::pair< Result, int32_t > mavsdk::MavlinkPassthrough::get_param_int(const std::string &name, std::optional< uint8_t > maybe_component_id, bool extended)
```


Request param (int).


**Parameters**

* const std::string& **name** - 
* std::optional< uint8_t > **maybe_component_id** - 
* bool **extended** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793), int32_t > - 

### get_param_float() {#classmavsdk_1_1_mavlink_passthrough_1a347f3921a786865c7bb62817b706c359}
```cpp
std::pair< Result, float > mavsdk::MavlinkPassthrough::get_param_float(const std::string &name, std::optional< uint8_t > maybe_component_id, bool extended)
```


Request param (float).


**Parameters**

* const std::string& **name** - 
* std::optional< uint8_t > **maybe_component_id** - 
* bool **extended** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a265eacaeea064a31de3fe16d1e357793), float > - 

### subscribe_message() {#classmavsdk_1_1_mavlink_passthrough_1ae2ab2426bc0d844e931c266aa26c3607}
```cpp
MessageHandle mavsdk::MavlinkPassthrough::subscribe_message(uint16_t message_id, const MessageCallback &callback)
```


Subscribe to messages using message ID.

This means that all future messages being received will trigger the callback to be called. To stop the subscription, call this method with `nullptr` as the argument.

**Parameters**

* uint16_t **message_id** - The MAVLink message ID.
* const [MessageCallback](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a97f94c54e84fcce94d922fd7f4e3d231)& **callback** - Callback to be called for message subscription.

**Returns**

&emsp;[MessageHandle](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a2e283239c4429eaeb33deb5821833066) - 

### unsubscribe_message() {#classmavsdk_1_1_mavlink_passthrough_1af4e722ae613e799b60020c30193656be}
```cpp
void mavsdk::MavlinkPassthrough::unsubscribe_message(uint16_t message_id, MessageHandle handle)
```


Unsubscribe from subscribe_message.


**Parameters**

* uint16_t **message_id** - The MAVLink message ID.
* [MessageHandle](classmavsdk_1_1_mavlink_passthrough.md#classmavsdk_1_1_mavlink_passthrough_1a2e283239c4429eaeb33deb5821833066) **handle** - The handle returned from subscribe_message.

### get_our_sysid() {#classmavsdk_1_1_mavlink_passthrough_1a985b269c1b78ec3e4e9d9468e46e19be}
```cpp
uint8_t mavsdk::MavlinkPassthrough::get_our_sysid() const
```


Get our own system ID.


**Returns**

&emsp;uint8_t - our own system ID.

### get_our_compid() {#classmavsdk_1_1_mavlink_passthrough_1a85ddd016ab35d5f3f487b1362723d3cf}
```cpp
uint8_t mavsdk::MavlinkPassthrough::get_our_compid() const
```


Get our own component ID.


**Returns**

&emsp;uint8_t - our own component ID.

### get_target_sysid() {#classmavsdk_1_1_mavlink_passthrough_1a2867d1f37649d62e757bbac0a73b3ebd}
```cpp
uint8_t mavsdk::MavlinkPassthrough::get_target_sysid() const
```


Get system ID of target.


**Returns**

&emsp;uint8_t - system ID of target.

### get_target_compid() {#classmavsdk_1_1_mavlink_passthrough_1a22ecab3905237a2f227f77bbab9afd17}
```cpp
uint8_t mavsdk::MavlinkPassthrough::get_target_compid() const
```


Get target component ID.

This defaults to the component ID of the autopilot (1) if available and otherwise to all components (0).

**Returns**

&emsp;uint8_t - component ID of target.

### operator=() {#classmavsdk_1_1_mavlink_passthrough_1a39b74b37094511cc5bc910a2233d024e}
```cpp
const MavlinkPassthrough & mavsdk::MavlinkPassthrough::operator=(const MavlinkPassthrough &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md)&  - 

**Returns**

&emsp;const [MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md) & - 