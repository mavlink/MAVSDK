# mavsdk::Info::FlightInfo Struct Reference
`#include: info.h`

----


[System](classmavsdk_1_1_system.md) flight information. 


## Data Fields


uint32_t [time_boot_ms](#structmavsdk_1_1_info_1_1_flight_info_1aa38f317ec1c38bc45a20522752a2838d) {} - Time since system boot.

uint64_t [flight_uid](#structmavsdk_1_1_info_1_1_flight_info_1adfae0cbebc461eb3ab144d7173cca9e8) {} - Flight counter. Starts from zero, is incremented at every disarm and is never reset (even after reboot)


## Field Documentation


### time_boot_ms {#structmavsdk_1_1_info_1_1_flight_info_1aa38f317ec1c38bc45a20522752a2838d}

```cpp
uint32_t mavsdk::Info::FlightInfo::time_boot_ms {}
```


Time since system boot.


### flight_uid {#structmavsdk_1_1_info_1_1_flight_info_1adfae0cbebc461eb3ab144d7173cca9e8}

```cpp
uint64_t mavsdk::Info::FlightInfo::flight_uid {}
```


Flight counter. Starts from zero, is incremented at every disarm and is never reset (even after reboot)

