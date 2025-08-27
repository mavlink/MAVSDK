# mavsdk::Telemetry::ActuatorOutputStatus Struct Reference
`#include: telemetry.h`

----


Actuator output status type. 


## Data Fields


uint32_t [active](#structmavsdk_1_1_telemetry_1_1_actuator_output_status_1a6325f5b35293353c43cccf1ceaa5a712) {0} - Active outputs.

std::vector< float > [actuator](#structmavsdk_1_1_telemetry_1_1_actuator_output_status_1a6fca8f50d2fb9f59fd7b384bf8a9b294) {} - Servo/motor output values.


## Field Documentation


### active {#structmavsdk_1_1_telemetry_1_1_actuator_output_status_1a6325f5b35293353c43cccf1ceaa5a712}

```cpp
uint32_t mavsdk::Telemetry::ActuatorOutputStatus::active {0}
```


Active outputs.


### actuator {#structmavsdk_1_1_telemetry_1_1_actuator_output_status_1a6fca8f50d2fb9f59fd7b384bf8a9b294}

```cpp
std::vector<float> mavsdk::Telemetry::ActuatorOutputStatus::actuator {}
```


Servo/motor output values.

