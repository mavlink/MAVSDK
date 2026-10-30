# mavsdk::Modes Class Reference
`#include: modes.hpp`

----


Discover and set flight modes at runtime. 


The modes are queried from the vehicle using the MAVLink standard modes protocol (AVAILABLE_MODES, CURRENT_MODE). For autopilots that don't implement it, a list of known modes is used as a fallback.


[Modes](classmavsdk_1_1_modes.md) are identified by name. Standard modes have a fixed name, e.g. "Position" or "Mission", custom modes use the name reported by the vehicle. 


## Data Structures


struct [Mode](structmavsdk_1_1_modes_1_1_mode.md)

## Public Types


Type | Description
--- | ---
enum [StandardMode](#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f) | Standard mode.
enum [Result](#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) | Possible results returned for mode requests.
std::function< void([Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c))> [ResultCallback](#classmavsdk_1_1_modes_1a7be484d39016a070a7197ca125c02e6e) | Callback type for asynchronous [Modes](classmavsdk_1_1_modes.md) calls.
std::function< void(std::vector< [Mode](structmavsdk_1_1_modes_1_1_mode.md) >)> [AvailableModesCallback](#classmavsdk_1_1_modes_1a72459eb3770c061df9091171724de0fc) | Callback type for subscribe_available_modes.
[Handle](classmavsdk_1_1_handle.md)< std::vector< [Mode](structmavsdk_1_1_modes_1_1_mode.md) > > [AvailableModesHandle](#classmavsdk_1_1_modes_1a2dd3e0a89d40f91253d2aa50f822ded1) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_available_modes.
std::function< void([Mode](structmavsdk_1_1_modes_1_1_mode.md))> [CurrentModeCallback](#classmavsdk_1_1_modes_1a78e6a9114493f8a2529aa2e78c209700) | Callback type for subscribe_current_mode.
[Handle](classmavsdk_1_1_handle.md)< [Mode](structmavsdk_1_1_modes_1_1_mode.md) > [CurrentModeHandle](#classmavsdk_1_1_modes_1a30444308c870abdc0d504510e13c01f1) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_current_mode.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Modes](#classmavsdk_1_1_modes_1a8eef812a07efdb7f8f25d8157d8d96b4) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Modes](#classmavsdk_1_1_modes_1a7116a9512aafb88d6d5dc2ee2befa10d) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Modes](#classmavsdk_1_1_modes_1ae2ad276b8627c9b21efe7c2cf7508601) () override | Destructor (internal use only).
&nbsp; | [Modes](#classmavsdk_1_1_modes_1a14e2bd6010e5b7c25470f4aea74f28dc) (const Modes & other) | Copy constructor.
[AvailableModesHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a2dd3e0a89d40f91253d2aa50f822ded1) | [subscribe_available_modes](#classmavsdk_1_1_modes_1a2e5140a6487f05df7df167db67bb67b8) (const [AvailableModesCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a72459eb3770c061df9091171724de0fc) & callback) | Subscribe to the list of available modes.
void | [unsubscribe_available_modes](#classmavsdk_1_1_modes_1a85b53fe77fb6eeac7653a3656e72fa31) ([AvailableModesHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a2dd3e0a89d40f91253d2aa50f822ded1) handle) | Unsubscribe from subscribe_available_modes.
std::vector< [Mode](structmavsdk_1_1_modes_1_1_mode.md) > | [available_modes](#classmavsdk_1_1_modes_1a526cfdd4da5e3525a835cb84c51d78c9) () const | Poll for 'std::vector<Mode>' (blocking).
[CurrentModeHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a30444308c870abdc0d504510e13c01f1) | [subscribe_current_mode](#classmavsdk_1_1_modes_1aac542b8cc01d0b9f80d6af64c3ea99fb) (const [CurrentModeCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a78e6a9114493f8a2529aa2e78c209700) & callback) | Subscribe to the current mode.
void | [unsubscribe_current_mode](#classmavsdk_1_1_modes_1af953a21419fb1c60f873df86908b5d4d) ([CurrentModeHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a30444308c870abdc0d504510e13c01f1) handle) | Unsubscribe from subscribe_current_mode.
[Mode](structmavsdk_1_1_modes_1_1_mode.md) | [current_mode](#classmavsdk_1_1_modes_1a1c1f29355c56a24b37434ad2c5908135) () const | Poll for '[Mode](structmavsdk_1_1_modes_1_1_mode.md)' (blocking).
void | [set_mode_async](#classmavsdk_1_1_modes_1ac3391e69db47e486b3a162e1b3862541) (std::string name, const [ResultCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a7be484d39016a070a7197ca125c02e6e) callback) | Set mode by name.
[Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) | [set_mode](#classmavsdk_1_1_modes_1abaff878b0f8963eb7df02e6c8f563712) (std::string name)const | Set mode by name.
void | [set_standard_mode_async](#classmavsdk_1_1_modes_1acc7168f461137eb6c1e44fff39f0cf5a) ([StandardMode](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f) standard_mode, const [ResultCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a7be484d39016a070a7197ca125c02e6e) callback) | Set a standard mode.
[Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) | [set_standard_mode](#classmavsdk_1_1_modes_1a335479e64ffb6f46779c6cb82972c07a) ([StandardMode](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f) standard_mode)const | Set a standard mode.
const [Modes](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a8eef812a07efdb7f8f25d8157d8d96b4) & | [operator=](#classmavsdk_1_1_modes_1ad3cb22ad695fc5c86cfc82d64fb73f32) (const [Modes](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a8eef812a07efdb7f8f25d8157d8d96b4) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Modes() {#classmavsdk_1_1_modes_1a8eef812a07efdb7f8f25d8157d8d96b4}
```cpp
mavsdk::Modes::Modes(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto modes = Modes(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Modes() {#classmavsdk_1_1_modes_1a7116a9512aafb88d6d5dc2ee2befa10d}
```cpp
mavsdk::Modes::Modes(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto modes = Modes(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Modes() {#classmavsdk_1_1_modes_1ae2ad276b8627c9b21efe7c2cf7508601}
```cpp
mavsdk::Modes::~Modes() override
```


Destructor (internal use only).


### Modes() {#classmavsdk_1_1_modes_1a14e2bd6010e5b7c25470f4aea74f28dc}
```cpp
mavsdk::Modes::Modes(const Modes &other)
```


Copy constructor.


**Parameters**

* const Modes& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_modes_1a7be484d39016a070a7197ca125c02e6e}

```cpp
using mavsdk::Modes::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Modes](classmavsdk_1_1_modes.md) calls.


### typedef AvailableModesCallback {#classmavsdk_1_1_modes_1a72459eb3770c061df9091171724de0fc}

```cpp
using mavsdk::Modes::AvailableModesCallback =  std::function<void(std::vector<Mode>)>
```


Callback type for subscribe_available_modes.


### typedef AvailableModesHandle {#classmavsdk_1_1_modes_1a2dd3e0a89d40f91253d2aa50f822ded1}

```cpp
using mavsdk::Modes::AvailableModesHandle =  Handle<std::vector<Mode>>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_available_modes.


### typedef CurrentModeCallback {#classmavsdk_1_1_modes_1a78e6a9114493f8a2529aa2e78c209700}

```cpp
using mavsdk::Modes::CurrentModeCallback =  std::function<void(Mode)>
```


Callback type for subscribe_current_mode.


### typedef CurrentModeHandle {#classmavsdk_1_1_modes_1a30444308c870abdc0d504510e13c01f1}

```cpp
using mavsdk::Modes::CurrentModeHandle =  Handle<Mode>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_current_mode.


## Member Enumeration Documentation


### enum StandardMode {#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f}


Standard mode.

See [https://mavlink.io/en/messages/common.html#MAV_STANDARD_MODE](https://mavlink.io/en/messages/common.html#MAV_STANDARD_MODE)

Value | Description
--- | ---
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa1a50650f175a7245612a1a9b54707b30"></span> `NonStandard` | Non-standard mode, specific to the autopilot. 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa8d2de68ea2876232af1076cd4f7e2565"></span> `PositionHold` | Position hold, named "Position". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa76e1ab7a1df8e64fe867b420645c8a68"></span> `Orbit` | Orbit, named "Orbit". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fab4137253e38d50243b9983114c671d69"></span> `Cruise` | Cruise, named "Cruise". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa567370df7e57e1c165827b5676af4957"></span> `AltitudeHold` | Altitude hold, named "Altitude". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa54df7f8a6a971da836ed3306f5a7d4df"></span> `SafeRecovery` | Safe recovery such as return to launch, named "Safe Recovery". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa70d529695c253d17e992cb9265abc57f"></span> `Mission` | [Mission](classmavsdk_1_1_mission.md), named "Mission". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa512ef7c688a2c8572d5e16f44e17e869"></span> `Land` | Land, named "Land". 
<span id="classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503fa56373a80447c41b9a29e500e62d6884e"></span> `Takeoff` | Takeoff, named "Takeoff". 

### enum Result {#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c}


Possible results returned for mode requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875cad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | [Vehicle](namespacemavsdk.md#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b) is busy. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | Request was denied, e.g. because the mode can't be entered right now. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875cab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | [Mode](structmavsdk_1_1_modes_1_1_mode.md) or command is not supported. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875cac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875cad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Request failed. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca9ebbfdcdae677078a0b4c5a3368f9d55"></span> `ModeNotFound` | No available mode with this name. 
<span id="classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875ca63b74536b30c2674f2f4312851913d4a"></span> `ModesNotKnownYet` | The available modes have not been received yet. 

## Member Function Documentation


### subscribe_available_modes() {#classmavsdk_1_1_modes_1a2e5140a6487f05df7df167db67bb67b8}
```cpp
AvailableModesHandle mavsdk::Modes::subscribe_available_modes(const AvailableModesCallback &callback)
```


Subscribe to the list of available modes.

The list is updated whenever the vehicle reports that its modes have changed.

**Parameters**

* const [AvailableModesCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a72459eb3770c061df9091171724de0fc)& **callback** - 

**Returns**

&emsp;[AvailableModesHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a2dd3e0a89d40f91253d2aa50f822ded1) - 

### unsubscribe_available_modes() {#classmavsdk_1_1_modes_1a85b53fe77fb6eeac7653a3656e72fa31}
```cpp
void mavsdk::Modes::unsubscribe_available_modes(AvailableModesHandle handle)
```


Unsubscribe from subscribe_available_modes.


**Parameters**

* [AvailableModesHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a2dd3e0a89d40f91253d2aa50f822ded1) **handle** - 

### available_modes() {#classmavsdk_1_1_modes_1a526cfdd4da5e3525a835cb84c51d78c9}
```cpp
std::vector< Mode > mavsdk::Modes::available_modes() const
```


Poll for 'std::vector<Mode>' (blocking).


**Returns**

&emsp;std::vector< [Mode](structmavsdk_1_1_modes_1_1_mode.md) > - One std::vector<Mode> update.

### subscribe_current_mode() {#classmavsdk_1_1_modes_1aac542b8cc01d0b9f80d6af64c3ea99fb}
```cpp
CurrentModeHandle mavsdk::Modes::subscribe_current_mode(const CurrentModeCallback &callback)
```


Subscribe to the current mode.


**Parameters**

* const [CurrentModeCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a78e6a9114493f8a2529aa2e78c209700)& **callback** - 

**Returns**

&emsp;[CurrentModeHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a30444308c870abdc0d504510e13c01f1) - 

### unsubscribe_current_mode() {#classmavsdk_1_1_modes_1af953a21419fb1c60f873df86908b5d4d}
```cpp
void mavsdk::Modes::unsubscribe_current_mode(CurrentModeHandle handle)
```


Unsubscribe from subscribe_current_mode.


**Parameters**

* [CurrentModeHandle](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a30444308c870abdc0d504510e13c01f1) **handle** - 

### current_mode() {#classmavsdk_1_1_modes_1a1c1f29355c56a24b37434ad2c5908135}
```cpp
Mode mavsdk::Modes::current_mode() const
```


Poll for '[Mode](structmavsdk_1_1_modes_1_1_mode.md)' (blocking).


**Returns**

&emsp;[Mode](structmavsdk_1_1_modes_1_1_mode.md) - One [Mode](structmavsdk_1_1_modes_1_1_mode.md) update.

### set_mode_async() {#classmavsdk_1_1_modes_1ac3391e69db47e486b3a162e1b3862541}
```cpp
void mavsdk::Modes::set_mode_async(std::string name, const ResultCallback callback)
```


Set mode by name.

The name needs to match exactly (case-sensitive) the name of one of the available modes.


This function is non-blocking. See 'set_mode' for the blocking counterpart.

**Parameters**

* std::string **name** - 
* const [ResultCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a7be484d39016a070a7197ca125c02e6e) **callback** - 

### set_mode() {#classmavsdk_1_1_modes_1abaff878b0f8963eb7df02e6c8f563712}
```cpp
Result mavsdk::Modes::set_mode(std::string name) const
```


Set mode by name.

The name needs to match exactly (case-sensitive) the name of one of the available modes.


This function is blocking. See 'set_mode_async' for the non-blocking counterpart.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) - [Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) of request.

### set_standard_mode_async() {#classmavsdk_1_1_modes_1acc7168f461137eb6c1e44fff39f0cf5a}
```cpp
void mavsdk::Modes::set_standard_mode_async(StandardMode standard_mode, const ResultCallback callback)
```


Set a standard mode.

This function is non-blocking. See 'set_standard_mode' for the blocking counterpart.

**Parameters**

* [StandardMode](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f) **standard_mode** - 
* const [ResultCallback](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a7be484d39016a070a7197ca125c02e6e) **callback** - 

### set_standard_mode() {#classmavsdk_1_1_modes_1a335479e64ffb6f46779c6cb82972c07a}
```cpp
Result mavsdk::Modes::set_standard_mode(StandardMode standard_mode) const
```


Set a standard mode.

This function is blocking. See 'set_standard_mode_async' for the non-blocking counterpart.

**Parameters**

* [StandardMode](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f) **standard_mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) - [Result](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a4f59f3b330144fa7f6134c82686d875c) of request.

### operator=() {#classmavsdk_1_1_modes_1ad3cb22ad695fc5c86cfc82d64fb73f32}
```cpp
const Modes & mavsdk::Modes::operator=(const Modes &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Modes](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a8eef812a07efdb7f8f25d8157d8d96b4)&  - 

**Returns**

&emsp;const [Modes](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a8eef812a07efdb7f8f25d8157d8d96b4) & - 