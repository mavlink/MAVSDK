# mavsdk::MissionRaw::MissionItem Struct Reference
`#include: mission_raw.h`

----


[Mission](classmavsdk_1_1_mission.md) item exactly identical to MAVLink MISSION_ITEM_INT. 


## Data Fields


uint32_t [seq](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a4100d5f8bf1e8a8a18495e4d3e2df9c7) {} - Sequence (uint16_t)

uint32_t [frame](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a003abd4f7877609a5352e8c2d5b6f878) {} - The coordinate system of the waypoint (actually uint8_t)

uint32_t [command](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a8a1a528bfbcdb3106c947cf3618967c9) {} - The scheduled action for the waypoint (actually uint16_t)

uint32_t [current](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a0853859a211fd5fbc002b83f60292232) {} - false:0, true:1 (actually uint8_t)

uint32_t [autocontinue](#structmavsdk_1_1_mission_raw_1_1_mission_item_1ab832f0f1788c75f0b5b0a1f12561aa06) {} - Autocontinue to next waypoint (actually uint8_t)

float [param1](#structmavsdk_1_1_mission_raw_1_1_mission_item_1ae1d81f82162e3103b47af5b6a4c1aa02) {} - PARAM1, see MAV_CMD enum.

float [param2](#structmavsdk_1_1_mission_raw_1_1_mission_item_1acc86497f3775bc36e9552d86140c3a7a) {} - PARAM2, see MAV_CMD enum.

float [param3](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a62949430aab15578085436e3f041fd74) {} - PARAM3, see MAV_CMD enum.

float [param4](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a12b9313b1b8c992412aa76e95db97fef) {} - PARAM4, see MAV_CMD enum.

int32_t [x](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a6081b94f11c03da8cdbfc75242795569) {} - PARAM5 / local: x position in meters * 1e4, global: latitude in degrees * 10^7.

int32_t [y](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a9dbe5c8be2a48a4d64da03ea26ab0693) {} - PARAM6 / y position: local: x position in meters * 1e4, global: longitude in degrees *10^7.

float [z](#structmavsdk_1_1_mission_raw_1_1_mission_item_1ab2805706c6b8ed802c2454ce91f28c9e) {} - PARAM7 / local: Z coordinate, global: altitude (relative or absolute, depending on frame)

uint32_t [mission_type](#structmavsdk_1_1_mission_raw_1_1_mission_item_1a2228069197f29aa98ec03e69a74434b3) {} - [Mission](classmavsdk_1_1_mission.md) type (actually uint8_t)


## Field Documentation


### seq {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a4100d5f8bf1e8a8a18495e4d3e2df9c7}

```cpp
uint32_t mavsdk::MissionRaw::MissionItem::seq {}
```


Sequence (uint16_t)


### frame {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a003abd4f7877609a5352e8c2d5b6f878}

```cpp
uint32_t mavsdk::MissionRaw::MissionItem::frame {}
```


The coordinate system of the waypoint (actually uint8_t)


### command {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a8a1a528bfbcdb3106c947cf3618967c9}

```cpp
uint32_t mavsdk::MissionRaw::MissionItem::command {}
```


The scheduled action for the waypoint (actually uint16_t)


### current {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a0853859a211fd5fbc002b83f60292232}

```cpp
uint32_t mavsdk::MissionRaw::MissionItem::current {}
```


false:0, true:1 (actually uint8_t)


### autocontinue {#structmavsdk_1_1_mission_raw_1_1_mission_item_1ab832f0f1788c75f0b5b0a1f12561aa06}

```cpp
uint32_t mavsdk::MissionRaw::MissionItem::autocontinue {}
```


Autocontinue to next waypoint (actually uint8_t)


### param1 {#structmavsdk_1_1_mission_raw_1_1_mission_item_1ae1d81f82162e3103b47af5b6a4c1aa02}

```cpp
float mavsdk::MissionRaw::MissionItem::param1 {}
```


PARAM1, see MAV_CMD enum.


### param2 {#structmavsdk_1_1_mission_raw_1_1_mission_item_1acc86497f3775bc36e9552d86140c3a7a}

```cpp
float mavsdk::MissionRaw::MissionItem::param2 {}
```


PARAM2, see MAV_CMD enum.


### param3 {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a62949430aab15578085436e3f041fd74}

```cpp
float mavsdk::MissionRaw::MissionItem::param3 {}
```


PARAM3, see MAV_CMD enum.


### param4 {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a12b9313b1b8c992412aa76e95db97fef}

```cpp
float mavsdk::MissionRaw::MissionItem::param4 {}
```


PARAM4, see MAV_CMD enum.


### x {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a6081b94f11c03da8cdbfc75242795569}

```cpp
int32_t mavsdk::MissionRaw::MissionItem::x {}
```


PARAM5 / local: x position in meters * 1e4, global: latitude in degrees * 10^7.


### y {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a9dbe5c8be2a48a4d64da03ea26ab0693}

```cpp
int32_t mavsdk::MissionRaw::MissionItem::y {}
```


PARAM6 / y position: local: x position in meters * 1e4, global: longitude in degrees *10^7.


### z {#structmavsdk_1_1_mission_raw_1_1_mission_item_1ab2805706c6b8ed802c2454ce91f28c9e}

```cpp
float mavsdk::MissionRaw::MissionItem::z {}
```


PARAM7 / local: Z coordinate, global: altitude (relative or absolute, depending on frame)


### mission_type {#structmavsdk_1_1_mission_raw_1_1_mission_item_1a2228069197f29aa98ec03e69a74434b3}

```cpp
uint32_t mavsdk::MissionRaw::MissionItem::mission_type {}
```


[Mission](classmavsdk_1_1_mission.md) type (actually uint8_t)

