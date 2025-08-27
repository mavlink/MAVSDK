# mavsdk::Offboard::ActuatorControlGroup Struct Reference
`#include: offboard.h`

----


Eight controls that will be given to the group. Each control is a normalized (-1..+1) command value, which will be mapped and scaled through the mixer. 


## Data Fields


std::vector< float > [controls](#structmavsdk_1_1_offboard_1_1_actuator_control_group_1a89026874ccf3960693342c634a029009) {} - Controls in the group.


## Field Documentation


### controls {#structmavsdk_1_1_offboard_1_1_actuator_control_group_1a89026874ccf3960693342c634a029009}

```cpp
std::vector<float> mavsdk::Offboard::ActuatorControlGroup::controls {}
```


Controls in the group.

