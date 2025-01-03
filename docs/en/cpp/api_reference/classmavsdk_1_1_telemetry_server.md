# mavsdk::TelemetryServer Class Reference
`#include: telemetry_server.h`

----


Allow users to provide vehicle telemetry and state information (e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates. 


## Data Structures


struct [AccelerationFrd](structmavsdk_1_1_telemetry_server_1_1_acceleration_frd.md)

struct [ActuatorControlTarget](structmavsdk_1_1_telemetry_server_1_1_actuator_control_target.md)

struct [ActuatorOutputStatus](structmavsdk_1_1_telemetry_server_1_1_actuator_output_status.md)

struct [AngularVelocityBody](structmavsdk_1_1_telemetry_server_1_1_angular_velocity_body.md)

struct [AngularVelocityFrd](structmavsdk_1_1_telemetry_server_1_1_angular_velocity_frd.md)

struct [Battery](structmavsdk_1_1_telemetry_server_1_1_battery.md)

struct [Covariance](structmavsdk_1_1_telemetry_server_1_1_covariance.md)

struct [DistanceSensor](structmavsdk_1_1_telemetry_server_1_1_distance_sensor.md)

struct [EulerAngle](structmavsdk_1_1_telemetry_server_1_1_euler_angle.md)

struct [FixedwingMetrics](structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics.md)

struct [GpsInfo](structmavsdk_1_1_telemetry_server_1_1_gps_info.md)

struct [GroundTruth](structmavsdk_1_1_telemetry_server_1_1_ground_truth.md)

struct [Heading](structmavsdk_1_1_telemetry_server_1_1_heading.md)

struct [Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md)

struct [MagneticFieldFrd](structmavsdk_1_1_telemetry_server_1_1_magnetic_field_frd.md)

struct [Odometry](structmavsdk_1_1_telemetry_server_1_1_odometry.md)

struct [Position](structmavsdk_1_1_telemetry_server_1_1_position.md)

struct [PositionBody](structmavsdk_1_1_telemetry_server_1_1_position_body.md)

struct [PositionNed](structmavsdk_1_1_telemetry_server_1_1_position_ned.md)

struct [PositionVelocityNed](structmavsdk_1_1_telemetry_server_1_1_position_velocity_ned.md)

struct [Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md)

struct [RawGps](structmavsdk_1_1_telemetry_server_1_1_raw_gps.md)

struct [RcStatus](structmavsdk_1_1_telemetry_server_1_1_rc_status.md)

struct [ScaledPressure](structmavsdk_1_1_telemetry_server_1_1_scaled_pressure.md)

struct [StatusText](structmavsdk_1_1_telemetry_server_1_1_status_text.md)

struct [VelocityBody](structmavsdk_1_1_telemetry_server_1_1_velocity_body.md)

struct [VelocityNed](structmavsdk_1_1_telemetry_server_1_1_velocity_ned.md)

## Public Types


Type | Description
--- | ---
enum [FixType](#classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36e) | GPS fix type.
enum [VtolState](#classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fda) | Maps to MAV_VTOL_STATE.
enum [StatusTextType](#classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55) | Status types.
enum [LandedState](#classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdb) | Landed State enumeration.
enum [Result](#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | Possible results returned for telemetry requests.
std::function< void([Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a))> [ResultCallback](#classmavsdk_1_1_telemetry_server_1a47ca7112b0b88f42b0cf7e51fa79d010) | Callback type for asynchronous [TelemetryServer](classmavsdk_1_1_telemetry_server.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [TelemetryServer](#classmavsdk_1_1_telemetry_server_1a49e68672755eaa7a4b0b42108bc8761a) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~TelemetryServer](#classmavsdk_1_1_telemetry_server_1a459bdc50c062ae94060a331c1a1d8ed8) () override | Destructor (internal use only).
&nbsp; | [TelemetryServer](#classmavsdk_1_1_telemetry_server_1a412c886b49f0c7b0cf64c13e7664cc7f) (const [TelemetryServer](classmavsdk_1_1_telemetry_server.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_position](#classmavsdk_1_1_telemetry_server_1ac323f672bf90a210f62bc460f45ce1b4) ([Position](structmavsdk_1_1_telemetry_server_1_1_position.md) position, [VelocityNed](structmavsdk_1_1_telemetry_server_1_1_velocity_ned.md) velocity_ned, [Heading](structmavsdk_1_1_telemetry_server_1_1_heading.md) heading)const | Publish to 'position' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_home](#classmavsdk_1_1_telemetry_server_1a4a7a504dc665a134b85ea82f90c4e1ac) ([Position](structmavsdk_1_1_telemetry_server_1_1_position.md) home)const | Publish to 'home position' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_sys_status](#classmavsdk_1_1_telemetry_server_1a4abd61400c28d53c0e244c8b7ad07730) ([Battery](structmavsdk_1_1_telemetry_server_1_1_battery.md) battery, bool rc_receiver_status, bool gyro_status, bool accel_status, bool mag_status, bool gps_status)const | Publish 'sys status' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_extended_sys_state](#classmavsdk_1_1_telemetry_server_1a8bcb2315e9b2f2f0999d83d61c951961) ([VtolState](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fda) vtol_state, [LandedState](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdb) landed_state)const | Publish 'extended sys state' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_raw_gps](#classmavsdk_1_1_telemetry_server_1a9a81a55e3da3a79251c03baf8838623f) ([RawGps](structmavsdk_1_1_telemetry_server_1_1_raw_gps.md) raw_gps, [GpsInfo](structmavsdk_1_1_telemetry_server_1_1_gps_info.md) gps_info)const | Publish to 'Raw GPS' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_battery](#classmavsdk_1_1_telemetry_server_1ae4a6bb3929517e74f01cfcbfc2bdd9e1) ([Battery](structmavsdk_1_1_telemetry_server_1_1_battery.md) battery)const | Publish to 'battery' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_status_text](#classmavsdk_1_1_telemetry_server_1a63de339cba22f7ead00cad6021f476c8) ([StatusText](structmavsdk_1_1_telemetry_server_1_1_status_text.md) status_text)const | Publish to 'status text' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_odometry](#classmavsdk_1_1_telemetry_server_1a5663a04d971242dcd4aeb0241304279c) ([Odometry](structmavsdk_1_1_telemetry_server_1_1_odometry.md) odometry)const | Publish to 'odometry' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_position_velocity_ned](#classmavsdk_1_1_telemetry_server_1ac20e6bd7250de7c1290ccf9d18b02b96) ([PositionVelocityNed](structmavsdk_1_1_telemetry_server_1_1_position_velocity_ned.md) position_velocity_ned)const | Publish to 'position velocity' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_ground_truth](#classmavsdk_1_1_telemetry_server_1af882e6fa87294895f6afb7da8c9f6761) ([GroundTruth](structmavsdk_1_1_telemetry_server_1_1_ground_truth.md) ground_truth)const | Publish to 'ground truth' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_imu](#classmavsdk_1_1_telemetry_server_1a077f33d7fdbdb0cc9b091bdf8e647969) ([Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) imu)const | Publish to 'IMU' updates (in SI units in NED body frame).
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_scaled_imu](#classmavsdk_1_1_telemetry_server_1aff52c34112c885662c711cda94b7c985) ([Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) imu)const | Publish to 'Scaled IMU' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_raw_imu](#classmavsdk_1_1_telemetry_server_1a92f3fcb090ffc96c70ce35d433a1a2a5) ([Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) imu)const | Publish to 'Raw IMU' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_unix_epoch_time](#classmavsdk_1_1_telemetry_server_1a27b1b901cd8baf91380029c2b95b2dac) (uint64_t time_us)const | Publish to 'unix epoch time' updates.
[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) | [publish_distance_sensor](#classmavsdk_1_1_telemetry_server_1a7532d068284fb7f55c00804a4a996a6d) ([DistanceSensor](structmavsdk_1_1_telemetry_server_1_1_distance_sensor.md) distance_sensor)const | Publish to "distance sensor" updates.
const [TelemetryServer](classmavsdk_1_1_telemetry_server.md) & | [operator=](#classmavsdk_1_1_telemetry_server_1a479502f1ce3bdc2c5be486911a20ca25) (const [TelemetryServer](classmavsdk_1_1_telemetry_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### TelemetryServer() {#classmavsdk_1_1_telemetry_server_1a49e68672755eaa7a4b0b42108bc8761a}
```cpp
mavsdk::TelemetryServer::TelemetryServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto telemetry_server = TelemetryServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~TelemetryServer() {#classmavsdk_1_1_telemetry_server_1a459bdc50c062ae94060a331c1a1d8ed8}
```cpp
mavsdk::TelemetryServer::~TelemetryServer() override
```


Destructor (internal use only).


### TelemetryServer() {#classmavsdk_1_1_telemetry_server_1a412c886b49f0c7b0cf64c13e7664cc7f}
```cpp
mavsdk::TelemetryServer::TelemetryServer(const TelemetryServer &other)
```


Copy constructor.


**Parameters**

* const [TelemetryServer](classmavsdk_1_1_telemetry_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_telemetry_server_1a47ca7112b0b88f42b0cf7e51fa79d010}

```cpp
using mavsdk::TelemetryServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [TelemetryServer](classmavsdk_1_1_telemetry_server.md) calls.


## Member Enumeration Documentation


### enum FixType {#classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36e}


GPS fix type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36eaeb42b0f27d1fc2ca9bd645212a14c874"></span> `NoGps` | No GPS connected. 
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36ea7458f6cf09e53df9495d3ee0d11868c4"></span> `NoFix` | No position information, GPS is connected. 
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36ead6800eded02a7eaceb638929dbd9ea55"></span> `Fix2D` | 2D position. 
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36ead791aa90d108c9b0c76ec28cd4dfbb0f"></span> `Fix3D` | 3D position. 
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36ea8372860807abaec59412bd6376f51b5f"></span> `FixDgps` | DGPS/SBAS aided 3D position. 
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36ea1eacb557d24c49af2ec6832c5fc32413"></span> `RtkFloat` | RTK float, 3D position. 
<span id="classmavsdk_1_1_telemetry_server_1ac3acfa87349708ecf8b53c8e7426e36ea9effa0afed44833d540fec2c57e67426"></span> `RtkFixed` | RTK Fixed, 3D position. 

### enum VtolState {#classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fda}


Maps to MAV_VTOL_STATE.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fdaaec0fc0100c4fc1ce4eea230c3dc10360"></span> `Undefined` | Not VTOL. 
<span id="classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fdaa45f439a04f9144adc1dcf5a4fbda0026"></span> `TransitionToFw` | Transitioning to fixed-wing. 
<span id="classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fdaaf3abadebd9602a0acb33350f64fa7661"></span> `TransitionToMc` | Transitioning to multi-copter. 
<span id="classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fdaaac81adaad0b2a7d6077edd5c319a6048"></span> `Mc` | Multi-copter. 
<span id="classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fdaa9b5de99abdffa3a42bd6517ed1a034e4"></span> `Fw` | Fixed-wing. 

### enum StatusTextType {#classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55}


Status types.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55aa603905470e2a5b8c13e96b579ef0dba"></span> `Debug` | Debug. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55a4059b0251f66a18cb56f544728796875"></span> `Info` | Information. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55a24efa7ee4511563b16144f39706d594f"></span> `Notice` | Notice. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55a0eaadb4fcb48a0a0ed7bc9868be9fbaa"></span> `Warning` | Warning. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55a278d01e5af56273bae1bb99a98b370cd"></span> `Critical` | Critical. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55ab92071d61c88498171928745ca53078b"></span> `Alert` | Alert. 
<span id="classmavsdk_1_1_telemetry_server_1a31ddd2e06b5f7c41eeaab4967f550c55aa3fa706f20bc0b7858b7ae6932261940"></span> `Emergency` | Emergency. 

### enum LandedState {#classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdb}


Landed State enumeration.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdba88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Landed state is unknown. 
<span id="classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdba2b9beed57034f5727573d7ded76cf777"></span> `OnGround` | The vehicle is on the ground. 
<span id="classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdbaee4e669a07b061d70b9b79dfed9cb5e7"></span> `InAir` | The vehicle is in the air. 
<span id="classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdba85916c5f3400cfa25d988f05b6736a94"></span> `TakingOff` | The vehicle is taking off. 
<span id="classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdba41bd61e268fedccfb0d91dd571dd28b2"></span> `Landing` | The vehicle is landing. 

### enum Result {#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a}


Possible results returned for telemetry requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Success: the telemetry command was accepted by the vehicle. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aa094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aa3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command refused by vehicle. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8aab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Request not supported. 

## Member Function Documentation


### publish_position() {#classmavsdk_1_1_telemetry_server_1ac323f672bf90a210f62bc460f45ce1b4}
```cpp
Result mavsdk::TelemetryServer::publish_position(Position position, VelocityNed velocity_ned, Heading heading) const
```


Publish to 'position' updates.

This function is blocking.

**Parameters**

* [Position](structmavsdk_1_1_telemetry_server_1_1_position.md) **position** - 
* [VelocityNed](structmavsdk_1_1_telemetry_server_1_1_velocity_ned.md) **velocity_ned** - 
* [Heading](structmavsdk_1_1_telemetry_server_1_1_heading.md) **heading** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_home() {#classmavsdk_1_1_telemetry_server_1a4a7a504dc665a134b85ea82f90c4e1ac}
```cpp
Result mavsdk::TelemetryServer::publish_home(Position home) const
```


Publish to 'home position' updates.

This function is blocking.

**Parameters**

* [Position](structmavsdk_1_1_telemetry_server_1_1_position.md) **home** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_sys_status() {#classmavsdk_1_1_telemetry_server_1a4abd61400c28d53c0e244c8b7ad07730}
```cpp
Result mavsdk::TelemetryServer::publish_sys_status(Battery battery, bool rc_receiver_status, bool gyro_status, bool accel_status, bool mag_status, bool gps_status) const
```


Publish 'sys status' updates.

This function is blocking.

**Parameters**

* [Battery](structmavsdk_1_1_telemetry_server_1_1_battery.md) **battery** - 
* bool **rc_receiver_status** - 
* bool **gyro_status** - 
* bool **accel_status** - 
* bool **mag_status** - 
* bool **gps_status** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_extended_sys_state() {#classmavsdk_1_1_telemetry_server_1a8bcb2315e9b2f2f0999d83d61c951961}
```cpp
Result mavsdk::TelemetryServer::publish_extended_sys_state(VtolState vtol_state, LandedState landed_state) const
```


Publish 'extended sys state' updates.

This function is blocking.

**Parameters**

* [VtolState](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a8ca81cee4e0f7702ab854c10bec91fda) **vtol_state** - 
* [LandedState](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a28e2047a3a679c5fcca3945a44f66bdb) **landed_state** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_raw_gps() {#classmavsdk_1_1_telemetry_server_1a9a81a55e3da3a79251c03baf8838623f}
```cpp
Result mavsdk::TelemetryServer::publish_raw_gps(RawGps raw_gps, GpsInfo gps_info) const
```


Publish to 'Raw GPS' updates.

This function is blocking.

**Parameters**

* [RawGps](structmavsdk_1_1_telemetry_server_1_1_raw_gps.md) **raw_gps** - 
* [GpsInfo](structmavsdk_1_1_telemetry_server_1_1_gps_info.md) **gps_info** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_battery() {#classmavsdk_1_1_telemetry_server_1ae4a6bb3929517e74f01cfcbfc2bdd9e1}
```cpp
Result mavsdk::TelemetryServer::publish_battery(Battery battery) const
```


Publish to 'battery' updates.

This function is blocking.

**Parameters**

* [Battery](structmavsdk_1_1_telemetry_server_1_1_battery.md) **battery** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_status_text() {#classmavsdk_1_1_telemetry_server_1a63de339cba22f7ead00cad6021f476c8}
```cpp
Result mavsdk::TelemetryServer::publish_status_text(StatusText status_text) const
```


Publish to 'status text' updates.

This function is blocking.

**Parameters**

* [StatusText](structmavsdk_1_1_telemetry_server_1_1_status_text.md) **status_text** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_odometry() {#classmavsdk_1_1_telemetry_server_1a5663a04d971242dcd4aeb0241304279c}
```cpp
Result mavsdk::TelemetryServer::publish_odometry(Odometry odometry) const
```


Publish to 'odometry' updates.

This function is blocking.

**Parameters**

* [Odometry](structmavsdk_1_1_telemetry_server_1_1_odometry.md) **odometry** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_position_velocity_ned() {#classmavsdk_1_1_telemetry_server_1ac20e6bd7250de7c1290ccf9d18b02b96}
```cpp
Result mavsdk::TelemetryServer::publish_position_velocity_ned(PositionVelocityNed position_velocity_ned) const
```


Publish to 'position velocity' updates.

This function is blocking.

**Parameters**

* [PositionVelocityNed](structmavsdk_1_1_telemetry_server_1_1_position_velocity_ned.md) **position_velocity_ned** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_ground_truth() {#classmavsdk_1_1_telemetry_server_1af882e6fa87294895f6afb7da8c9f6761}
```cpp
Result mavsdk::TelemetryServer::publish_ground_truth(GroundTruth ground_truth) const
```


Publish to 'ground truth' updates.

This function is blocking.

**Parameters**

* [GroundTruth](structmavsdk_1_1_telemetry_server_1_1_ground_truth.md) **ground_truth** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_imu() {#classmavsdk_1_1_telemetry_server_1a077f33d7fdbdb0cc9b091bdf8e647969}
```cpp
Result mavsdk::TelemetryServer::publish_imu(Imu imu) const
```


Publish to 'IMU' updates (in SI units in NED body frame).

This function is blocking.

**Parameters**

* [Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) **imu** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_scaled_imu() {#classmavsdk_1_1_telemetry_server_1aff52c34112c885662c711cda94b7c985}
```cpp
Result mavsdk::TelemetryServer::publish_scaled_imu(Imu imu) const
```


Publish to 'Scaled IMU' updates.

This function is blocking.

**Parameters**

* [Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) **imu** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_raw_imu() {#classmavsdk_1_1_telemetry_server_1a92f3fcb090ffc96c70ce35d433a1a2a5}
```cpp
Result mavsdk::TelemetryServer::publish_raw_imu(Imu imu) const
```


Publish to 'Raw IMU' updates.

This function is blocking.

**Parameters**

* [Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) **imu** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_unix_epoch_time() {#classmavsdk_1_1_telemetry_server_1a27b1b901cd8baf91380029c2b95b2dac}
```cpp
Result mavsdk::TelemetryServer::publish_unix_epoch_time(uint64_t time_us) const
```


Publish to 'unix epoch time' updates.

This function is blocking.

**Parameters**

* uint64_t **time_us** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### publish_distance_sensor() {#classmavsdk_1_1_telemetry_server_1a7532d068284fb7f55c00804a4a996a6d}
```cpp
Result mavsdk::TelemetryServer::publish_distance_sensor(DistanceSensor distance_sensor) const
```


Publish to "distance sensor" updates.

This function is blocking.

**Parameters**

* [DistanceSensor](structmavsdk_1_1_telemetry_server_1_1_distance_sensor.md) **distance_sensor** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry_server.md#classmavsdk_1_1_telemetry_server_1a39d62e69bdc289d55b73b0e4c3a3ac8a) - Result of request.

### operator=() {#classmavsdk_1_1_telemetry_server_1a479502f1ce3bdc2c5be486911a20ca25}
```cpp
const TelemetryServer& mavsdk::TelemetryServer::operator=(const TelemetryServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [TelemetryServer](classmavsdk_1_1_telemetry_server.md)&  - 

**Returns**

&emsp;const [TelemetryServer](classmavsdk_1_1_telemetry_server.md) & - 