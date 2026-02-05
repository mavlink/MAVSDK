# mavsdk::Telemetry::ScaledPressure Struct Reference
`#include: telemetry.h`

----


Scaled Pressure message type. 


## Data Fields


uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1a37d91f3f8470a21ee36ce55bd9977d67) {} - Timestamp (time since system boot)

float [absolute_pressure_hpa](#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1abacfc48910bc70f49b5852a4185898ef) {} - Absolute pressure in hPa.

float [differential_pressure_hpa](#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1ac60181897aae8504209912e0d8818e9b) {} - Differential pressure 1 in hPa.

float [temperature_deg](#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1adadf4a1be58f4d8cb5d856a9ca767ed4) {} - Absolute pressure temperature (in celsius)

float [differential_pressure_temperature_deg](#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1afa6c5d02d87be13bd08d5a2706b694aa) {} - Differential pressure temperature (in celsius, 0 if not available)


## Field Documentation


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1a37d91f3f8470a21ee36ce55bd9977d67}

```cpp
uint64_t mavsdk::Telemetry::ScaledPressure::timestamp_us {}
```


Timestamp (time since system boot)


### absolute_pressure_hpa {#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1abacfc48910bc70f49b5852a4185898ef}

```cpp
float mavsdk::Telemetry::ScaledPressure::absolute_pressure_hpa {}
```


Absolute pressure in hPa.


### differential_pressure_hpa {#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1ac60181897aae8504209912e0d8818e9b}

```cpp
float mavsdk::Telemetry::ScaledPressure::differential_pressure_hpa {}
```


Differential pressure 1 in hPa.


### temperature_deg {#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1adadf4a1be58f4d8cb5d856a9ca767ed4}

```cpp
float mavsdk::Telemetry::ScaledPressure::temperature_deg {}
```


Absolute pressure temperature (in celsius)


### differential_pressure_temperature_deg {#structmavsdk_1_1_telemetry_1_1_scaled_pressure_1afa6c5d02d87be13bd08d5a2706b694aa}

```cpp
float mavsdk::Telemetry::ScaledPressure::differential_pressure_temperature_deg {}
```


Differential pressure temperature (in celsius, 0 if not available)

