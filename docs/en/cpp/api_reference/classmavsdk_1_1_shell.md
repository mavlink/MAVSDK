# mavsdk::Shell Class Reference
`#include: shell.hpp`

----


Allow to communicate with the vehicle's system shell.

Under the hood this uses MAVLink SERIAL_CONTROL. The default device is SERIAL_CONTROL_DEV_SHELL. Callers can pass another SERIAL_CONTROL_DEV on Send (and observe the device on Receive) when the same framing is used for non-nsh serial bridges (for example TELEM2).


## Public Types


Type | Description
--- | ---
enum [Device](#enum-device) | MAVLink SERIAL_CONTROL_DEV values used by the shell plugin.
struct [Receive](#struct-receive) | Received shell data and source device.
enum [Result](#enum-result) | Possible results returned for shell requests.
std::function< void([Result](#enum-result))> [ResultCallback](#typedef-resultcallback) | Callback type for asynchronous [Shell](classmavsdk_1_1_shell.md) calls.
std::function< void([Receive](#struct-receive))> [ReceiveCallback](#typedef-receivecallback) | Callback type for subscribe_receive.
[Handle](classmavsdk_1_1_handle.md)< [Receive](#struct-receive) > [ReceiveHandle](#typedef-receivehandle) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_receive.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Shell](#shell-system) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Shell](#shell-shared-system) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Shell](#shell-dtor) () override | Destructor (internal use only).
&nbsp; | [Shell](#shell-copy) (const [Shell](classmavsdk_1_1_shell.md) & other) | Copy constructor.
[Result](#enum-result) | [send](#send) (std::string command, [Device](#enum-device) device)const | Send a command line.
[ReceiveHandle](#typedef-receivehandle) | [subscribe_receive](#subscribe_receive) (const [ReceiveCallback](#typedef-receivecallback) & callback) | Receive feedback from a sent command line.
void | [unsubscribe_receive](#unsubscribe_receive) ([ReceiveHandle](#typedef-receivehandle) handle) | Unsubscribe from subscribe_receive.
const [Shell](classmavsdk_1_1_shell.md) & | [operator=](#operator-eq) (const [Shell](classmavsdk_1_1_shell.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Shell() {#shell-system}
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

### Shell() {#shell-shared-system}
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

### ~Shell() {#shell-dtor}
```cpp
mavsdk::Shell::~Shell() override
```


Destructor (internal use only).


### Shell() {#shell-copy}
```cpp
mavsdk::Shell::Shell(const Shell &other)
```


Copy constructor.


**Parameters**

* const [Shell](classmavsdk_1_1_shell.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#typedef-resultcallback}

```cpp
using mavsdk::Shell::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Shell](classmavsdk_1_1_shell.md) calls.


### typedef ReceiveCallback {#typedef-receivecallback}

```cpp
using mavsdk::Shell::ReceiveCallback =  std::function<void(Receive)>
```


Callback type for subscribe_receive.


### typedef ReceiveHandle {#typedef-receivehandle}

```cpp
using mavsdk::Shell::ReceiveHandle =  Handle<Receive>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_receive.


## Member Enumeration Documentation


### enum Device {#enum-device}


MAVLink SERIAL_CONTROL_DEV values used by the shell plugin.


Value | Description
--- | ---
`Telem1` | SERIAL_CONTROL_DEV_TELEM1. 
`Telem2` | SERIAL_CONTROL_DEV_TELEM2. 
`Gps1` | SERIAL_CONTROL_DEV_GPS1. 
`Gps2` | SERIAL_CONTROL_DEV_GPS2. 
`Shell` | SERIAL_CONTROL_DEV_SHELL (default). 
`Serial0` | SERIAL_CONTROL_SERIAL0. 
`Serial1` | SERIAL_CONTROL_SERIAL1. 
`Serial2` | SERIAL_CONTROL_SERIAL2. 
`Serial3` | SERIAL_CONTROL_SERIAL3. 
`Serial4` | SERIAL_CONTROL_SERIAL4. 
`Serial5` | SERIAL_CONTROL_SERIAL5. 
`Serial6` | SERIAL_CONTROL_SERIAL6. 
`Serial7` | SERIAL_CONTROL_SERIAL7. 
`Serial8` | SERIAL_CONTROL_SERIAL8. 
`Serial9` | SERIAL_CONTROL_SERIAL9. 


### enum Result {#enum-result}


Possible results returned for shell requests.


Value | Description
--- | ---
`Unknown` | Unknown result. 
`Success` | Request succeeded. 
`NoSystem` | No system is connected. 
`ConnectionError` | Connection error. 
`NoResponse` | Response was not received. 
`Busy` | [Shell](classmavsdk_1_1_shell.md) busy (transfer in progress). 
`InvalidArgument` | Invalid device / argument. 

## Member Data Documentation


### struct Receive {#struct-receive}


Received shell data.


Field | Type | Description
--- | --- | ---
`data` | std::string | Received data. 
`device` | [Device](#enum-device) | SERIAL_CONTROL device the data came from. 

## Member Function Documentation


### send() {#send}
```cpp
Result mavsdk::Shell::send(std::string command, Device device) const
```


Send a command line.

This function is blocking.

**Parameters**

* std::string **command** - The command line to send.
* [Device](#enum-device) **device** - SERIAL_CONTROL device to target (default is `Device::Shell`).

**Returns**

&emsp;[Result](#enum-result) - Result of request.

### subscribe_receive() {#subscribe_receive}
```cpp
ReceiveHandle mavsdk::Shell::subscribe_receive(const ReceiveCallback &callback)
```


Receive feedback from a sent command line.

This subscription needs to be made before a command line is sent, otherwise, no response will be sent.

**Parameters**

* const [ReceiveCallback](#typedef-receivecallback)& **callback** - 

**Returns**

&emsp;[ReceiveHandle](#typedef-receivehandle) - 

### unsubscribe_receive() {#unsubscribe_receive}
```cpp
void mavsdk::Shell::unsubscribe_receive(ReceiveHandle handle)
```


Unsubscribe from subscribe_receive.


**Parameters**

* [ReceiveHandle](#typedef-receivehandle) **handle** - 

### operator=() {#operator-eq}
```cpp
const Shell & mavsdk::Shell::operator=(const Shell &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Shell](classmavsdk_1_1_shell.md)&  - 

**Returns**

&emsp;const [Shell](classmavsdk_1_1_shell.md) & - 
