# mavsdk::Mavsdk::MavlinkMessage Struct Reference
`#include: mavsdk.h`

----


A complete MAVLink message with all header information and fields. 


## Data Fields


std::string [message_name](#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a17ddcd78e5712403ef4c1b6798698cdb) {} - MAVLink message name (e.g., "HEARTBEAT", "GLOBAL_POSITION_INT")

uint32_t [system_id](#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1ab4e295a583a82deb0f1786a4d61968ce) {} - [System](classmavsdk_1_1_system.md) ID of the sender (for received messages)

uint32_t [component_id](#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a810d77876cacc800939d3bfb6453e2f7) {} - Component ID of the sender (for received messages)

uint32_t [target_system](#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1aa30e2970d9a40c17e7bf8e02b150d310) {} - Target system ID (for sending, 0 for broadcast)

uint32_t [target_component](#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a5cf69f1e41e5791da49528dfd1788865) {} - Target component ID (for sending, 0 for broadcast)

std::string [fields_json](#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a73165621a97083c143ba7c36461c9754) {} - All message fields as single JSON object.


## Field Documentation


### message_name {#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a17ddcd78e5712403ef4c1b6798698cdb}

```cpp
std::string mavsdk::Mavsdk::MavlinkMessage::message_name {}
```


MAVLink message name (e.g., "HEARTBEAT", "GLOBAL_POSITION_INT")


### system_id {#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1ab4e295a583a82deb0f1786a4d61968ce}

```cpp
uint32_t mavsdk::Mavsdk::MavlinkMessage::system_id {}
```


[System](classmavsdk_1_1_system.md) ID of the sender (for received messages)


### component_id {#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a810d77876cacc800939d3bfb6453e2f7}

```cpp
uint32_t mavsdk::Mavsdk::MavlinkMessage::component_id {}
```


Component ID of the sender (for received messages)


### target_system {#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1aa30e2970d9a40c17e7bf8e02b150d310}

```cpp
uint32_t mavsdk::Mavsdk::MavlinkMessage::target_system {}
```


Target system ID (for sending, 0 for broadcast)


### target_component {#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a5cf69f1e41e5791da49528dfd1788865}

```cpp
uint32_t mavsdk::Mavsdk::MavlinkMessage::target_component {}
```


Target component ID (for sending, 0 for broadcast)


### fields_json {#structmavsdk_1_1_mavsdk_1_1_mavlink_message_1a73165621a97083c143ba7c36461c9754}

```cpp
std::string mavsdk::Mavsdk::MavlinkMessage::fields_json {}
```


All message fields as single JSON object.

