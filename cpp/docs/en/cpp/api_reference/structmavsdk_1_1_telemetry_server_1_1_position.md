# mavsdk::TelemetryServer::Position Struct Reference
`#include: telemetry_server.h`

----


[Position](structmavsdk_1_1_telemetry_server_1_1_position.md) type in global coordinates. 


## Data Fields


double [latitude_deg](#structmavsdk_1_1_telemetry_server_1_1_position_1adfadb6ceb844257ffd9ff7782229687c) {double(NAN)} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_telemetry_server_1_1_position_1a721444c1ec9ce7493c16d67f8be250f4) {double(NAN)} - Longitude in degrees (range: -180 to +180)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_server_1_1_position_1ab965b564087c41013f59f52e4125fb24) { float(NAN)} - Altitude AMSL (above mean sea level) in metres.

float [relative_altitude_m](#structmavsdk_1_1_telemetry_server_1_1_position_1a2def78079e3bae2391606109eca6c0f1) { float(NAN)} - Altitude relative to takeoff altitude in metres.


## Field Documentation


### latitude_deg {#structmavsdk_1_1_telemetry_server_1_1_position_1adfadb6ceb844257ffd9ff7782229687c}

```cpp
double mavsdk::TelemetryServer::Position::latitude_deg {double(NAN)}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_telemetry_server_1_1_position_1a721444c1ec9ce7493c16d67f8be250f4}

```cpp
double mavsdk::TelemetryServer::Position::longitude_deg {double(NAN)}
```


Longitude in degrees (range: -180 to +180)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_server_1_1_position_1ab965b564087c41013f59f52e4125fb24}

```cpp
float mavsdk::TelemetryServer::Position::absolute_altitude_m { float(NAN)}
```


Altitude AMSL (above mean sea level) in metres.


### relative_altitude_m {#structmavsdk_1_1_telemetry_server_1_1_position_1a2def78079e3bae2391606109eca6c0f1}

```cpp
float mavsdk::TelemetryServer::Position::relative_altitude_m { float(NAN)}
```


Altitude relative to takeoff altitude in metres.

