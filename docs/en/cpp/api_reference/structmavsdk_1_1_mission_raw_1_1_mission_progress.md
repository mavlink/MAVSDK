# mavsdk::MissionRaw::MissionProgress Struct Reference
`#include: mission_raw.h`

----


[Mission](classmavsdk_1_1_mission.md) progress type. 


## Data Fields


int32_t [current](#structmavsdk_1_1_mission_raw_1_1_mission_progress_1ac1944774b7ed6216c19cffdc4911adbf) {} - Current mission item index (0-based), if equal to total, the mission is finished.

int32_t [total](#structmavsdk_1_1_mission_raw_1_1_mission_progress_1a60cef07998be10d414b7481fc0f21e0f) {} - Total number of mission items.


## Field Documentation


### current {#structmavsdk_1_1_mission_raw_1_1_mission_progress_1ac1944774b7ed6216c19cffdc4911adbf}

```cpp
int32_t mavsdk::MissionRaw::MissionProgress::current {}
```


Current mission item index (0-based), if equal to total, the mission is finished.


### total {#structmavsdk_1_1_mission_raw_1_1_mission_progress_1a60cef07998be10d414b7481fc0f21e0f}

```cpp
int32_t mavsdk::MissionRaw::MissionProgress::total {}
```


Total number of mission items.

