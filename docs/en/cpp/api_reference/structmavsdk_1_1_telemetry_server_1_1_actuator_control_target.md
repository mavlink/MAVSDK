# mavsdk::TelemetryServer::ActuatorControlTarget Struct Reference
`#include: telemetry_server.h`

----


Actuator control target type. 


## Data Fields


int32_t [group](#structmavsdk_1_1_telemetry_server_1_1_actuator_control_target_1a26b6abf67141f0366ebc571b0c5bb016) {0} - An actuator control group is e.g. 'attitude' for the core flight controls, or 'gimbal' for a payload.

std::vector< float > [controls](#structmavsdk_1_1_telemetry_server_1_1_actuator_control_target_1a8211de153fe0f6b0958f70e30c03719c) {} - Controls normed from -1 to 1, where 0 is neutral position.


## Field Documentation


### group {#structmavsdk_1_1_telemetry_server_1_1_actuator_control_target_1a26b6abf67141f0366ebc571b0c5bb016}

```cpp
int32_t mavsdk::TelemetryServer::ActuatorControlTarget::group {0}
```


An actuator control group is e.g. 'attitude' for the core flight controls, or 'gimbal' for a payload.


### controls {#structmavsdk_1_1_telemetry_server_1_1_actuator_control_target_1a8211de153fe0f6b0958f70e30c03719c}

```cpp
std::vector<float> mavsdk::TelemetryServer::ActuatorControlTarget::controls {}
```


Controls normed from -1 to 1, where 0 is neutral position.

