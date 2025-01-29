# mavsdk::Telemetry::Battery Struct Reference
`#include: telemetry.h`

----


[Battery](structmavsdk_1_1_telemetry_1_1_battery.md) type. 


## Data Fields


uint32_t [id](#structmavsdk_1_1_telemetry_1_1_battery_1a0f020a0775d3b25b91c7d9b035e7ebd7) {0} - [Battery](structmavsdk_1_1_telemetry_1_1_battery.md) ID, for systems with multiple batteries.

float [temperature_degc](#structmavsdk_1_1_telemetry_1_1_battery_1a6d55d76fac17548b06bc5b2bcfbf8207) {float(NAN)} - Temperature of the battery in degrees Celsius. NAN for unknown temperature.

float [voltage_v](#structmavsdk_1_1_telemetry_1_1_battery_1aefd1d5569ff2a69bf507b422cbe75742) {float(NAN)} - Voltage in volts.

float [current_battery_a](#structmavsdk_1_1_telemetry_1_1_battery_1a8c459191787a49013c73a402aa6441b6) {float(NAN)} - [Battery](structmavsdk_1_1_telemetry_1_1_battery.md) current in Amps, NAN if autopilot does not measure the current.

float [capacity_consumed_ah](#structmavsdk_1_1_telemetry_1_1_battery_1a3593f72c606514287f2bc467408326cc) { float(NAN)} - Consumed charge in Amp hours, NAN if autopilot does not provide consumption estimate.

float [remaining_percent](#structmavsdk_1_1_telemetry_1_1_battery_1aec821550739844e0fa3fe18f91e6fda7) { float(NAN)} - Estimated battery remaining (range: 0 to 100)


## Field Documentation


### id {#structmavsdk_1_1_telemetry_1_1_battery_1a0f020a0775d3b25b91c7d9b035e7ebd7}

```cpp
uint32_t mavsdk::Telemetry::Battery::id {0}
```


[Battery](structmavsdk_1_1_telemetry_1_1_battery.md) ID, for systems with multiple batteries.


### temperature_degc {#structmavsdk_1_1_telemetry_1_1_battery_1a6d55d76fac17548b06bc5b2bcfbf8207}

```cpp
float mavsdk::Telemetry::Battery::temperature_degc {float(NAN)}
```


Temperature of the battery in degrees Celsius. NAN for unknown temperature.


### voltage_v {#structmavsdk_1_1_telemetry_1_1_battery_1aefd1d5569ff2a69bf507b422cbe75742}

```cpp
float mavsdk::Telemetry::Battery::voltage_v {float(NAN)}
```


Voltage in volts.


### current_battery_a {#structmavsdk_1_1_telemetry_1_1_battery_1a8c459191787a49013c73a402aa6441b6}

```cpp
float mavsdk::Telemetry::Battery::current_battery_a {float(NAN)}
```


[Battery](structmavsdk_1_1_telemetry_1_1_battery.md) current in Amps, NAN if autopilot does not measure the current.


### capacity_consumed_ah {#structmavsdk_1_1_telemetry_1_1_battery_1a3593f72c606514287f2bc467408326cc}

```cpp
float mavsdk::Telemetry::Battery::capacity_consumed_ah { float(NAN)}
```


Consumed charge in Amp hours, NAN if autopilot does not provide consumption estimate.


### remaining_percent {#structmavsdk_1_1_telemetry_1_1_battery_1aec821550739844e0fa3fe18f91e6fda7}

```cpp
float mavsdk::Telemetry::Battery::remaining_percent { float(NAN)}
```


Estimated battery remaining (range: 0 to 100)

