# mavsdk::TelemetryServer::RcStatus Struct Reference
`#include: telemetry_server.h`

----


Remote control status type. 


## Data Fields


bool [was_available_once](#structmavsdk_1_1_telemetry_server_1_1_rc_status_1a6428e525f512f7a11132fd9198ecdbf6) {false} - True if an RC signal has been available once.

bool [is_available](#structmavsdk_1_1_telemetry_server_1_1_rc_status_1a93312bee38b9d17cdf90b2943ef9683d) {false} - True if the RC signal is available now.

float [signal_strength_percent](#structmavsdk_1_1_telemetry_server_1_1_rc_status_1a2f3b64bf1c164890956e25980d91cbcd) { float(NAN)} - Signal strength (range: 0 to 100, NaN if unknown)


## Field Documentation


### was_available_once {#structmavsdk_1_1_telemetry_server_1_1_rc_status_1a6428e525f512f7a11132fd9198ecdbf6}

```cpp
bool mavsdk::TelemetryServer::RcStatus::was_available_once {false}
```


True if an RC signal has been available once.


### is_available {#structmavsdk_1_1_telemetry_server_1_1_rc_status_1a93312bee38b9d17cdf90b2943ef9683d}

```cpp
bool mavsdk::TelemetryServer::RcStatus::is_available {false}
```


True if the RC signal is available now.


### signal_strength_percent {#structmavsdk_1_1_telemetry_server_1_1_rc_status_1a2f3b64bf1c164890956e25980d91cbcd}

```cpp
float mavsdk::TelemetryServer::RcStatus::signal_strength_percent { float(NAN)}
```


Signal strength (range: 0 to 100, NaN if unknown)

