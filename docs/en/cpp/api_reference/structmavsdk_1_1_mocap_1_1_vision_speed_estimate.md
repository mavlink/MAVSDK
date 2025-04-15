# mavsdk::Mocap::VisionSpeedEstimate Struct Reference
`#include: mocap.h`

----


Global speed estimate from a vision source. 


## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_mocap_1_1_vision_speed_estimate_1a17dc1893ea358e8b26d6df0323970174) {} - Timestamp UNIX Epoch time (0 to use Backend timestamp)

[SpeedNed](structmavsdk_1_1_mocap_1_1_speed_ned.md) [speed_ned](#structmavsdk_1_1_mocap_1_1_vision_speed_estimate_1a414e3e7f586aeeef28a5a353fdc4f43b) {} - Global speed (m/s)

[Covariance](structmavsdk_1_1_mocap_1_1_covariance.md) [speed_covariance](#structmavsdk_1_1_mocap_1_1_vision_speed_estimate_1a5e947837041cb9f477a3588172bb739f) {} - Linear velocity cross-covariance matrix.


## Field Documentation


### time_usec {#structmavsdk_1_1_mocap_1_1_vision_speed_estimate_1a17dc1893ea358e8b26d6df0323970174}

```cpp
uint64_t mavsdk::Mocap::VisionSpeedEstimate::time_usec {}
```


Timestamp UNIX Epoch time (0 to use Backend timestamp)


### speed_ned {#structmavsdk_1_1_mocap_1_1_vision_speed_estimate_1a414e3e7f586aeeef28a5a353fdc4f43b}

```cpp
SpeedNed mavsdk::Mocap::VisionSpeedEstimate::speed_ned {}
```


Global speed (m/s)


### speed_covariance {#structmavsdk_1_1_mocap_1_1_vision_speed_estimate_1a5e947837041cb9f477a3588172bb739f}

```cpp
Covariance mavsdk::Mocap::VisionSpeedEstimate::speed_covariance {}
```


Linear velocity cross-covariance matrix.

