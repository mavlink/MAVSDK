# Example: Fly Mission

The [Fly Mission](https://github.com/mavlink/MAVSDK/tree/main/examples/fly_mission) example shows how to create, upload, and run, pause, and restart missions using the SDK.

![Fly Mission QGC Screenshot](../../../assets/examples/fly_mission/fly_mission_example_qgc.jpg)


## Running the Example {#run_example}

The example is built and run in the normal way ([as described here](../examples/index.md#trying_the_examples)).

The example terminal output should be similar to that shown below:

::: info
This is from a debug build of the SDK.
A release build will omit the "Debug" messages.
:::

```
$ ./fly_mission udpin://0.0.0.0:14540
```
```
Waiting to discover system...
[09:36:08|Info ] New device on: 127.0.0.1:14557 (udp_connection.cpp:208)
[09:36:08|Debug] New: System ID: 1 Comp ID: 1 (dronecode_sdk_impl.cpp:292)
[09:36:08|Debug] Component Autopilot added. (system_impl.cpp:339)
[09:36:08|Debug] MAVLink: info: [logger] file: rootfs/fs/microsd/log/2018-07-09/1 (system_impl.cpp:277)
[09:36:09|Debug] Found 1 component(s). (system_impl.cpp:466)
[09:36:09|Debug] Discovered 4294967298 (system_impl.cpp:468)
Discovered system with UUID: 4294967298
Waiting for system to be ready
System ready
Creating and uploading mission
Uploading mission...
[09:36:10|Debug] Send mission item 0 (mission_impl.cpp:898)
[09:36:10|Debug] Send mission item 1 (mission_impl.cpp:898)
...
[09:36:10|Debug] Send mission item 19 (mission_impl.cpp:898)
[09:36:10|Debug] Send mission item 20 (mission_impl.cpp:898)
[09:36:10|Debug] Send mission item 21 (mission_impl.cpp:898)
[09:36:10|Debug] Send mission item 22 (mission_impl.cpp:898)
[09:36:10|Info ] Mission accepted (mission_impl.cpp:163)
Mission uploaded.
Arming...
[09:36:10|Debug] MAVLink: info: ARMED by arm/disarm component command (system_impl.cpp:277)
Armed.
Starting mission.
Started mission.
Mission status update: 0 / 6
[09:36:10|Debug] MAVLink: info: Executing Mission (system_impl.cpp:277)
[09:36:10|Debug] MAVLink: info: Takeoff to 10.0 meters above home. (system_impl.cpp:277)
[09:36:10|Debug] MAVLink: info: Takeoff detected (system_impl.cpp:277)
Mission status update: 0 / 6
Mission status update: 0 / 6
Mission status update: 1 / 6
...
Mission status update: 1 / 6
Mission status update: 2 / 6
Mission status update: 2 / 6
Pausing mission...
Mission paused.
[09:36:28|Debug] MAVLink: critical: MANUAL CONTROL LOST (at t=41381ms) (system_impl.cpp:277)
[09:36:28|Debug] MAVLink: info: MANUAL CONTROL REGAINED after 541ms (system_impl.cpp:277)
Resuming mission...
Resumed mission.
Mission status update: 2 / 6
...
Mission status update: 2 / 6
Mission status update: 3 / 6
...
Mission status update: 3 / 6
Mission status update: Mission status update: 44 / 6 / 6

Mission status update: 4 / 6
...
Mission status update: 4 / 6
Mission status update: 5 / 6
...
Mission status update: 5 / 6
[09:37:00|Debug] MAVLink: info: Mission finished, loitering. (system_impl.cpp:277)
Mission status update: 6 / 6
Commanding RTL...
Commanded RTL.
[09:37:01|Debug] MAVLink: info: RTL HOME activated (system_impl.cpp:277)
[09:37:01|Debug] MAVLink: info: RTL: climb to 518 m (31 m above home) (system_impl.cpp:277)
[09:37:09|Debug] MAVLink: info: RTL: return at 518 m (31 m above home) (system_impl.cpp:277)
[09:37:16|Debug] MAVLink: info: RTL: descend to 493 m (5 m above home) (system_impl.cpp:277)
[09:37:41|Debug] MAVLink: info: RTL: loiter 5.0s (system_impl.cpp:277)
[09:37:46|Debug] MAVLink: info: RTL: land at home (system_impl.cpp:277)
[09:37:56|Debug] MAVLink: info: Landing detected (system_impl.cpp:277)
[09:37:59|Debug] MAVLink: info: DISARMED by auto disarm on land (system_impl.cpp:277)
Mission status update: 6 / 6
Disarmed, exiting.
```

## How it works

The operation of most of this code is discussed in the guide: [Missions](../guide/missions.md).

## Source code {#source_code}

- [CMakeLists.txt](https://github.com/mavlink/MAVSDK/blob/main/examples/fly_mission/CMakeLists.txt)
- [fly_mission.cpp](https://github.com/mavlink/MAVSDK/blob/main/examples/fly_mission/fly_mission.cpp)
