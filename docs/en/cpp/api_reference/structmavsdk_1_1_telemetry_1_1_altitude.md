# mavsdk::Telemetry::Altitude Struct Reference
`#include: telemetry.h`

----


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) message type. 


## Data Fields


float [altitude_monotonic_m](#structmavsdk_1_1_telemetry_1_1_altitude_1a2dcb5cf0eaf256643b6c3382384164bf) {float( NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) in meters is initialized on system boot and monotonic.

float [altitude_amsl_m](#structmavsdk_1_1_telemetry_1_1_altitude_1a898ba184deccdaf9d09e35b32d00fe5b) { float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in meters.

float [altitude_local_m](#structmavsdk_1_1_telemetry_1_1_altitude_1ad99a82de545efff2db64fd122d983ddd) {float(NAN)} - Local altitude in meters.

float [altitude_relative_m](#structmavsdk_1_1_telemetry_1_1_altitude_1ab19d607e38ba15b6775e3477e28d097c) {float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) above home position in meters.

float [altitude_terrain_m](#structmavsdk_1_1_telemetry_1_1_altitude_1a3ff7fd4d1cb4add1315faed09665f6dd) {float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) above terrain in meters.

float [bottom_clearance_m](#structmavsdk_1_1_telemetry_1_1_altitude_1adb1f002956624c20524f33412bd8debc) { float(NAN)} - This is not the altitude, but the clear space below the system according to the fused clearance estimate in meters.


## Field Documentation


### altitude_monotonic_m {#structmavsdk_1_1_telemetry_1_1_altitude_1a2dcb5cf0eaf256643b6c3382384164bf}

```cpp
float mavsdk::Telemetry::Altitude::altitude_monotonic_m {float( NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) in meters is initialized on system boot and monotonic.


### altitude_amsl_m {#structmavsdk_1_1_telemetry_1_1_altitude_1a898ba184deccdaf9d09e35b32d00fe5b}

```cpp
float mavsdk::Telemetry::Altitude::altitude_amsl_m { float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) AMSL (above mean sea level) in meters.


### altitude_local_m {#structmavsdk_1_1_telemetry_1_1_altitude_1ad99a82de545efff2db64fd122d983ddd}

```cpp
float mavsdk::Telemetry::Altitude::altitude_local_m {float(NAN)}
```


Local altitude in meters.


### altitude_relative_m {#structmavsdk_1_1_telemetry_1_1_altitude_1ab19d607e38ba15b6775e3477e28d097c}

```cpp
float mavsdk::Telemetry::Altitude::altitude_relative_m {float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) above home position in meters.


### altitude_terrain_m {#structmavsdk_1_1_telemetry_1_1_altitude_1a3ff7fd4d1cb4add1315faed09665f6dd}

```cpp
float mavsdk::Telemetry::Altitude::altitude_terrain_m {float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) above terrain in meters.


### bottom_clearance_m {#structmavsdk_1_1_telemetry_1_1_altitude_1adb1f002956624c20524f33412bd8debc}

```cpp
float mavsdk::Telemetry::Altitude::bottom_clearance_m { float(NAN)}
```


This is not the altitude, but the clear space below the system according to the fused clearance estimate in meters.

