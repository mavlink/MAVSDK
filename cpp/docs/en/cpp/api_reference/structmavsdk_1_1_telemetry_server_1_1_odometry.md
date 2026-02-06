# mavsdk::TelemetryServer::Odometry Struct Reference
`#include: telemetry_server.h`

----


[Odometry](structmavsdk_1_1_telemetry_server_1_1_odometry.md) message type. 


## Public Types


Type | Description
--- | ---
enum [MavFrame](#structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21) | Mavlink frame id.

## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_telemetry_server_1_1_odometry_1ad50d90cde4c76704f7c1db09cf23d982) {} - Timestamp (0 to use Backend timestamp).

[MavFrame](structmavsdk_1_1_telemetry_server_1_1_odometry.md#structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21) [frame_id](#structmavsdk_1_1_telemetry_server_1_1_odometry_1a14fbb012f2a89e1666231877e54325fe) {} - Coordinate frame of reference for the pose data.

[MavFrame](structmavsdk_1_1_telemetry_server_1_1_odometry.md#structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21) [child_frame_id](#structmavsdk_1_1_telemetry_server_1_1_odometry_1a68701f1fcb99dd5467c9d4669a932340) {} - Coordinate frame of reference for the velocity in free space (twist) data.

[PositionBody](structmavsdk_1_1_telemetry_server_1_1_position_body.md) [position_body](#structmavsdk_1_1_telemetry_server_1_1_odometry_1a4869541a4dc353dd494200b88076935b) {} - [Position](structmavsdk_1_1_telemetry_server_1_1_position.md).

[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) [q](#structmavsdk_1_1_telemetry_server_1_1_odometry_1a8f3c69b64c46a1b1108e731fca0d1100) {} - [Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) components, w, x, y, z (1 0 0 0 is the null-rotation).

[VelocityBody](structmavsdk_1_1_telemetry_server_1_1_velocity_body.md) [velocity_body](#structmavsdk_1_1_telemetry_server_1_1_odometry_1af57aaacf9282011e105755f7e98d866b) {} - Linear velocity (m/s).

[AngularVelocityBody](structmavsdk_1_1_telemetry_server_1_1_angular_velocity_body.md) [angular_velocity_body](#structmavsdk_1_1_telemetry_server_1_1_odometry_1a90ee727a18f27f343ed1798fdecb7f7d) {} - Angular velocity (rad/s).

[Covariance](structmavsdk_1_1_telemetry_server_1_1_covariance.md) [pose_covariance](#structmavsdk_1_1_telemetry_server_1_1_odometry_1ae622bfaf71cb5fc89e7f2fa4a2617161) {} - Pose cross-covariance matrix.

[Covariance](structmavsdk_1_1_telemetry_server_1_1_covariance.md) [velocity_covariance](#structmavsdk_1_1_telemetry_server_1_1_odometry_1ad37345f4de48f88cc4c59b21f16d43eb) {} - Velocity cross-covariance matrix.


## Member Enumeration Documentation


### enum MavFrame {#structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21}


Mavlink frame id.


Value | Description
--- | ---
<span id="structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21a6bf5613f65a74a7f8388b26adefaef2b"></span> `Undef` | Frame is undefined.. 
<span id="structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21a91ac262e6e7347fc5f308053580ba3da"></span> `BodyNed` | Setpoint in body NED frame. This makes sense if all position control is externalized - e.g. useful to command 2 m/s^2 acceleration to the right.. 
<span id="structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21ad394115f3c073008cf8096b81fa37fc9"></span> `VisionNed` | [Odometry](structmavsdk_1_1_telemetry_server_1_1_odometry.md) local coordinate frame of data given by a vision estimation system, Z-down (x: north, y: east, z: down).. 
<span id="structmavsdk_1_1_telemetry_server_1_1_odometry_1a763ae4b3e7146b2cb5c79db6401d2e21a605883e5a1cf6c8184abc3de6f16630b"></span> `EstimNed` | [Odometry](structmavsdk_1_1_telemetry_server_1_1_odometry.md) local coordinate frame of data given by an estimator running onboard the vehicle, Z-down (x: north, y: east, z: down).. 

## Field Documentation


### time_usec {#structmavsdk_1_1_telemetry_server_1_1_odometry_1ad50d90cde4c76704f7c1db09cf23d982}

```cpp
uint64_t mavsdk::TelemetryServer::Odometry::time_usec {}
```


Timestamp (0 to use Backend timestamp).


### frame_id {#structmavsdk_1_1_telemetry_server_1_1_odometry_1a14fbb012f2a89e1666231877e54325fe}

```cpp
MavFrame mavsdk::TelemetryServer::Odometry::frame_id {}
```


Coordinate frame of reference for the pose data.


### child_frame_id {#structmavsdk_1_1_telemetry_server_1_1_odometry_1a68701f1fcb99dd5467c9d4669a932340}

```cpp
MavFrame mavsdk::TelemetryServer::Odometry::child_frame_id {}
```


Coordinate frame of reference for the velocity in free space (twist) data.


### position_body {#structmavsdk_1_1_telemetry_server_1_1_odometry_1a4869541a4dc353dd494200b88076935b}

```cpp
PositionBody mavsdk::TelemetryServer::Odometry::position_body {}
```


[Position](structmavsdk_1_1_telemetry_server_1_1_position.md).


### q {#structmavsdk_1_1_telemetry_server_1_1_odometry_1a8f3c69b64c46a1b1108e731fca0d1100}

```cpp
Quaternion mavsdk::TelemetryServer::Odometry::q {}
```


[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) components, w, x, y, z (1 0 0 0 is the null-rotation).


### velocity_body {#structmavsdk_1_1_telemetry_server_1_1_odometry_1af57aaacf9282011e105755f7e98d866b}

```cpp
VelocityBody mavsdk::TelemetryServer::Odometry::velocity_body {}
```


Linear velocity (m/s).


### angular_velocity_body {#structmavsdk_1_1_telemetry_server_1_1_odometry_1a90ee727a18f27f343ed1798fdecb7f7d}

```cpp
AngularVelocityBody mavsdk::TelemetryServer::Odometry::angular_velocity_body {}
```


Angular velocity (rad/s).


### pose_covariance {#structmavsdk_1_1_telemetry_server_1_1_odometry_1ae622bfaf71cb5fc89e7f2fa4a2617161}

```cpp
Covariance mavsdk::TelemetryServer::Odometry::pose_covariance {}
```


Pose cross-covariance matrix.


### velocity_covariance {#structmavsdk_1_1_telemetry_server_1_1_odometry_1ad37345f4de48f88cc4c59b21f16d43eb}

```cpp
Covariance mavsdk::TelemetryServer::Odometry::velocity_covariance {}
```


Velocity cross-covariance matrix.

