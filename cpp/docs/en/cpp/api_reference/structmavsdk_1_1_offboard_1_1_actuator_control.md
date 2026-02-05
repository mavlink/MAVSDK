# mavsdk::Offboard::ActuatorControl Struct Reference
`#include: offboard.h`

----


Type for actuator control. 


Control members should be normed to -1..+1 where 0 is neutral position. Throttle for single rotation direction motors is 0..1, negative range for reverse direction.


One group support eight controls.


Up to 16 actuator controls can be set. To ignore an output group, set all it controls to NaN. If one or more controls in group is not NaN, then all NaN controls will sent as zero. The first 8 actuator controls internally map to control group 0, the latter 8 actuator controls map to control group 1. Depending on what controls are set (instead of NaN) 1 or 2 MAVLink messages are actually sent.


In PX4 v1.9.0 Only first four Control Groups are supported ([https://github.com/PX4/Firmware/blob/v1.9.0/src/modules/mavlink/mavlink_receiver.cpp#L980](https://github.com/PX4/Firmware/blob/v1.9.0/src/modules/mavlink/mavlink_receiver.cpp#L980)). 


## Data Fields


std::vector< [ActuatorControlGroup](structmavsdk_1_1_offboard_1_1_actuator_control_group.md) > [groups](#structmavsdk_1_1_offboard_1_1_actuator_control_1a42ea86eb6b2150a552fb7e662e4a0226) {} - Control groups.


## Field Documentation


### groups {#structmavsdk_1_1_offboard_1_1_actuator_control_1a42ea86eb6b2150a552fb7e662e4a0226}

```cpp
std::vector<ActuatorControlGroup> mavsdk::Offboard::ActuatorControl::groups {}
```


Control groups.

