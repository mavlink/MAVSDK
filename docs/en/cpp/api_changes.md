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
