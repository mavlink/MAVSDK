# MAVSDK Paradigms/Usage

This topic provides general/overview information about how MAVSDK is used, designed and some limitations.

## Object Management

[Mavsdk](../api_reference/classmavsdk_1_1_mavsdk.md) is the main library class.
Applications must create a `Mavsdk` object and keep it alive as long as it or any of its [Systems](../api_reference/classmavsdk_1_1_system.md) are used.
The object is usually created as an automatic variable that is cleaned up when it goes out of scope (you can also dynamically create/destroy the object using `new`/`delete`).

API consumers use [Mavsdk](../api_reference/classmavsdk_1_1_mavsdk.md) to discover and connect to [System](../api_reference/classmavsdk_1_1_system.md) objects (vehicles/cameras etc.).

Access to drone information and control objects are provided [by plugins](../guide/using_plugins.md) (e.g. Telemetry, Action, Mission etc.).
Plugin objects are instantiated with a *specific* `System` object (a plugin instance must be created for every system that needs it).

Plugin objects are created as shared pointers that will be destroyed when all associated handles are out of scope.
All objects are automatically cleaned up when the parent `Mavsdk` object is destroyed.

:: info
The term plugin is a bit misleading as they are not "plugged in" (loaded at runtime), but modules compiled in.


## Error Handling

MAVSDK APIs do not raise exceptions! Instead, methods that can fail return success or an error reason as `enum` values.

::: tip
The error code usually reflects acknowledgment from the vehicle that it will perform the requested action (or not).
The operation itself may not yet have completed (e.g. taking off).
:::

The various classes also all provide stream operators for getting human readable strings from their associated result enum.
You can see how these are used in the example code.

## Callbacks

Some of the APIs use callbacks, e.g. subscriptions to a stream like [Telemetry::subscribe_position(PositionCallback callback)](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a61bda57b3ca47000ea7e4758b2a33134) or async functions like [Action::takeoff_async(ResultCallback callback)](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1ab658d938970326db41709d83e02b41e6).

All user callbacks are called from one thread.
Users must not do anything inside a callback that takes a long time to complete (e..g any IO), as this will stall subsequent callbacks.
For more information on how to work with and debug callbacks see [Troubleshooting](../troubleshooting.md#user_callbacks).


## Shared Vehicle Control

A vehicle can receive commands from multiple sources, including a Ground Control Station, or other MAVLink applications.

MAVSDK applications, which are running in environments where it is possible, can explicitly monitor for changes in flight mode (outside application control) and change behaviour appropriately (e.g. using [Telemetry::subscribe_flight_mode()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a53db5fb36bf10fbc7ac004a3be9100a4)).


## API Limitations/Behaviour

### Supported Vehicles

MAVSDK has been designed to manage *aircraft* that use the PX4 autopilot.
It has primarily been tested for use with multicopters, but also has basic support for fixed wing and [VTOL](../guide/vtol.md).

The APIs include methods that do not make sense for other vehicle types - including "takeoff" and "land".
While ground vehicles may work, they are not supported and are untested.
Similarly, other autopilots may well work, but, they are not explicitly supported and are untested.

Compatibility with ArduPilot is added piece by piece as functionality is used/tested with it.

### Connection Strings {#connection_string}

MAVSDK monitors a specified port for vehicles, see [Connecting to Systems (Vehicles)](../guide/connections.md).

### Connection Status

A system is considered to be disconnected (timed-out) if its heartbeat message is not detected within 3 seconds.

### Telemetry/Information

MAVSDK gets and stores vehicle state/telemetry information from received MAVLink messages.
The information is supplied to callback subscribers as soon as message updates are received.
Clients can also query the API synchronously, and will get the information from the last received message (depending on channel latency, this information will become increasingly "stale" between messages).

The rate at which update messages are sent by the vehicle can be specified using the API (but will be limited by the bandwidth of the channel).
Developers need to use a channel and a message update rate that allows their desired control algorithm to be effective - there is no point trying to use computer vision over an unreliable high-latency link.

`Info` information does not change for a particular vehicle, so will be accurate whenever read.


### Actions/Offboard

`Action` methods (and any other "vehicle instructions") return when the vehicle has confirmed that the message was received and will be acted on (or not).
The methods do not wait for the commanded action to complete.

So, for example, the [Action::land()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1af6429e1bdb2875deebfe98ed53da3d41) method returns as soon as the vehicle confirms it will land, but will actually land at some later point.

The implication is that developers will need to separately monitor the completion of the requested actions, if this is important to the application.


### Missions

The [Mission](../api_reference/classmavsdk_1_1_mission.html) and [MissionItem](../api_reference/structmavsdk_1_1_mission_1_1_mission_item.html) APIs provide a simplified but often useful *subset* of MAVLink mission commands as a developer-friendly API.

Not every mission command behaviour supported by the protocol and PX4 will be supported by the Mission plugins.

In order to access the full mission API, the [MissionRaw](../api_reference/classmavsdk_1_1_mission_raw.html) plugin can be used instead.

The MissionRaw also allows to [import QGC mission files](https://mavsdk.mavlink.io/main/en/cpp/api_reference/classmavsdk_1_1_mission_raw.html#classmavsdk_1_1_mission_raw_1a2a4ca261c37737e691c6954693d6d0a5).

