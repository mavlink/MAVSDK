# mavsdk::TelemetryServer::ActuatorOutputStatus Struct Reference
`#include: telemetry_server.h`

----


Actuator output status type. 


## Data Fields


uint32_t [active](#structmavsdk_1_1_telemetry_server_1_1_actuator_output_status_1a5f9af4a1df21ea5ef256fa49d56d108a) {0} - Active outputs.

std::vector< float > [actuator](#structmavsdk_1_1_telemetry_server_1_1_actuator_output_status_1a616047a678f4e811fa064df6b2e4194d) {} - Servo/motor output values.


## Field Documentation


### active {#structmavsdk_1_1_telemetry_server_1_1_actuator_output_status_1a5f9af4a1df21ea5ef256fa49d56d108a}

```cpp
uint32_t mavsdk::TelemetryServer::ActuatorOutputStatus::active {0}
```


Active outputs.


### actuator {#structmavsdk_1_1_telemetry_server_1_1_actuator_output_status_1a616047a678f4e811fa064df6b2e4194d}

```cpp
std::vector<float> mavsdk::TelemetryServer::ActuatorOutputStatus::actuator {}
```


Servo/motor output values.

