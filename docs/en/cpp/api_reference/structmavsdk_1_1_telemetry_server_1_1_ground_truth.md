# mavsdk::TelemetryServer::GroundTruth Struct Reference
`#include: telemetry_server.hpp`

----


[GroundTruth](structmavsdk_1_1_telemetry_server_1_1_ground_truth.md) message type. 


## Data Fields


double [latitude_deg](#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1a30e8fda7bbf72e6d891a3ac7a559062d) {double(NAN)} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1ac56635344c06e62105aaaadbd302bd87) {double(NAN)} - Longitude in degrees (range: -180 to 180)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1a50114cbbdf521bdd162a524e9978ca77) {float(NAN)} - Altitude AMSL (above mean sea level) in metres.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1afd45c27cbebbc8ed59658a10ddb81a56) {} - Timestamp in microseconds (since system boot)


## Field Documentation


### latitude_deg {#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1a30e8fda7bbf72e6d891a3ac7a559062d}

```cpp
double mavsdk::TelemetryServer::GroundTruth::latitude_deg {double(NAN)}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1ac56635344c06e62105aaaadbd302bd87}

```cpp
double mavsdk::TelemetryServer::GroundTruth::longitude_deg {double(NAN)}
```


Longitude in degrees (range: -180 to 180)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1a50114cbbdf521bdd162a524e9978ca77}

```cpp
float mavsdk::TelemetryServer::GroundTruth::absolute_altitude_m {float(NAN)}
```


Altitude AMSL (above mean sea level) in metres.


### timestamp_us {#structmavsdk_1_1_telemetry_server_1_1_ground_truth_1afd45c27cbebbc8ed59658a10ddb81a56}

```cpp
uint64_t mavsdk::TelemetryServer::GroundTruth::timestamp_us {}
```


Timestamp in microseconds (since system boot)

