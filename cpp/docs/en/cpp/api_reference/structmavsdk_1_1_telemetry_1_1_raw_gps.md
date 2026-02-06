# mavsdk::Telemetry::RawGps Struct Reference
`#include: telemetry.h`

----


Raw GPS information type. 


Warning: this is an advanced type! If you want the location of the drone, use the position instead. This message exposes the raw values of the GNSS sensor. 


## Data Fields


uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_raw_gps_1ad04b6e80ae5c54d7294d96406a2494b5) {} - Timestamp in microseconds (UNIX Epoch time or time since system boot, to be inferred)

double [latitude_deg](#structmavsdk_1_1_telemetry_1_1_raw_gps_1af616f2c868d1ff58c74842c30311af08) {} - Latitude in degrees (WGS84, EGM96 ellipsoid)

double [longitude_deg](#structmavsdk_1_1_telemetry_1_1_raw_gps_1a0951aaa70074f09f3bbd89654d778e35) {} - Longitude in degrees (WGS84, EGM96 ellipsoid)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_1_1_raw_gps_1aa935c918b44775a355c73d192d8ec864) {} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in metres.

float [hdop](#structmavsdk_1_1_telemetry_1_1_raw_gps_1abbbc803f5a59a201865204d77e3cf127) {} - GPS HDOP horizontal dilution of position (unitless). If unknown, set to NaN.

float [vdop](#structmavsdk_1_1_telemetry_1_1_raw_gps_1adb4779f0ae7583a2600ce04d968bf215) {} - GPS VDOP vertical dilution of position (unitless). If unknown, set to NaN.

float [velocity_m_s](#structmavsdk_1_1_telemetry_1_1_raw_gps_1a9f11f0838763d71c7f7b5e4fbeabf161) {} - Ground velocity in metres per second.

float [cog_deg](#structmavsdk_1_1_telemetry_1_1_raw_gps_1afd8dfadafbd6c5d8f683621b3fe905c8) {} - Course over ground (NOT heading, but direction of movement) in degrees. If unknown, set to NaN.

float [altitude_ellipsoid_m](#structmavsdk_1_1_telemetry_1_1_raw_gps_1a952184f0e2c43c7ecf3a65540974f38a) {} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) in metres (above WGS84, EGM96 ellipsoid)

float [horizontal_uncertainty_m](#structmavsdk_1_1_telemetry_1_1_raw_gps_1a2ab8e5415a6f74a9e41a410e45b3c424) {} - [Position](structmavsdk_1_1_telemetry_1_1_position.md) uncertainty in metres.

float [vertical_uncertainty_m](#structmavsdk_1_1_telemetry_1_1_raw_gps_1ac9b725d01e86be7db6d6f7e57956a885) {} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) uncertainty in metres.

float [velocity_uncertainty_m_s](#structmavsdk_1_1_telemetry_1_1_raw_gps_1a5191bc0046118664dd928f0338dadfe1) {} - Velocity uncertainty in metres per second.

float [heading_uncertainty_deg](#structmavsdk_1_1_telemetry_1_1_raw_gps_1af1dd338be444666f9dadc3393176a015) {} - [Heading](structmavsdk_1_1_telemetry_1_1_heading.md) uncertainty in degrees.

float [yaw_deg](#structmavsdk_1_1_telemetry_1_1_raw_gps_1a01c83a26d734e47e257492fdc574e45d) {} - Yaw in earth frame from north.


## Field Documentation


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_raw_gps_1ad04b6e80ae5c54d7294d96406a2494b5}

```cpp
uint64_t mavsdk::Telemetry::RawGps::timestamp_us {}
```


Timestamp in microseconds (UNIX Epoch time or time since system boot, to be inferred)


### latitude_deg {#structmavsdk_1_1_telemetry_1_1_raw_gps_1af616f2c868d1ff58c74842c30311af08}

```cpp
double mavsdk::Telemetry::RawGps::latitude_deg {}
```


Latitude in degrees (WGS84, EGM96 ellipsoid)


### longitude_deg {#structmavsdk_1_1_telemetry_1_1_raw_gps_1a0951aaa70074f09f3bbd89654d778e35}

```cpp
double mavsdk::Telemetry::RawGps::longitude_deg {}
```


Longitude in degrees (WGS84, EGM96 ellipsoid)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_1_1_raw_gps_1aa935c918b44775a355c73d192d8ec864}

```cpp
float mavsdk::Telemetry::RawGps::absolute_altitude_m {}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in metres.


### hdop {#structmavsdk_1_1_telemetry_1_1_raw_gps_1abbbc803f5a59a201865204d77e3cf127}

```cpp
float mavsdk::Telemetry::RawGps::hdop {}
```


GPS HDOP horizontal dilution of position (unitless). If unknown, set to NaN.


### vdop {#structmavsdk_1_1_telemetry_1_1_raw_gps_1adb4779f0ae7583a2600ce04d968bf215}

```cpp
float mavsdk::Telemetry::RawGps::vdop {}
```


GPS VDOP vertical dilution of position (unitless). If unknown, set to NaN.


### velocity_m_s {#structmavsdk_1_1_telemetry_1_1_raw_gps_1a9f11f0838763d71c7f7b5e4fbeabf161}

```cpp
float mavsdk::Telemetry::RawGps::velocity_m_s {}
```


Ground velocity in metres per second.


### cog_deg {#structmavsdk_1_1_telemetry_1_1_raw_gps_1afd8dfadafbd6c5d8f683621b3fe905c8}

```cpp
float mavsdk::Telemetry::RawGps::cog_deg {}
```


Course over ground (NOT heading, but direction of movement) in degrees. If unknown, set to NaN.


### altitude_ellipsoid_m {#structmavsdk_1_1_telemetry_1_1_raw_gps_1a952184f0e2c43c7ecf3a65540974f38a}

```cpp
float mavsdk::Telemetry::RawGps::altitude_ellipsoid_m {}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) in metres (above WGS84, EGM96 ellipsoid)


### horizontal_uncertainty_m {#structmavsdk_1_1_telemetry_1_1_raw_gps_1a2ab8e5415a6f74a9e41a410e45b3c424}

```cpp
float mavsdk::Telemetry::RawGps::horizontal_uncertainty_m {}
```


[Position](structmavsdk_1_1_telemetry_1_1_position.md) uncertainty in metres.


### vertical_uncertainty_m {#structmavsdk_1_1_telemetry_1_1_raw_gps_1ac9b725d01e86be7db6d6f7e57956a885}

```cpp
float mavsdk::Telemetry::RawGps::vertical_uncertainty_m {}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) uncertainty in metres.


### velocity_uncertainty_m_s {#structmavsdk_1_1_telemetry_1_1_raw_gps_1a5191bc0046118664dd928f0338dadfe1}

```cpp
float mavsdk::Telemetry::RawGps::velocity_uncertainty_m_s {}
```


Velocity uncertainty in metres per second.


### heading_uncertainty_deg {#structmavsdk_1_1_telemetry_1_1_raw_gps_1af1dd338be444666f9dadc3393176a015}

```cpp
float mavsdk::Telemetry::RawGps::heading_uncertainty_deg {}
```


[Heading](structmavsdk_1_1_telemetry_1_1_heading.md) uncertainty in degrees.


### yaw_deg {#structmavsdk_1_1_telemetry_1_1_raw_gps_1a01c83a26d734e47e257492fdc574e45d}

```cpp
float mavsdk::Telemetry::RawGps::yaw_deg {}
```


Yaw in earth frame from north.

