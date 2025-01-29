# API Changes

This page tracks changes between versions.

It covers both breaking (incompatible) and non-breaking changes.

## Semantic Versioning

MAVSDK follows [semver/Semantic Versioning](https://semver.org/) conventions where as possible.

The version number has the format: **major.minor.patch**.
Individual parts of the number are iterated when the:

- **major**: API is changed, or functionality is removed.
- **minor**: API is extended, functionality is added.
- **patch**: API is not changed, functionality is not changed, but a bug is fixed.

This means that breaking changes to the API result in a bump of the major version number (e.g. 1.4.3 to 2.0.0).

> **note**: bumping of the major version is unrelated to the stability of the library. E.g. v2.0.0 is not by definition more stable than v1.4.18. It just means that the API has changed with v2. As development is carried on, stability is likely increasing whenever the minor or patch versions increase as incremental fixes are added.

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

**Note:**
It is still possible to change the configuration later (although not recommended) using `Mavsdk::set_configuration(...)`.

**Rationale:**

The reasons behind this change is to make users aware of this configuration and not assume any default. The problem with a ground station default is that it can be potentially dangerous without the user being aware. We are aware that this makes the API more cumbersome and verbose to use but the goal of Mavsdk is always to erro on the side of safety.

For instance, in the case where MAVSDK is used on the companion computer, the autopilot would assume that it is connected to a ground station at all time, even if the real ground station was lost.

Also, having to set the default makes it less likely to misconfigure the overall integration and end up with several components that conflict.

**Note:**
It is also possible to set the sysid/compid directly, just check out the API of [Mavsdk::Configuration](cpp/api_reference/classmavsdk_1_1_mavsdk_1_1_configuration.md).


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
auto camera_server = mavsdk::CameraServer{mavsdk.server_component()};
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
