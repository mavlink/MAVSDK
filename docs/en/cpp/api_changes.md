# API Changes

This page tracks changes between versions.

It covers both breaking (incompatible) and non-breaking changes.

::: info
All examples below are assuming `using namespace mavsdk`.
:::

## Semantic Versioning

MAVSDK follows [semver/Semantic Versioning](https://semver.org/) conventions where as possible.

The version number has the format: **major.minor.patch**.
Individual parts of the number are iterated when the:

- **major**: API is changed, or functionality is removed.
- **minor**: API is extended, functionality is added.
- **patch**: API is not changed, functionality is not changed, but a bug is fixed.

This means that breaking changes to the API result in a bump of the major version number (e.g. 1.4.3 to 2.0.0).

::: info
Bumping of the major version is unrelated to the stability of the library. E.g. v2.0.0 is not by definition more stable than v1.4.18. It just means that the API has changed with v2. As development is carried on, stability is likely increasing whenever the minor or patch versions increase as incremental fixes are added.
:::

## v4

Most application code builds against v4 after renaming the includes and adapting to a few type changes. This section lists what changed, starting with what breaks the build.

::: info
Using MAVSDK from Python? From v4, the `mavsdk` package on PyPI is a new native binding. See [Migrating from MAVSDK-Python](../python/migration.md).
:::

### Headers are now .hpp

All public C++ headers were renamed from `.h` to `.hpp`, both in core and in the plugins:

```cpp
// v3
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>

// v4
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
```

There are no compatibility headers. The only exception is `mavsdk/mavsdk_export.h`, which keeps its name because it is shared with the C bindings.

The install directories are unchanged.

### Building and linking

- **C++ standard:** the library itself is now built as C++20. Applications need C++17 or later to use the headers, as with v3.
- **CMake:** the package and target are unchanged: `find_package(MAVSDK REQUIRED)` and `MAVSDK::mavsdk`. The package only accepts requests for the same major version, so `find_package(MAVSDK 3 ...)` has to become `find_package(MAVSDK 4 ...)`.
- **Relink:** the shared library's major version is part of its soname (`libmavsdk.so.4`), so applications have to be rebuilt against v4.
- **Static builds:** with `BUILD_SHARED_LIBS=OFF`, the CMake config now looks for `fmt`, `nlohmann_json` (instead of `jsoncpp`) and `libevents`, so `CMAKE_PREFIX_PATH` has to include where these are installed (for a superbuild, `build/third_party/install`). None of them appear in the public headers.
- **Building from source:** the C++ project moved into the `cpp/` directory. Configure with `cmake -S cpp -B build`, use `add_subdirectory(MAVSDK/cpp)` instead of `add_subdirectory(MAVSDK)`, and set `SOURCE_SUBDIR cpp` with `FetchContent`. Scripts moved from `tools/` to `cpp/tools/`. Without the superbuild, the system dependencies are now `nlohmann-json3-dev`, `libfmt-dev` and `libasio-dev` instead of `libjsoncpp-dev`.
- **Prebuilt packages:** `.deb` packages are no longer provided for Ubuntu 20.04 and Debian 11.

### System IDs are 32 bit

In preparation for 32-bit MAVLink system IDs, system IDs in the API are now `uint32_t` instead of `uint8_t`:

| API | v3 | v4 |
|---|---|---|
| `Mavsdk::Configuration(system_id, component_id, always_send_heartbeats)` | `uint8_t system_id` | `uint32_t system_id` |
| `Mavsdk::Configuration::get_system_id()` / `set_system_id()` | `uint8_t` | `uint32_t` |
| `System::get_system_id()` | `uint8_t` | `uint32_t` |
| `System::init(system_id, component_id)` | `uint8_t system_id` | `uint32_t system_id` |
| `MavlinkAddress::system_id` | `uint8_t` | `uint32_t` |
| `MavlinkPassthrough::CommandLong::target_sysid`, `CommandInt::target_sysid` | `uint8_t` | `uint32_t` |
| `MavlinkPassthrough::get_our_sysid()`, `get_target_sysid()`, `make_command_ack_message()` | `uint8_t` | `uint32_t` |

Component IDs stay `uint8_t`.

MAVSDK does not support system IDs above 255 yet. Configuring one, or passing one to `System::init()`, logs an error and aborts. `MavlinkDirect::send_message()` returns `Result::InvalidField` for target IDs above 255, where v3 silently truncated them.

Watch out for narrowing: assigning `get_system_id()` to a `uint8_t` still compiles, but may warn.

### Plugin API changes

- **Telemetry:** `home()` and `subscribe_home()` now provide a `Telemetry::HomePosition` instead of a `Telemetry::Position`. It has the same `latitude_deg`, `longitude_deg`, `absolute_altitude_m` and `relative_altitude_m` fields, plus the local position, orientation and approach vector from `HOME_POSITION`. Code using `auto` and those fields keeps compiling.

  ```cpp
  // v3
  Telemetry::Position home = telemetry.home();
  // v4
  Telemetry::HomePosition home = telemetry.home();
  ```

- **Shell:** `send()` takes the device to send to, and received data says which device it came from. `Shell::Device::Shell` is the MAVLink shell used in v3:

  ```cpp
  // v3
  shell.subscribe_receive([](std::string output) { std::cout << output; });
  shell.send("ver all");
  // v4
  shell.subscribe_receive([](Shell::Receive receive) { std::cout << receive.data; });
  shell.send("ver all", Shell::Device::Shell);
  ```

- **Info:** `get_flight_information()` was removed. Use `subscribe_flight_information()` instead.
- **Ftp:** `ListDirectoryData` has a single `entries` list of `Ftp::FilesystemEntry`, with the name, type (file or directory), size and modification time of each entry, instead of the separate `dirs` and `files` lists of names.

  ```cpp
  // v3
  for (const auto& file : data.files) { std::cout << file << '\n'; }
  // v4
  for (const auto& entry : data.entries) {
      if (entry.entry_type == Ftp::FilesystemEntry::EntryType::File) {
          std::cout << entry.name << " (" << entry.size_bytes << " bytes)\n";
      }
  }
  ```

### Enum values follow the proto definitions

Generated enums now carry the numeric values from the proto definitions. Before, they were numbered sequentially. Names are unchanged, so this only matters to code that stores, sends or casts enum values as integers. These enums changed:

| Enum | Changed values (v3 → v4) |
|---|---|
| `Camera::Storage::StorageType`, `CameraServer::StorageInformation::StorageType` | `Hd` 4 → 7, `Other` 5 → 254 |
| `Failure::FailureUnit` | `SystemBattery` 9 → 100, `SystemMotor` 10 → 101, `SystemServo` 11 → 102, `SystemAvoidance` 12 → 103, `SystemRcSignal` 13 → 104, `SystemMavlinkSignal` 14 → 105 |
| `Mission::Result` | `UnsupportedMissionCmd` 9 → 11, `TransferCancelled` 10 → 12, `NoSystem` 11 → 13, `Next` 12 → 14, `Denied` 13 → 15, `ProtocolError` 14 → 16, `IntMessagesNotSupported` 15 → 17 |
| `MissionRawServer::Result` | `UnsupportedMissionCmd` 9 → 11, `TransferCancelled` 10 → 12, `NoSystem` 11 → 13, `Next` 12 → 14 |
| `Rtk::Result` | `NoSystem` 3 → 5, `ConnectionError` 4 → 6 |
| `Telemetry::Odometry::MavFrame`, `TelemetryServer::Odometry::MavFrame` | `BodyNed` 1 → 8, `VisionNed` 2 → 16, `EstimNed` 3 → 18 |

The new `Shell::Device` enum is not sequential either, so don't derive its values from their position.

### MAVLink C headers are opt-in

The MAVSDK headers no longer include the MAVLink C headers, so `mavlink_message_t`, `MAV_TYPE`, `MAV_COMP_ID_...` and the other MAVLink C types, functions and macros are not available anymore just by including `mavsdk.hpp`.

The APIs that are built on those types, `MavlinkPassthrough` and `Mavsdk::intercept_incoming_messages_async()` / `intercept_outgoing_messages_async()`, are deprecated, and only available if `MAVSDK_ENABLE_MAVLINK_C_API` is defined before including MAVSDK. Including `mavlink_passthrough.hpp` without it fails with an error.

To keep using them for now, define it for your target:

```cmake
target_compile_definitions(your_app PRIVATE MAVSDK_ENABLE_MAVLINK_C_API)
```

This also brings back the MAVLink C headers. If your code only needs those, e.g. for `MAV_...` constants, you can also include your own copy of the MAVLink C headers instead.

To move away from these APIs, use [MavlinkDirect](guide/mavlink_direct.md) instead of MavlinkPassthrough, and `Mavsdk::subscribe_incoming_messages_json()` / `subscribe_outgoing_messages_json()` instead of the interception. Unlike MavlinkPassthrough, MavlinkDirect does not depend on the MAVLink dialect MAVSDK was built with.

### MAV_TYPE is an enum

`Mavsdk::Configuration::get_mav_type()` and `set_mav_type()` use the new `MavType` enum instead of a `uint8_t`, and `to_vehicle_from_mav_type()` takes a `MavType` instead of a `MAV_TYPE`. The values of `MavType` are the same as for `MAV_TYPE`.

```cpp
// v3
configuration.set_mav_type(MAV_TYPE_FIXED_WING);
// v4
configuration.set_mav_type(MavType::FixedWing);
```

### Behaviour changes

**Nothing handed out by Mavsdk may outlive it.** Destroying `Mavsdk` while a `System`, `ServerComponent` or plugin is still alive now aborts with an explanation. In v3, this was undefined behaviour that crashed later or went unnoticed. Look out for member declaration order, since members are destroyed in reverse:

```cpp
// Aborts on destruction: _action is destroyed after _mavsdk.
class Drone {
    std::optional<Action> _action;
    Mavsdk _mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
};

// Fine: _mavsdk is declared first, so it is destroyed last.
class Drone {
    Mavsdk _mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    std::optional<Action> _action;
};
```

The same applies to systems or plugins kept in globals, statics or containers that live longer than the `Mavsdk` instance.

**Slow subscribers lose updates, not results.** User callbacks still run one at a time on a single thread. In v3, once 100 callbacks were queued, any callback was dropped, including the result of a command, which could make a blocking call such as `Action::arm()` hang. In v4, results are never dropped. Instead, the oldest updates of subscriptions (e.g. telemetry) are dropped, so a slow subscriber gets recent data with gaps. The log message for this changed, see [Troubleshooting](troubleshooting.md#user_callbacks).

**Callbacks can still run shortly after unsubscribing.** This is not new, but easier to hit: a callback that is already queued still runs after `unsubscribe_...()` returns. Don't capture stack variables by reference in a callback that can outlive the function, e.g. a `std::promise` used to wait for one update. Capture a `std::shared_ptr` by value instead.

**Logging goes to stderr.** MAVSDK's default log output is now written to stderr instead of stdout. The log callback API is unchanged, but many messages were reworded, so check any code that matches on message text.

**Timing and connections.**

- A system is discovered on its first heartbeat, about a second earlier than in v3, and MAVSDK sends its own first heartbeat as soon as a connection is added.
- Sending over TCP and serial connections is asynchronous: a stalled peer no longer blocks MAVSDK, and write errors are reported through `subscribe_connection_errors()`. TCP client connections and hostname lookups happen in the background.
- `pass_received_raw_bytes()` processes the bytes asynchronously, and can be called from several threads.

**MavlinkDirect JSON.** The JSON is now always valid: strings are escaped, invalid UTF-8 is replaced, floats keep full precision, and `uint64` fields are no longer converted to signed. The `param_value` field of `PARAM_EXT_*` messages is now a byte array instead of a string.

**Gimbals** are only announced once their `GIMBAL_DEVICE_INFORMATION` has been received (or a few seconds of retries have passed), so that vendor and model are filled in.

**Camera definitions** from a vehicle are only looked up by file name, and the size of compressed definitions is limited.

### New in v4

Not breaking, but worth knowing about:

- The [heartbeat watchdog](guide/heartbeat_watchdog.md), to stop sending heartbeats when your application hangs.
- tlog recording with `Mavsdk::start_tlog_recording()`.
- The `MavlinkDirectServer` plugin, the server-side counterpart of MavlinkDirect.
- `Action::set_home()` and `Action::goto_location_fixedwing()`, camera focus control in `Camera` and `CameraServer`, mission upload with progress in `MissionRaw`.
- `to_string()` for all enums.
- C bindings (`libcmavsdk`), a native Python binding, and Kotlin bindings, all in the MAVSDK repository.

## v3

### Connections

In the past the connection syntax has caused quite a bit of confusion. Therefore, we decided to adapt the syntax for UDP and TCP connections slightly, mostly adopting the syntax that's already used in other software such as for instance pymavlink and mavlink-router.

The change makes it explicit whether we are listening on a port (think server), or connecting to a port (think client).

The new syntax for the 3 connection methods are described below:

#### UDP connections

```
- UDP in  (server): udpin://our_ip:port
- UDP out (client): udpout://remote_ip:port
```

For `udpin`, where we bind locally to that port, the IP can be set to:
- `0.0.0.0` to listen on all network interfaces.
- `127.0.0.1` to listen on the local loopback interface only.
- to our IP which means we listen on this network interfaces only.

#### TCP connections

```
- TCP in  (server):  tcpin://our_ip:port
- TCP out (client): tcpout://remote_ip:port
```

For `tcpin`, where we listen locally on that port, the IP can be set to:
- `0.0.0.0` to listen on all network interfaces.
- `127.0.0.1` to listen on the local loopback interface only.
- to our IP which means we listen on this network interfaces only.

#### Serial connections

```
- Serial: serial://dev_node:baudrate
- Serial with flow control: serial_flowcontrol://dev_node:baudrate
```

This is unchanged apart from the one caveat that the baudrate is no longer optional. This is to avoid cases where the baudrate is omitted by mistake.

### Mavsdk configuration

`ComponentType` is moved out of `Mavsdk` to its own scope, so the instantiation changes as follows:

Old instantiation:
```
Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
```

New instantiation:
```
Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
```

### Camera plugin: major redesign

The camera plugin API and internals have changed considerably. The aim was to be able to address multiple cameras from one plugin which enables more than one camera to be used in language wrappers such as Python. This wasn't previously possible.

Moreover, we now support MAVLink FTP and https to download the camera definition files, and not just http:

The API has changed to include a camera ID. Below is an example how usage changes:

```

auto camera = Camera{system};
camera.select_camera(0);

auto result = camera.zoom_range(component_id, 2.0f);
```

To:

```
auto camera = Camera{system};

// Assuming one camera is connected:
assert(camera.camera_list().cameras.size() == 1);
auto component_id = camera.camera_list().cameras[0].component_id;

auto result = camera.zoom_range(component_id, 2.0f);
```

### Rtk and LogStreaming plugins: base64 for binary data

With v2 it was hard or impossible to use APIs involving binary data in certain language wrappers, specifically Python. This meant that the RTK corrections and log streaming data were exposed as Python strings which didn't match the binary nature of the data.

Therefore, we are now using base64 for the data.

The assumption is that the performance hit of having to encode to and decode from base64 is negligible on platforms where MAVSDK is usually run.

Mavsdk provides helper functions to help with encoding and decoding base64:

```
#include <mavsdk/base64.h>

std::string encoded = ...;
std::vector<uint8_t> binary_data = base64_decode(encoded);
```

## v2

### Mavsdk configuration

The `Mavsdk` class no longer assumes the configuration of a ground station by default but instead a configuration needs to be passed as a constructor argument.

The configuration argument sets the identification, so the MAVLink `MAV_TYPE` and MAVLink component ID (also called compid) of the Mavsdk instance itself, so the source and not the target that you're connecting to.

Old instantiation:
```
Mavsdk mavsdk;
```

New instantiation:
```
Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
```

::: info
It is still possible to change the configuration later (although not recommended) using `Mavsdk::set_configuration(...)`.
:::

**Rationale:**

The reasons behind this change is to make users aware of this configuration and not assume any default. The problem with a ground station default is that it can be potentially dangerous without the user being aware. We are aware that this makes the API more cumbersome and verbose to use but the goal of Mavsdk is always to erro on the side of safety.

For instance, in the case where MAVSDK is used on the companion computer, the autopilot would assume that it is connected to a ground station at all time, even if the real ground station was lost.

Also, having to set the default makes it less likely to misconfigure the overall integration and end up with several components that conflict.

::: info
It is also possible to set the sysid/compid directly, just check out the API of [Mavsdk::Configuration](cpp/api_reference/classmavsdk_1_1_mavsdk_1_1_configuration.md).
:::

### Subscription API

The subscription API (also called async functions) has changed:

Old:

```
// Subscribe
telemetry.subscribe_position([](Telemetry::Position position) {
    std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
});

// Unsubscribe again
telemetry.subscribe_position(nullptr);
```

New:

```
// Subscribe
auto handle = telemetry.subscribe_position([](Telemetry::Position position) {
    std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
});

// Unsubscribe again
telemetry.unsubscribe_position(handle);
```

**Rationale:**
MAVSDK subscriptions had the drawback that there could only be one subscription per API. So subsequent subscribers would overwrite previous subscribers. With the new API there can be more than one subscriber per API, and each can unsubscribe using their own handle.


### Server component plugins

A lost of work has gone into Mavsdk v2 to support "server" components. These are components that typically run on the drone, e.g. a companion computer or a camera.

Server component plugins are all plugins that end in `_server`. They don't need a `System` to be instantiated as they are "connected to and don't care who connects to them".

For instance a camera server plugin can be used as follows:

```
Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::Camera}};
auto camera_server = CameraServer{mavsdk.server_component()};
```

It's also possible to add more than one MAVLink component to one MAVSDK instance, e.g. a gimbal could be added like this:


### Connecting to first autopilot system

Discovering vehicles/systems required quite a bit of boilerplate code in the past. This has been simplified using a `Mavsdk::first_autopilot(double timeout_s)` method.

Old discovering of first autopilot:

```
std::cout << "Waiting to discover system...\n";
auto prom = std::promise<std::shared_ptr<System>>{};
auto fut = prom.get_future();

// We wait for new systems to be discovered, once we find one that has an
// autopilot, we decide to use it.
mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
    auto system = mavsdk.systems().back();

    if (system->has_autopilot()) {
        std::cout << "Discovered autopilot\n";

        // Unsubscribe again as we only want to find one system.
        mavsdk.subscribe_on_new_system(nullptr);
        prom.set_value(system);
    }
});

// We usually receive heartbeats at 1Hz, therefore we should find a
// system after around 3 seconds max, surely.
if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
    std::cerr << "No autopilot found.\n";
    return {};
}

// Get discovered system now.
auto system = fut.get();
```

New simplified way:

```
auto system = mavsdk.first_autopilot(3.0);
```

## v1

### Linking

With version 1, we introduce bigger changes in the way the MAVSDK library is linked to. Essentially, the library is no longer split up in multiple library files, one for the core (e.g. `mavsdk.so`) and one each for every plugin (e.g. `mavsdk_action.so`) and instead everything is merged into one library file (usually `mavsdk.so`).

This means that linking in cmake gets easier.

Previously, this was required:
```
target_link_libraries(my_executable
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_mission
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
)
```

Now, this is all that is needed:
```
target_link_libraries(my_executable
    MAVSDK::mavsdk
)
```

#### Rationale

**Why were the "plugins" split up in the first place?**

- The plugins were split because they were supposed to be extendable plugins. However, the last few years have shown that this is not really required, as there were no requests or contributions for it.
- Also, having a real plugin architecture is not always easy. For instance, loading plugins at runtime with `dlopen` [is not possible](https://github.com/bpowers/musl/blob/master/src/ldso/dlopen.c) when the library is compiled with [musl](https://www.musl-libc.org/). (We use musl to compile `mavsdk_server` statically without dependencies, so that it can be shipped with PyPi and runs on any Linux distribution and version as well as cross-compiled on armv6/7/8).

**What are the advantages of the change?**

- Having one library to link to is easier and more inline with other libraries. When more functionality is needed, CMakeLists.txt doesn't need to be changed every time.
- Having only one library makes it easier to check for ABI breaks.
- The file overhead for every plugin is actually very small, so the overhead should be negligible.
