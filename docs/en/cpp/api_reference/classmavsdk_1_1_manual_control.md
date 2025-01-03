# mavsdk::ManualControl Class Reference
`#include: manual_control.h`

----


Enable manual control using e.g. a joystick or gamepad. 


## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) | Possible results returned for manual control requests.
std::function< void([Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799))> [ResultCallback](#classmavsdk_1_1_manual_control_1a08b010bafdf770d3e4ea6060f56a0f3b) | Callback type for asynchronous [ManualControl](classmavsdk_1_1_manual_control.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ManualControl](#classmavsdk_1_1_manual_control_1af89d947dd2c843cc41f2ea183ad17b03) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [ManualControl](#classmavsdk_1_1_manual_control_1acf87299c9d9b632dbbc0975244c537e9) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~ManualControl](#classmavsdk_1_1_manual_control_1a7e1bfb49a11b98c541b88fdef8fe76f7) () override | Destructor (internal use only).
&nbsp; | [ManualControl](#classmavsdk_1_1_manual_control_1a8253b8063d4cd4832d84acd2afa26775) (const [ManualControl](classmavsdk_1_1_manual_control.md) & other) | Copy constructor.
void | [start_position_control_async](#classmavsdk_1_1_manual_control_1a2a3352c11c56dd14d55537b3a6413792) (const [ResultCallback](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a08b010bafdf770d3e4ea6060f56a0f3b) callback) | Start position control using e.g. joystick input.
[Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) | [start_position_control](#classmavsdk_1_1_manual_control_1a570c7e78fc9c5083d9a2f9570c32caaa) () const | Start position control using e.g. joystick input.
void | [start_altitude_control_async](#classmavsdk_1_1_manual_control_1abf7659d98949c01b634e421a9c2df079) (const [ResultCallback](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a08b010bafdf770d3e4ea6060f56a0f3b) callback) | Start altitude control.
[Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) | [start_altitude_control](#classmavsdk_1_1_manual_control_1a67609e46bafb34309d5b5d943a29f23c) () const | Start altitude control.
[Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) | [set_manual_control_input](#classmavsdk_1_1_manual_control_1a1b4b34db391d7238e33b821614fc1f29) (float x, float y, float z, float r)const | Set manual control input.
const [ManualControl](classmavsdk_1_1_manual_control.md) & | [operator=](#classmavsdk_1_1_manual_control_1aca51fff98c33fbd5201495101ddf1368) (const [ManualControl](classmavsdk_1_1_manual_control.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ManualControl() {#classmavsdk_1_1_manual_control_1af89d947dd2c843cc41f2ea183ad17b03}
```cpp
mavsdk::ManualControl::ManualControl(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto manual_control = ManualControl(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### ManualControl() {#classmavsdk_1_1_manual_control_1acf87299c9d9b632dbbc0975244c537e9}
```cpp
mavsdk::ManualControl::ManualControl(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto manual_control = ManualControl(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~ManualControl() {#classmavsdk_1_1_manual_control_1a7e1bfb49a11b98c541b88fdef8fe76f7}
```cpp
mavsdk::ManualControl::~ManualControl() override
```


Destructor (internal use only).


### ManualControl() {#classmavsdk_1_1_manual_control_1a8253b8063d4cd4832d84acd2afa26775}
```cpp
mavsdk::ManualControl::ManualControl(const ManualControl &other)
```


Copy constructor.


**Parameters**

* const [ManualControl](classmavsdk_1_1_manual_control.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_manual_control_1a08b010bafdf770d3e4ea6060f56a0f3b}

```cpp
using mavsdk::ManualControl::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ManualControl](classmavsdk_1_1_manual_control.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799}


Possible results returned for manual control requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command refused by vehicle. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a2add56a53420744e7e68e6a5e03e41bc"></span> `InputOutOfRange` | Input out of range. 
<span id="classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799a1368182b3469b603a554715a4dd81e50"></span> `InputNotSet` | No Input set. 

## Member Function Documentation


### start_position_control_async() {#classmavsdk_1_1_manual_control_1a2a3352c11c56dd14d55537b3a6413792}
```cpp
void mavsdk::ManualControl::start_position_control_async(const ResultCallback callback)
```


Start position control using e.g. joystick input.

Requires manual control input to be sent regularly already. Requires a valid position using e.g. GPS, external vision, or optical flow.


This function is non-blocking. See 'start_position_control' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a08b010bafdf770d3e4ea6060f56a0f3b) **callback** - 

### start_position_control() {#classmavsdk_1_1_manual_control_1a570c7e78fc9c5083d9a2f9570c32caaa}
```cpp
Result mavsdk::ManualControl::start_position_control() const
```


Start position control using e.g. joystick input.

Requires manual control input to be sent regularly already. Requires a valid position using e.g. GPS, external vision, or optical flow.


This function is blocking. See 'start_position_control_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) - Result of request.

### start_altitude_control_async() {#classmavsdk_1_1_manual_control_1abf7659d98949c01b634e421a9c2df079}
```cpp
void mavsdk::ManualControl::start_altitude_control_async(const ResultCallback callback)
```


Start altitude control.

Requires manual control input to be sent regularly already. Does not require a valid position e.g. GPS.


This function is non-blocking. See 'start_altitude_control' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a08b010bafdf770d3e4ea6060f56a0f3b) **callback** - 

### start_altitude_control() {#classmavsdk_1_1_manual_control_1a67609e46bafb34309d5b5d943a29f23c}
```cpp
Result mavsdk::ManualControl::start_altitude_control() const
```


Start altitude control.

Requires manual control input to be sent regularly already. Does not require a valid position e.g. GPS.


This function is blocking. See 'start_altitude_control_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) - Result of request.

### set_manual_control_input() {#classmavsdk_1_1_manual_control_1a1b4b34db391d7238e33b821614fc1f29}
```cpp
Result mavsdk::ManualControl::set_manual_control_input(float x, float y, float z, float r) const
```


Set manual control input.

The manual control input needs to be sent at a rate high enough to prevent triggering of RC loss, a good minimum rate is 10 Hz.


This function is blocking.

**Parameters**

* float **x** - 
* float **y** - 
* float **z** - 
* float **r** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_manual_control.md#classmavsdk_1_1_manual_control_1a6c7dbd25e051b6e1369a65fd05a22799) - Result of request.

### operator=() {#classmavsdk_1_1_manual_control_1aca51fff98c33fbd5201495101ddf1368}
```cpp
const ManualControl& mavsdk::ManualControl::operator=(const ManualControl &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ManualControl](classmavsdk_1_1_manual_control.md)&  - 

**Returns**

&emsp;const [ManualControl](classmavsdk_1_1_manual_control.md) & - 