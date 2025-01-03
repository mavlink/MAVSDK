# Server Plugins

MAVSDK _Server_ plugins are used to expose MAVLink services on non-MAVLink flight stacks, companions computers, cameras, gimbals and other components.

The plugins listen for commands from a ground station or other MAVLink client, pass them to the underlying native system using callbacks, and respond as needed. 

> **Note** They are called "server" plugins because they perform the server role in communication - listening for commands and responding.
> The client role is usually performed by a ground station (or "ordinary" MAVSDK plugin)


## Adding MAVLink support to your vehicle

Adding MAVLink support to your vehicle is as simple as instantiating the required server plugin and actioning the vehicle on the appropriate callback.

## Examples

The [Autopilot Server](./examples/autopilot_server.md) example uses many different MAVSDK server plugins to replicate a simple vehicle and autopilot system, with telemetry, mission handling, parameters and more.

## Plugins

The plugins developed specifically for this case are usually suffixed with `Server`, e.g.:

- [ActionServer](./api_reference/classmavsdk_1_1_action_server.md)
- [MissionRawServer](./api_reference/classmavsdk_1_1_mission_raw_server.md)
- [ParamServer](./api_reference/classmavsdk_1_1_param_server.md)
- [TelemetryServer](./api_reference/classmavsdk_1_1_telemetry_server.md)
- [TrackingServer](./api_reference/classmavsdk_1_1_tracking_server.md)

These server plugins can potentially also be used to test the "client" plugins of MAVSDK in the future. The [Autopilot Server](./examples/autopilot_server.md) example uses multiple MAVSDK instances in this way.
