# mavsdk::Events::HealthAndArmingCheckReport Struct Reference
`#include: events.h`

----


Health and arming check report type. 


## Data Fields


[HealthAndArmingCheckMode](structmavsdk_1_1_events_1_1_health_and_arming_check_mode.md) [current_mode_intention](#structmavsdk_1_1_events_1_1_health_and_arming_check_report_1a00f1d9c7d6beb8d4a9b310aa6a72572f) {} - Report for currently intended mode.

std::vector< [HealthComponentReport](structmavsdk_1_1_events_1_1_health_component_report.md) > [health_components](#structmavsdk_1_1_events_1_1_health_and_arming_check_report_1af3267879f370b4f7cd84840f3625c0fc) {} - Health components list (e.g. for "gps")

std::vector< [HealthAndArmingCheckProblem](structmavsdk_1_1_events_1_1_health_and_arming_check_problem.md) > [all_problems](#structmavsdk_1_1_events_1_1_health_and_arming_check_report_1adb355887bc72bee03dceccfda4836e4a) {} - Complete list of problems.


## Field Documentation


### current_mode_intention {#structmavsdk_1_1_events_1_1_health_and_arming_check_report_1a00f1d9c7d6beb8d4a9b310aa6a72572f}

```cpp
HealthAndArmingCheckMode mavsdk::Events::HealthAndArmingCheckReport::current_mode_intention {}
```


Report for currently intended mode.


### health_components {#structmavsdk_1_1_events_1_1_health_and_arming_check_report_1af3267879f370b4f7cd84840f3625c0fc}

```cpp
std::vector<HealthComponentReport> mavsdk::Events::HealthAndArmingCheckReport::health_components {}
```


Health components list (e.g. for "gps")


### all_problems {#structmavsdk_1_1_events_1_1_health_and_arming_check_report_1adb355887bc72bee03dceccfda4836e4a}

```cpp
std::vector<HealthAndArmingCheckProblem> mavsdk::Events::HealthAndArmingCheckReport::all_problems {}
```


Complete list of problems.

