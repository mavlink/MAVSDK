# mavsdk::Telemetry::RcStatus Struct Reference
`#include: telemetry.h`

----


Remote control status type. 


## Data Fields


bool [was_available_once](#structmavsdk_1_1_telemetry_1_1_rc_status_1ab137802b6d9e6d9fde0822d3d42fe900) {false} - True if an RC signal has been available once.

bool [is_available](#structmavsdk_1_1_telemetry_1_1_rc_status_1a6949a32def1be5468cab9b4ce5c15b07) {false} - True if the RC signal is available now.

float [signal_strength_percent](#structmavsdk_1_1_telemetry_1_1_rc_status_1a5d637a687d2bcc84abef28ff0f1ab31e) { float(NAN)} - Signal strength (range: 0 to 100, NaN if unknown)


## Field Documentation


### was_available_once {#structmavsdk_1_1_telemetry_1_1_rc_status_1ab137802b6d9e6d9fde0822d3d42fe900}

```cpp
bool mavsdk::Telemetry::RcStatus::was_available_once {false}
```


True if an RC signal has been available once.


### is_available {#structmavsdk_1_1_telemetry_1_1_rc_status_1a6949a32def1be5468cab9b4ce5c15b07}

```cpp
bool mavsdk::Telemetry::RcStatus::is_available {false}
```


True if the RC signal is available now.


### signal_strength_percent {#structmavsdk_1_1_telemetry_1_1_rc_status_1a5d637a687d2bcc84abef28ff0f1ab31e}

```cpp
float mavsdk::Telemetry::RcStatus::signal_strength_percent { float(NAN)}
```


Signal strength (range: 0 to 100, NaN if unknown)

