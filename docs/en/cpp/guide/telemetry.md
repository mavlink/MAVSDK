# Telemetry

The [Telemetry](../api_reference/classmavsdk_1_1_telemetry.md) class is used to get vehicle telemetry, including state and flight mode information.

All the methods have both synchronous and asynchronous versions, and users can set the rate at which the vehicle provides updates for each type of information.
All the methods of a particular type (synchronous, asynchronous, and set_rate methods) are used in the same way.


## API Overview

The `Telemetry` API provides methods to return the following types of information:

* [AccelerationFrd](../api_reference/structmavsdk_1_1_telemetry_1_1_acceleration_frd.md)
* [ActuatorControlTarget](../api_reference/structmavsdk_1_1_telemetry_1_1_actuator_control_target.md)
* [ActuatorOutputStatus](../api_reference/structmavsdk_1_1_telemetry_1_1_actuator_output_status.md)
* [AngularVelocityBody](../api_reference/structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md)
* [AngularVelocityNed](../api_reference/structmavsdk_1_1_telemetry_1_1_angular_velocity_frd.md)
* [Battery](../api_reference/structmavsdk_1_1_telemetry_1_1_battery.md) - voltage and percentage power remaining.
* [Covariance](../api_reference/structmavsdk_1_1_telemetry_1_1_covariance.md)
* [EulerAngle](../api_reference/structmavsdk_1_1_telemetry_1_1_euler_angle.md) - vehicle attitude/orientation as Euler Angle
* [FixedwingMetrics](../api_reference/structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md)
* [GpsInfo](../api_reference/structmavsdk_1_1_telemetry_1_1_gps_info.md) - type of fix, if any, and number of satellites.
* [GroundTruth](../api_reference/structmavsdk_1_1_telemetry_1_1_ground_truth.md)
* [Health](../api_reference/structmavsdk_1_1_telemetry_1_1_health.md) - calibration status of various sensors and confirmation that position estimates are good enough for position control.
* [Imu](../api_reference/structmavsdk_1_1_telemetry_1_1_imu.md)
* [MagneticFieldFrd](../api_reference/structmavsdk_1_1_telemetry_1_1_magnetic_field_frd.md)
* [Odometry](../api_reference/structmavsdk_1_1_telemetry_1_1_odometry.md)
* [Position](../api_reference/structmavsdk_1_1_telemetry_1_1_position.md) - latitude and longitude in degrees, and altitude relative to sea level and to the takeoff altitude.
* [PositionBody](../api_reference/structmavsdk_1_1_telemetry_1_1_position_body.md)
* [PositionNed](../api_reference/structmavsdk_1_1_telemetry_1_1_position_ned.md)
* [VelocityNed](../api_reference/structmavsdk_1_1_telemetry_1_1_velocity_ned.md)
* [VelocityBody](../api_reference/structmavsdk_1_1_telemetry_1_1_velocity_body.md)
* [PositionVelocityNed](../api_reference/structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md)
* [Quaternion](../api_reference/structmavsdk_1_1_telemetry_1_1_quaternion.md) - vehicle attitude/orientation as a quaternion
* [RcStatus](../api_reference/structmavsdk_1_1_telemetry_1_1_rc_status.md) - connection status, signal strength, and whether RC has ever been connected.
* [StatusText](../api_reference/structmavsdk_1_1_telemetry_1_1_status_text.md)


In addition there are a number of methods that return vehicle "state":
* Current flight mode ([flight_mode()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4972a3968e379d565e7700f2f51158dd)).
* Whether the vehicle is "all healthy" (aggregates the vehicle [Health](../api_reference/structmavsdk_1_1_telemetry_1_1_health.md) information).
  This is used to check if the vehicle is *ready* to arm.
* Whether the vehicle is armed/disarmed.
* Whether the vehicle is flying/in air.


## Create the Plugin

::: tip
`Telemetry` objects are created in the same way as objects in other SDK plugins.
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
1. Create an instance of `Telemetry` with the `system`:
   ```
   #include <mavsdk/plugins/telemetry/telemetry.h>
   auto telemetry = Telemetry{system};
   ```

The `telemetry` object can then used to access the plugin API (as shown in the following sections).


## Setting the Update Rate {#update-rate}

The telemetry update rate determines the frequency at which callbacks will be invoked with new information, and also the probable "freshness" of data obtained when using synchronous telemetry APIs. The default update rate depends on the autopilot and may also be limited by the characteristics of the communications channel.
You can set the rate for *each* type of telemetry, and both synchronous or asynchronous rate-setting methods are provided.

