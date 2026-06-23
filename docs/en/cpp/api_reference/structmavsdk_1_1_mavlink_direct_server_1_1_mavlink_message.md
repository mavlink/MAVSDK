# mavsdk::MavlinkDirectServer::MavlinkMessage Struct Reference
`#include: mavlink_direct_server.hpp`

----


A complete MAVLink message with all header information and fields. 


## Data Fields


std::string [message_name](#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1a82f43e33a3197bfb57a9f834495a0859) {} - MAVLink message name (e.g., "HEARTBEAT", "GLOBAL_POSITION_INT")

uint32_t [system_id](#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1a477a1bc824967ebf7a30f9bd92a00be4) {} - [System](classmavsdk_1_1_system.md) ID of the sender (for received messages)

uint32_t [component_id](#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1af8b4a4f08feeed25b53cdb568f893e51) {} - Component ID of the sender (for received messages)

uint32_t [target_system_id](#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1af3090b2bc5d040c7629ee4f8090230e6) {} - Target system ID (for sending, 0 for broadcast)

uint32_t [target_component_id](#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1af2fb08fd267fa9fddfa637baa02b51b4) {} - Target component ID (for sending, 0 for broadcast)

std::string [fields_json](#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1a1afe271d0a4e8a6bba068688beb22726) {} - All message fields as single JSON object.


## Field Documentation


### message_name {#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1a82f43e33a3197bfb57a9f834495a0859}

```cpp
std::string mavsdk::MavlinkDirectServer::MavlinkMessage::message_name {}
```


MAVLink message name (e.g., "HEARTBEAT", "GLOBAL_POSITION_INT")


### system_id {#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1a477a1bc824967ebf7a30f9bd92a00be4}

```cpp
uint32_t mavsdk::MavlinkDirectServer::MavlinkMessage::system_id {}
```


[System](classmavsdk_1_1_system.md) ID of the sender (for received messages)


### component_id {#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1af8b4a4f08feeed25b53cdb568f893e51}

```cpp
uint32_t mavsdk::MavlinkDirectServer::MavlinkMessage::component_id {}
```


Component ID of the sender (for received messages)


### target_system_id {#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1af3090b2bc5d040c7629ee4f8090230e6}

```cpp
uint32_t mavsdk::MavlinkDirectServer::MavlinkMessage::target_system_id {}
```


Target system ID (for sending, 0 for broadcast)


### target_component_id {#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1af2fb08fd267fa9fddfa637baa02b51b4}

```cpp
uint32_t mavsdk::MavlinkDirectServer::MavlinkMessage::target_component_id {}
```


Target component ID (for sending, 0 for broadcast)


### fields_json {#structmavsdk_1_1_mavlink_direct_server_1_1_mavlink_message_1a1afe271d0a4e8a6bba068688beb22726}

```cpp
std::string mavsdk::MavlinkDirectServer::MavlinkMessage::fields_json {}
```


All message fields as single JSON object.

