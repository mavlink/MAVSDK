# FAQ

## Why was the core of MAVSDK written in C++?

The aim was to have an API in a language which is cross-platform and has many language bindings.
The library also needs to be lightweight and fast, so it does not slow down for onboard usage at higher rate messaging.
Additionally, MAVSDK should be able to run efficiently in embedded setups, e.g. as part of an app or onboard a drone on a companion computer, as well as provide a simple and safe API. This favoured C++ over C as it allows for more expressive but type-safe APIs using standard library containers (e.g. `std::vector<MissionItem>`) for usage in embedded setups.

## Are multiple vehicles supported?

Yes.
- The MAVSDK C++ library allows C++ applications to connect to multiple vehicles at a time.
- Python, Swift, and other language wrappers can only connect to a _single vehicle at a time_.
  However you can instantiate multiple copies of wrappers in order to connect to multiple systems.

A maximum of 255 vehicles can be connected.

::: info
The maximum number of vehicles is defined by the MAVLink protocol.
MAVLink refers to vehicles as "systems", which are comprised of components (for example, a drone with a gimbal and a camera).
Each system has a network-unique MAVLink system ID, with a value between 1 and 255 (0 is a "broadcast address").
Each component in a system shares its system id, and has a system-unique component ID, again with a value 1 and 255.
:::

## Is MAVLink 1 supported?

No. MAVSDK only supports [MAVLink 2.0](https://mavlink.io/en/guide/mavlink_2.html).

## What sorts of vehicles are supported?

The MAVSDK API is designed for interacting with *aircraft*.
It has primarily been tested for use with multicopters, but also has basic support for fixed wing and [VTOL](cpp/guide/vtol.md).

The API *may* "work" with ground-based or other types of vehicles, but some methods will not make sense.
This use-case is mostly unsupported and untested.

## Does MAVSDK support indoor use?

Indoor use is supported, however, some modes such as mission or position control are not available indoor, unless some additional positioning method is available (e.g. optical flow, visual-inertial odometry, a motion capture system, etc.).

Note that PX4 currently does not support missions using "local coordinates" (i.e. meters) but only supports using "global coordinates" (i. e. latitude/longitude).

## What UAV flight stacks are supported?

MAVSDK, so far, is optimised for use with the PX4 flight stack and most testing is done against PX4.
That being said, we are more and more trying to test MAVSDK against ArduPilot as well.

While many features work for ArduPilot there are several implementation differences at the [MAVLink microservices level](https://mavlink.io/en/protocol/overview.html) which means that not every API will work.
For example, PX4 and ArduPilot implement the parameter protocol differently, and vary slightly in the mission upload/download protocol (e.g. ArduPilot uses the 0 entry as the home position).

::: info
MAVSDK welcomes contributions to better support flight stacks other than PX4.
We do however expect contributors to also help with testing and maintenance support for using the MAVSDK with their autopilot.
:::

## Are serial connections supported?

Yes. Serial, TCP, and UDP connections are supported, see [notes on connection](cpp/guide/connections.md).

## How are plugins added?

Check out [C++/Contributing/Plugins](cpp/contributing/plugins.md).

## Can a plugin depend on another one?

Yes - but it should not!
The idea is that plugins don't have any coupling between each other which allows for any plugin to be changed or removed without breaking the rest.
This will lead to some duplication at places, and that's an acceptable trade-off.
If the same code is used in many places, it can always be moved to core and get re-used from there.

We have sometimes moved functionality from plugins to the core if it was exposed in multiple plugins. As an example mission upload/download has been moved to the core so it can be used in the Mission plugin as well as the MissionRaw and Geofence plugins.

## Can MAVSDK run on an embedded platform / microcontroller?

MAVSDK is generally written at a bit higher level and using the POSIX interfaces for system calls, depending on the C++ standard library including threads.

As MAVSDK does not actually require too much CPU power, it could in theory be run on a microcontroller such as an ARM Cortex M4, however, it would require the POSIX APIs for serial and networking communication, as well as the C++ standard library for classes/functions such as `std::thread` or `std::vector`.

The recommendation for usage with a microcontroller would be to use the pure [C MAVLink headers](https://mavlink.io/en/mavgen_c/).

## Why is MAVLink Passthrough only available in C++?

The C++ MAVLink passthrough plugin basically exposes the direct C MAVLink API. While it would be nice to have access to all MAVLink messages in the language wrappers, there are some technical challenges:

- Essentially, it would mean that all MAVLink APIs would have to be duplicated by the [proto APIs](https://github.com/mavlink/MAVSDK-Proto/tree/main/protos).
  This would increase the API and code size considerably.
- Alternatively, the API could be exposed without types, using some sort of runtime access like `get_message("ATTITUDE").get_field("pitch").as_float()`.
  This would come with the cost of no type safety and runtime overhead for parsing the strings.

From a MAVSDK project point of view, there is also an advantage of not having a passthrough available in language wrappers; it encourages that required features are contributed back to the open-source project, rather than implemented in private using passthrough, and thus benefitting everyone.

## Why is gRPC used for the language wrappers?

There are multiple ways to support multiple programming languages all with their pros and cons:

1. Separate implementation per language:
   `+` Nice native API.
   `+` No non-native dependencies (often) in language wrappers.
   `-` Lots of implementation effort required, does not scale well with a small team.
   `-` Languages will not have consistency of feature parity.
2. Direct language bindings based on [Swig](http://www.swig.org/).
   `+` Many languages "almost for free".
   `-` Not always optimal/super clean API, unless another layer is written manually.
   `-` Requires linking to non-native dependencies.
3. Direct language bindings manually written, or based on various tools like [pybind11](https://github.com/pybind/pybind11).
   `+` Some languages "almost for free".
   `-` Varies on the language. If done manually likely to have bugs.
   `-` Requires linking to nan-native dependencies.
4. Language bindings based on proto definitions (protobuf) and gRPC.
   `+` Nice native APIs can be auto-generated from proto files.
   `+` No non-native dependencies in language wrappers.
   `+` mavsdk_server (gRPC server) can be run on other machine, or over network, in cloud, etc.
   `-` gRPC requires quite a few dependencies.

When writing the language wrappers for MAVSDK we decided to try gRPC, however, we underestimated just how much work was required to write all the auto-generation for the various function signatures (sync, async, result handling, streams, etc.), and also were not aware about the pain that comes with every dependency.

You can [read more about the auto-generation](cpp/contributing/autogen.md), and [learn how to add functionality](cpp/contributing/plugins.md#add-api-to-proto).

We are not ruling out direct-bindings for the future, there is e.g. a [prototype for Python using pybind11](https://github.com/mavlink/MAVSDK/pull/1283), so this is an ongoing topic.

## Use of deleted function Mavsdk()

```
error: use of deleted function ‘mavsdk::Mavsdk::Mavsdk()’

mavsdk::Mavsdk mavsdk;
```

The API to construct `Mavsdk` has changed with v2. Check [API changes](cpp/api_changes.md):

## What is the difference between unit, system, and integration tests?

The unit tests are only concerned with one class or less. It's often used for MAVSDK internal logic or helpers.

System tests are used to test the MAVLink implementation, by running a plugin against it's server plugin counterpart. This way we can test both at the same time while keeping the tests fast because they don't rely on an simulation that would need to be started up.

We have opted to slowly fade out integration tests and use a few examples instead (also see (explanation)[documentation.md#integration_tests]).

## Why are the plugins called plugins?

The initial idea was to support runtime plugins. However, this turned out to be very tricky when combined with language wrappers. To avoid that huge unnecessary complexity we opted for compile-time modules instead but left the name plugin as is.
