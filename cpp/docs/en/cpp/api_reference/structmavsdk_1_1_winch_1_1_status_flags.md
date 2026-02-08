# mavsdk::Winch::StatusFlags Struct Reference
`#include: winch.h`

----


[Winch](classmavsdk_1_1_winch.md) [Status](structmavsdk_1_1_winch_1_1_status.md) Flags. 


The status flags are defined in mavlink [https://mavlink.io/en/messages/common.html#MAV_WINCH_STATUS_FLAG](https://mavlink.io/en/messages/common.html#MAV_WINCH_STATUS_FLAG).


Multiple status fields can be set simultaneously. Mavlink does not specify which states are mutually exclusive. 


## Data Fields


bool [healthy](#structmavsdk_1_1_winch_1_1_status_flags_1a63f04e0567746c0646a9194bb1583cbd) {} - [Winch](classmavsdk_1_1_winch.md) is healthy.

bool [fully_retracted](#structmavsdk_1_1_winch_1_1_status_flags_1a51c658b256b68a3545c58b6827ace590) {} - [Winch](classmavsdk_1_1_winch.md) line is fully retracted.

bool [moving](#structmavsdk_1_1_winch_1_1_status_flags_1ada7685ec99d0c98b232bb53caea07b2f) {} - [Winch](classmavsdk_1_1_winch.md) motor is moving.

bool [clutch_engaged](#structmavsdk_1_1_winch_1_1_status_flags_1a65af9160f89a0d5785654e296075ad4d) {} - [Winch](classmavsdk_1_1_winch.md) clutch is engaged allowing motor to move freely.

bool [locked](#structmavsdk_1_1_winch_1_1_status_flags_1a371d1645c40a760ad79af898f826f171) {} - [Winch](classmavsdk_1_1_winch.md) is locked by locking mechanism.

bool [dropping](#structmavsdk_1_1_winch_1_1_status_flags_1a2be9f17e0179da027803db8e281d9cd7) {} - [Winch](classmavsdk_1_1_winch.md) is gravity dropping payload.

bool [arresting](#structmavsdk_1_1_winch_1_1_status_flags_1ac062b1893ba031799b1bdc15f294921f) {} - [Winch](classmavsdk_1_1_winch.md) is arresting payload descent.

bool [ground_sense](#structmavsdk_1_1_winch_1_1_status_flags_1ac4f6d986083de3f286de279a30dca15d) {} - [Winch](classmavsdk_1_1_winch.md) is using torque measurements to sense the ground.

bool [retracting](#structmavsdk_1_1_winch_1_1_status_flags_1a354c024b92ea68bb4403233da7ea4617) {} - [Winch](classmavsdk_1_1_winch.md) is returning to the fully retracted position.

bool [redeliver](#structmavsdk_1_1_winch_1_1_status_flags_1a469d932faaca0ef7f090233e4da7c33b) {} - [Winch](classmavsdk_1_1_winch.md) is redelivering the payload. This is a failover state if the line tension goes above a threshold during RETRACTING.

bool [abandon_line](#structmavsdk_1_1_winch_1_1_status_flags_1aaa8e61dd71fd9ef079c792fdce4683ea) {} - [Winch](classmavsdk_1_1_winch.md) is abandoning the line and possibly payload. [Winch](classmavsdk_1_1_winch.md) unspools the entire calculated line length. This is a failover state from REDELIVER if the number of attempts exceeds a threshold.

bool [locking](#structmavsdk_1_1_winch_1_1_status_flags_1a66c2970228f4006dd72a40ca48ed40a3) {} - [Winch](classmavsdk_1_1_winch.md) is engaging the locking mechanism.

bool [load_line](#structmavsdk_1_1_winch_1_1_status_flags_1aab19d12646c787b60e2658449f4b66ce) {} - [Winch](classmavsdk_1_1_winch.md) is spooling on line.

bool [load_payload](#structmavsdk_1_1_winch_1_1_status_flags_1aa1734d72a1e9fa4d72ba9faf9d979964) {} - [Winch](classmavsdk_1_1_winch.md) is loading a payload.


## Field Documentation


### healthy {#structmavsdk_1_1_winch_1_1_status_flags_1a63f04e0567746c0646a9194bb1583cbd}

```cpp
bool mavsdk::Winch::StatusFlags::healthy {}
```


[Winch](classmavsdk_1_1_winch.md) is healthy.


### fully_retracted {#structmavsdk_1_1_winch_1_1_status_flags_1a51c658b256b68a3545c58b6827ace590}

```cpp
bool mavsdk::Winch::StatusFlags::fully_retracted {}
```


[Winch](classmavsdk_1_1_winch.md) line is fully retracted.


### moving {#structmavsdk_1_1_winch_1_1_status_flags_1ada7685ec99d0c98b232bb53caea07b2f}

```cpp
bool mavsdk::Winch::StatusFlags::moving {}
```


[Winch](classmavsdk_1_1_winch.md) motor is moving.


### clutch_engaged {#structmavsdk_1_1_winch_1_1_status_flags_1a65af9160f89a0d5785654e296075ad4d}

```cpp
bool mavsdk::Winch::StatusFlags::clutch_engaged {}
```


[Winch](classmavsdk_1_1_winch.md) clutch is engaged allowing motor to move freely.


### locked {#structmavsdk_1_1_winch_1_1_status_flags_1a371d1645c40a760ad79af898f826f171}

```cpp
bool mavsdk::Winch::StatusFlags::locked {}
```


[Winch](classmavsdk_1_1_winch.md) is locked by locking mechanism.


### dropping {#structmavsdk_1_1_winch_1_1_status_flags_1a2be9f17e0179da027803db8e281d9cd7}

```cpp
bool mavsdk::Winch::StatusFlags::dropping {}
```


[Winch](classmavsdk_1_1_winch.md) is gravity dropping payload.


### arresting {#structmavsdk_1_1_winch_1_1_status_flags_1ac062b1893ba031799b1bdc15f294921f}

```cpp
bool mavsdk::Winch::StatusFlags::arresting {}
```


[Winch](classmavsdk_1_1_winch.md) is arresting payload descent.


### ground_sense {#structmavsdk_1_1_winch_1_1_status_flags_1ac4f6d986083de3f286de279a30dca15d}

```cpp
bool mavsdk::Winch::StatusFlags::ground_sense {}
```


[Winch](classmavsdk_1_1_winch.md) is using torque measurements to sense the ground.


### retracting {#structmavsdk_1_1_winch_1_1_status_flags_1a354c024b92ea68bb4403233da7ea4617}

```cpp
bool mavsdk::Winch::StatusFlags::retracting {}
```


[Winch](classmavsdk_1_1_winch.md) is returning to the fully retracted position.


### redeliver {#structmavsdk_1_1_winch_1_1_status_flags_1a469d932faaca0ef7f090233e4da7c33b}

```cpp
bool mavsdk::Winch::StatusFlags::redeliver {}
```


[Winch](classmavsdk_1_1_winch.md) is redelivering the payload. This is a failover state if the line tension goes above a threshold during RETRACTING.


### abandon_line {#structmavsdk_1_1_winch_1_1_status_flags_1aaa8e61dd71fd9ef079c792fdce4683ea}

```cpp
bool mavsdk::Winch::StatusFlags::abandon_line {}
```


[Winch](classmavsdk_1_1_winch.md) is abandoning the line and possibly payload. [Winch](classmavsdk_1_1_winch.md) unspools the entire calculated line length. This is a failover state from REDELIVER if the number of attempts exceeds a threshold.


### locking {#structmavsdk_1_1_winch_1_1_status_flags_1a66c2970228f4006dd72a40ca48ed40a3}

```cpp
bool mavsdk::Winch::StatusFlags::locking {}
```


[Winch](classmavsdk_1_1_winch.md) is engaging the locking mechanism.


### load_line {#structmavsdk_1_1_winch_1_1_status_flags_1aab19d12646c787b60e2658449f4b66ce}

```cpp
bool mavsdk::Winch::StatusFlags::load_line {}
```


[Winch](classmavsdk_1_1_winch.md) is spooling on line.


### load_payload {#structmavsdk_1_1_winch_1_1_status_flags_1aa1734d72a1e9fa4d72ba9faf9d979964}

```cpp
bool mavsdk::Winch::StatusFlags::load_payload {}
```


[Winch](classmavsdk_1_1_winch.md) is loading a payload.