::: info
For PX4 running on hardware the *default* rates are set on a per-mode basis in [mavlink_main.cpp](https://github.com/PX4/Firmware/blob/master/src/modules/mavlink/mavlink_main.cpp#L2025) (search on `configure_stream`).
For PX4 running on SITL, the default rates are defined in the [startup file](https://docs.px4.io/master/en/concept/system_startup.html).
:::

The rate-setting methods are all used in the same way, so we just show one example for both the asynchronous and synchronous methods below.
In both cases we set the rate for position updates.

To set the position update rate synchronously (in this case using [set_rate_position()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a665439f3d5f8c58b3ef3dd427cf4782b)):
```cpp
// Set position update rate to 1 Hz.
const Telemetry::Result set_rate_result = telemetry.set_rate_position(1.0);
if (set_rate_result != Telemetry::Result::Success) {
    // handle rate-setting failure (in this case print error)
    std::cout << "Setting rate failed:" << set_rate_result << '\n';
}
```

To set the position update rate asynchronously with [set_rate_position_async()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad7e5b576edb9398c8f5f2f14626b984a) (here we use a *promise* to block until we have a result):
```cpp
{
    std::cout << "Setting rate updates..." << '\n';

    auto prom = std::make_shared<std::promise<Telemetry::Result>>();
    auto future_result = prom->get_future();
    // Set position update rate to 1 Hz.
    telemetry.set_rate_position_async(1.0, [prom](Telemetry::Result result) {
        prom->set_value(result); //fulfill promise
    });

    //Block until promise is fulfilled (in callback function)
    const Telemetry::Result result = future_result.get();
    if (result != Telemetry::Result::Success) {
        // handle rate-setting failure (in this case print error)
        std::cout << "Setting telemetry rate failed (" << result << ")." << '\n';
    }
}
```


## Getting Regular Updates

The best way to get telemetry updates is to use the asynchronous methods.
These methods are non-blocking; they take a callback function argument and return immediately.
The callback will be invoked with a populated `struct` of the associated type as soon as an update message arrives from the vehicle.
The rate at which this occurs can be set through the API [as discussed above](#update-rate).

For example, the [Telemetry::subscribe_position()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a61bda57b3ca47000ea7e4758b2a33134) has the following prototype, where `callback` is called with a populated [PositionCallback](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a978b371d636226e198995462afa63552) function:
```cpp
void mavsdk::Telemetry::subscribe_position(PositionCallback callback)
```

The code fragment below shows this method being use with a lambda function for the callback, which simply prints out the current position and altitude).
```cpp
telemetry.subscribe_position([](Telemetry::Position position) {
    std::cout << "Altitude: " << position.relative_altitude_m << " m" << std::endl
              << "Latitude: " << position.latitude_deg << std::endl
              << "Longitude: " << position.longitude_deg << '\n';
});

```


## Detecting Changes (only)

The asynchronous callbacks are updated every time new information is provided by the vehicle.
For some types of telemetry you may only wish to report only when the value changes.

The example below shows how to use [subscribe_flight_mode()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a53db5fb36bf10fbc7ac004a3be9100a4) to report only when the current flight mode changes.
The example uses a lambda function callback that captures a variable for the last mode.
This variable is compared to the current flight mode to determine whether the value has changed and needs to be reported.

```cpp
// Set up callback to monitor flight mode 'changes'
Telemetry::FlightMode oldFlightMode=Telemetry::FlightMode::Unknown;
telemetry.subscribe_flight_mode([&oldFlightMode](Telemetry::FlightMode flightMode) {
    if (oldFlightMode != flightMode) {
        //Flight mode changed. Print!
        std::cout << "FlightMode: " << flightMode << '\n';
        oldFlightMode=flightMode; //Save updated mode.
    }
});
```

You can stop flight mode updates altogether with:
```cpp
telemetry.subscribe_flight_mode(nullptr);
```

This same approach can be used to report only messages that meet some condition.

::: tip
In future we may add a mechanism *in the API* to support just reporting changes: [Issue #63](https://github.com/mavlink/MAVSDK/issues/63).
:::


## Blocking Telemetry Requests (State Management)

Some commands/vehicle operations must be performed in a certain sequence, or can only be called when the vehicle reaches a certain state.
For example, in order to takeoff the vehicle must first be armed, and in order to arm, the vehicle must have a home position/GPS lock.
For these cases you will want to monitor for the vehicle being in a certain state and block other commands until it is ready to proceed.

Often the easiest approach is to use synchronous methods and poll for the result.
For example, the following code shows how you might poll for whether the vehicle is ready to arm ([health_all_ok()](../api_reference/classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad6d833741b5576f07204d268c5cd4d06)).

```cpp
// Check if vehicle is ready to arm
while (!telemetry.health_all_ok()) {
    std::cout << "Vehicle not ready to arm" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
// now ready to arm ...
```

Similarly, you can use the asynchronous method and block

```cpp
{
    std::cout << "Waiting for system to be ready" << '\n';
    auto prom = std::make_shared<std::promise<void>>();
    auto future_result = prom->get_future();
    telemetry.subscribe_health_all_ok(
    [prom](bool result) {
        //fulfill promise if health is OK
        if (result) {// health OK
            prom->set_value();
        }
    });

    future_result.get(); //Block until promise is fulfilled.
    // Now ready to arm
}
```
::: info
Note that `prom->set_value()` must only be called once.
If your code can potentially call this multiple times, remember to unsubscribe from the callback after it succeeds (e.g. using `telemetry.subscribe_health_all_ok(nullptr)`).
:::

Depending on the architecture of your application, you may even wish to arm the vehicle in your callback function.
Usually though it is easier to understand program flow using the approach above.


## Further Information

Additional information/examples for the Telemetry API are linked below:

* [SDK Examples](../examples/index.md)
* Integration tests:
  * [telemetry_async.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/telemetry_async.cpp)
  * [telemetry_health.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/telemetry_health.cpp)
  * [telemetry_modes.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/telemetry_modes.cpp)
  * [telemetry_sync.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/telemetry_sync.cpp)
