# mavsdk::MavlinkPassthrough::CommandInt Struct Reference
`#include: mavlink_passthrough.h`

----


Type for MAVLink command_int. 


## Data Fields


uint8_t [target_sysid](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a306288fb9fa75a3304ba16dd8d9bb0c6)  - [System](classmavsdk_1_1_system.md) ID to send command to.

uint8_t [target_compid](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1aa93fd091a743292535c2419579f62204)  - Component ID to send command to.

uint16_t [command](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a17dda91c83b7ae7d01dc64a3ed7fc7c0)  - command to send.

MAV_FRAME [frame](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a31cbea5392e711069d5f5c990d2cba92)  -

float [param1](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1af667d2f79bb1a196c5d1423869078203)  - param1.

float [param2](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a1dd4bce33e0281ec6468c97b7a7b03a1)  - param2.

float [param3](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1ab63eb6898d7ab2838567c52bd1339635)  - param3.

float [param4](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1acb8b24402f78d37eb6e1aae1a44ed86c)  - param4.

int32_t [x](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1aeb66966f4af3b080fa565094e2c9df51)  - x.

int32_t [y](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a2b7f8e7e6cf1905f096f5dea43c669ce)  - y.

float [z](#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a06b57c3981c70b9b79fcc28b097ecdf6)  - z.


## Field Documentation


### target_sysid {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a306288fb9fa75a3304ba16dd8d9bb0c6}

```cpp
uint8_t mavsdk::MavlinkPassthrough::CommandInt::target_sysid
```


[System](classmavsdk_1_1_system.md) ID to send command to.


### target_compid {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1aa93fd091a743292535c2419579f62204}

```cpp
uint8_t mavsdk::MavlinkPassthrough::CommandInt::target_compid
```


Component ID to send command to.


### command {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a17dda91c83b7ae7d01dc64a3ed7fc7c0}

```cpp
uint16_t mavsdk::MavlinkPassthrough::CommandInt::command
```


command to send.


### frame {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a31cbea5392e711069d5f5c990d2cba92}

```cpp
MAV_FRAME mavsdk::MavlinkPassthrough::CommandInt::frame
```


Frame of command.

### param1 {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1af667d2f79bb1a196c5d1423869078203}

```cpp
float mavsdk::MavlinkPassthrough::CommandInt::param1
```


param1.


### param2 {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a1dd4bce33e0281ec6468c97b7a7b03a1}

```cpp
float mavsdk::MavlinkPassthrough::CommandInt::param2
```


param2.


### param3 {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1ab63eb6898d7ab2838567c52bd1339635}

```cpp
float mavsdk::MavlinkPassthrough::CommandInt::param3
```


param3.


### param4 {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1acb8b24402f78d37eb6e1aae1a44ed86c}

```cpp
float mavsdk::MavlinkPassthrough::CommandInt::param4
```


param4.


### x {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1aeb66966f4af3b080fa565094e2c9df51}

```cpp
int32_t mavsdk::MavlinkPassthrough::CommandInt::x
```


x.


### y {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a2b7f8e7e6cf1905f096f5dea43c669ce}

```cpp
int32_t mavsdk::MavlinkPassthrough::CommandInt::y
```


y.


### z {#structmavsdk_1_1_mavlink_passthrough_1_1_command_int_1a06b57c3981c70b9b79fcc28b097ecdf6}

```cpp
float mavsdk::MavlinkPassthrough::CommandInt::z
```


z.

