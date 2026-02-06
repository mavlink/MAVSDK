# mavsdk::Mocap::Odometry Struct Reference
`#include: mocap.h`

----


[Odometry](structmavsdk_1_1_mocap_1_1_odometry.md) message to communicate odometry information with an external interface. 


## Public Types


Type | Description
--- | ---
enum [MavFrame](#structmavsdk_1_1_mocap_1_1_odometry_1a9c98852de68b23f07a78a0fc021c8a33) | Mavlink frame id.

## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_mocap_1_1_odometry_1a015fd5755c9953ab7d7be4736fad5300) {} - Timestamp (0 to use Backend timestamp).

[MavFrame](structmavsdk_1_1_mocap_1_1_odometry.md#structmavsdk_1_1_mocap_1_1_odometry_1a9c98852de68b23f07a78a0fc021c8a33) [frame_id](#structmavsdk_1_1_mocap_1_1_odometry_1ab7ce4f3ea27857eb44d2efe7a010e3d3) {} - Coordinate frame of reference for the pose data.

[PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) [position_body](#structmavsdk_1_1_mocap_1_1_odometry_1af7b586261930981742683b3253fc9989) {} - Body Position.

[Quaternion](structmavsdk_1_1_mocap_1_1_quaternion.md) [q](#structmavsdk_1_1_mocap_1_1_odometry_1ae5a8c05815a15250f2cf37e1d5a21be5) {} - [Quaternion](structmavsdk_1_1_mocap_1_1_quaternion.md) components, w, x, y, z (1 0 0 0 is the null-rotation).

[SpeedBody](structmavsdk_1_1_mocap_1_1_speed_body.md) [speed_body](#structmavsdk_1_1_mocap_1_1_odometry_1a123c544d4ea98bf91d0740321aa6bb00) {} - Linear speed (m/s).

[AngularVelocityBody](structmavsdk_1_1_mocap_1_1_angular_velocity_body.md) [angular_velocity_body](#structmavsdk_1_1_mocap_1_1_odometry_1acf8cf54ebe80fb872250aa0e91b7e721) {} - Angular speed (rad/s).

[Covariance](structmavsdk_1_1_mocap_1_1_covariance.md) [pose_covariance](#structmavsdk_1_1_mocap_1_1_odometry_1a38673fa707dfbbd8c195bb743d55d1b7) {} - Pose cross-covariance matrix.

[Covariance](structmavsdk_1_1_mocap_1_1_covariance.md) [velocity_covariance](#structmavsdk_1_1_mocap_1_1_odometry_1a76cc8691a7aa85e2351b99f1c6b0c126) {} - Velocity cross-covariance matrix.


## Member Enumeration Documentation


### enum MavFrame {#structmavsdk_1_1_mocap_1_1_odometry_1a9c98852de68b23f07a78a0fc021c8a33}


Mavlink frame id.


Value | Description
--- | ---
<span id="structmavsdk_1_1_mocap_1_1_odometry_1a9c98852de68b23f07a78a0fc021c8a33aa596c29912446dcd690daf29d1ca68af"></span> `MocapNed` | MAVLink number: 14. [Odometry](structmavsdk_1_1_mocap_1_1_odometry.md) local coordinate frame of data given by a motion capture system, Z-down (x: north, y: east, z: down).. 
<span id="structmavsdk_1_1_mocap_1_1_odometry_1a9c98852de68b23f07a78a0fc021c8a33a4feb9efb49990fff5a73858673d7ffde"></span> `LocalFrd` | MAVLink number: 20. Forward, Right, Down coordinate frame. This is a local frame with Z-down and arbitrary F/R alignment (i.e. not aligned with NED/earth frame). Replacement for MAV_FRAME_MOCAP_NED, MAV_FRAME_VISION_NED, MAV_FRAME_ESTIM_NED.. 

## Field Documentation


### time_usec {#structmavsdk_1_1_mocap_1_1_odometry_1a015fd5755c9953ab7d7be4736fad5300}

```cpp
uint64_t mavsdk::Mocap::Odometry::time_usec {}
```


Timestamp (0 to use Backend timestamp).


### frame_id {#structmavsdk_1_1_mocap_1_1_odometry_1ab7ce4f3ea27857eb44d2efe7a010e3d3}

```cpp
MavFrame mavsdk::Mocap::Odometry::frame_id {}
```


Coordinate frame of reference for the pose data.


### position_body {#structmavsdk_1_1_mocap_1_1_odometry_1af7b586261930981742683b3253fc9989}

```cpp
PositionBody mavsdk::Mocap::Odometry::position_body {}
```


Body Position.


### q {#structmavsdk_1_1_mocap_1_1_odometry_1ae5a8c05815a15250f2cf37e1d5a21be5}

```cpp
Quaternion mavsdk::Mocap::Odometry::q {}
```


[Quaternion](structmavsdk_1_1_mocap_1_1_quaternion.md) components, w, x, y, z (1 0 0 0 is the null-rotation).


### speed_body {#structmavsdk_1_1_mocap_1_1_odometry_1a123c544d4ea98bf91d0740321aa6bb00}

```cpp
SpeedBody mavsdk::Mocap::Odometry::speed_body {}
```


Linear speed (m/s).


### angular_velocity_body {#structmavsdk_1_1_mocap_1_1_odometry_1acf8cf54ebe80fb872250aa0e91b7e721}

```cpp
AngularVelocityBody mavsdk::Mocap::Odometry::angular_velocity_body {}
```


Angular speed (rad/s).


### pose_covariance {#structmavsdk_1_1_mocap_1_1_odometry_1a38673fa707dfbbd8c195bb743d55d1b7}

```cpp
Covariance mavsdk::Mocap::Odometry::pose_covariance {}
```


Pose cross-covariance matrix.


### velocity_covariance {#structmavsdk_1_1_mocap_1_1_odometry_1a76cc8691a7aa85e2351b99f1c6b0c126}

```cpp
Covariance mavsdk::Mocap::Odometry::velocity_covariance {}
```


Velocity cross-covariance matrix.

