# mavsdk::TelemetryServer::RawGps Struct Reference
`#include: telemetry_server.h`

----


Raw GPS information type. 


Warning: this is an advanced type! If you want the location of the drone, use the position instead. This message exposes the raw values of the GNSS sensor. 


## Data Fields


uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a7b0063601df79e5d70da522adbcca5aa) {} - Timestamp in microseconds (UNIX Epoch time or time since system boot, to be inferred)

double [latitude_deg](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ad6f274fcb87f3b13a25343975354cbd0) {} - Latitude in degrees (WGS84, EGM96 ellipsoid)

double [longitude_deg](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a7762ac5c26d315a873141082606d0bf5) {} - Longitude in degrees (WGS84, EGM96 ellipsoid)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a5f5a72f832923a804fbc75bca9b552d9) {} - Altitude AMSL (above mean sea level) in metres.

float [hdop](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ab6e9c5f46d04b33465d5f728a0b83683) {} - GPS HDOP horizontal dilution of position (unitless). If unknown, set to NaN.

float [vdop](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a293888940dad82a0367c60c8a22816ee) {} - GPS VDOP vertical dilution of position (unitless). If unknown, set to NaN.

float [velocity_m_s](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a9122eb8b3e4697f6c40a5b9f23d2213e) {} - Ground velocity in metres per second.

float [cog_deg](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a33e06571290f518551ef9f4463b9d6f0) {} - Course over ground (NOT heading, but direction of movement) in degrees. If unknown, set to NaN.

float [altitude_ellipsoid_m](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a034d03a361938df870409af81a741257) {} - Altitude in metres (above WGS84, EGM96 ellipsoid)

float [horizontal_uncertainty_m](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ae7ce24396bab295f4fed7d2db6883e3e) {} - [Position](structmavsdk_1_1_telemetry_server_1_1_position.md) uncertainty in metres.

float [vertical_uncertainty_m](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1abe7a51108c9be02e9235b7f8fb89c1c8) {} - Altitude uncertainty in metres.

float [velocity_uncertainty_m_s](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1af38084cdd3e3d61f59f4ef8d6c43fc2e) {} - Velocity uncertainty in metres per second.

float [heading_uncertainty_deg](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ab5cba0147ec920f676fef753f30e5ce7) {} - [Heading](structmavsdk_1_1_telemetry_server_1_1_heading.md) uncertainty in degrees.

float [yaw_deg](#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a93520d189d39f93d89bf9a84cd688d24) {} - Yaw in earth frame from north.


## Field Documentation


### timestamp_us {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a7b0063601df79e5d70da522adbcca5aa}

```cpp
uint64_t mavsdk::TelemetryServer::RawGps::timestamp_us {}
```


Timestamp in microseconds (UNIX Epoch time or time since system boot, to be inferred)


### latitude_deg {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ad6f274fcb87f3b13a25343975354cbd0}

```cpp
double mavsdk::TelemetryServer::RawGps::latitude_deg {}
```


Latitude in degrees (WGS84, EGM96 ellipsoid)


### longitude_deg {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a7762ac5c26d315a873141082606d0bf5}

```cpp
double mavsdk::TelemetryServer::RawGps::longitude_deg {}
```


Longitude in degrees (WGS84, EGM96 ellipsoid)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a5f5a72f832923a804fbc75bca9b552d9}

```cpp
float mavsdk::TelemetryServer::RawGps::absolute_altitude_m {}
```


Altitude AMSL (above mean sea level) in metres.


### hdop {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ab6e9c5f46d04b33465d5f728a0b83683}

```cpp
float mavsdk::TelemetryServer::RawGps::hdop {}
```


GPS HDOP horizontal dilution of position (unitless). If unknown, set to NaN.


### vdop {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a293888940dad82a0367c60c8a22816ee}

```cpp
float mavsdk::TelemetryServer::RawGps::vdop {}
```


GPS VDOP vertical dilution of position (unitless). If unknown, set to NaN.


### velocity_m_s {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a9122eb8b3e4697f6c40a5b9f23d2213e}

```cpp
float mavsdk::TelemetryServer::RawGps::velocity_m_s {}
```


Ground velocity in metres per second.


### cog_deg {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a33e06571290f518551ef9f4463b9d6f0}

```cpp
float mavsdk::TelemetryServer::RawGps::cog_deg {}
```


Course over ground (NOT heading, but direction of movement) in degrees. If unknown, set to NaN.


### altitude_ellipsoid_m {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a034d03a361938df870409af81a741257}

```cpp
float mavsdk::TelemetryServer::RawGps::altitude_ellipsoid_m {}
```


Altitude in metres (above WGS84, EGM96 ellipsoid)


### horizontal_uncertainty_m {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ae7ce24396bab295f4fed7d2db6883e3e}

```cpp
float mavsdk::TelemetryServer::RawGps::horizontal_uncertainty_m {}
```


[Position](structmavsdk_1_1_telemetry_server_1_1_position.md) uncertainty in metres.


### vertical_uncertainty_m {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1abe7a51108c9be02e9235b7f8fb89c1c8}

```cpp
float mavsdk::TelemetryServer::RawGps::vertical_uncertainty_m {}
```


Altitude uncertainty in metres.


### velocity_uncertainty_m_s {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1af38084cdd3e3d61f59f4ef8d6c43fc2e}

```cpp
float mavsdk::TelemetryServer::RawGps::velocity_uncertainty_m_s {}
```


Velocity uncertainty in metres per second.


### heading_uncertainty_deg {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1ab5cba0147ec920f676fef753f30e5ce7}

```cpp
float mavsdk::TelemetryServer::RawGps::heading_uncertainty_deg {}
```


[Heading](structmavsdk_1_1_telemetry_server_1_1_heading.md) uncertainty in degrees.


### yaw_deg {#structmavsdk_1_1_telemetry_server_1_1_raw_gps_1a93520d189d39f93d89bf9a84cd688d24}

```cpp
float mavsdk::TelemetryServer::RawGps::yaw_deg {}
```


Yaw in earth frame from north.

