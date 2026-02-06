# mavsdk::Telemetry::ActuatorControlTarget Struct Reference
`#include: telemetry.h`

----


Actuator control target type. 


## Data Fields


int32_t [group](#structmavsdk_1_1_telemetry_1_1_actuator_control_target_1a31c7cfc07e6b4de2ee87439230db6e47) {0} - An actuator control group is e.g. 'attitude' for the core flight controls, or 'gimbal' for a payload.

std::vector< float > [controls](#structmavsdk_1_1_telemetry_1_1_actuator_control_target_1ae12790f5d28dfb2b979618c155136c5f) {} - Controls normed from -1 to 1, where 0 is neutral position.


## Field Documentation


### group {#structmavsdk_1_1_telemetry_1_1_actuator_control_target_1a31c7cfc07e6b4de2ee87439230db6e47}

```cpp
int32_t mavsdk::Telemetry::ActuatorControlTarget::group {0}
```


An actuator control group is e.g. 'attitude' for the core flight controls, or 'gimbal' for a payload.


### controls {#structmavsdk_1_1_telemetry_1_1_actuator_control_target_1ae12790f5d28dfb2b979618c155136c5f}

```cpp
std::vector<float> mavsdk::Telemetry::ActuatorControlTarget::controls {}
```


Controls normed from -1 to 1, where 0 is neutral position.

