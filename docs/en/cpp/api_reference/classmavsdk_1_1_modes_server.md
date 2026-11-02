# mavsdk::ModesServer Class Reference
`#include: modes_server.hpp`

----


Provide the flight modes of an autopilot, the counterpart of the [Modes](classmavsdk_1_1_modes.md) plugin. 


The modes are announced using the MAVLink standard modes protocol (AVAILABLE_MODES, AVAILABLE_MODES_MONITOR, CURRENT_MODE).


Once modes are set, this plugin also handles mode change requests (MAV_CMD_DO_SET_STANDARD_MODE, MAV_CMD_DO_SET_MODE and SET_MODE) for those modes, instead of the deprecated flight mode API of the [ActionServer](classmavsdk_1_1_action_server.md) plugin. 


## Data Structures


struct [Mode](structmavsdk_1_1_modes_server_1_1_mode.md)

## Public Types


Type | Description
--- | ---
enum [ModeFeedback](#classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219c) | Possible answers to a mode change request.
enum [Result](#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) | Possible results returned for mode requests.
std::function< void([Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a))> [ResultCallback](#classmavsdk_1_1_modes_server_1adc0788b3fc6bbdf6f449121c414ea59c) | Callback type for asynchronous [ModesServer](classmavsdk_1_1_modes_server.md) calls.
std::function< void([Mode](structmavsdk_1_1_modes_server_1_1_mode.md))> [ModeRequestedCallback](#classmavsdk_1_1_modes_server_1a4bcaa8a17a3546ad76bdefb2a5314fb8) | Callback type for subscribe_mode_requested.
[Handle](classmavsdk_1_1_handle.md)< [Mode](structmavsdk_1_1_modes_server_1_1_mode.md) > [ModeRequestedHandle](#classmavsdk_1_1_modes_server_1af6c27431e60a90a79017e21b4d97a4e9) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_mode_requested.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ModesServer](#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ModesServer](#classmavsdk_1_1_modes_server_1a3f9840f59c4b0bab3abba35ad614d1cf) () override | Destructor (internal use only).
&nbsp; | [ModesServer](#classmavsdk_1_1_modes_server_1a63077f268464eee70088ae7a9e70ebd6) (const ModesServer & other) | Copy constructor.
[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) | [set_available_modes](#classmavsdk_1_1_modes_server_1a5ea1014dee85569bac6967c502e518b0) (std::vector< [Mode](structmavsdk_1_1_modes_server_1_1_mode.md) > modes)const | Set the modes the vehicle provides.
[ModeRequestedHandle](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1af6c27431e60a90a79017e21b4d97a4e9) | [subscribe_mode_requested](#classmavsdk_1_1_modes_server_1ac03d37bc09edef54349c5229babea4d7) (const [ModeRequestedCallback](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a4bcaa8a17a3546ad76bdefb2a5314fb8) & callback) | Subscribe to mode change requests.
void | [unsubscribe_mode_requested](#classmavsdk_1_1_modes_server_1a3212f581fb5017c47906175ddfa6d8f7) ([ModeRequestedHandle](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1af6c27431e60a90a79017e21b4d97a4e9) handle) | Unsubscribe from subscribe_mode_requested.
[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) | [respond_mode_requested](#classmavsdk_1_1_modes_server_1a69d9c04583434cf742190cff95ebb164) ([ModeFeedback](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219c) mode_feedback)const | Respond to the last mode change request.
[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) | [set_current_mode](#classmavsdk_1_1_modes_server_1a666e48235b9bdb461162a41a36281731) (std::string name)const | Set the mode the vehicle is in.
const [ModesServer](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7) & | [operator=](#classmavsdk_1_1_modes_server_1a4422c316ea30ac818a654db207649f9d) (const [ModesServer](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ModesServer() {#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7}
```cpp
mavsdk::ModesServer::ModesServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto modes_server = ModesServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~ModesServer() {#classmavsdk_1_1_modes_server_1a3f9840f59c4b0bab3abba35ad614d1cf}
```cpp
mavsdk::ModesServer::~ModesServer() override
```


Destructor (internal use only).


### ModesServer() {#classmavsdk_1_1_modes_server_1a63077f268464eee70088ae7a9e70ebd6}
```cpp
mavsdk::ModesServer::ModesServer(const ModesServer &other)
```


Copy constructor.


**Parameters**

* const ModesServer& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_modes_server_1adc0788b3fc6bbdf6f449121c414ea59c}

```cpp
using mavsdk::ModesServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ModesServer](classmavsdk_1_1_modes_server.md) calls.


### typedef ModeRequestedCallback {#classmavsdk_1_1_modes_server_1a4bcaa8a17a3546ad76bdefb2a5314fb8}

```cpp
using mavsdk::ModesServer::ModeRequestedCallback =  std::function<void(Mode)>
```


Callback type for subscribe_mode_requested.


### typedef ModeRequestedHandle {#classmavsdk_1_1_modes_server_1af6c27431e60a90a79017e21b4d97a4e9}

```cpp
using mavsdk::ModesServer::ModeRequestedHandle =  Handle<Mode>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_mode_requested.


## Member Enumeration Documentation


### enum ModeFeedback {#classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219c}


Possible answers to a mode change request.


Value | Description
--- | ---
<span id="classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219ca88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown. 
<span id="classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219ca382ab522931673c11e398ead1b7b1678"></span> `Accepted` | The vehicle switches to the mode. 
<span id="classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219cad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | The vehicle can't switch to the mode right now, e.g. it has no GPS. 
<span id="classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219ca58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | The vehicle doesn't allow switching to the mode. 
<span id="classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219cad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Switching to the mode failed. 

### enum Result {#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a}


Possible results returned for mode requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aaf0bc7a07b072f514539f8883a6ebe2a8"></span> `InvalidModes` | [Modes](classmavsdk_1_1_modes.md) are invalid, e.g. none are given, a name or custom mode is used twice, or a standard mode name is unknown. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aa9ebbfdcdae677078a0b4c5a3368f9d55"></span> `ModeNotFound` | No available mode with this name. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aa74e327c40abd5945a491e5da73b93550"></span> `NoRequest` | There is no mode change request to respond to. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aaed116b632b3019cb9de3d888c9fa6d29"></span> `InvalidFeedback` | The feedback is unknown. 

## Member Function Documentation


### set_available_modes() {#classmavsdk_1_1_modes_server_1a5ea1014dee85569bac6967c502e518b0}
```cpp
Result mavsdk::ModesServer::set_available_modes(std::vector< Mode > modes) const
```


Set the modes the vehicle provides.

Ground stations are told that the list changed so they download it again. From then on, mode change requests are handled by this plugin.


This function is blocking.

**Parameters**

* std::vector< [Mode](structmavsdk_1_1_modes_server_1_1_mode.md) > **modes** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) - [Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) of request.

### subscribe_mode_requested() {#classmavsdk_1_1_modes_server_1ac03d37bc09edef54349c5229babea4d7}
```cpp
ModeRequestedHandle mavsdk::ModesServer::subscribe_mode_requested(const ModeRequestedCallback &callback)
```


Subscribe to mode change requests.

Requests for a mode in the list are reported here and need to be answered with RespondModeRequested. Until then, the ground station is told that the request is in progress.

**Parameters**

* const [ModeRequestedCallback](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a4bcaa8a17a3546ad76bdefb2a5314fb8)& **callback** - 

**Returns**

&emsp;[ModeRequestedHandle](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1af6c27431e60a90a79017e21b4d97a4e9) - 

### unsubscribe_mode_requested() {#classmavsdk_1_1_modes_server_1a3212f581fb5017c47906175ddfa6d8f7}
```cpp
void mavsdk::ModesServer::unsubscribe_mode_requested(ModeRequestedHandle handle)
```


Unsubscribe from subscribe_mode_requested.


**Parameters**

* [ModeRequestedHandle](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1af6c27431e60a90a79017e21b4d97a4e9) **handle** - 

### respond_mode_requested() {#classmavsdk_1_1_modes_server_1a69d9c04583434cf742190cff95ebb164}
```cpp
Result mavsdk::ModesServer::respond_mode_requested(ModeFeedback mode_feedback) const
```


Respond to the last mode change request.

If the request is accepted, the requested mode becomes the current mode. Otherwise the vehicle stays in its current mode, e.g. if it can't switch to Position without GPS.


This function is blocking.

**Parameters**

* [ModeFeedback](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a13da9ec5c7fb5e9431685bc78fbd219c) **mode_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) - [Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) of request.

### set_current_mode() {#classmavsdk_1_1_modes_server_1a666e48235b9bdb461162a41a36281731}
```cpp
Result mavsdk::ModesServer::set_current_mode(std::string name) const
```


Set the mode the vehicle is in.

This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) - [Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) of request.

### operator=() {#classmavsdk_1_1_modes_server_1a4422c316ea30ac818a654db207649f9d}
```cpp
const ModesServer & mavsdk::ModesServer::operator=(const ModesServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ModesServer](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7)&  - 

**Returns**

&emsp;const [ModesServer](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7) & - 