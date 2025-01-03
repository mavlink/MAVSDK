# mavsdk::TelemetryServer::DistanceSensor Struct Reference
`#include: telemetry_server.h`

----


[DistanceSensor](structmavsdk_1_1_telemetry_server_1_1_distance_sensor.md) message type. 


## Data Fields


float [minimum_distance_m](#structmavsdk_1_1_telemetry_server_1_1_distance_sensor_1a0f1cf74af825a80e8582444d8daf9db6) { float(NAN)} - Minimum distance the sensor can measure, NaN if unknown.

float [maximum_distance_m](#structmavsdk_1_1_telemetry_server_1_1_distance_sensor_1a143b36be80cd3fc8f39ec783de825c3a) { float(NAN)} - Maximum distance the sensor can measure, NaN if unknown.

float [current_distance_m](#structmavsdk_1_1_telemetry_server_1_1_distance_sensor_1ae66872d58d507199e0c99090f9f64015) { float(NAN)} - Current distance reading, NaN if unknown.


## Field Documentation


### minimum_distance_m {#structmavsdk_1_1_telemetry_server_1_1_distance_sensor_1a0f1cf74af825a80e8582444d8daf9db6}

```cpp
float mavsdk::TelemetryServer::DistanceSensor::minimum_distance_m { float(NAN)}
```


Minimum distance the sensor can measure, NaN if unknown.


### maximum_distance_m {#structmavsdk_1_1_telemetry_server_1_1_distance_sensor_1a143b36be80cd3fc8f39ec783de825c3a}

```cpp
float mavsdk::TelemetryServer::DistanceSensor::maximum_distance_m { float(NAN)}
```


Maximum distance the sensor can measure, NaN if unknown.


### current_distance_m {#structmavsdk_1_1_telemetry_server_1_1_distance_sensor_1ae66872d58d507199e0c99090f9f64015}

```cpp
float mavsdk::TelemetryServer::DistanceSensor::current_distance_m { float(NAN)}
```


Current distance reading, NaN if unknown.

