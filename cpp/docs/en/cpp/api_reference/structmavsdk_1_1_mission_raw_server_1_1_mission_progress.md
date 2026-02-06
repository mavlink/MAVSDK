# mavsdk::MissionRawServer::MissionProgress Struct Reference
`#include: mission_raw_server.h`

----


[Mission](classmavsdk_1_1_mission.md) progress type. 


## Data Fields


int32_t [current](#structmavsdk_1_1_mission_raw_server_1_1_mission_progress_1ab499616d64f4a01770bcbc947a625ed6) {} - Current mission item index (0-based), if equal to total, the mission is finished.

int32_t [total](#structmavsdk_1_1_mission_raw_server_1_1_mission_progress_1a181927204fc1e7bace72be06e90fa0fb) {} - Total number of mission items.


## Field Documentation


### current {#structmavsdk_1_1_mission_raw_server_1_1_mission_progress_1ab499616d64f4a01770bcbc947a625ed6}

```cpp
int32_t mavsdk::MissionRawServer::MissionProgress::current {}
```


Current mission item index (0-based), if equal to total, the mission is finished.


### total {#structmavsdk_1_1_mission_raw_server_1_1_mission_progress_1a181927204fc1e7bace72be06e90fa0fb}

```cpp
int32_t mavsdk::MissionRawServer::MissionProgress::total {}
```


Total number of mission items.

