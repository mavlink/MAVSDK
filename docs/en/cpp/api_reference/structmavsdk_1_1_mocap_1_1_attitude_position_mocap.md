# mavsdk::Mocap::AttitudePositionMocap Struct Reference
`#include: mocap.h`

----


Motion capture attitude and position. 


## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a71fe88d56f731ccf1006ab4824dd28a5) {} - [PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) frame timestamp UNIX Epoch time (0 to use Backend timestamp)

[Quaternion](structmavsdk_1_1_mocap_1_1_quaternion.md) [q](#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a7a0dc18912abfd898ab277fa1cde1ea6) {} - Attitude quaternion (w, x, y, z order, zero-rotation is 1, 0, 0, 0)

[PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) [position_body](#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a5fc4e26d1c619304149d5fa5f7fed6a0) {} - Body Position (NED)

[Covariance](structmavsdk_1_1_mocap_1_1_covariance.md) [pose_covariance](#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a5e927086fb1877984f364884cbce42e9) {} - Pose cross-covariance matrix.


## Field Documentation


### time_usec {#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a71fe88d56f731ccf1006ab4824dd28a5}

```cpp
uint64_t mavsdk::Mocap::AttitudePositionMocap::time_usec {}
```


[PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) frame timestamp UNIX Epoch time (0 to use Backend timestamp)


### q {#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a7a0dc18912abfd898ab277fa1cde1ea6}

```cpp
Quaternion mavsdk::Mocap::AttitudePositionMocap::q {}
```


Attitude quaternion (w, x, y, z order, zero-rotation is 1, 0, 0, 0)


### position_body {#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a5fc4e26d1c619304149d5fa5f7fed6a0}

```cpp
PositionBody mavsdk::Mocap::AttitudePositionMocap::position_body {}
```


Body Position (NED)


### pose_covariance {#structmavsdk_1_1_mocap_1_1_attitude_position_mocap_1a5e927086fb1877984f364884cbce42e9}

```cpp
Covariance mavsdk::Mocap::AttitudePositionMocap::pose_covariance {}
```


Pose cross-covariance matrix.

