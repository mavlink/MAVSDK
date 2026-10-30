# mavsdk::ModesServer Class Reference
`#include: modes_server.hpp`

----


Announce the flight modes of an autopilot, the counterpart of the [Modes](classmavsdk_1_1_modes.md) plugin. 


The modes are announced using the MAVLink standard modes protocol (AVAILABLE_MODES, AVAILABLE_MODES_MONITOR, CURRENT_MODE). The current mode is the custom mode of the server component.


This plugin doesn't change modes itself: MAV_CMD_DO_SET_STANDARD_MODE is answered as unsupported, so clients fall back to MAV_CMD_DO_SET_MODE, which the [ActionServer](classmavsdk_1_1_action_server.md) plugin handles. 


## Data Structures


struct [Mode](structmavsdk_1_1_modes_server_1_1_mode.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) | Possible results returned for mode requests.
std::function< void([Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a))> [ResultCallback](#classmavsdk_1_1_modes_server_1adc0788b3fc6bbdf6f449121c414ea59c) | Callback type for asynchronous [ModesServer](classmavsdk_1_1_modes_server.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ModesServer](#classmavsdk_1_1_modes_server_1a6357a45a1aa9df1a5ac66130042c41f7) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ModesServer](#classmavsdk_1_1_modes_server_1a3f9840f59c4b0bab3abba35ad614d1cf) () override | Destructor (internal use only).
&nbsp; | [ModesServer](#classmavsdk_1_1_modes_server_1a63077f268464eee70088ae7a9e70ebd6) (const ModesServer & other) | Copy constructor.
[Result](classmavsdk_1_1_modes_server.md#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a) | [set_available_modes](#classmavsdk_1_1_modes_server_1a5ea1014dee85569bac6967c502e518b0) (std::vector< [Mode](structmavsdk_1_1_modes_server_1_1_mode.md) > modes)const | Set the modes the vehicle provides.
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


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34a}


Possible results returned for mode requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_modes_server_1acab9f3b90c5ac28d673b6dead77ae34aaf0bc7a07b072f514539f8883a6ebe2a8"></span> `InvalidModes` | [Modes](classmavsdk_1_1_modes.md) are invalid, e.g. a name or custom mode is used twice, or a standard mode name is unknown. 

## Member Function Documentation


### set_available_modes() {#classmavsdk_1_1_modes_server_1a5ea1014dee85569bac6967c502e518b0}
```cpp
Result mavsdk::ModesServer::set_available_modes(std::vector< Mode > modes) const
```


Set the modes the vehicle provides.

Ground stations are told that the list changed so they download it again.


This function is blocking.

**Parameters**

* std::vector< [Mode](structmavsdk_1_1_modes_server_1_1_mode.md) > **modes** - 

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