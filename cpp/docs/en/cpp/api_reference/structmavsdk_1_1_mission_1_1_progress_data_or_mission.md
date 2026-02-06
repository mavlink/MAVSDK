# mavsdk::Mission::ProgressDataOrMission Struct Reference
`#include: mission.h`

----


Progress data coming from mission download, or the mission itself (if the transfer succeeds). 


## Data Fields


bool [has_progress](#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1aa62b873987a36a1f0933d553eee8fc96) { false} - Whether this [ProgressData](structmavsdk_1_1_mission_1_1_progress_data.md) contains a 'progress' status or not.

float [progress](#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1a75ab9d493dfd9cda21df2018b5713eb9) {float(NAN)} - Progress (0..1.0)

bool [has_mission](#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1a3d404ffdc43a720c6dc56268af635a48) {} - Whether this [ProgressData](structmavsdk_1_1_mission_1_1_progress_data.md) contains a 'mission_plan' or not.

[MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) [mission_plan](#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1a2ba4b1a384bf61298a7c174ca90f703a) {} - [Mission](classmavsdk_1_1_mission.md) plan.


## Field Documentation


### has_progress {#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1aa62b873987a36a1f0933d553eee8fc96}

```cpp
bool mavsdk::Mission::ProgressDataOrMission::has_progress { false}
```


Whether this [ProgressData](structmavsdk_1_1_mission_1_1_progress_data.md) contains a 'progress' status or not.


### progress {#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1a75ab9d493dfd9cda21df2018b5713eb9}

```cpp
float mavsdk::Mission::ProgressDataOrMission::progress {float(NAN)}
```


Progress (0..1.0)


### has_mission {#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1a3d404ffdc43a720c6dc56268af635a48}

```cpp
bool mavsdk::Mission::ProgressDataOrMission::has_mission {}
```


Whether this [ProgressData](structmavsdk_1_1_mission_1_1_progress_data.md) contains a 'mission_plan' or not.


### mission_plan {#structmavsdk_1_1_mission_1_1_progress_data_or_mission_1a2ba4b1a384bf61298a7c174ca90f703a}

```cpp
MissionPlan mavsdk::Mission::ProgressDataOrMission::mission_plan {}
```


[Mission](classmavsdk_1_1_mission.md) plan.

