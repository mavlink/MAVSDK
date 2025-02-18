# mavsdk::TelemetryServer::FixedwingMetrics Struct Reference
`#include: telemetry_server.h`

----


[FixedwingMetrics](structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics.md) message type. 


## Data Fields


float [airspeed_m_s](#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1ac93b4a14fbb25992c7bd30ef109c28ff) { float(NAN)} - Current indicated airspeed (IAS) in metres per second.

float [throttle_percentage](#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1a98714b66349e101105e6746903bbfd30) {float(NAN)} - Current throttle setting (0 to 100)

float [climb_rate_m_s](#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1a8486ccc7e96add636a228399a74b1019) {float(NAN)} - Current climb rate in metres per second.

float [groundspeed_m_s](#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1ac3d6a9651edb283a1217ef2dcee91fce) {float(NAN)} - Current groundspeed metres per second.

float [heading_deg](#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1ab5cad9e76b47fec7f40282b0de8b75f3) { float(NAN)} - Current heading in compass units (0-360, 0=north)

float [absolute_altitude_m](#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1adb56c63202cdcd9abcab1464ee138dbd) {float(NAN)} - Current altitude in metres (MSL)


## Field Documentation


### airspeed_m_s {#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1ac93b4a14fbb25992c7bd30ef109c28ff}

```cpp
float mavsdk::TelemetryServer::FixedwingMetrics::airspeed_m_s { float(NAN)}
```


Current indicated airspeed (IAS) in metres per second.


### throttle_percentage {#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1a98714b66349e101105e6746903bbfd30}

```cpp
float mavsdk::TelemetryServer::FixedwingMetrics::throttle_percentage {float(NAN)}
```


Current throttle setting (0 to 100)


### climb_rate_m_s {#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1a8486ccc7e96add636a228399a74b1019}

```cpp
float mavsdk::TelemetryServer::FixedwingMetrics::climb_rate_m_s {float(NAN)}
```


Current climb rate in metres per second.


### groundspeed_m_s {#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1ac3d6a9651edb283a1217ef2dcee91fce}

```cpp
float mavsdk::TelemetryServer::FixedwingMetrics::groundspeed_m_s {float(NAN)}
```


Current groundspeed metres per second.


### heading_deg {#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1ab5cad9e76b47fec7f40282b0de8b75f3}

```cpp
float mavsdk::TelemetryServer::FixedwingMetrics::heading_deg { float(NAN)}
```


Current heading in compass units (0-360, 0=north)


### absolute_altitude_m {#structmavsdk_1_1_telemetry_server_1_1_fixedwing_metrics_1adb56c63202cdcd9abcab1464ee138dbd}

```cpp
float mavsdk::TelemetryServer::FixedwingMetrics::absolute_altitude_m {float(NAN)}
```


Current altitude in metres (MSL)

