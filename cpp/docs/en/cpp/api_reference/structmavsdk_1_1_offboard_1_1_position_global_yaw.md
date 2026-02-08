# mavsdk::Offboard::PositionGlobalYaw Struct Reference
`#include: offboard.h`

----


Type for position commands in Global (Latitude, Longitude, Altitude) coordinates and yaw. 


## Public Types


Type | Description
--- | ---
enum [AltitudeType](#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4aa4d0ec8118f24ecc968c6fbfc0383e) | Possible altitude options.

## Data Fields


double [lat_deg](#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a403f9dd320b8f78f02814480f9fe3f41) {} - Latitude (in degrees)

double [lon_deg](#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a863f22ca8854073bf70b8b1208c44bfb) {} - Longitude (in degrees)

float [alt_m](#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a64fa1a81a97035da316f1d1d76fb7728) {} - altitude (in metres)

float [yaw_deg](#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4e9dcedc454807a90d3adcced041def9) {} - Yaw in degrees (0 North, positive is clock-wise looking from above)

[AltitudeType](structmavsdk_1_1_offboard_1_1_position_global_yaw.md#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4aa4d0ec8118f24ecc968c6fbfc0383e) [altitude_type](#structmavsdk_1_1_offboard_1_1_position_global_yaw_1ad96893eb8e298b6af25eb7f0755cb03a) {} - altitude type for this position


## Member Enumeration Documentation


### enum AltitudeType {#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4aa4d0ec8118f24ecc968c6fbfc0383e}


Possible altitude options.


Value | Description
--- | ---
<span id="structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4aa4d0ec8118f24ecc968c6fbfc0383eaedad9c0f4320a5340b7d31f696aa3f34"></span> `RelHome` | Altitude relative to the Home position. 
<span id="structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4aa4d0ec8118f24ecc968c6fbfc0383ea599c0521b829e92ef2dff137e0db9ff9"></span> `Amsl` | Altitude above mean sea level (AMSL). 
<span id="structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4aa4d0ec8118f24ecc968c6fbfc0383eabed872c811a88d00b9aa2aa16156c57b"></span> `Agl` | Altitude above ground level (AGL). 

## Field Documentation


### lat_deg {#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a403f9dd320b8f78f02814480f9fe3f41}

```cpp
double mavsdk::Offboard::PositionGlobalYaw::lat_deg {}
```


Latitude (in degrees)


### lon_deg {#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a863f22ca8854073bf70b8b1208c44bfb}

```cpp
double mavsdk::Offboard::PositionGlobalYaw::lon_deg {}
```


Longitude (in degrees)


### alt_m {#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a64fa1a81a97035da316f1d1d76fb7728}

```cpp
float mavsdk::Offboard::PositionGlobalYaw::alt_m {}
```


altitude (in metres)


### yaw_deg {#structmavsdk_1_1_offboard_1_1_position_global_yaw_1a4e9dcedc454807a90d3adcced041def9}

```cpp
float mavsdk::Offboard::PositionGlobalYaw::yaw_deg {}
```


Yaw in degrees (0 North, positive is clock-wise looking from above)


### altitude_type {#structmavsdk_1_1_offboard_1_1_position_global_yaw_1ad96893eb8e298b6af25eb7f0755cb03a}

```cpp
AltitudeType mavsdk::Offboard::PositionGlobalYaw::altitude_type {}
```


altitude type for this position

