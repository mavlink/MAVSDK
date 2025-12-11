# mavsdk::RemoteId Class Reference
`#include: UNKNOWN`

----


## Data Structures


struct [ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md)

struct [BasicId](structmavsdk_1_1_remote_id_1_1_basic_id.md)

struct [Location](structmavsdk_1_1_remote_id_1_1_location.md)

struct [OperatorId](structmavsdk_1_1_remote_id_1_1_operator_id.md)

struct [SelfId](structmavsdk_1_1_remote_id_1_1_self_id.md)

struct [SystemId](structmavsdk_1_1_remote_id_1_1_system_id.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) | Possible results returned for remote_id commands.
std::function< void([Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f))> [ResultCallback](#classmavsdk_1_1_remote_id_1a0ef037766fe7950a268604c1dab45eee) | Callback type for asynchronous [RemoteId](classmavsdk_1_1_remote_id.md) calls.
std::function< void([ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md))> [ArmStatusCallback](#classmavsdk_1_1_remote_id_1ac8cbe00a1b4e2e6ed305fe19e8c43917) | Callback type for subscribe_arm_status.
[Handle](classmavsdk_1_1_handle.md)< [ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md) > [ArmStatusHandle](#classmavsdk_1_1_remote_id_1ad0b845827527f615d80d9fb87c7f71c2) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_arm_status.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [RemoteId](#classmavsdk_1_1_remote_id_1a85744d4601b530b4c3eab2c6667bfc99) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [RemoteId](#classmavsdk_1_1_remote_id_1a7f263fe9a042be107a9cc7b019596d42) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~RemoteId](#classmavsdk_1_1_remote_id_1acc2dcdab3a73ec314bff012e342797c0) () override | Destructor (internal use only).
&nbsp; | [RemoteId](#classmavsdk_1_1_remote_id_1a4ae800b738b5e27c4c24009da383b651) (const [RemoteId](classmavsdk_1_1_remote_id.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) | [set_basic_id](#classmavsdk_1_1_remote_id_1ac712901a49783dae9a4cff6176dafed2) ([BasicId](structmavsdk_1_1_remote_id_1_1_basic_id.md) basic_id)const | Update the [BasicId](structmavsdk_1_1_remote_id_1_1_basic_id.md) structure sent with the basic_id packet.
[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) | [set_location](#classmavsdk_1_1_remote_id_1a8286526708c4d879048f2e5711aa8e31) ([Location](structmavsdk_1_1_remote_id_1_1_location.md) location)const | Update the [Location](structmavsdk_1_1_remote_id_1_1_location.md) structure sent with the location packet.
[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) | [set_system](#classmavsdk_1_1_remote_id_1a2dda9deef2fc4edfd0f8048ad918b832) ([SystemId](structmavsdk_1_1_remote_id_1_1_system_id.md) system)const | Update the [System](classmavsdk_1_1_system.md) structure sent with the system packet.
[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) | [set_operator_id](#classmavsdk_1_1_remote_id_1af851f77ea8be3e0ae0d7f8d666c0226e) ([OperatorId](structmavsdk_1_1_remote_id_1_1_operator_id.md) system)const | Update the [OperatorId](structmavsdk_1_1_remote_id_1_1_operator_id.md) structure sent with the operator_id packet.
[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) | [set_self_id](#classmavsdk_1_1_remote_id_1a6360566541b3fd737c6ad4fbc3067082) ([SelfId](structmavsdk_1_1_remote_id_1_1_self_id.md) self_id)const | Update the SetSelfId structure sent with the self_id packet.
[ArmStatusHandle](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ad0b845827527f615d80d9fb87c7f71c2) | [subscribe_arm_status](#classmavsdk_1_1_remote_id_1a055d1d2a3acdba5a0f4affdd683ed266) (const [ArmStatusCallback](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac8cbe00a1b4e2e6ed305fe19e8c43917) & callback) | Subscribe to arm status updates.
void | [unsubscribe_arm_status](#classmavsdk_1_1_remote_id_1ade1ca076aa7dd4d9e4c31d0305172d0e) ([ArmStatusHandle](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ad0b845827527f615d80d9fb87c7f71c2) handle) | Unsubscribe from subscribe_arm_status.
[ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md) | [arm_status](#classmavsdk_1_1_remote_id_1a8d52d5cf1c4f2a0efb14b09f8edc6896) () const | Poll for '[ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md)' (blocking).
const [RemoteId](classmavsdk_1_1_remote_id.md) & | [operator=](#classmavsdk_1_1_remote_id_1acdf4d016fb9699d48d1ff473dc944566) (const [RemoteId](classmavsdk_1_1_remote_id.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### RemoteId() {#classmavsdk_1_1_remote_id_1a85744d4601b530b4c3eab2c6667bfc99}
```cpp
mavsdk::RemoteId::RemoteId(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto remote_id = RemoteId(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### RemoteId() {#classmavsdk_1_1_remote_id_1a7f263fe9a042be107a9cc7b019596d42}
```cpp
mavsdk::RemoteId::RemoteId(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto remote_id = RemoteId(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~RemoteId() {#classmavsdk_1_1_remote_id_1acc2dcdab3a73ec314bff012e342797c0}
```cpp
mavsdk::RemoteId::~RemoteId() override
```


Destructor (internal use only).


### RemoteId() {#classmavsdk_1_1_remote_id_1a4ae800b738b5e27c4c24009da383b651}
```cpp
mavsdk::RemoteId::RemoteId(const RemoteId &other)
```


Copy constructor.


**Parameters**

* const [RemoteId](classmavsdk_1_1_remote_id.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_remote_id_1a0ef037766fe7950a268604c1dab45eee}

```cpp
using mavsdk::RemoteId::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [RemoteId](classmavsdk_1_1_remote_id.md) calls.


### typedef ArmStatusCallback {#classmavsdk_1_1_remote_id_1ac8cbe00a1b4e2e6ed305fe19e8c43917}

```cpp
using mavsdk::RemoteId::ArmStatusCallback =  std::function<void(ArmStatus)>
```


Callback type for subscribe_arm_status.


### typedef ArmStatusHandle {#classmavsdk_1_1_remote_id_1ad0b845827527f615d80d9fb87c7f71c2}

```cpp
using mavsdk::RemoteId::ArmStatusHandle =  Handle<ArmStatus>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_arm_status.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f}


Possible results returned for remote_id commands.


Value | Description
--- | ---
<span id="classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760fa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760fa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Command executed successfully. 
<span id="classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760fa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760fa902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | An error has occurred while executing the command. 

## Member Function Documentation


### set_basic_id() {#classmavsdk_1_1_remote_id_1ac712901a49783dae9a4cff6176dafed2}
```cpp
Result mavsdk::RemoteId::set_basic_id(BasicId basic_id) const
```


Update the [BasicId](structmavsdk_1_1_remote_id_1_1_basic_id.md) structure sent with the basic_id packet.

This function is blocking.

**Parameters**

* [BasicId](structmavsdk_1_1_remote_id_1_1_basic_id.md) **basic_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) - Result of request.

### set_location() {#classmavsdk_1_1_remote_id_1a8286526708c4d879048f2e5711aa8e31}
```cpp
Result mavsdk::RemoteId::set_location(Location location) const
```


Update the [Location](structmavsdk_1_1_remote_id_1_1_location.md) structure sent with the location packet.

This function is blocking.

**Parameters**

* [Location](structmavsdk_1_1_remote_id_1_1_location.md) **location** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) - Result of request.

### set_system() {#classmavsdk_1_1_remote_id_1a2dda9deef2fc4edfd0f8048ad918b832}
```cpp
Result mavsdk::RemoteId::set_system(SystemId system) const
```


Update the [System](classmavsdk_1_1_system.md) structure sent with the system packet.

This function is blocking.

**Parameters**

* [SystemId](structmavsdk_1_1_remote_id_1_1_system_id.md) **system** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) - Result of request.

### set_operator_id() {#classmavsdk_1_1_remote_id_1af851f77ea8be3e0ae0d7f8d666c0226e}
```cpp
Result mavsdk::RemoteId::set_operator_id(OperatorId system) const
```


Update the [OperatorId](structmavsdk_1_1_remote_id_1_1_operator_id.md) structure sent with the operator_id packet.

This function is blocking.

**Parameters**

* [OperatorId](structmavsdk_1_1_remote_id_1_1_operator_id.md) **system** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) - Result of request.

### set_self_id() {#classmavsdk_1_1_remote_id_1a6360566541b3fd737c6ad4fbc3067082}
```cpp
Result mavsdk::RemoteId::set_self_id(SelfId self_id) const
```


Update the SetSelfId structure sent with the self_id packet.

This function is blocking.

**Parameters**

* [SelfId](structmavsdk_1_1_remote_id_1_1_self_id.md) **self_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac291c0dd75e05e1d6728affac56d760f) - Result of request.

### subscribe_arm_status() {#classmavsdk_1_1_remote_id_1a055d1d2a3acdba5a0f4affdd683ed266}
```cpp
ArmStatusHandle mavsdk::RemoteId::subscribe_arm_status(const ArmStatusCallback &callback)
```


Subscribe to arm status updates.


**Parameters**

* const [ArmStatusCallback](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ac8cbe00a1b4e2e6ed305fe19e8c43917)& **callback** - 

**Returns**

&emsp;[ArmStatusHandle](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ad0b845827527f615d80d9fb87c7f71c2) - 

### unsubscribe_arm_status() {#classmavsdk_1_1_remote_id_1ade1ca076aa7dd4d9e4c31d0305172d0e}
```cpp
void mavsdk::RemoteId::unsubscribe_arm_status(ArmStatusHandle handle)
```


Unsubscribe from subscribe_arm_status.


**Parameters**

* [ArmStatusHandle](classmavsdk_1_1_remote_id.md#classmavsdk_1_1_remote_id_1ad0b845827527f615d80d9fb87c7f71c2) **handle** - 

### arm_status() {#classmavsdk_1_1_remote_id_1a8d52d5cf1c4f2a0efb14b09f8edc6896}
```cpp
ArmStatus mavsdk::RemoteId::arm_status() const
```


Poll for '[ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md)' (blocking).


**Returns**

&emsp;[ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md) - One [ArmStatus](structmavsdk_1_1_remote_id_1_1_arm_status.md) update.

### operator=() {#classmavsdk_1_1_remote_id_1acdf4d016fb9699d48d1ff473dc944566}
```cpp
const RemoteId & mavsdk::RemoteId::operator=(const RemoteId &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [RemoteId](classmavsdk_1_1_remote_id.md)&  - 

**Returns**

&emsp;const [RemoteId](classmavsdk_1_1_remote_id.md) & - 