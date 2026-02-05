# mavsdk::FollowMe::Config Struct Reference
`#include: follow_me.h`

----


Configuration type. 


## Public Types


Type | Description
--- | ---
enum [FollowAltitudeMode](#structmavsdk_1_1_follow_me_1_1_config_1a25f3b39bc6d0eda07891537478bed85b) | Altitude mode to configure which altitude the follow me will assume the target to be at.

## Data Fields


float [follow_height_m](#structmavsdk_1_1_follow_me_1_1_config_1a221d2136ba04ffc0e46cd17f992f0d86) { 8.0f} - [m] Follow height in meters (recommended minimum 8 meters)

float [follow_distance_m](#structmavsdk_1_1_follow_me_1_1_config_1a2f1407099516da129f650875e676a7eb) {8.0f} - [m] Follow distance to target in meters (recommended minimum 4 meter)

float [responsiveness](#structmavsdk_1_1_follow_me_1_1_config_1a3b0bced94506c4248cbaa9cc0894616a) {0.1f} - How responsive the vehicle is to the motion of the target, Lower value = More responsive (range 0.0 to 1.0)

[FollowAltitudeMode](structmavsdk_1_1_follow_me_1_1_config.md#structmavsdk_1_1_follow_me_1_1_config_1a25f3b39bc6d0eda07891537478bed85b) [altitude_mode](#structmavsdk_1_1_follow_me_1_1_config_1a70d86547c8f23e1fbd2b66632b484dcd) {} - Follow Altitude control mode.

float [max_tangential_vel_m_s](#structmavsdk_1_1_follow_me_1_1_config_1aa466aeef1806c3053a9558c9e1ebf8b9) { 8.0f} - [m/s] Maximum orbit tangential velocity relative to the target, in meters per second. Higher value = More aggressive follow angle tracking.

float [follow_angle_deg](#structmavsdk_1_1_follow_me_1_1_config_1a15d09482cd57a3d4a51ce9896ac37143) { 180.0f} - [deg] Follow Angle relative to the target. 0 equals following in front of the target's direction. Angle increases in Clockwise direction, so following from right would be 90 degrees, from the left is -90 degrees, and so on.


## Member Enumeration Documentation


### enum FollowAltitudeMode {#structmavsdk_1_1_follow_me_1_1_config_1a25f3b39bc6d0eda07891537478bed85b}


Altitude mode to configure which altitude the follow me will assume the target to be at.


Value | Description
--- | ---
<span id="structmavsdk_1_1_follow_me_1_1_config_1a25f3b39bc6d0eda07891537478bed85bacb17869fe51048b5a5c4c6106551a255"></span> `Constant` | Target assumed to be mobing at a constant altitude of home position (where the vehicle armed). 
<span id="structmavsdk_1_1_follow_me_1_1_config_1a25f3b39bc6d0eda07891537478bed85ba4ccfea7a25fae3c1d31555f0856d2b42"></span> `Terrain` | Target assumed to be at the terrain level sensed by the distance sensor. 
<span id="structmavsdk_1_1_follow_me_1_1_config_1a25f3b39bc6d0eda07891537478bed85ba3ee2c99c1564fda419307854d3417ac7"></span> `TargetGps` | Target GPS altitude taken into account to do 3D tracking. 

## Field Documentation


### follow_height_m {#structmavsdk_1_1_follow_me_1_1_config_1a221d2136ba04ffc0e46cd17f992f0d86}

```cpp
float mavsdk::FollowMe::Config::follow_height_m { 8.0f}
```


[m] Follow height in meters (recommended minimum 8 meters)


### follow_distance_m {#structmavsdk_1_1_follow_me_1_1_config_1a2f1407099516da129f650875e676a7eb}

```cpp
float mavsdk::FollowMe::Config::follow_distance_m {8.0f}
```


[m] Follow distance to target in meters (recommended minimum 4 meter)


### responsiveness {#structmavsdk_1_1_follow_me_1_1_config_1a3b0bced94506c4248cbaa9cc0894616a}

```cpp
float mavsdk::FollowMe::Config::responsiveness {0.1f}
```


How responsive the vehicle is to the motion of the target, Lower value = More responsive (range 0.0 to 1.0)


### altitude_mode {#structmavsdk_1_1_follow_me_1_1_config_1a70d86547c8f23e1fbd2b66632b484dcd}

```cpp
FollowAltitudeMode mavsdk::FollowMe::Config::altitude_mode {}
```


Follow Altitude control mode.


### max_tangential_vel_m_s {#structmavsdk_1_1_follow_me_1_1_config_1aa466aeef1806c3053a9558c9e1ebf8b9}

```cpp
float mavsdk::FollowMe::Config::max_tangential_vel_m_s { 8.0f}
```


[m/s] Maximum orbit tangential velocity relative to the target, in meters per second. Higher value = More aggressive follow angle tracking.


### follow_angle_deg {#structmavsdk_1_1_follow_me_1_1_config_1a15d09482cd57a3d4a51ce9896ac37143}

```cpp
float mavsdk::FollowMe::Config::follow_angle_deg { 180.0f}
```


[deg] Follow Angle relative to the target. 0 equals following in front of the target's direction. Angle increases in Clockwise direction, so following from right would be 90 degrees, from the left is -90 degrees, and so on.

