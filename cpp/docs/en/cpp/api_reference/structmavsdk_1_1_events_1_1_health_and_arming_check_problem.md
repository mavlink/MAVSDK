# mavsdk::Events::HealthAndArmingCheckProblem Struct Reference
`#include: events.h`

----


Health and arming check problem type. 


## Data Fields


std::string [message](#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1afd72427f3eba7f5592dce7f69731bbd6) {} - Short, single-line message.

std::string [description](#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1a81dc36ffb30a64e4a47b5512df37a45f) {} - Detailed description (optional, might be multiple lines)

[LogLevel](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecc) [log_level](#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1ac05048809875f80d0c02b643165b0c33) {} - Log level of message.

std::string [health_component](#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1a37d19a2c8d83f6fe03ac47525487d80d) {} - Associated health component, e.g. "gps".


## Field Documentation


### message {#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1afd72427f3eba7f5592dce7f69731bbd6}

```cpp
std::string mavsdk::Events::HealthAndArmingCheckProblem::message {}
```


Short, single-line message.


### description {#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1a81dc36ffb30a64e4a47b5512df37a45f}

```cpp
std::string mavsdk::Events::HealthAndArmingCheckProblem::description {}
```


Detailed description (optional, might be multiple lines)


### log_level {#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1ac05048809875f80d0c02b643165b0c33}

```cpp
LogLevel mavsdk::Events::HealthAndArmingCheckProblem::log_level {}
```


Log level of message.


### health_component {#structmavsdk_1_1_events_1_1_health_and_arming_check_problem_1a37d19a2c8d83f6fe03ac47525487d80d}

```cpp
std::string mavsdk::Events::HealthAndArmingCheckProblem::health_component {}
```


Associated health component, e.g. "gps".

