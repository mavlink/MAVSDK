# mavsdk::Gripper Class Reference
`#include: gripper.h`

----


Allows users to send gripper actions. 


## Public Types


Type | Description
--- | ---
enum [GripperAction](#classmavsdk_1_1_gripper_1a2088ab8cdb0f343fa576a54af41cf36f) | [Gripper](classmavsdk_1_1_gripper.md) Actions.
enum [Result](#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534) | Possible results returned for gripper action requests.
std::function< void([Result](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534))> [ResultCallback](#classmavsdk_1_1_gripper_1ac8cce161ab7fe573125f4915de0a993f) | Callback type for asynchronous [Gripper](classmavsdk_1_1_gripper.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Gripper](#classmavsdk_1_1_gripper_1a566993706ef599eae17761f4b0810c4b) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Gripper](#classmavsdk_1_1_gripper_1a83a53647a8fb1bf1ef463fd3ddd14726) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Gripper](#classmavsdk_1_1_gripper_1a90b0ea53617492b3f3fa6406430a08e8) () override | Destructor (internal use only).
&nbsp; | [Gripper](#classmavsdk_1_1_gripper_1a3d5e59b3647fd93c8cf43910cdf01468) (const [Gripper](classmavsdk_1_1_gripper.md) & other) | Copy constructor.
void | [grab_async](#classmavsdk_1_1_gripper_1a90c5ebf377fa1691a045a3ae70d885fa) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1ac8cce161ab7fe573125f4915de0a993f) callback) | [Gripper](classmavsdk_1_1_gripper.md) grab cargo.
[Result](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534) | [grab](#classmavsdk_1_1_gripper_1abba980736acaf541721c7e23a9f2f8b1) (uint32_t instance)const | [Gripper](classmavsdk_1_1_gripper.md) grab cargo.
void | [release_async](#classmavsdk_1_1_gripper_1acb9a3f03a7c67f1a5c066aa2380a5e34) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1ac8cce161ab7fe573125f4915de0a993f) callback) | [Gripper](classmavsdk_1_1_gripper.md) release cargo.
[Result](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534) | [release](#classmavsdk_1_1_gripper_1a7b0d151d7a596d3977f2da9861204d8c) (uint32_t instance)const | [Gripper](classmavsdk_1_1_gripper.md) release cargo.
const [Gripper](classmavsdk_1_1_gripper.md) & | [operator=](#classmavsdk_1_1_gripper_1a6b26bd1ca3f795dce56b5a14717b9c40) (const [Gripper](classmavsdk_1_1_gripper.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Gripper() {#classmavsdk_1_1_gripper_1a566993706ef599eae17761f4b0810c4b}
```cpp
mavsdk::Gripper::Gripper(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto gripper = Gripper(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Gripper() {#classmavsdk_1_1_gripper_1a83a53647a8fb1bf1ef463fd3ddd14726}
```cpp
mavsdk::Gripper::Gripper(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto gripper = Gripper(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Gripper() {#classmavsdk_1_1_gripper_1a90b0ea53617492b3f3fa6406430a08e8}
```cpp
mavsdk::Gripper::~Gripper() override
```


Destructor (internal use only).


### Gripper() {#classmavsdk_1_1_gripper_1a3d5e59b3647fd93c8cf43910cdf01468}
```cpp
mavsdk::Gripper::Gripper(const Gripper &other)
```


Copy constructor.


**Parameters**

* const [Gripper](classmavsdk_1_1_gripper.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_gripper_1ac8cce161ab7fe573125f4915de0a993f}

```cpp
using mavsdk::Gripper::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Gripper](classmavsdk_1_1_gripper.md) calls.


## Member Enumeration Documentation


### enum GripperAction {#classmavsdk_1_1_gripper_1a2088ab8cdb0f343fa576a54af41cf36f}


[Gripper](classmavsdk_1_1_gripper.md) Actions.

Available gripper actions are defined in mavlink under [https://mavlink.io/en/messages/common.html#GRIPPER_ACTIONS](https://mavlink.io/en/messages/common.html#GRIPPER_ACTIONS)

Value | Description
--- | ---
<span id="classmavsdk_1_1_gripper_1a2088ab8cdb0f343fa576a54af41cf36fab8e7b465df7c5979dc731d06e84ce2cf"></span> `Release` | Open the gripper to release the cargo. 
<span id="classmavsdk_1_1_gripper_1a2088ab8cdb0f343fa576a54af41cf36fab635ceb01a10e96cdbefa95d72b25750"></span> `Grab` | Close the gripper and grab onto cargo. 

### enum Result {#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534}


Possible results returned for gripper action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Temporarily rejected. 
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | [Action](classmavsdk_1_1_action.md) not supported. 
<span id="classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | [Action](classmavsdk_1_1_action.md) failed. 

## Member Function Documentation


### grab_async() {#classmavsdk_1_1_gripper_1a90c5ebf377fa1691a045a3ae70d885fa}
```cpp
void mavsdk::Gripper::grab_async(uint32_t instance, const ResultCallback callback)
```


[Gripper](classmavsdk_1_1_gripper.md) grab cargo.

This function is non-blocking. See 'grab' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1ac8cce161ab7fe573125f4915de0a993f) **callback** - 

### grab() {#classmavsdk_1_1_gripper_1abba980736acaf541721c7e23a9f2f8b1}
```cpp
Result mavsdk::Gripper::grab(uint32_t instance) const
```


[Gripper](classmavsdk_1_1_gripper.md) grab cargo.

This function is blocking. See 'grab_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534) - Result of request.

### release_async() {#classmavsdk_1_1_gripper_1acb9a3f03a7c67f1a5c066aa2380a5e34}
```cpp
void mavsdk::Gripper::release_async(uint32_t instance, const ResultCallback callback)
```


[Gripper](classmavsdk_1_1_gripper.md) release cargo.

This function is non-blocking. See 'release' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1ac8cce161ab7fe573125f4915de0a993f) **callback** - 

### release() {#classmavsdk_1_1_gripper_1a7b0d151d7a596d3977f2da9861204d8c}
```cpp
Result mavsdk::Gripper::release(uint32_t instance) const
```


[Gripper](classmavsdk_1_1_gripper.md) release cargo.

This function is blocking. See 'release_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gripper.md#classmavsdk_1_1_gripper_1a82c5d2dec9badc2fad436ae56962b534) - Result of request.

### operator=() {#classmavsdk_1_1_gripper_1a6b26bd1ca3f795dce56b5a14717b9c40}
```cpp
const Gripper& mavsdk::Gripper::operator=(const Gripper &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Gripper](classmavsdk_1_1_gripper.md)&  - 

**Returns**

&emsp;const [Gripper](classmavsdk_1_1_gripper.md) & - 