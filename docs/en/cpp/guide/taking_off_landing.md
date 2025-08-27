# Actions (Take-off, Landing, Arming, etc)

The [Action](../api_reference/classmavsdk_1_1_action.md) class is used for commanding the vehicle to arm, takeoff, land, return home and land, disarm, kill and transition between VTOL modes.

Most of the methods have both synchronous and asynchronous versions.
The methods send commands to a vehicle, and return/complete with the vehicle's response.
It is important to understand that a successful response indicates whether or not the vehicle intends to act on the command, not that it has finished the action (e.g. arming, landing, taking off etc.).

::: info
The implication is that you may need to monitor for completion of actions!
:::


## Create the Plugin

::: tip
`Action` objects are created in the same way as for other MAVSDK plugins.
General instructions are provided in the topic: [Using Plugins](../guide/using_plugins.md).

The main steps are:

1. [Create a connection](../guide/connections.md) to a `system`. For example (basic code without error checking):
   ```
   #include <mavsdk/mavsdk.h>
   Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
   ConnectionResult conn_result = mavsdk.add_udp_connection();
   // Wait for the system to connect via heartbeat
   while (mavsdk.system().size() == 0) {
      sleep_for(seconds(1));
   }
   // System got discovered.
   System system = mavsdk.systems()[0];
   ```
1. Create an instance of `Action` with the `system`:
   ```
   #include <mavsdk/plugins/action/action.h>
   auto action = Action{system};
   ```

The `action` object can then used to access the plugin API (as shown in the following sections).

::: info
Some of the sections below additionally assume you have created a `Telemetry` instance that can be accessed using `telemetry`.
:::


## Taking Off

