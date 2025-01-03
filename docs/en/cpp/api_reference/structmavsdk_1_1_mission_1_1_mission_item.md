# mavsdk::Mission::MissionItem Struct Reference
`#include: mission.h`

----


Type representing a mission item. 


A [MissionItem](structmavsdk_1_1_mission_1_1_mission_item.md) can contain a position and/or actions. [Mission](classmavsdk_1_1_mission.md) items are building blocks to assemble a mission, which can be sent to (or received from) a system. They cannot be used independently. 


## Public Types


Type | Description
--- | ---
enum [CameraAction](#structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4) | Possible camera actions at a mission item.
enum [VehicleAction](#structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743) | Possible vehicle actions at a mission item.

## Data Fields


double [latitude_deg](#structmavsdk_1_1_mission_1_1_mission_item_1af39e7086391121ace640feea0bfdd289) {double(NAN)} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_mission_1_1_mission_item_1afecfad42d0976a85842cc9489694c062) {double(NAN)} - Longitude in degrees (range: -180 to +180)

float [relative_altitude_m](#structmavsdk_1_1_mission_1_1_mission_item_1aeb6371f7a6fe5f81f96d4826db550602) { float(NAN)} - Altitude relative to takeoff altitude in metres.

float [speed_m_s](#structmavsdk_1_1_mission_1_1_mission_item_1ab48b7ba0f5a85056ac516585f8df9729) { float(NAN)} - Speed to use after this mission item (in metres/second)

bool [is_fly_through](#structmavsdk_1_1_mission_1_1_mission_item_1ac01a5fd31bee531537ec87658a4a3561) { false} - True will make the drone fly through without stopping, while false will make the drone stop on the waypoint.

float [gimbal_pitch_deg](#structmavsdk_1_1_mission_1_1_mission_item_1af76dd7536ffd1fcad98c4e08fe33c791) {float(NAN)} - [Gimbal](classmavsdk_1_1_gimbal.md) pitch (in degrees)

float [gimbal_yaw_deg](#structmavsdk_1_1_mission_1_1_mission_item_1a6c0cf351a535a5e77b8079e5d532974b) {float(NAN)} - [Gimbal](classmavsdk_1_1_gimbal.md) yaw (in degrees)

[CameraAction](structmavsdk_1_1_mission_1_1_mission_item.md#structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4) [camera_action](#structmavsdk_1_1_mission_1_1_mission_item_1a2f38fdb200201180ab370d811bb21f2f) {} - [Camera](classmavsdk_1_1_camera.md) action to trigger at this mission item.

float [loiter_time_s](#structmavsdk_1_1_mission_1_1_mission_item_1aa54196266e57f2a05ab46aa41fce1dd0) {float(NAN)} - Loiter time (in seconds)

double [camera_photo_interval_s](#structmavsdk_1_1_mission_1_1_mission_item_1a7b48c46ecb52f115e5461a944f619612) { 1.0} - [Camera](classmavsdk_1_1_camera.md) photo interval to use after this mission item (in seconds)

float [acceptance_radius_m](#structmavsdk_1_1_mission_1_1_mission_item_1a9114a16c5405dacb35f45ae37f343665) { float(NAN)} - Radius for completing a mission item (in metres)

float [yaw_deg](#structmavsdk_1_1_mission_1_1_mission_item_1a40af7ff4f2a346492d276255febd92c7) {float(NAN)} - Absolute yaw angle (in degrees)

float [camera_photo_distance_m](#structmavsdk_1_1_mission_1_1_mission_item_1a0a2802a1cab28552e8f49389f472e1a9) { NAN} - [Camera](classmavsdk_1_1_camera.md) photo distance to use after this mission item (in meters)

[VehicleAction](structmavsdk_1_1_mission_1_1_mission_item.md#structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743) [vehicle_action](#structmavsdk_1_1_mission_1_1_mission_item_1a0892ef1c7dd89ea18535689f69d5493a) {} - Vehicle action to trigger at this mission item.


## Member Enumeration Documentation


### enum CameraAction {#structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4}


Possible camera actions at a mission item.


Value | Description
--- | ---
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a6adf97f83acf6453d4a6a4b1070f3754"></span> `None` | No action. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a32e4460786600f3883452550664c13af"></span> `TakePhoto` | Take a single photo. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a7cd902daf19f053b0078caf0fb2977c3"></span> `StartPhotoInterval` | Start capturing photos at regular intervals. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4aa5ca4f2a945ea616f186b130f8f36a18"></span> `StopPhotoInterval` | Stop capturing photos at regular intervals. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a7a9102c91e389c449d24a3ca0d8bdf4f"></span> `StartVideo` | Start capturing video. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a3ab23501110c4bf07668c2178168da01"></span> `StopVideo` | Stop capturing video. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a9aabe55729b070dbba61b3b7e9e8faa3"></span> `StartPhotoDistance` | Start capturing photos at regular distance. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1a0299fbbe7c7b03bc43eb116f96b48df4a3e66812860fd78f0707062e31b98cd1e"></span> `StopPhotoDistance` | Stop capturing photos at regular distance. 

### enum VehicleAction {#structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743}


Possible vehicle actions at a mission item.


Value | Description
--- | ---
<span id="structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743a6adf97f83acf6453d4a6a4b1070f3754"></span> `None` | No action. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743a56373a80447c41b9a29e500e62d6884e"></span> `Takeoff` | Vehicle will takeoff and go to defined waypoint. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743a512ef7c688a2c8572d5e16f44e17e869"></span> `Land` | When a waypoint is reached vehicle will land at current position. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743a45f439a04f9144adc1dcf5a4fbda0026"></span> `TransitionToFw` | When a waypoint is reached vehicle will transition to fixed-wing mode. 
<span id="structmavsdk_1_1_mission_1_1_mission_item_1abf579024f942deef18ef902839513743af3abadebd9602a0acb33350f64fa7661"></span> `TransitionToMc` | When a waypoint is reached vehicle will transition to multi-copter mode. 

## Field Documentation


### latitude_deg {#structmavsdk_1_1_mission_1_1_mission_item_1af39e7086391121ace640feea0bfdd289}

```cpp
double mavsdk::Mission::MissionItem::latitude_deg {double(NAN)}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_mission_1_1_mission_item_1afecfad42d0976a85842cc9489694c062}

```cpp
double mavsdk::Mission::MissionItem::longitude_deg {double(NAN)}
```


Longitude in degrees (range: -180 to +180)


### relative_altitude_m {#structmavsdk_1_1_mission_1_1_mission_item_1aeb6371f7a6fe5f81f96d4826db550602}

```cpp
float mavsdk::Mission::MissionItem::relative_altitude_m { float(NAN)}
```


Altitude relative to takeoff altitude in metres.


### speed_m_s {#structmavsdk_1_1_mission_1_1_mission_item_1ab48b7ba0f5a85056ac516585f8df9729}

```cpp
float mavsdk::Mission::MissionItem::speed_m_s { float(NAN)}
```


Speed to use after this mission item (in metres/second)


### is_fly_through {#structmavsdk_1_1_mission_1_1_mission_item_1ac01a5fd31bee531537ec87658a4a3561}

```cpp
bool mavsdk::Mission::MissionItem::is_fly_through { false}
```


True will make the drone fly through without stopping, while false will make the drone stop on the waypoint.


### gimbal_pitch_deg {#structmavsdk_1_1_mission_1_1_mission_item_1af76dd7536ffd1fcad98c4e08fe33c791}

```cpp
float mavsdk::Mission::MissionItem::gimbal_pitch_deg {float(NAN)}
```


[Gimbal](classmavsdk_1_1_gimbal.md) pitch (in degrees)


### gimbal_yaw_deg {#structmavsdk_1_1_mission_1_1_mission_item_1a6c0cf351a535a5e77b8079e5d532974b}

```cpp
float mavsdk::Mission::MissionItem::gimbal_yaw_deg {float(NAN)}
```


[Gimbal](classmavsdk_1_1_gimbal.md) yaw (in degrees)


### camera_action {#structmavsdk_1_1_mission_1_1_mission_item_1a2f38fdb200201180ab370d811bb21f2f}

```cpp
CameraAction mavsdk::Mission::MissionItem::camera_action {}
```


[Camera](classmavsdk_1_1_camera.md) action to trigger at this mission item.


### loiter_time_s {#structmavsdk_1_1_mission_1_1_mission_item_1aa54196266e57f2a05ab46aa41fce1dd0}

```cpp
float mavsdk::Mission::MissionItem::loiter_time_s {float(NAN)}
```


Loiter time (in seconds)


### camera_photo_interval_s {#structmavsdk_1_1_mission_1_1_mission_item_1a7b48c46ecb52f115e5461a944f619612}

```cpp
double mavsdk::Mission::MissionItem::camera_photo_interval_s { 1.0}
```


[Camera](classmavsdk_1_1_camera.md) photo interval to use after this mission item (in seconds)


### acceptance_radius_m {#structmavsdk_1_1_mission_1_1_mission_item_1a9114a16c5405dacb35f45ae37f343665}

```cpp
float mavsdk::Mission::MissionItem::acceptance_radius_m { float(NAN)}
```


Radius for completing a mission item (in metres)


### yaw_deg {#structmavsdk_1_1_mission_1_1_mission_item_1a40af7ff4f2a346492d276255febd92c7}

```cpp
float mavsdk::Mission::MissionItem::yaw_deg {float(NAN)}
```


Absolute yaw angle (in degrees)


### camera_photo_distance_m {#structmavsdk_1_1_mission_1_1_mission_item_1a0a2802a1cab28552e8f49389f472e1a9}

```cpp
float mavsdk::Mission::MissionItem::camera_photo_distance_m { NAN}
```


[Camera](classmavsdk_1_1_camera.md) photo distance to use after this mission item (in meters)


### vehicle_action {#structmavsdk_1_1_mission_1_1_mission_item_1a0892ef1c7dd89ea18535689f69d5493a}

```cpp
VehicleAction mavsdk::Mission::MissionItem::vehicle_action {}
```


Vehicle action to trigger at this mission item.

