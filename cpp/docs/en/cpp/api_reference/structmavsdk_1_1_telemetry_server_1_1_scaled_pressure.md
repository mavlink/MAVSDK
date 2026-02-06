# mavsdk::TelemetryServer::ScaledPressure Struct Reference
`#include: telemetry_server.h`

----


Scaled Pressure message type. 


## Data Fields


uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a985af5fb636ad7818ebe3da318d4e5fa) {} - Timestamp (time since system boot)

float [absolute_pressure_hpa](#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1ac7d3c32691d636842b407337d8459db9) {} - Absolute pressure in hPa.

float [differential_pressure_hpa](#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a92258fbd49abcbde0dda09de5a7f2f48) {} - Differential pressure 1 in hPa.

float [temperature_deg](#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a185972a055327c1ed4352df1c8e169e9) {} - Absolute pressure temperature (in celsius)

float [differential_pressure_temperature_deg](#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a0e4bad5a33365365892759fdc7b69ccf) {} - Differential pressure temperature (in celsius, 0 if not available)


## Field Documentation


### timestamp_us {#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a985af5fb636ad7818ebe3da318d4e5fa}

```cpp
uint64_t mavsdk::TelemetryServer::ScaledPressure::timestamp_us {}
```


Timestamp (time since system boot)


### absolute_pressure_hpa {#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1ac7d3c32691d636842b407337d8459db9}

```cpp
float mavsdk::TelemetryServer::ScaledPressure::absolute_pressure_hpa {}
```


Absolute pressure in hPa.


### differential_pressure_hpa {#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a92258fbd49abcbde0dda09de5a7f2f48}

```cpp
float mavsdk::TelemetryServer::ScaledPressure::differential_pressure_hpa {}
```


Differential pressure 1 in hPa.


### temperature_deg {#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a185972a055327c1ed4352df1c8e169e9}

```cpp
float mavsdk::TelemetryServer::ScaledPressure::temperature_deg {}
```


Absolute pressure temperature (in celsius)


### differential_pressure_temperature_deg {#structmavsdk_1_1_telemetry_server_1_1_scaled_pressure_1a0e4bad5a33365365892759fdc7b69ccf}

```cpp
float mavsdk::TelemetryServer::ScaledPressure::differential_pressure_temperature_deg {}
```


Differential pressure temperature (in celsius, 0 if not available)

