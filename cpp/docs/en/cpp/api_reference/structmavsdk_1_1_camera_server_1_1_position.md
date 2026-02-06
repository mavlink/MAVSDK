# mavsdk::CameraServer::Position Struct Reference
`#include: camera_server.h`

----


[Position](structmavsdk_1_1_camera_server_1_1_position.md) type in global coordinates. 


## Data Fields


double [latitude_deg](#structmavsdk_1_1_camera_server_1_1_position_1a879741c938a78d8e6feceb820e070205) {} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_camera_server_1_1_position_1a88229f67ee10f7fc8afae89ff7bda666) {} - Longitude in degrees (range: -180 to +180)

float [absolute_altitude_m](#structmavsdk_1_1_camera_server_1_1_position_1a105d7b07865698601d31c13f02447ed8) {} - Altitude AMSL (above mean sea level) in metres.

float [relative_altitude_m](#structmavsdk_1_1_camera_server_1_1_position_1a1c512c78274de0d7ca7ce4f7ca46c507) {} - Altitude relative to takeoff altitude in metres.


## Field Documentation


### latitude_deg {#structmavsdk_1_1_camera_server_1_1_position_1a879741c938a78d8e6feceb820e070205}

```cpp
double mavsdk::CameraServer::Position::latitude_deg {}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_camera_server_1_1_position_1a88229f67ee10f7fc8afae89ff7bda666}

```cpp
double mavsdk::CameraServer::Position::longitude_deg {}
```


Longitude in degrees (range: -180 to +180)


### absolute_altitude_m {#structmavsdk_1_1_camera_server_1_1_position_1a105d7b07865698601d31c13f02447ed8}

```cpp
float mavsdk::CameraServer::Position::absolute_altitude_m {}
```


Altitude AMSL (above mean sea level) in metres.


### relative_altitude_m {#structmavsdk_1_1_camera_server_1_1_position_1a1c512c78274de0d7ca7ce4f7ca46c507}

```cpp
float mavsdk::CameraServer::Position::relative_altitude_m {}
```


Altitude relative to takeoff altitude in metres.

