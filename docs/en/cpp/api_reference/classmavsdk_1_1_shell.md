# mavsdk::Shell Class Reference
`#include: shell.hpp`

----


Allow to communicate with the vehicle's system shell. 


Under the hood this uses MAVLink SERIAL_CONTROL. The default device is SERIAL_CONTROL_DEV_SHELL. Callers can pass another SERIAL_CONTROL_DEV on Send (and observe the device on [Receive](structmavsdk_1_1_shell_1_1_receive.md)) when the same framing is used for non-nsh serial bridges (for example TELEM2). 


## Data Structures


struct [Receive](structmavsdk_1_1_shell_1_1_receive.md)

## Public Types


Type | Description
--- | ---
enum [Device](#classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9) | MAVLink SERIAL_CONTROL_DEV values used by the shell plugin.
enum [Result](#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8) | Possible results returned for shell requests.
std::function< void([Result](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8))> [ResultCallback](#classmavsdk_1_1_shell_1a4937843446c999606349ad438f8d682d) | Callback type for asynchronous [Shell](classmavsdk_1_1_shell.md) calls.
std::function< void([Receive](structmavsdk_1_1_shell_1_1_receive.md))> [ReceiveCallback](#classmavsdk_1_1_shell_1adf8a75f2d359ef74242095876cf85c80) | Callback type for subscribe_receive.
[Handle](classmavsdk_1_1_handle.md)< [Receive](structmavsdk_1_1_shell_1_1_receive.md) > [ReceiveHandle](#classmavsdk_1_1_shell_1a99fb31adcf5a5065c2e886456093c097) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_receive.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Shell](#classmavsdk_1_1_shell_1a31a80044ee4822e8b9ac1c515b0eea90) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Shell](#classmavsdk_1_1_shell_1ae6c98c4c854ff0803260fe49bad20a31) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Shell](#classmavsdk_1_1_shell_1aad035d078495e85c700d4c0148c5f4f9) () override | Destructor (internal use only).
&nbsp; | [Shell](#classmavsdk_1_1_shell_1aa9d95d880297fca1a5cba341633e660e) (const [Shell](classmavsdk_1_1_shell.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8) | [send](#classmavsdk_1_1_shell_1a40ed558219758cfada313801664dc6ef) (std::string command, [Device](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9) device)const | Send a command line.
[ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a99fb31adcf5a5065c2e886456093c097) | [subscribe_receive](#classmavsdk_1_1_shell_1a2794ac389f4df4f1aaa344612bc8c470) (const [ReceiveCallback](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1adf8a75f2d359ef74242095876cf85c80) & callback) | [Receive](structmavsdk_1_1_shell_1_1_receive.md) feedback from a sent command line.
void | [unsubscribe_receive](#classmavsdk_1_1_shell_1a5b696e1651459dbc3ceef2a393af433d) ([ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a99fb31adcf5a5065c2e886456093c097) handle) | Unsubscribe from subscribe_receive.
const [Shell](classmavsdk_1_1_shell.md) & | [operator=](#classmavsdk_1_1_shell_1abd920b11e6535152ad85cb4187b0c620) (const [Shell](classmavsdk_1_1_shell.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Shell() {#classmavsdk_1_1_shell_1a31a80044ee4822e8b9ac1c515b0eea90}
```cpp
mavsdk::Shell::Shell(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto shell = Shell(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Shell() {#classmavsdk_1_1_shell_1ae6c98c4c854ff0803260fe49bad20a31}
```cpp
mavsdk::Shell::Shell(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto shell = Shell(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Shell() {#classmavsdk_1_1_shell_1aad035d078495e85c700d4c0148c5f4f9}
```cpp
mavsdk::Shell::~Shell() override
```


Destructor (internal use only).


### Shell() {#classmavsdk_1_1_shell_1aa9d95d880297fca1a5cba341633e660e}
```cpp
mavsdk::Shell::Shell(const Shell &other)
```


Copy constructor.


**Parameters**

* const [Shell](classmavsdk_1_1_shell.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_shell_1a4937843446c999606349ad438f8d682d}

```cpp
using mavsdk::Shell::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Shell](classmavsdk_1_1_shell.md) calls.


### typedef ReceiveCallback {#classmavsdk_1_1_shell_1adf8a75f2d359ef74242095876cf85c80}

```cpp
using mavsdk::Shell::ReceiveCallback =  std::function<void(Receive)>
```


Callback type for subscribe_receive.


### typedef ReceiveHandle {#classmavsdk_1_1_shell_1a99fb31adcf5a5065c2e886456093c097}

```cpp
using mavsdk::Shell::ReceiveHandle =  Handle<Receive>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_receive.


## Member Enumeration Documentation


### enum Device {#classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9}


MAVLink SERIAL_CONTROL_DEV values used by the shell plugin.


Value | Description
--- | ---
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a02799d4aa8b02b75acc9c93c2b180599"></span> `Telem1` | SERIAL_CONTROL_DEV_TELEM1. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a89ca01a95fb7263652d4ea8eef2881a6"></span> `Telem2` | SERIAL_CONTROL_DEV_TELEM2. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a2f28eb5d0326b1414b86f4e75d47e76e"></span> `Gps1` | SERIAL_CONTROL_DEV_GPS1. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9aebc01a35aabdad443277eb765d9bb1d6"></span> `Gps2` | SERIAL_CONTROL_DEV_GPS2. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9aea89b68c34ce4a63c0f77e17413c6e30"></span> `Shell` | SERIAL_CONTROL_DEV_SHELL (default). 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9ae7624a5b8b92b37d2ee08349ca2d63b8"></span> `Serial0` | SERIAL_CONTROL_SERIAL0. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a628522b8331388ef51656fd3841b3cc6"></span> `Serial1` | SERIAL_CONTROL_SERIAL1. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9ae5e7275234bd4dfd0d8cb155bf615a0b"></span> `Serial2` | SERIAL_CONTROL_SERIAL2. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a6855bbc823205117f17ea4ace755c1fe"></span> `Serial3` | SERIAL_CONTROL_SERIAL3. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a5c95b1fbe533ed08d841b0b23a045d3d"></span> `Serial4` | SERIAL_CONTROL_SERIAL4. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a9e736ee03303dc898a6e64ec1c22bc0c"></span> `Serial5` | SERIAL_CONTROL_SERIAL5. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a75831248370b0b9916db1972c3c7b8ae"></span> `Serial6` | SERIAL_CONTROL_SERIAL6. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a99c456d707ba02509976c4916e4765ca"></span> `Serial7` | SERIAL_CONTROL_SERIAL7. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9a746f18f16e5da5c7ab448950b5b6e737"></span> `Serial8` | SERIAL_CONTROL_SERIAL8. 
<span id="classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9afb7cc9fda54daeb0005fb4c798008605"></span> `Serial9` | SERIAL_CONTROL_SERIAL9. 

### enum Result {#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8}


Possible results returned for shell requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8a0e976dcd18516429d344402e6f5524d3"></span> `NoResponse` | Response was not received. 
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | [Shell](classmavsdk_1_1_shell.md) busy (transfer in progress). 
<span id="classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8a253ca7dd096ee0956cccee4d376cab8b"></span> `InvalidArgument` | Invalid device / argument. 

## Member Function Documentation


### send() {#classmavsdk_1_1_shell_1a40ed558219758cfada313801664dc6ef}
```cpp
Result mavsdk::Shell::send(std::string command, Device device) const
```


Send a command line.

This function is blocking.

**Parameters**

* std::string **command** - 
* [Device](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a02ecd441e182bfef27e9034ea5a74aa9) **device** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8) - Result of request.

### subscribe_receive() {#classmavsdk_1_1_shell_1a2794ac389f4df4f1aaa344612bc8c470}
```cpp
ReceiveHandle mavsdk::Shell::subscribe_receive(const ReceiveCallback &callback)
```


[Receive](structmavsdk_1_1_shell_1_1_receive.md) feedback from a sent command line.

This subscription needs to be made before a command line is sent, otherwise, no response will be sent.

**Parameters**

* const [ReceiveCallback](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1adf8a75f2d359ef74242095876cf85c80)& **callback** - 

**Returns**

&emsp;[ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a99fb31adcf5a5065c2e886456093c097) - 

### unsubscribe_receive() {#classmavsdk_1_1_shell_1a5b696e1651459dbc3ceef2a393af433d}
```cpp
void mavsdk::Shell::unsubscribe_receive(ReceiveHandle handle)
```


Unsubscribe from subscribe_receive.


**Parameters**

* [ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a99fb31adcf5a5065c2e886456093c097) **handle** - 

### operator=() {#classmavsdk_1_1_shell_1abd920b11e6535152ad85cb4187b0c620}
```cpp
const Shell & mavsdk::Shell::operator=(const Shell &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Shell](classmavsdk_1_1_shell.md)&  - 

**Returns**

&emsp;const [Shell](classmavsdk_1_1_shell.md) & - 