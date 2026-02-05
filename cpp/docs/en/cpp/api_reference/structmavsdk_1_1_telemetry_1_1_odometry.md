# mavsdk::Telemetry::Odometry Struct Reference
`#include: telemetry.h`

----


[Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) message type. 


## Public Types


Type | Description
--- | ---
enum [MavFrame](#structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59) | Mavlink frame id.

## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_telemetry_1_1_odometry_1aa58eb5c15f53acf35333115c8d3673f9) {} - Timestamp (0 to use Backend timestamp).

[MavFrame](structmavsdk_1_1_telemetry_1_1_odometry.md#structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59) [frame_id](#structmavsdk_1_1_telemetry_1_1_odometry_1a40cde4148b2b495fc5b1d8606978355a) {} - Coordinate frame of reference for the pose data.

[MavFrame](structmavsdk_1_1_telemetry_1_1_odometry.md#structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59) [child_frame_id](#structmavsdk_1_1_telemetry_1_1_odometry_1a624b96ff786cb7d3498ceb64c10323c7) {} - Coordinate frame of reference for the velocity in free space (twist) data.

[PositionBody](structmavsdk_1_1_telemetry_1_1_position_body.md) [position_body](#structmavsdk_1_1_telemetry_1_1_odometry_1a836f0338481b24874e4dcf7a82bc391b) {} - [Position](structmavsdk_1_1_telemetry_1_1_position.md).

[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) [q](#structmavsdk_1_1_telemetry_1_1_odometry_1a32f49ead9d8964da05af2b92fd6e2127) {} - [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) components, w, x, y, z (1 0 0 0 is the null-rotation).

[VelocityBody](structmavsdk_1_1_telemetry_1_1_velocity_body.md) [velocity_body](#structmavsdk_1_1_telemetry_1_1_odometry_1aed1ca26d563e40c6fca6b2a6807c882e) {} - Linear velocity (m/s).

[AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md) [angular_velocity_body](#structmavsdk_1_1_telemetry_1_1_odometry_1a023deb0e97bc06d5ea19c5dfc42e230a) {} - Angular velocity (rad/s).

[Covariance](structmavsdk_1_1_telemetry_1_1_covariance.md) [pose_covariance](#structmavsdk_1_1_telemetry_1_1_odometry_1a2f3082632fa756680141224749d662ab) {} - Pose cross-covariance matrix.

[Covariance](structmavsdk_1_1_telemetry_1_1_covariance.md) [velocity_covariance](#structmavsdk_1_1_telemetry_1_1_odometry_1a32471f75a46451d56e393ba9fcd9c118) {} - Velocity cross-covariance matrix.


## Member Enumeration Documentation


### enum MavFrame {#structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59}


Mavlink frame id.


Value | Description
--- | ---
<span id="structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59a6bf5613f65a74a7f8388b26adefaef2b"></span> `Undef` | Frame is undefined.. 
<span id="structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59a91ac262e6e7347fc5f308053580ba3da"></span> `BodyNed` | Setpoint in body NED frame. This makes sense if all position control is externalized - e.g. useful to command 2 m/s^2 acceleration to the right.. 
<span id="structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59ad394115f3c073008cf8096b81fa37fc9"></span> `VisionNed` | [Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) local coordinate frame of data given by a vision estimation system, Z-down (x: north, y: east, z: down).. 
<span id="structmavsdk_1_1_telemetry_1_1_odometry_1ae14a60c5ce30809d8de3aafc73e6fa59a605883e5a1cf6c8184abc3de6f16630b"></span> `EstimNed` | [Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) local coordinate frame of data given by an estimator running onboard the vehicle, Z-down (x: north, y: east, z: down).. 

## Field Documentation


### time_usec {#structmavsdk_1_1_telemetry_1_1_odometry_1aa58eb5c15f53acf35333115c8d3673f9}

```cpp
uint64_t mavsdk::Telemetry::Odometry::time_usec {}
```


Timestamp (0 to use Backend timestamp).


### frame_id {#structmavsdk_1_1_telemetry_1_1_odometry_1a40cde4148b2b495fc5b1d8606978355a}

```cpp
MavFrame mavsdk::Telemetry::Odometry::frame_id {}
```


Coordinate frame of reference for the pose data.


### child_frame_id {#structmavsdk_1_1_telemetry_1_1_odometry_1a624b96ff786cb7d3498ceb64c10323c7}

```cpp
MavFrame mavsdk::Telemetry::Odometry::child_frame_id {}
```


Coordinate frame of reference for the velocity in free space (twist) data.


### position_body {#structmavsdk_1_1_telemetry_1_1_odometry_1a836f0338481b24874e4dcf7a82bc391b}

```cpp
PositionBody mavsdk::Telemetry::Odometry::position_body {}
```


[Position](structmavsdk_1_1_telemetry_1_1_position.md).


### q {#structmavsdk_1_1_telemetry_1_1_odometry_1a32f49ead9d8964da05af2b92fd6e2127}

```cpp
Quaternion mavsdk::Telemetry::Odometry::q {}
```


[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) components, w, x, y, z (1 0 0 0 is the null-rotation).


### velocity_body {#structmavsdk_1_1_telemetry_1_1_odometry_1aed1ca26d563e40c6fca6b2a6807c882e}

```cpp
VelocityBody mavsdk::Telemetry::Odometry::velocity_body {}
```


Linear velocity (m/s).


### angular_velocity_body {#structmavsdk_1_1_telemetry_1_1_odometry_1a023deb0e97bc06d5ea19c5dfc42e230a}

```cpp
AngularVelocityBody mavsdk::Telemetry::Odometry::angular_velocity_body {}
```


Angular velocity (rad/s).


### pose_covariance {#structmavsdk_1_1_telemetry_1_1_odometry_1a2f3082632fa756680141224749d662ab}

```cpp
Covariance mavsdk::Telemetry::Odometry::pose_covariance {}
```


Pose cross-covariance matrix.


### velocity_covariance {#structmavsdk_1_1_telemetry_1_1_odometry_1a32471f75a46451d56e393ba9fcd9c118}

```cpp
Covariance mavsdk::Telemetry::Odometry::velocity_covariance {}
```


Velocity cross-covariance matrix.

