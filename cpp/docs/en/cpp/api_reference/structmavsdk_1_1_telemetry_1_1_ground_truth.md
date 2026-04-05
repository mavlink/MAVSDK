# mavsdk::Telemetry::GroundTruth Struct Reference
`#include: telemetry.hpp`

----


[GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md) message type. 


## Data Fields


double [latitude_deg](#structmavsdk_1_1_telemetry_1_1_ground_truth_1a6eba659dc09388c324077c1c0dca2de4) {double(NAN)} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_telemetry_1_1_ground_truth_1abf02a4edee18e077b939fb8f6b919ae3) {double(NAN)} - Longitude in degrees (range: -180 to 180)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_1_1_ground_truth_1a6f2b0c73e4e737b7f684b5d426c848fa) {float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in metres.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_ground_truth_1a9b2e727f39afc9f787891e3e0cf3d87d) {} - Timestamp in microseconds (since system boot)


## Field Documentation


### latitude_deg {#structmavsdk_1_1_telemetry_1_1_ground_truth_1a6eba659dc09388c324077c1c0dca2de4}

```cpp
double mavsdk::Telemetry::GroundTruth::latitude_deg {double(NAN)}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_telemetry_1_1_ground_truth_1abf02a4edee18e077b939fb8f6b919ae3}

```cpp
double mavsdk::Telemetry::GroundTruth::longitude_deg {double(NAN)}
```


Longitude in degrees (range: -180 to 180)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_1_1_ground_truth_1a6f2b0c73e4e737b7f684b5d426c848fa}

```cpp
float mavsdk::Telemetry::GroundTruth::absolute_altitude_m {float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in metres.


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_ground_truth_1a9b2e727f39afc9f787891e3e0cf3d87d}

```cpp
uint64_t mavsdk::Telemetry::GroundTruth::timestamp_us {}
```


Timestamp in microseconds (since system boot)

