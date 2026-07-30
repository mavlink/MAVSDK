# mavsdk::Telemetry::HomePosition Struct Reference
`#include: telemetry.hpp`

----


Home position type. 


Includes the global GPS position, local NED position, surface quaternion, and approach vector from the MAVLink HOME_POSITION message. 


## Data Fields


uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_home_position_1a18b800aacb7b11b6ae921cbb5110d895) {} - Timestamp (UNIX Epoch or since system boot) in microseconds.

double [latitude_deg](#structmavsdk_1_1_telemetry_1_1_home_position_1a48885024514a93cba0e4f66254ab8b86) {double(NAN)} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_telemetry_1_1_home_position_1ad0b92f7470ef58559bec8413551a3a89) {double(NAN)} - Longitude in degrees (range: -180 to +180)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_1_1_home_position_1ae7cf0d5626a6320adbb7a30c2e663e86) {float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in metres.

float [relative_altitude_m](#structmavsdk_1_1_telemetry_1_1_home_position_1a14d7f4229671cc0930a79508714a04e6) {float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) relative to takeoff altitude in metres.

float [local_north_m](#structmavsdk_1_1_telemetry_1_1_home_position_1a561fed7a952f8576f5433867d1e46296) {float(NAN)} - Local North position in NED frame (m)

float [local_east_m](#structmavsdk_1_1_telemetry_1_1_home_position_1ac80ecec85e247172eeaf69c24453535c) {float(NAN)} - Local East position in NED frame (m)

float [local_down_m](#structmavsdk_1_1_telemetry_1_1_home_position_1ab3032281ac75d85375f96fe6cf538468) {float(NAN)} - Local Down position in NED frame (m, positive down)

[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) [q](#structmavsdk_1_1_telemetry_1_1_home_position_1a7a65993c008b12bc6bfd2edb261bac8f) {} - Surface quaternion (world-to-surface-normal and heading)

float [approach_north_m](#structmavsdk_1_1_telemetry_1_1_home_position_1aa11149df80be3ba3264558e0e2eca607) {float(NAN)} - Local North position of the approach vector end in NED frame (m)

float [approach_east_m](#structmavsdk_1_1_telemetry_1_1_home_position_1a7298df07a879ecdc1c07050c7b7b014b) {float(NAN)} - Local East position of the approach vector end in NED frame (m)

float [approach_down_m](#structmavsdk_1_1_telemetry_1_1_home_position_1ae0903521bd0c116cbdfc54a60d8a8578) {float(NAN)} - Local Down position of the approach vector end in NED frame (m)


## Field Documentation


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_home_position_1a18b800aacb7b11b6ae921cbb5110d895}

```cpp
uint64_t mavsdk::Telemetry::HomePosition::timestamp_us {}
```


Timestamp (UNIX Epoch or since system boot) in microseconds.


### latitude_deg {#structmavsdk_1_1_telemetry_1_1_home_position_1a48885024514a93cba0e4f66254ab8b86}

```cpp
double mavsdk::Telemetry::HomePosition::latitude_deg {double(NAN)}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_telemetry_1_1_home_position_1ad0b92f7470ef58559bec8413551a3a89}

```cpp
double mavsdk::Telemetry::HomePosition::longitude_deg {double(NAN)}
```


Longitude in degrees (range: -180 to +180)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_1_1_home_position_1ae7cf0d5626a6320adbb7a30c2e663e86}

```cpp
float mavsdk::Telemetry::HomePosition::absolute_altitude_m {float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in metres.


### relative_altitude_m {#structmavsdk_1_1_telemetry_1_1_home_position_1a14d7f4229671cc0930a79508714a04e6}

```cpp
float mavsdk::Telemetry::HomePosition::relative_altitude_m {float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) relative to takeoff altitude in metres.


### local_north_m {#structmavsdk_1_1_telemetry_1_1_home_position_1a561fed7a952f8576f5433867d1e46296}

```cpp
float mavsdk::Telemetry::HomePosition::local_north_m {float(NAN)}
```


Local North position in NED frame (m)


### local_east_m {#structmavsdk_1_1_telemetry_1_1_home_position_1ac80ecec85e247172eeaf69c24453535c}

```cpp
float mavsdk::Telemetry::HomePosition::local_east_m {float(NAN)}
```


Local East position in NED frame (m)


### local_down_m {#structmavsdk_1_1_telemetry_1_1_home_position_1ab3032281ac75d85375f96fe6cf538468}

```cpp
float mavsdk::Telemetry::HomePosition::local_down_m {float(NAN)}
```


Local Down position in NED frame (m, positive down)


### q {#structmavsdk_1_1_telemetry_1_1_home_position_1a7a65993c008b12bc6bfd2edb261bac8f}

```cpp
Quaternion mavsdk::Telemetry::HomePosition::q {}
```


Surface quaternion (world-to-surface-normal and heading)


### approach_north_m {#structmavsdk_1_1_telemetry_1_1_home_position_1aa11149df80be3ba3264558e0e2eca607}

```cpp
float mavsdk::Telemetry::HomePosition::approach_north_m {float(NAN)}
```


Local North position of the approach vector end in NED frame (m)


### approach_east_m {#structmavsdk_1_1_telemetry_1_1_home_position_1a7298df07a879ecdc1c07050c7b7b014b}

```cpp
float mavsdk::Telemetry::HomePosition::approach_east_m {float(NAN)}
```


Local East position of the approach vector end in NED frame (m)


### approach_down_m {#structmavsdk_1_1_telemetry_1_1_home_position_1ae0903521bd0c116cbdfc54a60d8a8578}

```cpp
float mavsdk::Telemetry::HomePosition::approach_down_m {float(NAN)}
```


Local Down position of the approach vector end in NED frame (m)

