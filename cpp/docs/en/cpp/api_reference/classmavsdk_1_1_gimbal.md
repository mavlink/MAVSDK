# mavsdk::Gimbal Class Reference
`#include: gimbal.h`

----


Provide control over a gimbal within the MAVLink [Gimbal](classmavsdk_1_1_gimbal.md) Protocol: [https://mavlink.io/en/services/gimbal_v2.html](https://mavlink.io/en/services/gimbal_v2.html). 


## Data Structures


struct [AngularVelocityBody](structmavsdk_1_1_gimbal_1_1_angular_velocity_body.md)

struct [Attitude](structmavsdk_1_1_gimbal_1_1_attitude.md)

struct [ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md)

struct [EulerAngle](structmavsdk_1_1_gimbal_1_1_euler_angle.md)

struct [GimbalItem](structmavsdk_1_1_gimbal_1_1_gimbal_item.md)

struct [GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md)

struct [Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md)

## Public Types


Type | Description
--- | ---
enum [GimbalMode](#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) | [Gimbal](classmavsdk_1_1_gimbal.md) mode type.
enum [ControlMode](#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) | Control mode.
enum [SendMode](#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) | The send mode type.
enum [Result](#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | Possible results returned for gimbal commands.
std::function< void([Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac))> [ResultCallback](#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) | Callback type for asynchronous [Gimbal](classmavsdk_1_1_gimbal.md) calls.
std::function< void([GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md))> [GimbalListCallback](#classmavsdk_1_1_gimbal_1a1656fb54230e1adc61b3a050a423a6c9) | Callback type for subscribe_gimbal_list.
[Handle](classmavsdk_1_1_handle.md)< [GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md) > [GimbalListHandle](#classmavsdk_1_1_gimbal_1a51e98cc1239dd440d37d4dbed78f4ae7) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_gimbal_list.
std::function< void([ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md))> [ControlStatusCallback](#classmavsdk_1_1_gimbal_1a2ab2120cb3ff6c6a1f726542bd0d8dea) | Callback type for subscribe_control_status.
[Handle](classmavsdk_1_1_handle.md)< [ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) > [ControlStatusHandle](#classmavsdk_1_1_gimbal_1aade6db709ce87cb0ec5536ae243cb50d) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_control_status.
std::function< void([Attitude](structmavsdk_1_1_gimbal_1_1_attitude.md))> [AttitudeCallback](#classmavsdk_1_1_gimbal_1a07eafc661a23ee1d55ab0dd86e3c71ed) | Callback type for subscribe_attitude.
[Handle](classmavsdk_1_1_handle.md)< [Attitude](structmavsdk_1_1_gimbal_1_1_attitude.md) > [AttitudeHandle](#classmavsdk_1_1_gimbal_1af7c71cdd8516aec8c9d160977f7c5396) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Gimbal](#classmavsdk_1_1_gimbal_1aa33f4df704c7f09698884083c379f787) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Gimbal](#classmavsdk_1_1_gimbal_1aa3cb6e1e37a2f275ab5cc8ed4dd71d93) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Gimbal](#classmavsdk_1_1_gimbal_1a3b86d8209172a578f238928b9f5bfdd6) () override | Destructor (internal use only).
&nbsp; | [Gimbal](#classmavsdk_1_1_gimbal_1ae69df278ca37deaee22fb13053fd4f2d) (const [Gimbal](classmavsdk_1_1_gimbal.md) & other) | Copy constructor.
void | [set_angles_async](#classmavsdk_1_1_gimbal_1a2da4d10fa81ec525dfda9c0036f94302) (int32_t gimbal_id, float roll_deg, float pitch_deg, float yaw_deg, [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) gimbal_mode, [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) send_mode, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal roll, pitch and yaw angles.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_angles](#classmavsdk_1_1_gimbal_1a481cc6658ef52d40469d71d4b3a1ebda) (int32_t gimbal_id, float roll_deg, float pitch_deg, float yaw_deg, [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) gimbal_mode, [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) send_mode)const | Set gimbal roll, pitch and yaw angles.
void | [set_angular_rates_async](#classmavsdk_1_1_gimbal_1a0f0751ee5268f95d1833651dff78b956) (int32_t gimbal_id, float roll_rate_deg_s, float pitch_rate_deg_s, float yaw_rate_deg_s, [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) gimbal_mode, [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) send_mode, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal angular rates.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_angular_rates](#classmavsdk_1_1_gimbal_1ac69110603cc67a3988791f371706b698) (int32_t gimbal_id, float roll_rate_deg_s, float pitch_rate_deg_s, float yaw_rate_deg_s, [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) gimbal_mode, [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) send_mode)const | Set gimbal angular rates.
void | [set_roi_location_async](#classmavsdk_1_1_gimbal_1a762050f84df6be7adc1eecae4f1b2e48) (int32_t gimbal_id, double latitude_deg, double longitude_deg, float altitude_m, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Set gimbal region of interest (ROI).
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [set_roi_location](#classmavsdk_1_1_gimbal_1a5010c2665f20524d3b86ccfe6b33cdfa) (int32_t gimbal_id, double latitude_deg, double longitude_deg, float altitude_m)const | Set gimbal region of interest (ROI).
void | [take_control_async](#classmavsdk_1_1_gimbal_1a814363300828b02981bf2e818267c9be) (int32_t gimbal_id, [ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) control_mode, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Take control.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [take_control](#classmavsdk_1_1_gimbal_1afe512b2ce37eda69a28d285716076db7) (int32_t gimbal_id, [ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) control_mode)const | Take control.
void | [release_control_async](#classmavsdk_1_1_gimbal_1a68305d969bc88353513890aa2a7a627f) (int32_t gimbal_id, const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) callback) | Release control.
[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) | [release_control](#classmavsdk_1_1_gimbal_1ad0b0665a3502fac6199ac903b24bbd6f) (int32_t gimbal_id)const | Release control.
[GimbalListHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a51e98cc1239dd440d37d4dbed78f4ae7) | [subscribe_gimbal_list](#classmavsdk_1_1_gimbal_1a4ca185553a4a327f46c317942d2731d0) (const [GimbalListCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a1656fb54230e1adc61b3a050a423a6c9) & callback) | Subscribe to list of gimbals.
void | [unsubscribe_gimbal_list](#classmavsdk_1_1_gimbal_1a82e516c2080c56d056cbf7a22ff4e716) ([GimbalListHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a51e98cc1239dd440d37d4dbed78f4ae7) handle) | Unsubscribe from subscribe_gimbal_list.
[GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md) | [gimbal_list](#classmavsdk_1_1_gimbal_1ad67df5fc01473dd94797d42c6be368fd) () const | Poll for '[GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md)' (blocking).
[ControlStatusHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aade6db709ce87cb0ec5536ae243cb50d) | [subscribe_control_status](#classmavsdk_1_1_gimbal_1ab34d06ee1ea0af89e334c3010aeacfd9) (const [ControlStatusCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a2ab2120cb3ff6c6a1f726542bd0d8dea) & callback) | Subscribe to control status updates.
void | [unsubscribe_control_status](#classmavsdk_1_1_gimbal_1a7314f438bd2b91ba98374cd2a1a2b285) ([ControlStatusHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aade6db709ce87cb0ec5536ae243cb50d) handle) | Unsubscribe from subscribe_control_status.
std::pair< [Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac), [Gimbal::ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) > | [get_control_status](#classmavsdk_1_1_gimbal_1a6c4a10de4ecb79c7116654c1d99236f3) (int32_t gimbal_id)const | Get control status for specific gimbal.
[AttitudeHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1af7c71cdd8516aec8c9d160977f7c5396) | [subscribe_attitude](#classmavsdk_1_1_gimbal_1aa3500df0c7adff3819493dd7d38976a3) (const [AttitudeCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a07eafc661a23ee1d55ab0dd86e3c71ed) & callback) | Subscribe to attitude updates.
void | [unsubscribe_attitude](#classmavsdk_1_1_gimbal_1a11b1e0d91ca5c8ec50afd802d9d2532e) ([AttitudeHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1af7c71cdd8516aec8c9d160977f7c5396) handle) | Unsubscribe from subscribe_attitude.
std::pair< [Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac), [Gimbal::Attitude](structmavsdk_1_1_gimbal_1_1_attitude.md) > | [get_attitude](#classmavsdk_1_1_gimbal_1a57c6e6c7297e55f5cc660e40e91b898b) (int32_t gimbal_id)const | Get attitude for specific gimbal.
const [Gimbal](classmavsdk_1_1_gimbal.md) & | [operator=](#classmavsdk_1_1_gimbal_1a2d16cfd6fbe82b0e34d35133ab5c9617) (const [Gimbal](classmavsdk_1_1_gimbal.md) &)=delete | Equality operator (object is not copyable).


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


### typedef GimbalListCallback {#classmavsdk_1_1_gimbal_1a1656fb54230e1adc61b3a050a423a6c9}

```cpp
using mavsdk::Gimbal::GimbalListCallback =  std::function<void(GimbalList)>
```


Callback type for subscribe_gimbal_list.


### typedef GimbalListHandle {#classmavsdk_1_1_gimbal_1a51e98cc1239dd440d37d4dbed78f4ae7}

```cpp
using mavsdk::Gimbal::GimbalListHandle =  Handle<GimbalList>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_gimbal_list.


### typedef ControlStatusCallback {#classmavsdk_1_1_gimbal_1a2ab2120cb3ff6c6a1f726542bd0d8dea}

```cpp
using mavsdk::Gimbal::ControlStatusCallback =  std::function<void(ControlStatus)>
```


Callback type for subscribe_control_status.


### typedef ControlStatusHandle {#classmavsdk_1_1_gimbal_1aade6db709ce87cb0ec5536ae243cb50d}

```cpp
using mavsdk::Gimbal::ControlStatusHandle =  Handle<ControlStatus>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_control_status.


### typedef AttitudeCallback {#classmavsdk_1_1_gimbal_1a07eafc661a23ee1d55ab0dd86e3c71ed}

```cpp
using mavsdk::Gimbal::AttitudeCallback =  std::function<void(Attitude)>
```


Callback type for subscribe_attitude.


### typedef AttitudeHandle {#classmavsdk_1_1_gimbal_1af7c71cdd8516aec8c9d160977f7c5396}

```cpp
using mavsdk::Gimbal::AttitudeHandle =  Handle<Attitude>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude.


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

### enum SendMode {#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2}


The send mode type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2ae1a9dc9f23534e63de9df0d540ac1611"></span> `Once` | Send command exactly once with quality of service (use for sporadic commands slower than 1 Hz). 
<span id="classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2aeae835e83c0494a376229f254f7d3392"></span> `Stream` | Stream setpoint without quality of service (use for setpoints faster than 1 Hz).. 

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
<span id="classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76aca253ca7dd096ee0956cccee4d376cab8b"></span> `InvalidArgument` | Invalid argument. 

## Member Function Documentation


### set_angles_async() {#classmavsdk_1_1_gimbal_1a2da4d10fa81ec525dfda9c0036f94302}
```cpp
void mavsdk::Gimbal::set_angles_async(int32_t gimbal_id, float roll_deg, float pitch_deg, float yaw_deg, GimbalMode gimbal_mode, SendMode send_mode, const ResultCallback callback)
```


Set gimbal roll, pitch and yaw angles.

This sets the desired roll, pitch and yaw angles of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually be set to the new angles.


Note that the roll angle needs to be set to 0 when send_mode is Once.


This function is non-blocking. See 'set_angles' for the blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* float **roll_deg** - 
* float **pitch_deg** - 
* float **yaw_deg** - 
* [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) **gimbal_mode** - 
* [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) **send_mode** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_angles() {#classmavsdk_1_1_gimbal_1a481cc6658ef52d40469d71d4b3a1ebda}
```cpp
Result mavsdk::Gimbal::set_angles(int32_t gimbal_id, float roll_deg, float pitch_deg, float yaw_deg, GimbalMode gimbal_mode, SendMode send_mode) const
```


Set gimbal roll, pitch and yaw angles.

This sets the desired roll, pitch and yaw angles of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually be set to the new angles.


Note that the roll angle needs to be set to 0 when send_mode is Once.


This function is blocking. See 'set_angles_async' for the non-blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* float **roll_deg** - 
* float **pitch_deg** - 
* float **yaw_deg** - 
* [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) **gimbal_mode** - 
* [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) **send_mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### set_angular_rates_async() {#classmavsdk_1_1_gimbal_1a0f0751ee5268f95d1833651dff78b956}
```cpp
void mavsdk::Gimbal::set_angular_rates_async(int32_t gimbal_id, float roll_rate_deg_s, float pitch_rate_deg_s, float yaw_rate_deg_s, GimbalMode gimbal_mode, SendMode send_mode, const ResultCallback callback)
```


Set gimbal angular rates.

This sets the desired angular rates around roll, pitch and yaw axes of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually reach the angular rate.


Note that the roll angle needs to be set to 0 when send_mode is Once.


This function is non-blocking. See 'set_angular_rates' for the blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* float **roll_rate_deg_s** - 
* float **pitch_rate_deg_s** - 
* float **yaw_rate_deg_s** - 
* [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) **gimbal_mode** - 
* [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) **send_mode** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_angular_rates() {#classmavsdk_1_1_gimbal_1ac69110603cc67a3988791f371706b698}
```cpp
Result mavsdk::Gimbal::set_angular_rates(int32_t gimbal_id, float roll_rate_deg_s, float pitch_rate_deg_s, float yaw_rate_deg_s, GimbalMode gimbal_mode, SendMode send_mode) const
```


Set gimbal angular rates.

This sets the desired angular rates around roll, pitch and yaw axes of a gimbal. Will return when the command is accepted, however, it might take the gimbal longer to actually reach the angular rate.


Note that the roll angle needs to be set to 0 when send_mode is Once.


This function is blocking. See 'set_angular_rates_async' for the non-blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* float **roll_rate_deg_s** - 
* float **pitch_rate_deg_s** - 
* float **yaw_rate_deg_s** - 
* [GimbalMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1afb92614c5d5915d3960bcea51bec2dca) **gimbal_mode** - 
* [SendMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a625fee23155be376ebd67853bf9383a2) **send_mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### set_roi_location_async() {#classmavsdk_1_1_gimbal_1a762050f84df6be7adc1eecae4f1b2e48}
```cpp
void mavsdk::Gimbal::set_roi_location_async(int32_t gimbal_id, double latitude_deg, double longitude_deg, float altitude_m, const ResultCallback callback)
```


Set gimbal region of interest (ROI).

This sets a region of interest that the gimbal will point to. The gimbal will continue to point to the specified region until it receives a new command. The function will return when the command is accepted, however, it might take the gimbal longer to actually rotate to the ROI.


This function is non-blocking. See 'set_roi_location' for the blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* double **latitude_deg** - 
* double **longitude_deg** - 
* float **altitude_m** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### set_roi_location() {#classmavsdk_1_1_gimbal_1a5010c2665f20524d3b86ccfe6b33cdfa}
```cpp
Result mavsdk::Gimbal::set_roi_location(int32_t gimbal_id, double latitude_deg, double longitude_deg, float altitude_m) const
```


Set gimbal region of interest (ROI).

This sets a region of interest that the gimbal will point to. The gimbal will continue to point to the specified region until it receives a new command. The function will return when the command is accepted, however, it might take the gimbal longer to actually rotate to the ROI.


This function is blocking. See 'set_roi_location_async' for the non-blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* double **latitude_deg** - 
* double **longitude_deg** - 
* float **altitude_m** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### take_control_async() {#classmavsdk_1_1_gimbal_1a814363300828b02981bf2e818267c9be}
```cpp
void mavsdk::Gimbal::take_control_async(int32_t gimbal_id, ControlMode control_mode, const ResultCallback callback)
```


Take control.

There can be only two components in control of a gimbal at any given time. One with "primary" control, and one with "secondary" control. The way the secondary control is implemented is not specified and hence depends on the vehicle.


Components are expected to be cooperative, which means that they can override each other and should therefore do it carefully.


This function is non-blocking. See 'take_control' for the blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* [ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) **control_mode** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### take_control() {#classmavsdk_1_1_gimbal_1afe512b2ce37eda69a28d285716076db7}
```cpp
Result mavsdk::Gimbal::take_control(int32_t gimbal_id, ControlMode control_mode) const
```


Take control.

There can be only two components in control of a gimbal at any given time. One with "primary" control, and one with "secondary" control. The way the secondary control is implemented is not specified and hence depends on the vehicle.


Components are expected to be cooperative, which means that they can override each other and should therefore do it carefully.


This function is blocking. See 'take_control_async' for the non-blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* [ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) **control_mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### release_control_async() {#classmavsdk_1_1_gimbal_1a68305d969bc88353513890aa2a7a627f}
```cpp
void mavsdk::Gimbal::release_control_async(int32_t gimbal_id, const ResultCallback callback)
```


Release control.

Release control, such that other components can control the gimbal.


This function is non-blocking. See 'release_control' for the blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 
* const [ResultCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a88ee7dd17821fb9b12c44b2a3630c197) **callback** - 

### release_control() {#classmavsdk_1_1_gimbal_1ad0b0665a3502fac6199ac903b24bbd6f}
```cpp
Result mavsdk::Gimbal::release_control(int32_t gimbal_id) const
```


Release control.

Release control, such that other components can control the gimbal.


This function is blocking. See 'release_control_async' for the non-blocking counterpart.

**Parameters**

* int32_t **gimbal_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac) - Result of request.

### subscribe_gimbal_list() {#classmavsdk_1_1_gimbal_1a4ca185553a4a327f46c317942d2731d0}
```cpp
GimbalListHandle mavsdk::Gimbal::subscribe_gimbal_list(const GimbalListCallback &callback)
```


Subscribe to list of gimbals.

This allows to find out what gimbals are connected to the system. Based on the gimbal ID, we can then address a specific gimbal.

**Parameters**

* const [GimbalListCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a1656fb54230e1adc61b3a050a423a6c9)& **callback** - 

**Returns**

&emsp;[GimbalListHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a51e98cc1239dd440d37d4dbed78f4ae7) - 

### unsubscribe_gimbal_list() {#classmavsdk_1_1_gimbal_1a82e516c2080c56d056cbf7a22ff4e716}
```cpp
void mavsdk::Gimbal::unsubscribe_gimbal_list(GimbalListHandle handle)
```


Unsubscribe from subscribe_gimbal_list.


**Parameters**

* [GimbalListHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a51e98cc1239dd440d37d4dbed78f4ae7) **handle** - 

### gimbal_list() {#classmavsdk_1_1_gimbal_1ad67df5fc01473dd94797d42c6be368fd}
```cpp
GimbalList mavsdk::Gimbal::gimbal_list() const
```


Poll for '[GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md)' (blocking).


**Returns**

&emsp;[GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md) - One [GimbalList](structmavsdk_1_1_gimbal_1_1_gimbal_list.md) update.

### subscribe_control_status() {#classmavsdk_1_1_gimbal_1ab34d06ee1ea0af89e334c3010aeacfd9}
```cpp
ControlStatusHandle mavsdk::Gimbal::subscribe_control_status(const ControlStatusCallback &callback)
```


Subscribe to control status updates.

This allows a component to know if it has primary, secondary or no control over the gimbal. Also, it gives the system and component ids of the other components in control (if any).

**Parameters**

* const [ControlStatusCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a2ab2120cb3ff6c6a1f726542bd0d8dea)& **callback** - 

**Returns**

&emsp;[ControlStatusHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aade6db709ce87cb0ec5536ae243cb50d) - 

### unsubscribe_control_status() {#classmavsdk_1_1_gimbal_1a7314f438bd2b91ba98374cd2a1a2b285}
```cpp
void mavsdk::Gimbal::unsubscribe_control_status(ControlStatusHandle handle)
```


Unsubscribe from subscribe_control_status.


**Parameters**

* [ControlStatusHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aade6db709ce87cb0ec5536ae243cb50d) **handle** - 

### get_control_status() {#classmavsdk_1_1_gimbal_1a6c4a10de4ecb79c7116654c1d99236f3}
```cpp
std::pair< Result, Gimbal::ControlStatus > mavsdk::Gimbal::get_control_status(int32_t gimbal_id) const
```


Get control status for specific gimbal.

This function is blocking.

**Parameters**

* int32_t **gimbal_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac), [Gimbal::ControlStatus](structmavsdk_1_1_gimbal_1_1_control_status.md) > - Result of request.

### subscribe_attitude() {#classmavsdk_1_1_gimbal_1aa3500df0c7adff3819493dd7d38976a3}
```cpp
AttitudeHandle mavsdk::Gimbal::subscribe_attitude(const AttitudeCallback &callback)
```


Subscribe to attitude updates.

This gets you the gimbal's attitude and angular rate.

**Parameters**

* const [AttitudeCallback](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a07eafc661a23ee1d55ab0dd86e3c71ed)& **callback** - 

**Returns**

&emsp;[AttitudeHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1af7c71cdd8516aec8c9d160977f7c5396) - 

### unsubscribe_attitude() {#classmavsdk_1_1_gimbal_1a11b1e0d91ca5c8ec50afd802d9d2532e}
```cpp
void mavsdk::Gimbal::unsubscribe_attitude(AttitudeHandle handle)
```


Unsubscribe from subscribe_attitude.


**Parameters**

* [AttitudeHandle](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1af7c71cdd8516aec8c9d160977f7c5396) **handle** - 

### get_attitude() {#classmavsdk_1_1_gimbal_1a57c6e6c7297e55f5cc660e40e91b898b}
```cpp
std::pair< Result, Gimbal::Attitude > mavsdk::Gimbal::get_attitude(int32_t gimbal_id) const
```


Get attitude for specific gimbal.

This function is blocking.

**Parameters**

* int32_t **gimbal_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1aa732ec0bd49ac03b7910199d635f76ac), [Gimbal::Attitude](structmavsdk_1_1_gimbal_1_1_attitude.md) > - Result of request.

### operator=() {#classmavsdk_1_1_gimbal_1a2d16cfd6fbe82b0e34d35133ab5c9617}
```cpp
const Gimbal & mavsdk::Gimbal::operator=(const Gimbal &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Gimbal](classmavsdk_1_1_gimbal.md)&  - 

**Returns**

&emsp;const [Gimbal](classmavsdk_1_1_gimbal.md) & - 