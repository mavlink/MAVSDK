# mavsdk::Gimbal Class Reference
`#include: gimbal.h`

----


Provide control over a gimbal. 


## Data Structures


struct [ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md)

## Public Types


Type | Description
--- | ---
enum [GimbalMode](#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) | [Gimbal](classmavsdk_1_1_gimbal.md) mode type.
enum [ControlMode](#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) | Control mode.
enum [Result](#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | Possible results returned for gimbal commands.
std::function< void([Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac))> [ResultCallback](#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) | Callback type for asynchronous [Gimbal](classmavsdk_1_1_gimbal.md) calls.
std::function< void([ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md))> [ControlCallback](#classmavsdk_1_1_gimbal_1a1645ab20c41161e6c47620b7352eef62) | Callback type for subscribe_control.
[Handle](classmavsdk_1_1_handle.md)< [ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) > [ControlHandle](#classmavsdk_1_1_gimbal_1accab76c321008685a455ccff45811397) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_control.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Gimbal](#classmavsdk_1_1_gimbal_1aa33f4df704c7f09698884083c379f787) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Gimbal](#classmavsdk_1_1_gimbal_1aa3cb6e1e37a2f275ab5cc8ed4dd71d93) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Gimbal](#classmavsdk_1_1_gimbal_1a3b86d8209172a578f238928b9f5bfdd6) () override | Destructor (internal use only).
&nbsp; | [Gimbal](#classmavsdk_1_1_gimbal_1ae69df278ca37deaee22fb13053fd4f2d) (const [Gimbal](classmavsdk_1_1_gimbal.md) & other) | Copy constructor.
void | [set_pitch_and_yaw_async](#classmavsdk_1_1_gimbal_1a325a49cc256359013cbc917b3576f292) (float pitch_deg, float yaw_deg, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal pitch and yaw angles.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_pitch_and_yaw](#classmavsdk_1_1_gimbal_1ad65ba3258833fe78f2939b9b72dc3b88) (float pitch_deg, float yaw_deg)const | Set gimbal pitch and yaw angles.
void | [set_pitch_rate_and_yaw_rate_async](#classmavsdk_1_1_gimbal_1a161b3f85cd9fa30439774ef47a10c51d) (float pitch_rate_deg_s, float yaw_rate_deg_s, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal angular rates around pitch and yaw axes.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_pitch_rate_and_yaw_rate](#classmavsdk_1_1_gimbal_1a2ad12582d192d8594d7da315e2729129) (float pitch_rate_deg_s, float yaw_rate_deg_s)const | Set gimbal angular rates around pitch and yaw axes.
void | [set_mode_async](#classmavsdk_1_1_gimbal_1ad69853994c134b0e88d0f94744254066) ([GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) gimbal_mode, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal mode.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_mode](#classmavsdk_1_1_gimbal_1a037285883ceba14e0df9c7f8c19f4423) ([GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) gimbal_mode)const | Set gimbal mode.
void | [set_roi_location_async](#classmavsdk_1_1_gimbal_1ab3c42a7042231e48dfab881030fe30c0) (double latitude_deg, double longitude_deg, float altitude_m, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal region of interest (ROI).
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_roi_location](#classmavsdk_1_1_gimbal_1a035ddc270efce19a9be54b98add57919) (double latitude_deg, double longitude_deg, float altitude_m)const | Set gimbal region of interest (ROI).
void | [take_control_async](#classmavsdk_1_1_gimbal_1a331139df593e8ccef0f8ca7652f2d2bc) ([ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) control_mode, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Take control.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [take_control](#classmavsdk_1_1_gimbal_1a7dabe20d1bceb7031440fefba59cd707) ([ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) control_mode)const | Take control.
void | [release_control_async](#classmavsdk_1_1_gimbal_1aa58402c4e2d9506dbe9839ef8cbfb920) (const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Release control.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [release_control](#classmavsdk_1_1_gimbal_1ab994d4130b2956e2d33613ffb2127335) () const | Release control.
[ControlHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1accab76c321008685a455ccff45811397) | [subscribe_control](#classmavsdk_1_1_gimbal_1aafd016582be5b63d9b378d414f2faf30) (const [ControlCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a1645ab20c41161e6c47620b7352eef62) & callback) | Subscribe to control status updates.
void | [unsubscribe_control](#classmavsdk_1_1_gimbal_1a899b442fafac1d83b9450bd219f6975e) ([ControlHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1accab76c321008685a455ccff45811397) handle) | Unsubscribe from subscribe_control.
[ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) | [control](#classmavsdk_1_1_gimbal_1aae172788140e37d6125b224f5e79829e) () const | Poll for '[ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md)' (blocking).
const [Gimbal](classmavsdk_1_1_gimbal.md) & | [operator=](#classmavsdk_1_1_gimbal_1ac9a6e1936f58ce8f957be7c6bcc0d134) (const [Gimbal](classmavsdk_1_1_gimbal.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Gimbal() {#classmavsdk_1_1_gimbal_1aa33f4df704c7f09698884083c379f787}
```cpp
mavsdk::Gimbal::Gimbal(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto gimbal = Gimbal(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Gimbal() {#classmavsdk_1_1_gimbal_1aa3cb6e1e37a2f275ab5cc8ed4dd71d93}
```cpp
mavsdk::Gimbal::Gimbal(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto gimbal = Gimbal(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Gimbal() {#classmavsdk_1_1_gimbal_1a3b86d8209172a578f238928b9f5bfdd6}
```cpp
mavsdk::Gimbal::~Gimbal() override
```


Destructor (internal use only).


### Gimbal() {#classmavsdk_1_1_gimbal_1ae69df278ca37deaee22fb13053fd4f2d}
```cpp
mavsdk::Gimbal::Gimbal(const Gimbal &other)
```


Copy constructor.


**Parameters**

* const [Gimbal](classmavsdk_1_1_gimbal.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197}

```cpp
using mavsdk::Gimbal::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Gimbal](classmavsdk_1_1_gimbal.md) calls.


### typedef ControlCallback {#classmavsdk_1_1_gimbal_1a1645ab20c41161e6c47620b7352eef62}

```cpp
using mavsdk::Gimbal::ControlCallback =  std::function<void(ControlStatus)>
```


Callback type for subscribe_control.


### typedef ControlHandle {#classmavsdk_1_1_gimbal_1accab76c321008685a455ccff45811397}

```cpp
using mavsdk::Gimbal::ControlHandle =  Handle<ControlStatus>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_control.


## Member Enumeration Documentation


### enum GimbalMode {#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca}


[Gimbal](classmavsdk_1_1_gimbal.md) mode type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dcaae370d3502f507d9c9ea57d6fd3c4ed7b"></span> `YawFollow` | Yaw follow will point the gimbal to the vehicle heading. 
<span id="classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dcaaf0a46b0b7151dca1ab120e02b6e6663e"></span> `YawLock` | Yaw lock will fix the gimbal pointing to an absolute direction. 

### enum ControlMode {#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3}


Control mode.


Value | Description
--- | ---
<span id="classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3a6adf97f83acf6453d4a6a4b1070f3754"></span> `None` | Indicates that the component does not have control over the gimbal. 
<span id="classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3a074de7e7c226d5c60f8af14c20725352"></span> `Primary` | To take primary control over the gimbal. 
<span id="classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3a0c7d6cf19426e01dcfa32434828ed266"></span> `Secondary` | To take secondary control over the gimbal. 

### enum Result {#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac}


Possible results returned for gimbal commands.


Value | Description
--- | ---
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76aca88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76aca505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Command was accepted. 
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76aca902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error occurred sending the command. 
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76acac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Command timed out. 
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76acab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Functionality not supported. 
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76aca1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 

## Member Function Documentation


### set_pitch_and_yaw_async() {#classmavsdk_1_1_gimbal_1a325a49cc256359013cbc917b3576f292}
```cpp
void mavsdk::Gimbal::set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, const ResultCallback callback)
```


Set gimbal pitch and yaw angles.

This sets the desired pitch and yaw angles of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually be set to the new angles.


This function is non-blocking. See 'set_pitch_and_yaw' for the blocking counterpart.

**Parameters**

* float **pitch_deg** - 
* float **yaw_deg** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_pitch_and_yaw() {#classmavsdk_1_1_gimbal_1ad65ba3258833fe78f2939b9b72dc3b88}
```cpp
Result mavsdk::Gimbal::set_pitch_and_yaw(float pitch_deg, float yaw_deg) const
```


Set gimbal pitch and yaw angles.

This sets the desired pitch and yaw angles of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually be set to the new angles.


This function is blocking. See 'set_pitch_and_yaw_async' for the non-blocking counterpart.

**Parameters**

* float **pitch_deg** - 
* float **yaw_deg** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### set_pitch_rate_and_yaw_rate_async() {#classmavsdk_1_1_gimbal_1a161b3f85cd9fa30439774ef47a10c51d}
```cpp
void mavsdk::Gimbal::set_pitch_rate_and_yaw_rate_async(float pitch_rate_deg_s, float yaw_rate_deg_s, const ResultCallback callback)
```


Set gimbal angular rates around pitch and yaw axes.

This sets the desired angular rates around pitch and yaw axes of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually reach the angular rate.


This function is non-blocking. See 'set_pitch_rate_and_yaw_rate' for the blocking counterpart.

**Parameters**

* float **pitch_rate_deg_s** - 
* float **yaw_rate_deg_s** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_pitch_rate_and_yaw_rate() {#classmavsdk_1_1_gimbal_1a2ad12582d192d8594d7da315e2729129}
```cpp
Result mavsdk::Gimbal::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s) const
```


Set gimbal angular rates around pitch and yaw axes.

This sets the desired angular rates around pitch and yaw axes of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually reach the angular rate.


This function is blocking. See 'set_pitch_rate_and_yaw_rate_async' for the non-blocking counterpart.

**Parameters**

* float **pitch_rate_deg_s** - 
* float **yaw_rate_deg_s** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### set_mode_async() {#classmavsdk_1_1_gimbal_1ad69853994c134b0e88d0f94744254066}
```cpp
void mavsdk::Gimbal::set_mode_async(GimbalMode gimbal_mode, const ResultCallback callback)
```


Set gimbal mode.

This sets the desired yaw mode of a gimbal. Will return when the command is accepted. However, it might take the gimbal longer to actually be set to the new angles.


This function is non-blocking. See 'set_mode' for the blocking counterpart.

**Parameters**

* [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) **gimbal_mode** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_mode() {#classmavsdk_1_1_gimbal_1a037285883ceba14e0df9c7f8c19f4423}
```cpp
Result mavsdk::Gimbal::set_mode(GimbalMode gimbal_mode) const
```


Set gimbal mode.

This sets the desired yaw mode of a gimbal. Will return when the command is accepted. However, it might take the gimbal longer to actually be set to the new angles.


This function is blocking. See 'set_mode_async' for the non-blocking counterpart.

**Parameters**

* [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) **gimbal_mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### set_roi_location_async() {#classmavsdk_1_1_gimbal_1ab3c42a7042231e48dfab881030fe30c0}
```cpp
void mavsdk::Gimbal::set_roi_location_async(double latitude_deg, double longitude_deg, float altitude_m, const ResultCallback callback)
```


Set gimbal region of interest (ROI).

This sets a region of interest that the gimbal will point to. The gimbal will continue to point to the specified region until it receives a new command. The function will return when the command is accepted, however, it might take the gimbal longer to actually rotate to the ROI.


This function is non-blocking. See 'set_roi_location' for the blocking counterpart.

**Parameters**

* double **latitude_deg** - 
* double **longitude_deg** - 
* float **altitude_m** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_roi_location() {#classmavsdk_1_1_gimbal_1a035ddc270efce19a9be54b98add57919}
```cpp
Result mavsdk::Gimbal::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m) const
```


Set gimbal region of interest (ROI).

This sets a region of interest that the gimbal will point to. The gimbal will continue to point to the specified region until it receives a new command. The function will return when the command is accepted, however, it might take the gimbal longer to actually rotate to the ROI.


This function is blocking. See 'set_roi_location_async' for the non-blocking counterpart.

**Parameters**

* double **latitude_deg** - 
* double **longitude_deg** - 
* float **altitude_m** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### take_control_async() {#classmavsdk_1_1_gimbal_1a331139df593e8ccef0f8ca7652f2d2bc}
```cpp
void mavsdk::Gimbal::take_control_async(ControlMode control_mode, const ResultCallback callback)
```


Take control.

There can be only two components in control of a gimbal at any given time. One with "primary" control, and one with "secondary" control. The way the secondary control is implemented is not specified and hence depends on the vehicle.


Components are expected to be cooperative, which means that they can override each other and should therefore do it carefully.


This function is non-blocking. See 'take_control' for the blocking counterpart.

**Parameters**

* [ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) **control_mode** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### take_control() {#classmavsdk_1_1_gimbal_1a7dabe20d1bceb7031440fefba59cd707}
```cpp
Result mavsdk::Gimbal::take_control(ControlMode control_mode) const
```


Take control.

There can be only two components in control of a gimbal at any given time. One with "primary" control, and one with "secondary" control. The way the secondary control is implemented is not specified and hence depends on the vehicle.


Components are expected to be cooperative, which means that they can override each other and should therefore do it carefully.


This function is blocking. See 'take_control_async' for the non-blocking counterpart.

**Parameters**

* [ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) **control_mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### release_control_async() {#classmavsdk_1_1_gimbal_1aa58402c4e2d9506dbe9839ef8cbfb920}
```cpp
void mavsdk::Gimbal::release_control_async(const ResultCallback callback)
```


Release control.

Release control, such that other components can control the gimbal.


This function is non-blocking. See 'release_control' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### release_control() {#classmavsdk_1_1_gimbal_1ab994d4130b2956e2d33613ffb2127335}
```cpp
Result mavsdk::Gimbal::release_control() const
```


Release control.

Release control, such that other components can control the gimbal.


This function is blocking. See 'release_control_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### subscribe_control() {#classmavsdk_1_1_gimbal_1aafd016582be5b63d9b378d414f2faf30}
```cpp
ControlHandle mavsdk::Gimbal::subscribe_control(const ControlCallback &callback)
```


Subscribe to control status updates.

This allows a component to know if it has primary, secondary or no control over the gimbal. Also, it gives the system and component ids of the other components in control (if any).

**Parameters**

* const [ControlCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a1645ab20c41161e6c47620b7352eef62)& **callback** - 

**Returns**

&emsp;[ControlHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1accab76c321008685a455ccff45811397) - 

### unsubscribe_control() {#classmavsdk_1_1_gimbal_1a899b442fafac1d83b9450bd219f6975e}
```cpp
void mavsdk::Gimbal::unsubscribe_control(ControlHandle handle)
```


Unsubscribe from subscribe_control.


**Parameters**

* [ControlHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1accab76c321008685a455ccff45811397) **handle** - 

### control() {#classmavsdk_1_1_gimbal_1aae172788140e37d6125b224f5e79829e}
```cpp
ControlStatus mavsdk::Gimbal::control() const
```


Poll for '[ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md)' (blocking).


**Returns**

&emsp;[ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) - One [ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) update.

### operator=() {#classmavsdk_1_1_gimbal_1ac9a6e1936f58ce8f957be7c6bcc0d134}
```cpp
const Gimbal& mavsdk::Gimbal::operator=(const Gimbal &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Gimbal](classmavsdk_1_1_gimbal.md)&  - 

**Returns**

&emsp;const [Gimbal](classmavsdk_1_1_gimbal.md) & - 