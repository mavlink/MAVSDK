# mavsdk::Shell Class Reference
`#include: shell.h`

----


<ul>
<li><p>Allow to communicate with the vehicle's system shell. </p>
</li>
</ul>


## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8) | Possible results returned for shell requests.
std::function< void([Result](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8))> [ResultCallback](#classmavsdk_1_1_shell_1a4937843446c999606349ad438f8d682d) | Callback type for asynchronous [Shell](classmavsdk_1_1_shell.md) calls.
std::function< void(std::string)> [ReceiveCallback](#classmavsdk_1_1_shell_1adfa64ede96967ae1ab5a5ecd83032dbb) | Callback type for subscribe_receive.
[Handle](classmavsdk_1_1_handle.md)< std::string > [ReceiveHandle](#classmavsdk_1_1_shell_1aea7ab47a9a86aa3f91e71306cc9b430b) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_receive.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Shell](#classmavsdk_1_1_shell_1a31a80044ee4822e8b9ac1c515b0eea90) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Shell](#classmavsdk_1_1_shell_1ae6c98c4c854ff0803260fe49bad20a31) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Shell](#classmavsdk_1_1_shell_1aad035d078495e85c700d4c0148c5f4f9) () override | Destructor (internal use only).
&nbsp; | [Shell](#classmavsdk_1_1_shell_1aa9d95d880297fca1a5cba341633e660e) (const [Shell](classmavsdk_1_1_shell.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8) | [send](#classmavsdk_1_1_shell_1a7b39022ce3be914eec82b53a76d19bc7) (std::string command)const | Send a command line.
[ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1aea7ab47a9a86aa3f91e71306cc9b430b) | [subscribe_receive](#classmavsdk_1_1_shell_1a2794ac389f4df4f1aaa344612bc8c470) (const [ReceiveCallback](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1adfa64ede96967ae1ab5a5ecd83032dbb) & callback) | Receive feedback from a sent command line.
void | [unsubscribe_receive](#classmavsdk_1_1_shell_1a5b696e1651459dbc3ceef2a393af433d) ([ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1aea7ab47a9a86aa3f91e71306cc9b430b) handle) | Unsubscribe from subscribe_receive.
const [Shell](classmavsdk_1_1_shell.md) & | [operator=](#classmavsdk_1_1_shell_1a492f8b2e36ef2468522bfd0f51f4b9b8) (const [Shell](classmavsdk_1_1_shell.md) &)=delete | Equality operator (object is not copyable).


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


### typedef ReceiveCallback {#classmavsdk_1_1_shell_1adfa64ede96967ae1ab5a5ecd83032dbb}

```cpp
using mavsdk::Shell::ReceiveCallback =  std::function<void(std::string)>
```


Callback type for subscribe_receive.


### typedef ReceiveHandle {#classmavsdk_1_1_shell_1aea7ab47a9a86aa3f91e71306cc9b430b}

```cpp
using mavsdk::Shell::ReceiveHandle =  Handle<std::string>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_receive.


## Member Enumeration Documentation


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

## Member Function Documentation


### send() {#classmavsdk_1_1_shell_1a7b39022ce3be914eec82b53a76d19bc7}
```cpp
Result mavsdk::Shell::send(std::string command) const
```


Send a command line.

This function is blocking.

**Parameters**

* std::string **command** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1a768bfa296ba3309f936f887fb86c9ba8) - Result of request.

### subscribe_receive() {#classmavsdk_1_1_shell_1a2794ac389f4df4f1aaa344612bc8c470}
```cpp
ReceiveHandle mavsdk::Shell::subscribe_receive(const ReceiveCallback &callback)
```


Receive feedback from a sent command line.

This subscription needs to be made before a command line is sent, otherwise, no response will be sent.

**Parameters**

* const [ReceiveCallback](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1adfa64ede96967ae1ab5a5ecd83032dbb)& **callback** - 

**Returns**

&emsp;[ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1aea7ab47a9a86aa3f91e71306cc9b430b) - 

### unsubscribe_receive() {#classmavsdk_1_1_shell_1a5b696e1651459dbc3ceef2a393af433d}
```cpp
void mavsdk::Shell::unsubscribe_receive(ReceiveHandle handle)
```


Unsubscribe from subscribe_receive.


**Parameters**

* [ReceiveHandle](classmavsdk_1_1_shell.md#classmavsdk_1_1_shell_1aea7ab47a9a86aa3f91e71306cc9b430b) **handle** - 

### operator=() {#classmavsdk_1_1_shell_1a492f8b2e36ef2468522bfd0f51f4b9b8}
```cpp
const Shell& mavsdk::Shell::operator=(const Shell &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Shell](classmavsdk_1_1_shell.md)&  - 

**Returns**

&emsp;const [Shell](classmavsdk_1_1_shell.md) & - 