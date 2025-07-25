# mavsdk::MavlinkDirect::MavlinkMessage Struct Reference
`#include: mavlink_direct.h`

----


A complete MAVLink message with all header information and fields. 


## Data Fields


std::string [message_name](#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1aed4366125771f7def35372ca9ccf0285) {} - MAVLink message name (e.g., "HEARTBEAT", "GLOBAL_POSITION_INT")

uint32_t [system_id](#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1a0fea4a98941a9f97b28a220f2de11948) {} - [System](classmavsdk_1_1_system.md) ID of the sender (for received messages)

uint32_t [component_id](#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1a742b128da448bb76b80ea842dc04161e) {} - Component ID of the sender (for received messages)

uint32_t [target_system](#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1abe3432b0a8d0fdcccf57091ef8ffd674) {} - Target system ID (for sending, 0 for broadcast)

uint32_t [target_component](#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1abed450709e187a3f0bedd2763680620c) {} - Target component ID (for sending, 0 for broadcast)

std::string [fields_json](#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1a6f3a89236007a00cf69f2063dcbcac50) {} - All message fields as single JSON object.


## Field Documentation


### message_name {#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1aed4366125771f7def35372ca9ccf0285}

```cpp
std::string mavsdk::MavlinkDirect::MavlinkMessage::message_name {}
```


MAVLink message name (e.g., "HEARTBEAT", "GLOBAL_POSITION_INT")


### system_id {#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1a0fea4a98941a9f97b28a220f2de11948}

```cpp
uint32_t mavsdk::MavlinkDirect::MavlinkMessage::system_id {}
```


[System](classmavsdk_1_1_system.md) ID of the sender (for received messages)


### component_id {#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1a742b128da448bb76b80ea842dc04161e}

```cpp
uint32_t mavsdk::MavlinkDirect::MavlinkMessage::component_id {}
```


Component ID of the sender (for received messages)


### target_system {#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1abe3432b0a8d0fdcccf57091ef8ffd674}

```cpp
uint32_t mavsdk::MavlinkDirect::MavlinkMessage::target_system {}
```


Target system ID (for sending, 0 for broadcast)


### target_component {#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1abed450709e187a3f0bedd2763680620c}

```cpp
uint32_t mavsdk::MavlinkDirect::MavlinkMessage::target_component {}
```


Target component ID (for sending, 0 for broadcast)


### fields_json {#structmavsdk_1_1_mavlink_direct_1_1_mavlink_message_1a6f3a89236007a00cf69f2063dcbcac50}

```cpp
std::string mavsdk::MavlinkDirect::MavlinkMessage::fields_json {}
```


All message fields as single JSON object.

