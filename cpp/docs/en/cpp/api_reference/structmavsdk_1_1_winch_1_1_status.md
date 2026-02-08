# mavsdk::Winch::Status Struct Reference
`#include: winch.h`

----


[Status](structmavsdk_1_1_winch_1_1_status.md) type. 


## Data Fields


uint64_t [time_usec](#structmavsdk_1_1_winch_1_1_status_1ac16c203027d78163509f1277eef6717d) {} - Time in usec.

float [line_length_m](#structmavsdk_1_1_winch_1_1_status_1a32c6d3d7df9065d23ec9be034f2b05e3) {} - Length of the line in meters.

float [speed_m_s](#structmavsdk_1_1_winch_1_1_status_1aa9b42cc41e9cb6e0f0e0eb75634eeb01) {} - Speed in meters per second.

float [tension_kg](#structmavsdk_1_1_winch_1_1_status_1a8db557ad854c3b8ed402c9fa6c3baa04) {} - Tension in kilograms.

float [voltage_v](#structmavsdk_1_1_winch_1_1_status_1a463d998f9f24f8cf027c9efc4519079d) {} - Voltage in volts.

float [current_a](#structmavsdk_1_1_winch_1_1_status_1a429c06e0b58c24130a05c7a410c352f6) {} - Current in amperes.

int32_t [temperature_c](#structmavsdk_1_1_winch_1_1_status_1a7dfd8af2b0e59381f8c66b1a5f1f1197) {} - Temperature in Celsius.

[StatusFlags](structmavsdk_1_1_winch_1_1_status_flags.md) [status_flags](#structmavsdk_1_1_winch_1_1_status_1aee139ac0ecbae6d02f6115f64fcb1b11) {} - [Status](structmavsdk_1_1_winch_1_1_status.md) flags.


## Field Documentation


### time_usec {#structmavsdk_1_1_winch_1_1_status_1ac16c203027d78163509f1277eef6717d}

```cpp
uint64_t mavsdk::Winch::Status::time_usec {}
```


Time in usec.


### line_length_m {#structmavsdk_1_1_winch_1_1_status_1a32c6d3d7df9065d23ec9be034f2b05e3}

```cpp
float mavsdk::Winch::Status::line_length_m {}
```


Length of the line in meters.


### speed_m_s {#structmavsdk_1_1_winch_1_1_status_1aa9b42cc41e9cb6e0f0e0eb75634eeb01}

```cpp
float mavsdk::Winch::Status::speed_m_s {}
```


Speed in meters per second.


### tension_kg {#structmavsdk_1_1_winch_1_1_status_1a8db557ad854c3b8ed402c9fa6c3baa04}

```cpp
float mavsdk::Winch::Status::tension_kg {}
```


Tension in kilograms.


### voltage_v {#structmavsdk_1_1_winch_1_1_status_1a463d998f9f24f8cf027c9efc4519079d}

```cpp
float mavsdk::Winch::Status::voltage_v {}
```


Voltage in volts.


### current_a {#structmavsdk_1_1_winch_1_1_status_1a429c06e0b58c24130a05c7a410c352f6}

```cpp
float mavsdk::Winch::Status::current_a {}
```


Current in amperes.


### temperature_c {#structmavsdk_1_1_winch_1_1_status_1a7dfd8af2b0e59381f8c66b1a5f1f1197}

```cpp
int32_t mavsdk::Winch::Status::temperature_c {}
```


Temperature in Celsius.


### status_flags {#structmavsdk_1_1_winch_1_1_status_1aee139ac0ecbae6d02f6115f64fcb1b11}

```cpp
StatusFlags mavsdk::Winch::Status::status_flags {}
```


[Status](structmavsdk_1_1_winch_1_1_status.md) flags.

