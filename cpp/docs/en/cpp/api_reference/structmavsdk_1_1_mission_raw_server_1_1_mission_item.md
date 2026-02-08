# mavsdk::MissionRawServer::MissionItem Struct Reference
`#include: mission_raw_server.h`

----


[Mission](classmavsdk_1_1_mission.md) item exactly identical to MAVLink MISSION_ITEM_INT. 


## Data Fields


uint32_t [seq](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a092dabd74c220d25cec7b6a51b390ebf) {} - Sequence (uint16_t)

uint32_t [frame](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a106ba1ec51be8f9af968376e50abe09d) {} - The coordinate system of the waypoint (actually uint8_t)

uint32_t [command](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aee980795d98404f05469505802bd3860) {} - The scheduled action for the waypoint (actually uint16_t)

uint32_t [current](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a495cc2fb336f0b075c38140772cf9c0e) {} - false:0, true:1 (actually uint8_t)

uint32_t [autocontinue](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a798872e29696ad78c12de35ff013157d) {} - Autocontinue to next waypoint (actually uint8_t)

float [param1](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aaf09bf8604f290b18817d9440b0fd2df) {} - PARAM1, see MAV_CMD enum.

float [param2](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aff0e711bb5a19b8dfd128888da7ba5fc) {} - PARAM2, see MAV_CMD enum.

float [param3](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a1b1bb7bba00276e30bf231e566a71066) {} - PARAM3, see MAV_CMD enum.

float [param4](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a2138e23fde95b71c1b32eca8b3fb5860) {} - PARAM4, see MAV_CMD enum.

int32_t [x](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a10dbaabc66e323382e193546a8a111b6) {} - PARAM5 / local: x position in meters * 1e4, global: latitude in degrees * 10^7.

int32_t [y](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aeb1c4e1399f493f128cc43b3f3d166a3) {} - PARAM6 / y position: local: x position in meters * 1e4, global: longitude in degrees *10^7.

float [z](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a9513624833c36a8a29472cb582e175b8) {} - PARAM7 / local: Z coordinate, global: altitude (relative or absolute, depending on frame)

uint32_t [mission_type](#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1acb990a5dee9f2aae87c578095534747c) {} - [Mission](classmavsdk_1_1_mission.md) type (actually uint8_t)


## Field Documentation


### seq {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a092dabd74c220d25cec7b6a51b390ebf}

```cpp
uint32_t mavsdk::MissionRawServer::MissionItem::seq {}
```


Sequence (uint16_t)


### frame {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a106ba1ec51be8f9af968376e50abe09d}

```cpp
uint32_t mavsdk::MissionRawServer::MissionItem::frame {}
```


The coordinate system of the waypoint (actually uint8_t)


### command {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aee980795d98404f05469505802bd3860}

```cpp
uint32_t mavsdk::MissionRawServer::MissionItem::command {}
```


The scheduled action for the waypoint (actually uint16_t)


### current {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a495cc2fb336f0b075c38140772cf9c0e}

```cpp
uint32_t mavsdk::MissionRawServer::MissionItem::current {}
```


false:0, true:1 (actually uint8_t)


### autocontinue {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a798872e29696ad78c12de35ff013157d}

```cpp
uint32_t mavsdk::MissionRawServer::MissionItem::autocontinue {}
```


Autocontinue to next waypoint (actually uint8_t)


### param1 {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aaf09bf8604f290b18817d9440b0fd2df}

```cpp
float mavsdk::MissionRawServer::MissionItem::param1 {}
```


PARAM1, see MAV_CMD enum.


### param2 {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aff0e711bb5a19b8dfd128888da7ba5fc}

```cpp
float mavsdk::MissionRawServer::MissionItem::param2 {}
```


PARAM2, see MAV_CMD enum.


### param3 {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a1b1bb7bba00276e30bf231e566a71066}

```cpp
float mavsdk::MissionRawServer::MissionItem::param3 {}
```


PARAM3, see MAV_CMD enum.


### param4 {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a2138e23fde95b71c1b32eca8b3fb5860}

```cpp
float mavsdk::MissionRawServer::MissionItem::param4 {}
```


PARAM4, see MAV_CMD enum.


### x {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a10dbaabc66e323382e193546a8a111b6}

```cpp
int32_t mavsdk::MissionRawServer::MissionItem::x {}
```


PARAM5 / local: x position in meters * 1e4, global: latitude in degrees * 10^7.


### y {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1aeb1c4e1399f493f128cc43b3f3d166a3}

```cpp
int32_t mavsdk::MissionRawServer::MissionItem::y {}
```


PARAM6 / y position: local: x position in meters * 1e4, global: longitude in degrees *10^7.


### z {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1a9513624833c36a8a29472cb582e175b8}

```cpp
float mavsdk::MissionRawServer::MissionItem::z {}
```


PARAM7 / local: Z coordinate, global: altitude (relative or absolute, depending on frame)


### mission_type {#structmavsdk_1_1_mission_raw_server_1_1_mission_item_1acb990a5dee9f2aae87c578095534747c}

```cpp
uint32_t mavsdk::MissionRawServer::MissionItem::mission_type {}
```


[Mission](classmavsdk_1_1_mission.md) type (actually uint8_t)

