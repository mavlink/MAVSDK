# mavsdk::Info::Identification Struct Reference
`#include: info.h`

----


[System](classmavsdk_1_1_system.md) identification. 


## Data Fields


std::string [hardware_uid](#structmavsdk_1_1_info_1_1_identification_1a68be9823aae193b5473191d287b5e385) {} - UID of the hardware. This refers to uid2 of MAVLink. If the system does not support uid2 yet, this is all zeros.

uint64_t [legacy_uid](#structmavsdk_1_1_info_1_1_identification_1a2429c1ffc3fbda0d55e85fa5a6f79dc1) {} - Legacy UID of the hardware, referred to as uid in MAVLink (formerly exposed during system discovery as UUID).


## Field Documentation


### hardware_uid {#structmavsdk_1_1_info_1_1_identification_1a68be9823aae193b5473191d287b5e385}

```cpp
std::string mavsdk::Info::Identification::hardware_uid {}
```


UID of the hardware. This refers to uid2 of MAVLink. If the system does not support uid2 yet, this is all zeros.


### legacy_uid {#structmavsdk_1_1_info_1_1_identification_1a2429c1ffc3fbda0d55e85fa5a6f79dc1}

```cpp
uint64_t mavsdk::Info::Identification::legacy_uid {}
```


Legacy UID of the hardware, referred to as uid in MAVLink (formerly exposed during system discovery as UUID).