The recommended way to take off using the SDK (and PX4) is to use either of the [takeoff()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a0121d39baf922b1d88283230207ab5d0) or [takeoff_async()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1ab658d938970326db41709d83e02b41e6) methods. If a takeoff command is accepted the vehicle will change to the [Takeoff mode](https://docs.px4.io/master/en/flight_modes/takeoff.html), fly to the takeoff altitude, and then hover (in takeoff mode) until another instruction is received.

::: info
PX4/SDK also provides other ways to take off:
- A copter or VTOL will take off automatically if a mission is started (fixed-wing will not).
- You can also take off by manually driving the vehicle using the offboard API.
:::

The vehicle will only take off once *armed*, and can only arm once it is "healthy" (has been calibrated, the home position has been set, and there is a high-enough quality GPS lock). After it starts flying, code needs to check that takeoff is complete before sending additional instructions.


### Health Check

The code fragment below shows very simple code to synchronously poll for health status (using [Telemetry:health_all_ok()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad6d833741b5576f07204d268c5cd4d06)) prior to arming:

```cpp
// Wait until health is OK and vehicle is ready to arm
while (telemetry.health_all_ok() != true) {
    std::cout << "Vehicle not ready to arm ..." << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

The code fragment below performs the same task, but additionally exits the app if calibration is required reports what conditions are still required before the vehicle is "healthy":
<!-- update for subscribe_health_all_ok -->
```cpp
// Exit if calibration is required
Telemetry::Health check_health = telemetry.health();
bool calibration_required = false;
if (!check_health.gyrometer_calibration_ok) {
    std::cout << ERROR_CONSOLE_TEXT << "Gyro requires calibration." << NORMAL_CONSOLE_TEXT << '\n';
    calibration_required=true;
}
if (!check_health.accelerometer_calibration_ok) {
    std::cout << ERROR_CONSOLE_TEXT << "Accelerometer requires calibration." << NORMAL_CONSOLE_TEXT << '\n';
    calibration_required=true;
}
if (!check_health.magnetometer_calibration_ok) {
    std::cout << ERROR_CONSOLE_TEXT << "Magnetometer (compass) requires calibration." << NORMAL_CONSOLE_TEXT << '\n';
    calibration_required=true;
}
if (!check_health.level_calibration_ok) {
    std::cout << ERROR_CONSOLE_TEXT << "Level calibration required." << NORMAL_CONSOLE_TEXT << '\n';
    calibration_required=true;
}
if (calibration_required) {
    return 1;
}


// Check if ready to arm (reporting status)
while (telemetry.health_all_ok() != true) {
    std::cout << ERROR_CONSOLE_TEXT << "Vehicle not ready to arm. Waiting on:" << NORMAL_CONSOLE_TEXT << '\n';
    Telemetry::Health current_health = telemetry.health();
    if (!current_health.global_position_ok) {
        std::cout << ERROR_CONSOLE_TEXT << "  - GPS fix." << NORMAL_CONSOLE_TEXT << '\n';
    }
    if (!current_health.local_position_ok) {
        std::cout << ERROR_CONSOLE_TEXT << "  - Local position estimate." << NORMAL_CONSOLE_TEXT << '\n';
    }
    if (!current_health.home_position_ok) {
        std::cout << ERROR_CONSOLE_TEXT << "  - Home position to be set." << NORMAL_CONSOLE_TEXT << '\n';
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

::: info
Vehicle health can also be checked using [Telemetry:subscribe_health_all_ok()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0f94ea6d707ff314e412367d6681bd8f).
:::


### Arming

Once the vehicle is ready, use the following synchronous code to arm:
```cpp
// Arm vehicle
std::cout << "Arming..." << '\n';
const Action::Result arm_result = action.arm();

if (arm_result != Action::Result::Success) {
    std::cout << "Arming failed:" << arm_result <<  '\n';
    return 1; //Exit if arming fails
}
```

::: tip
If the `arm()` method returns `Action::Result::Success` then the vehicle is armed and can proceed to takeoff. This can be confirmed using [Telemetry::armed()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a6620142adc47f069262e5bf69dbb3876).
:::


### Get/Set Takeoff Altitude

The default/current takeoff altitude can be queried using [get_takeoff_altitude()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a85df48432c5ed2c6e23831409139ed39).
This target can be changed at any point before takeoff using [set_takeoff_altitude()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1ace2188fe367b3bb10b17b89c88d1f952).
The code fragment below shows how to set the takeoff altitude to 3 metres:
```cpp
action.set_takeoff_altitude(3.0);
```

### Takeoff Action

Once the vehicle is armed it can be commanded to take off.
The code below uses the synchronous `takeoff()` method, and fails if the vehicle refuses the command:
```cpp
// Command Take off
std::cout << "Taking off..." << '\n';
const Action::Result takeoff_result = action.takeoff();
if (takeoff_result != Action::Result::Success) {
    std::cout << "Takeoff failed:" << takeoff_result << '\n';
    return 1;
}
```

If the command succeeds the vehicle will takeoff, and hover at the takeoff altitude. Code should wait until takeoff has completed before sending the next instruction. Unfortunately there is no specific indicator to inform code that takeoff is complete.

::: info
One alternative is to simply wait for enough time that the vehicle *should* have reached the takeoff altitude.
:::

The code below checks for takeoff completion by polling the current altitude until the target altitude is reached:

```cpp
float target_alt=action.get_takeoff_altitude_m();
float current_position=0;
while (current_position<target_alt) {
    current_position = telemetry.position().relative_altitude_m;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
// Reached target altitude - continue with next instruction.
```


## Landing

The best way to land the vehicle at the current location is to use the [land()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1af6429e1bdb2875deebfe98ed53da3d41) or [land_async()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a89d146a766d49f1c706c66a3e2b9252d) methods.
If the command is accepted the vehicle will change to the [Land mode](https://docs.px4.io/master/en/flight_modes/land.html) and land at the current point.

::: info
The SDK does not at time of writing recommend other approaches for landing: land mission items are not supported and manually landing the vehicle using the offboard is not as safe.
:::

The code below shows how to use the land action.

```cpp
const Action::Result land_result = action.land();
if (land_result != Action::Result::Success) {
    //Land failed, so exit (in reality might try a return to land or kill.)
    return 1;
}
```

The vehicle should land and then automatically disarm.
If you want to monitor the landing you can trigger completion of the app based on the armed state, as shown below.

```cpp
while (telemetry.armed()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
std::cout << "Disarmed, exiting." << '\n';
```


## Return/RTL

[Return mode](https://docs.px4.io/master/en/flight_modes/return.html) (also known as "Return to Launch", "Return to Land", "Return to Home") flies the vehicle back to the home position and may also land the vehicle (depending on vehicle configuration).
This mode is invoked from `Action` using the [return_to_launch()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1afd7c225df0495b0947f00e7d2dd64877) or [return_to_launch_async()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1afd7c225df0495b0947f00e7d2dd64877) methods.

The code below shows how to use the synchronous method:

```cpp
const Action::Result rtl_result = telemetry.return_to_launch();
if (rtl_result != Action::Result::Success) {
    //RTL failed, so exit (in reality might send kill command.)
    return 1;
}
```

Depending on the vehicle it may land or hover around the return point. For vehicles that are configured to land you can use the same code as in the [previous section](#landing) to determine when the vehicle has disarmed.

## Disarm/Kill

Use the disarm or kill methods to stop the drone motors (the difference is that disarming will only succeed if the drone considers itself landed, while kill will disarm a vehicle even in flight).

The methods are listed below. These are used in the same way as other similar `Action` APIs:
- [disarm()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a44c61110965bcdd3dfb90a08d3c6b6b9), [disarm_async](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a3107f7f5a2f4a478024667f187f8f2aa)
- [kill()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1af40fc1ddf588b3796134a9303ebc3667), [kill_async](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a78c1f15c3b190ba94793045621819e69)


::: tip
PX4 will automatically disarm the vehicle after landing. The disarm methods explicitly invoke the same functionality.
:::


## Get/Set Cruise Speed

You can get/set the normal horizontal velocity used in *Return mode*, *Hold mode*, *Takeoff* (and other [AUTO Flight Modes](https://docs.px4.io/master/en/getting_started/flight_modes.html#categories) using the following methods:
* [set_maximum_speed()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a5fccee1636215bccf8d77d9dca15134e)
* [get_maximum_speed](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a128bf73fe8d0d359f36a3a9a327799ee)

::: info
These methods get/set the [MPC_XY_CRUISE](https://docs.px4.io/master/en/advanced_config/parameter_reference.html#MPC_XY_CRUISE) parameter.
They are used in the same way as the other `Action` methods.
:::



## Switch Between VTOL Modes {#transition_vtol}

`Action` provides methods to transition between VTOL fixed wing and multicopter modes, with both synchronous and asynchronous versions:
* [transition_to_fixedwing()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a8d5cf999a48ea3859ec75db27cf4fbda), [transition_to_multicopter()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1aac94bfb8613a8e9869e3620b3dc9bb8e)
* [transition_to_fixedwing_async()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1aa56181441cd64e092a8fb91a38c7c9fd), [transition_to_multicopter_async()](../api_reference/classmavsdk_1_1_action.md#classmavsdk_1_1_action_1a8c109076641b5c9aa6dd78ea8b913529)

The associated action will only be executed for VTOL vehicles (on other vehicle types the command will fail with a `Result` of `VTOL_TRANSITION_SUPPORT_UNKNOWN` or `NO_VTOL_TRANSITION_SUPPORT`).
The command will succeed if called when the vehicle is already in the mode.

The code fragment below shows how to call the synchronous action to transition to fixed wing,
and to print the result of the call (the other synchronous method is used in the same way).

```cpp
const Action::Result fw_result = action.transition_to_fixedwing();

if (fw_result != Action::Result::Success) {
    std::cout << "Transition to fixed wing failed: "
        << fw_result << '\n';
}
```


## Further Information

Additional information/examples for the Action API are linked below:

* [Example: Takeoff and Land](../examples/takeoff_and_land.md)
* [Example: VTOL Transitions](../examples/transition_vtol_fixed_wing.md)
* Integration tests:
  * [action_hover_async.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/action_hover_async.cpp)
  * [action_takeoff_and_kill.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/action_takeoff_and_kill.cpp)
  * [action_transition_multicopter_fixedwing.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/action_transition_multicopter_fixedwing.cpp)

