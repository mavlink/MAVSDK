# mavsdk::Mission::MissionProgress Struct Reference
`#include: mission.h`

----


[Mission](classmavsdk_1_1_mission.md) progress type. 


## Data Fields


int32_t [current](#structmavsdk_1_1_mission_1_1_mission_progress_1a5a68f76d6da0d32e9416c17ddd3a9178) {} - Current mission item index (0-based), if equal to total, the mission is finished.

int32_t [total](#structmavsdk_1_1_mission_1_1_mission_progress_1aa9d57135c3d81ec2ee5e7c18805cedb2) {} - Total number of mission items.


## Field Documentation


### current {#structmavsdk_1_1_mission_1_1_mission_progress_1a5a68f76d6da0d32e9416c17ddd3a9178}

```cpp
int32_t mavsdk::Mission::MissionProgress::current {}
```


Current mission item index (0-based), if equal to total, the mission is finished.


### total {#structmavsdk_1_1_mission_1_1_mission_progress_1aa9d57135c3d81ec2ee5e7c18805cedb2}

```cpp
int32_t mavsdk::Mission::MissionProgress::total {}
```


Total number of mission items.

