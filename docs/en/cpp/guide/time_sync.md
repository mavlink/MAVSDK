# Time Synchronization

MAVSDK can estimate the offset between its own clock and the clock of the autopilot using the [TIMESYNC](https://mavlink.io/en/messages/common.html#TIMESYNC) protocol.
The application can use the offset to convert between autopilot timestamps and local time, and MAVSDK uses it for the timestamps of the messages that the [Mocap](../api_reference/classmavsdk_1_1_mocap.md) plugin sends.

## Usage

Time synchronization is disabled by default and is enabled per system:

```cpp
system->enable_timesync();
```

MAVSDK then sends a `TIMESYNC` request every 5 seconds.
Only answers that arrive within 10 ms are used, so on slow links the offset may never become valid.

The state can be polled or subscribed to:

```cpp
const auto status = system->timesync_status();
if (status.synced) {
    std::cout << "Autopilot clock offset: " << status.offset_ns << " ns, RTT: " << status.rtt_ns << " ns\n";
}

auto handle = system->subscribe_timesync_status([](System::TimesyncStatus status) {
    // Called for every TIMESYNC answer and when the system disconnects.
});

// ...

system->unsubscribe_timesync_status(handle);
```

The offset is defined as autopilot time minus local time, where local time is `std::chrono::system_clock`:

```cpp
// Autopilot time (e.g. a time_boot_ms or time_usec field) to local time
const auto local = std::chrono::system_clock::time_point{
    std::chrono::nanoseconds(autopilot_time_ns - status.offset_ns)};
```

MAVSDK does not change the timestamps of incoming messages: they stay in the autopilot's time.

The [timesync example](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/timesync/) shows the status and converts an autopilot timestamp to local time.

::: info
The `TIMESYNC` time of PX4 is its time since boot, so the offset is roughly minus the time at which the autopilot booted.
:::

## Setting the system clock from the vehicle

`TIMESYNC` gives the offset to the autopilot's clock, not the current date and time, so it can't be used to set the clock of a companion computer.
For that, use the wall clock time that the autopilot gets from GPS, which it sends in [SYSTEM_TIME](https://mavlink.io/en/messages/common.html#SYSTEM_TIME), available as [Telemetry::subscribe_unix_epoch_time()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a172eb7793c61744a8195e38c0612ec1f).
It can for example be fed to [chrony](https://chrony-project.org/) as a reference clock, by an application that writes it to chrony's `SOCK` refclock.
Only use it once the autopilot's clock is valid, e.g. after it got a GPS fix.

## Language bindings

The same functions are available in the native bindings:

- C: `mavsdk_system_enable_timesync()`, `mavsdk_system_timesync_status()`, `mavsdk_system_subscribe_timesync_status()`, `mavsdk_system_unsubscribe_timesync_status()`
- Python: `System.enable_timesync()`, `timesync_status()`, `subscribe_timesync_status()`, `unsubscribe_timesync_status()`, and `timesync_status_updates()` in `mavsdk.asyncio`
- Kotlin: `System.enableTimesync()`, `getTimesyncStatus()`, `subscribeTimesyncStatus()`

Time synchronization is not available through *mavsdk_server*.
