# Example: Takeoff and Land

This simple example shows the basic use of many MAVSDK features.

It sets up a UDP connection, waits for a vehicle (system) to appear, arms it, and commands it to takeoff.
After a short wait the vehicle lands.
While flying the vehicle receives telemetry. The example is implemented in C++ (only).

::: tip
The full source code for the example [can be found here](https://github.com/mavlink/MAVSDK/tree/main/examples/takeoff_land).
:::

## Running the Example {#run_example}

The example is built and run [as described here](../examples/index.md#trying_the_examples) (the standard way).

The example terminal output should be similar to that shown below:

::: info
This is from a debug build of the SDK.
A release build will omit the "Debug" messages.
:::

```sh
$ ./takeoff_and_land udpin://0.0.0.0:14540
```
```sh
Waiting to discover system...
[06:40:03|Info ] New device on: 127.0.0.1:14557 (udp_connection.cpp:208)
[06:40:03|Debug] New: System ID: 1 Comp ID: 1 (dronecode_sdk_impl.cpp:292)
[06:40:03|Debug] Component Autopilot added. (system_impl.cpp:339)
[06:40:03|Debug] MAVLink: info: [logger] file: rootfs/fs/microsd/log/2018-07-09/0 (system_impl.cpp:277)
[06:40:04|Debug] Found 1 component(s). (system_impl.cpp:466)
[06:40:04|Debug] Discovered 4294967298 (system_impl.cpp:468)
Discovered system with UUID: 4294967298
Vehicle is getting ready to arm
Altitude: -0.004 m
Vehicle is getting ready to arm
Altitude: -0.007 m
Arming...
[06:40:07|Debug] MAVLink: info: ARMED by arm/disarm component command (system_impl.cpp:277)
Taking off...
[06:40:07|Debug] MAVLink: info: Using minimum takeoff altitude: 2.50 m (system_impl.cpp:277)
[06:40:07|Debug] MAVLink: info: Takeoff detected (system_impl.cpp:277)
[06:40:07|Debug] MAVLink: critical: Using minimum takeoff altitude: 2.50 m (system_impl.cpp:277)
Altitude: 0.96 m
Altitude: 2.044 m
Altitude: 2.381 m
...
Altitude: 2.502 m
Altitude: 2.5 m
Landing...
[06:40:17|Debug] MAVLink: info: Landing at current position (system_impl.cpp:277)
Altitude: 1.934 m
Altitude: 1.208 m
...
Altitude: -0.524 m
Finished...
```


## Source code {#source_code}

::: tip
The full source code for the example [can be found on Github here](https://github.com/mavlink/MAVSDK/tree/main/examples/takeoff_and_land).
:::

- [CMakeLists.txt](https://github.com/mavlink/MAVSDK/blob/main/examples/takeoff_and_land/CMakeLists.txt)
- [takeoff_and_land.cpp](https://github.com/mavlink/MAVSDK/blob/main/examples/takeoff_and_land/takeoff_and_land.cpp)
