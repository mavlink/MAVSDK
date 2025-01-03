# mavsdk::Mocap Class Reference
`#include: mocap.h`

----


<ul>
<li><p>Allows interfacing a vehicle with a motion capture system in order to allow navigation without global positioning sources available (e.g. indoors, or when flying under a bridge. etc.). </p>
</li>
</ul>


## Data Structures


struct [AngleBody](structmavsdk_1_1_mocap_1_1_angle_body.md)

struct [AngularVelocityBody](structmavsdk_1_1_mocap_1_1_angular_velocity_body.md)

struct [AttitudePositionMocap](structmavsdk_1_1_mocap_1_1_attitude_position_mocap.md)

struct [Covariance](structmavsdk_1_1_mocap_1_1_covariance.md)

struct [Odometry](structmavsdk_1_1_mocap_1_1_odometry.md)

struct [PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md)

struct [Quaternion](structmavsdk_1_1_mocap_1_1_quaternion.md)

struct [SpeedBody](structmavsdk_1_1_mocap_1_1_speed_body.md)

struct [VisionPositionEstimate](structmavsdk_1_1_mocap_1_1_vision_position_estimate.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) | Possible results returned for mocap requests.
std::function< void([Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5))> [ResultCallback](#classmavsdk_1_1_mocap_1a7743939450a5e816dc9e7de237ec3934) | Callback type for asynchronous [Mocap](classmavsdk_1_1_mocap.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Mocap](#classmavsdk_1_1_mocap_1a993147a5f0ae4c8a4ddf8be4258690cc) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Mocap](#classmavsdk_1_1_mocap_1a981aaa6a7e313376b88557013649de65) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Mocap](#classmavsdk_1_1_mocap_1ad78b5299367dc1db3236d0b674816e22) () override | Destructor (internal use only).
&nbsp; | [Mocap](#classmavsdk_1_1_mocap_1a0c2242e86da34ecd94c0a40dcec4858f) (const [Mocap](classmavsdk_1_1_mocap.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) | [set_vision_position_estimate](#classmavsdk_1_1_mocap_1a22d007409839e28a45d7b10f10e22fd6) ([VisionPositionEstimate](structmavsdk_1_1_mocap_1_1_vision_position_estimate.md) vision_position_estimate)const | Send Global position/attitude estimate from a vision source.
[Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) | [set_attitude_position_mocap](#classmavsdk_1_1_mocap_1a5f9a63d8bbed750056e139640b38cd7f) ([AttitudePositionMocap](structmavsdk_1_1_mocap_1_1_attitude_position_mocap.md) attitude_position_mocap)const | Send motion capture attitude and position.
[Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) | [set_odometry](#classmavsdk_1_1_mocap_1a149fa242e0b01bc0aee9204118b00f59) ([Odometry](structmavsdk_1_1_mocap_1_1_odometry.md) odometry)const | Send odometry information with an external interface.
const [Mocap](classmavsdk_1_1_mocap.md) & | [operator=](#classmavsdk_1_1_mocap_1adf2f33e3befbec23f43e066946050eab) (const [Mocap](classmavsdk_1_1_mocap.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Mocap() {#classmavsdk_1_1_mocap_1a993147a5f0ae4c8a4ddf8be4258690cc}
```cpp
mavsdk::Mocap::Mocap(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mocap = Mocap(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Mocap() {#classmavsdk_1_1_mocap_1a981aaa6a7e313376b88557013649de65}
```cpp
mavsdk::Mocap::Mocap(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mocap = Mocap(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Mocap() {#classmavsdk_1_1_mocap_1ad78b5299367dc1db3236d0b674816e22}
```cpp
mavsdk::Mocap::~Mocap() override
```


Destructor (internal use only).


### Mocap() {#classmavsdk_1_1_mocap_1a0c2242e86da34ecd94c0a40dcec4858f}
```cpp
mavsdk::Mocap::Mocap(const Mocap &other)
```


Copy constructor.


**Parameters**

* const [Mocap](classmavsdk_1_1_mocap.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_mocap_1a7743939450a5e816dc9e7de237ec3934}

```cpp
using mavsdk::Mocap::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Mocap](classmavsdk_1_1_mocap.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5}


Possible results returned for mocap requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown error. 
<span id="classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5aa7bcc6c4cca7f1edddd6823f37a6b3b6"></span> `InvalidRequestData` | Invalid request data. 
<span id="classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Function unsupported. 

## Member Function Documentation


### set_vision_position_estimate() {#classmavsdk_1_1_mocap_1a22d007409839e28a45d7b10f10e22fd6}
```cpp
Result mavsdk::Mocap::set_vision_position_estimate(VisionPositionEstimate vision_position_estimate) const
```


Send Global position/attitude estimate from a vision source.

This function is blocking.

**Parameters**

* [VisionPositionEstimate](structmavsdk_1_1_mocap_1_1_vision_position_estimate.md) **vision_position_estimate** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) - Result of request.

### set_attitude_position_mocap() {#classmavsdk_1_1_mocap_1a5f9a63d8bbed750056e139640b38cd7f}
```cpp
Result mavsdk::Mocap::set_attitude_position_mocap(AttitudePositionMocap attitude_position_mocap) const
```


Send motion capture attitude and position.

This function is blocking.

**Parameters**

* [AttitudePositionMocap](structmavsdk_1_1_mocap_1_1_attitude_position_mocap.md) **attitude_position_mocap** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) - Result of request.

### set_odometry() {#classmavsdk_1_1_mocap_1a149fa242e0b01bc0aee9204118b00f59}
```cpp
Result mavsdk::Mocap::set_odometry(Odometry odometry) const
```


Send odometry information with an external interface.

This function is blocking.

**Parameters**

* [Odometry](structmavsdk_1_1_mocap_1_1_odometry.md) **odometry** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mocap.md#classmavsdk_1_1_mocap_1a3af8c27b8ad9a4567feb1045e82884d5) - Result of request.

### operator=() {#classmavsdk_1_1_mocap_1adf2f33e3befbec23f43e066946050eab}
```cpp
const Mocap& mavsdk::Mocap::operator=(const Mocap &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Mocap](classmavsdk_1_1_mocap.md)&  - 

**Returns**

&emsp;const [Mocap](classmavsdk_1_1_mocap.md) & - 