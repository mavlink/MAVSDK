# mavsdk::System::TimesyncStatus Struct Reference
`#include: system.hpp`

----


Time synchronization state, see [enable_timesync()](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a7c7177fb0789aefbfb375f4bb12ce824). 


The autopilot's TIMESYNC clock (usually its time since boot) is: local time (std::chrono::system_clock) + [offset_ns](structmavsdk_1_1_system_1_1_timesync_status.md#structmavsdk_1_1_system_1_1_timesync_status_1aa2ea9343ef8b1d5acaf10ffb6fbc0cf9).


Incoming message timestamps are not changed, so use the offset to convert between the two clocks. 


## Data Fields


bool [synced](#structmavsdk_1_1_system_1_1_timesync_status_1ae571bb88d11f10a05166cc5d4482cd15) {false} - Whether the offset is valid.

int64_t [offset_ns](#structmavsdk_1_1_system_1_1_timesync_status_1aa2ea9343ef8b1d5acaf10ffb6fbc0cf9) {0} - [Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) time minus local time, in nanoseconds.

int64_t [rtt_ns](#structmavsdk_1_1_system_1_1_timesync_status_1a476af259437f69e10ffbc465d6df0002) {0} - Round trip time of the last TIMESYNC, in nanoseconds.


## Field Documentation


### synced {#structmavsdk_1_1_system_1_1_timesync_status_1ae571bb88d11f10a05166cc5d4482cd15}

```cpp
bool mavsdk::System::TimesyncStatus::synced {false}
```


Whether the offset is valid.


### offset_ns {#structmavsdk_1_1_system_1_1_timesync_status_1aa2ea9343ef8b1d5acaf10ffb6fbc0cf9}

```cpp
int64_t mavsdk::System::TimesyncStatus::offset_ns {0}
```


[Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) time minus local time, in nanoseconds.


### rtt_ns {#structmavsdk_1_1_system_1_1_timesync_status_1a476af259437f69e10ffbc465d6df0002}

```cpp
int64_t mavsdk::System::TimesyncStatus::rtt_ns {0}
```


Round trip time of the last TIMESYNC, in nanoseconds.

