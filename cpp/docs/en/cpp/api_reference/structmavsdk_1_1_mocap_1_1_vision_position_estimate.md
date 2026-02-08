# mavsdk::Mocap::VisionPositionEstimate Struct Reference
`#include: mocap.h`

----


Global position/attitude estimate from a vision source. 


## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1ada844c4f8f0a9599526ec733b56f0618) {} - [PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) frame timestamp UNIX Epoch time (0 to use Backend timestamp)

[PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) [position_body](#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1a730623cc79240c166f9b38ff7d621350) {} - Global position (m)

[AngleBody](structmavsdk_1_1_mocap_1_1_angle_body.md) [angle_body](#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1a6b6cafb1452c6c3ce2c85c2f917b9479) {} - Body angle (rad).

[Covariance](structmavsdk_1_1_mocap_1_1_covariance.md) [pose_covariance](#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1abba582506ff4046b4293c06380269c75) {} - Pose cross-covariance matrix.


## Field Documentation


### time_usec {#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1ada844c4f8f0a9599526ec733b56f0618}

```cpp
uint64_t mavsdk::Mocap::VisionPositionEstimate::time_usec {}
```


[PositionBody](structmavsdk_1_1_mocap_1_1_position_body.md) frame timestamp UNIX Epoch time (0 to use Backend timestamp)


### position_body {#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1a730623cc79240c166f9b38ff7d621350}

```cpp
PositionBody mavsdk::Mocap::VisionPositionEstimate::position_body {}
```


Global position (m)


### angle_body {#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1a6b6cafb1452c6c3ce2c85c2f917b9479}

```cpp
AngleBody mavsdk::Mocap::VisionPositionEstimate::angle_body {}
```


Body angle (rad).


### pose_covariance {#structmavsdk_1_1_mocap_1_1_vision_position_estimate_1abba582506ff4046b4293c06380269c75}

```cpp
Covariance mavsdk::Mocap::VisionPositionEstimate::pose_covariance {}
```


Pose cross-covariance matrix.

