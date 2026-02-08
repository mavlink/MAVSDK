# mavsdk::Events::HealthComponentReport Struct Reference
`#include: events.h`

----


Health component report type. 


## Data Fields


std::string [name](#structmavsdk_1_1_events_1_1_health_component_report_1ae33eb7667f6ee51585fed8d2d7a13cb6) {} - Unique component name, e.g. "gps".

std::string [label](#structmavsdk_1_1_events_1_1_health_component_report_1a5798959a7d5b8b65d3763e4981c4a1b6) {} - Human readable label of the component, e.g. "GPS" or "Accelerometer".

bool [is_present](#structmavsdk_1_1_events_1_1_health_component_report_1a5c71a42e740f3c137359f3d53de52cf9) {} - If the component is present.

bool [has_error](#structmavsdk_1_1_events_1_1_health_component_report_1a79f0e8503febb4c19a07b49668c76ba7) {} - If the component has errors.

bool [has_warning](#structmavsdk_1_1_events_1_1_health_component_report_1ab37c768a8f671ee40224034e86794af4) {} - If the component has warnings.


## Field Documentation


### name {#structmavsdk_1_1_events_1_1_health_component_report_1ae33eb7667f6ee51585fed8d2d7a13cb6}

```cpp
std::string mavsdk::Events::HealthComponentReport::name {}
```


Unique component name, e.g. "gps".


### label {#structmavsdk_1_1_events_1_1_health_component_report_1a5798959a7d5b8b65d3763e4981c4a1b6}

```cpp
std::string mavsdk::Events::HealthComponentReport::label {}
```


Human readable label of the component, e.g. "GPS" or "Accelerometer".


### is_present {#structmavsdk_1_1_events_1_1_health_component_report_1a5c71a42e740f3c137359f3d53de52cf9}

```cpp
bool mavsdk::Events::HealthComponentReport::is_present {}
```


If the component is present.


### has_error {#structmavsdk_1_1_events_1_1_health_component_report_1a79f0e8503febb4c19a07b49668c76ba7}

```cpp
bool mavsdk::Events::HealthComponentReport::has_error {}
```


If the component has errors.


### has_warning {#structmavsdk_1_1_events_1_1_health_component_report_1ab37c768a8f671ee40224034e86794af4}

```cpp
bool mavsdk::Events::HealthComponentReport::has_warning {}
```


If the component has warnings.

