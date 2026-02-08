# mavsdk::Camera::Position Struct Reference
`#include: camera.h`

----


[Position](structmavsdk_1_1_camera_1_1_position.md) type in global coordinates. 


## Data Fields


double [latitude_deg](#structmavsdk_1_1_camera_1_1_position_1a6f8941d78449d5228fa43fe8c1182317) {} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_camera_1_1_position_1a7b464ed04437649533787e1442400264) {} - Longitude in degrees (range: -180 to +180)

float [absolute_altitude_m](#structmavsdk_1_1_camera_1_1_position_1a543460f2a6039e6be3a4dfe50fd3b2d0) {} - Altitude AMSL (above mean sea level) in metres.

float [relative_altitude_m](#structmavsdk_1_1_camera_1_1_position_1a07b0b2baef1707c4a99eaa29cd3912dd) {} - Altitude relative to takeoff altitude in metres.


## Field Documentation


### latitude_deg {#structmavsdk_1_1_camera_1_1_position_1a6f8941d78449d5228fa43fe8c1182317}

```cpp
double mavsdk::Camera::Position::latitude_deg {}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_camera_1_1_position_1a7b464ed04437649533787e1442400264}

```cpp
double mavsdk::Camera::Position::longitude_deg {}
```


Longitude in degrees (range: -180 to +180)


### absolute_altitude_m {#structmavsdk_1_1_camera_1_1_position_1a543460f2a6039e6be3a4dfe50fd3b2d0}

```cpp
float mavsdk::Camera::Position::absolute_altitude_m {}
```


Altitude AMSL (above mean sea level) in metres.


### relative_altitude_m {#structmavsdk_1_1_camera_1_1_position_1a07b0b2baef1707c4a99eaa29cd3912dd}

```cpp
float mavsdk::Camera::Position::relative_altitude_m {}
```


Altitude relative to takeoff altitude in metres.

