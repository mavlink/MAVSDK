# Example: Autopilot Server

The [Autopilot Server](https://github.com/mavlink/MAVSDK/tree/main/examples/autopilot_server) example creates two instances of MAVSDK, representing a client (GCS) and a server (Vehicle).
The MAVSDK instances communicates with each other via UDP transmitting telemetry, publishing parameters, missions and takeoff commands.

::: info
This example shows how to use [MAVSDK Server Plugins](../server_plugins.md)!
This is a good starting point if you are looking to implement MAVLink services to a non-MAVLink compatible system.
:::


## Running the Example {#run_example}

The example is built and run in the normal way ([as described here](../examples/index.md#trying_the_examples)).

The example terminal output should be similar to that shown below:

::: info
This is from a debug build of the SDK.
A release build will omit the "Debug" messages.
:::

```
$ ./autopilot_server
```
```
[04:44:24|Info ] MAVSDK version: v0.44.0 (mavsdk_impl.cpp:29)
[04:44:24|Info ] MAVSDK version: v0.44.0 (mavsdk_impl.cpp:29)
Connected!
[04:44:24|Error] Sending message failed (mavsdk_impl.cpp:278)
[04:44:24|Debug] Initializing connection to remote system... (mavsdk_impl.cpp:473)
Connected autopilot server side!
Sleeping thread 
[04:44:24|Info ] New system on: 127.0.0.1:40799 (with sysid: 1) (udp_connection.cpp:194)
[04:44:24|Debug] New: System ID: 1 Comp ID: 1 (mavsdk_impl.cpp:475)
[04:44:24|Debug] Component Autopilot (1) added. (system_impl.cpp:372)
[04:44:24|Debug] New: System ID: 245 Comp ID: 190 (mavsdk_impl.cpp:222)
[04:44:24|Debug] Component Unsupported component (190) added. (system_impl.cpp:372)
[04:44:25|Debug] Discovered 1 component(s) (system_impl.cpp:520)
Discovered Autopilot from Client
[04:44:25|Debug] Autopilot Capabilities Request (system_impl.cpp:297)
Setting System
MissionRawServer created
[04:44:35|Debug] Setting interval for msg id: 245 interval_ms:1000 (telemetry_server_impl.cpp:38)
[04:44:35|Debug] Request Param CAL_GYRO0_ID (mavlink_parameters.cpp:756)
[04:44:35|Debug] Request Param CAL_ACC0_ID (mavlink_parameters.cpp:756)
[04:44:35|Debug] Request Param CAL_MAG0_ID (mavlink_parameters.cpp:756)
[04:44:35|Debug] Request Param SYS_HITL (mavlink_parameters.cpp:756)
[04:44:40|Warn ] command unsupported (512). (mavlink_commands.cpp:205)
[04:44:41|Debug] Falling back to gimbal protocol v1 (mission_impl.cpp:131)
[04:44:45|Debug] Request Param my_param (mavlink_parameters.cpp:756)
Found Param my_param: 1
Creating Mission
Uploading mission...
SystemID1
[04:44:45|Debug] Receive Mission Count in Server (mission_raw_server_impl.cpp:150)
Received Uploaded Mission!
mission_plan:
{
    mission_items: [mission_item:
{
    seq: 0
    frame: 6
    command: 16
    current: 1
    autocontinue: 1
    param1: 0.5
    param2: 1
    param3: 0
    param4: nan
Mission uploaded.    x: 
473981703
    y: 85456490
    z: 10
    mission_type: 0
}, mission_item:
{
    seq: 1
    frame: 2
    command: 178
    current: 0
    autocontinue: 1
    param1: 1
    param2: 5
    param3: -1
    param4: 0
    x: 0
    y: 0
    z: nan
    mission_type: 0
}, mission_item:
{
    seq: 2
    frame: 2
    command: 204
    current: 0
    autocontinue: 1
    param1: 2
    param2: 0
    param3: 0
    param4: 1
    x: 0
    y: 0
    z: 2
    mission_type: 0
}, mission_item:
{
    seq: 3
    frame: 2
    command: 205
    current: 0
    autocontinue: 1
    param1: 20
    param2: 0
    param3: 60
    param4: nan
    x: 0
    y: 0
    z: 2
    mission_type: 0
}]
}
Current Item Changed!
Current Item: mission_item:
{
    seq: 0
    frame: 6
    command: 16
    current: 1
    autocontinue: 1
    param1: 0.5
    param2: 1
    param3: 0
    param4: nan
    x: 473981703
    y: [8545649004:44:45|Debug] 
    z: 10
    mission_type: 0
}
current: 0, total: 1Current Item Changed! (
Current Item: mission_item:
{
mission_impl.cpp    seq: 1
    frame: 2
    command: 178
    current: 0
    autocontinue: 1
    param1: :1839)
Current: 0
Total: 1

    param2: 5
    param3: -1
    param4: 0
    x: 0
    y: 0
    z: nan
    mission_type: 0
}
[Current Item Changed!
Current Item: mission_item:
{
    seq: 2
    frame: 2
    command: 204
    current: 0
    autocontinue: 1
    param1: 2
    param2: 0
    param3: 0
    param4: 1
    x: 0
    y: 0
    z: 2
    mission_type: 0
}
Current Item Changed!
Current Item: mission_item:
{
    seq: 3
    frame: 2
    command: 205
    current: 0
    autocontinue: 1
    param1: 20
    param2: 0
    param3: 60
    param4: nan
    x: 0
    y: 0
    z: 2
    mission_type: 0
}
Current Item Changed!
Current Item: mission_item:
{
    seq: 0
    frame: 6
    command: 16
    current: 1
    autocontinue: 1
    param1: 0.5
    param2: 1
    param3: 0
    param4: nan
    x: 473981703
    y: 85456490
    z: 10
    mission_type: 0
}
04:44:45|Debug] current: 1, total: 1 (mission_impl.cpp:839)
Current: 1
Total: 1
Clear All Mission!
Clear Mission Request
[04:45:05|Debug] Setting interval for msg id: 33 interval_ms:1000 (telemetry_server_impl.cpp:38)
Arming...
Altitude: 0 m
[04:45:05|Debug] Set Param Request: MIS_TAKEOFF_ALT (mavlink_parameters.cpp:724)
Taking off...
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
Altitude: 10 m
...
```

## How it works

By creating two MAVSDK instances on separate threads, configuring them and then using different plugins on each, we are able to create a full MAVLink system (GCS <-> Vehicle)
in one program.

Each plugin (and it's respective server plugin) implements a particular MAVLink service.
By utilising the server plugins and client plugins we are able to create a full MAVLink
system, without the need for a separate external autopilot.

## Source code {#source_code}

- [CMakeLists.txt](https://github.com/mavlink/MAVSDK/blob/main/examples/autopilot_server/CMakeLists.txt)
- [autopilot_server.cpp](https://github.com/mavlink/MAVSDK/blob/main/examples/autopilot_server/autopilot_server.cpp)
