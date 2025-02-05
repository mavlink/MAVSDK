# Offboard Control

The [Offboard](../api_reference/classmavsdk_1_1_offboard.md) MAVSDK plugin provides a simple API for controlling the vehicle using velocity and yaw setpoints.
It is useful for tasks requiring direct control from a companion computer; for example to implement collision avoidance.

::: info
The API uses the PX4 [Offboard flight mode](https://docs.px4.io/master/en/flight_modes/offboard.html).
The class can only be used with copter and VTOL vehicles (not fixed wing - a PX4 limitation) and currently only supports *velocity setpoint commands* (PX4 additionally supports position and thrust setpoints).
:::

Client code must specify a setpoint before starting *Offboard mode*.
The Offboard plugin automatically resends setpoints at 20Hz (PX4 requires that setpoints are minimally resent at 2Hz).
If more precise control is required, clients can call the setpoint methods at whatever rate is required or whenever an updated setpoint is available.


## Create the Plugin

::: tip
`Offboard` objects are created in the same way as other SDK plugins.
General instructions are provided in the topic: [Using Plugins](../guide/using_plugins.md).
:::

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
1. Create a shared pointer to an instance of `Offboard` instantiated with the `system`:
   ```
   #include <mavsdk/plugins/offboard/offboard.h>
   auto offboard = Offboard{system};
   ```

The `offboard` pointer can then used to access the plugin API (as shown in the following sections).

## Starting/Stopping Offboard Mode

To use offboard mode you must first create a setpoint using any of the setpoint setter methods (e.g. [set_velocity_ned()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a4edbc6e4528ff955d4e46e7c4e711732) or [set_velocity_body()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1abe7364f0a48dda4df34c5c67d177cfb4)).
You can use any setpoint you like - the vehicle will start acting on the current setpoint as soon as the mode starts.

After you have created a setpoint call [start()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1ab71d0dd2a81f76e3a0330b0304daa30b) or [start_async()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a0c880ad3f663142e194dd6f187cfc934) to switch to offboard mode.

```cpp
// Create a setpoint before starting offboard mode (in this case a null setpoint)
offboard.set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});

// Start offboard mode.
Offboard::Result offboard_result = offboard.start();
if (result != Offboard::Result::Success) {
        std::cerr << "Offboard::start() failed: " << offboard_result << '\n';
    }
```

The methods return/complete with a [Result](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) indicating whether the command was successful.
Above we use the synchronous API, and then print a human readable string for the returned enum.

You can change the setpoints as needed (new setpoints replace any old setpoints).

To stop offboard mode call [Offboard::stop()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a626810cbfa02b36019dde2d2fd4c3da9) or [stop_async()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a86c163d7fa1217b4e82a03daf52065c3).
The SDK will then clear the current setpoint and put the vehicle into [Hold flight mode](https://docs.px4.io/master/en/flight_modes/hold.html).
The synchronous API is used as shown below:

```cpp
//Stop offboard mode
offboard_result = offboard.stop();
if (result != Offboard::Result::Success) {
        std::cerr << "Offboard::stop() failed: " << offboard_result << '\n';
    }
```

::: info
Offboard mode can also be stopped by moving the vehicle into another mode (e.g. using the `Action` API).
:::


## Velocity Setpoints

The API provides methods to set velocity and yaw components using the NED frame (`set_velocity_ned()`) and the body frame (`set_velocity_body()`).
The difference is that NED is relative to an absolute coordinate system (North, East, Down) while body frame is relative to the vehicle orientation (front, right, down).

The NED frame is used to move towards a specific compass direction or face the vehicle in a specific compass direction.
Body frame is usually used for tasks where the vehicle needs to *deviate* from the current path (e.g. to avoid an obstacle) or to rotate the vehicle at a specific rate.
Movement up/down is the same in either frame.

The following sections provide some common usage examples.

### Move in Compass Direction

The `set_velocity_ned()` can be used to move towards any particular compass direction - e.g. North, West, South-East, etc.

Calling `set_velocity_ned()` using an initialiser list type declaration for the [VelocityNEDYaw](../api_reference/structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) argument,
the first three values are the velocity components in North, East, and Down directions (in metres/second).

Examples:

* Head North at 3 m/s:
  ```cpp
  offboard.set_velocity_ned({3.0f, 0.0f, 0.0f, 0.0f});
  ```
* Head North-West with 5 m/s on each velocity component (notice that a negative value is required on the `east_m_s` value to move West):
  ```cpp
  offboard.set_velocity_ned({5.0f, -5.0f, 0.0f, 0.0f});
  ```


### Go Up or Down

Both coordinate systems use the same definition for "down", and both methods take an argument where the third value is used to specify the velocity component in this direction.
The following examples show how you set the velocity component down (positive) or up (negative) using the two methods:

Examples:

* Go *up* at 2 m/s (note, negative value to go up!):
  ```cpp
  offboard.set_velocity_ned({0.0f, 0.0f, -2.0f, 0.0f});
  ```
* Go down at 3 m/s:
  ```cpp
  offboard.set_velocity_body({0.0f, 0.0f, 3.0f, 0.0f});
  ```


### Turn/Yaw Vehicle to Face a Compass Direction

The `set_velocity_ned()` can be used to face the vehicle in a particular direction, independent of the direction of travel.
The direction is specified in clockwise degrees relative to North (0 is North, 90 is East, 180 is South, etc.)

Calling `set_velocity_ned()` using an initialiser list type declaration for the `VelocityNEDYaw` argument,
the final (fourth) value is the yaw direction.

Examples:
* Turn to face West:
  ```cpp
  offboard.set_velocity_ned({0.0f, 0.0f, 0.0f, 270.0f});
  ```
* Turn to face North:
  ```cpp
  offboard.set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
  ```

It is not possible to control the rate or direction that the vehicle will use to turn towards the setpoint direction (it will turn in whatever direction reaches the setpoint fastest).


### Turn/Yaw Vehicle in specified Direction/at Rate

The `set_velocity_body()` can be used to rotate the vehicle at a specific rate and in a specified direction.
This is set in [VelocityBodyYawspeed::yawspeed_deg_s](../api_reference/structmavsdk_1_1_offboard_1_1_velocity_body_yawspeed.md#structmavsdk_1_1_offboard_1_1_velocity_body_yawspeed_1a102d7011d9a87c6e103f1c1b92d169ee), as the angular rate in degrees/second. If viewed from above, the vehicle will turn clockwise if the value is positive and anticlockwise if it is negative.

Calling `set_velocity_body()` using an initialiser list type declaration the final (fourth) value is the yaw rate/direction.

Examples:

* Turn clock-wise at 60 degrees per second:
  ```cpp
  offboard.set_velocity_body({0.0f, 0.0f, 0.0f, 60.0f});
  ```
* Turn anti clock-wise at 5 degrees per second:
  ```cpp
  offboard.set_velocity_body({0.0f, 0.0f, 0.0f, -5.0f});
  ```

### Fly Forwards

Use `set_velocity_body()` to set the velocity components relative to the body frame.
To fly forwards, simply set the first parameter (`Offboard::VelocityBodyYawspeed::forward_m_s`) when the vehicle is not rotating.

```cpp
offboard.set_velocity_body({5.0f, 0.0f, 0.0f, 0.0f});
```

### Fly a Circle

To fly a circle, use `set_velocity_body()` with both forward and rotational components.
This will force the vehicle to travel in a curved path.

```cpp
offboard.set_velocity_body({5.0f, 0.0f, 0.0f, 30.0f});
```

You can force the vehicle to fly sideways by using the (`Offboard::VelocityBodyYawspeed::right_m_s` value), and in the other direction by using a negative rotation value:
```cpp
// Fly a circle sideways
offboard.set_velocity_body({0.0f, -5.0f, 0.0f, -30.0f});
```


## Position/Thrust Setpoints

The SDK does not support position or thrust setpoints (at time of writing).


## Waiting on Setpoints

The vehicle will obey the last setpoint called (when you call a setpoint the last one is cleared).
The SDK examples use timers to separate commands (e.g. `sleep_for(seconds(8))`) but in a real-world use case you might use telemetry or sensors to control when the setpoint is changed.


## Monitoring Offboard Mode

The vehicle may change out of offboard mode outside the control of your application (for example if a GCS were to put the vehicle into *Hold mode*).
In this case, the SDK will automatically stop sending setpoints and [Offboard::is_active()](../api_reference/classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1aa5e0f3c02a03f2667f82d5e162221ff5) will change from `true` to `false`.

Calls to change the setpoint do not return an error!
Depending on the particular use case, offboard code may need to explicitly monitor for flight mode and change behaviour appropriately (e.g. using [Telemetry::subscribe_flight_mode()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a53db5fb36bf10fbc7ac004a3be9100a4)).


## Further Information

Additional information/examples for the Offboard API are linked below:

* [Example: Offboard Velocity](../examples/offboard_velocity.md)
* Integration tests:
  * [offboard_velocity.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/offboard_velocity.cpp)


