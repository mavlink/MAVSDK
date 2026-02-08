# mavsdk::Events::HealthAndArmingCheckMode Struct Reference
`#include: events.h`

----


Arming checks for a specific mode. 


## Data Fields


std::string [mode_name](#structmavsdk_1_1_events_1_1_health_and_arming_check_mode_1ade334ef65152cc7c6fcee3d14a1e0168) {} - Mode name, e.g. "Position".

bool [can_arm_or_run](#structmavsdk_1_1_events_1_1_health_and_arming_check_mode_1a0054b9e84b9159715cda9e11b4920d16) {} - If disarmed: indicates if arming is possible. If armed: indicates if the mode can be selected.

std::vector< [HealthAndArmingCheckProblem](structmavsdk_1_1_events_1_1_health_and_arming_check_problem.md) > [problems](#structmavsdk_1_1_events_1_1_health_and_arming_check_mode_1a1003ed614d692a6db439821e626b1748) {} - List of reported problems for the mode.


## Field Documentation


### mode_name {#structmavsdk_1_1_events_1_1_health_and_arming_check_mode_1ade334ef65152cc7c6fcee3d14a1e0168}

```cpp
std::string mavsdk::Events::HealthAndArmingCheckMode::mode_name {}
```


Mode name, e.g. "Position".


### can_arm_or_run {#structmavsdk_1_1_events_1_1_health_and_arming_check_mode_1a0054b9e84b9159715cda9e11b4920d16}

```cpp
bool mavsdk::Events::HealthAndArmingCheckMode::can_arm_or_run {}
```


If disarmed: indicates if arming is possible. If armed: indicates if the mode can be selected.


### problems {#structmavsdk_1_1_events_1_1_health_and_arming_check_mode_1a1003ed614d692a6db439821e626b1748}

```cpp
std::vector<HealthAndArmingCheckProblem> mavsdk::Events::HealthAndArmingCheckMode::problems {}
```


List of reported problems for the mode.

