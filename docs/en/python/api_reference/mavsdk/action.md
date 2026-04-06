# Action

Enable simple actions such as arming, taking off, and landing.

## Enums

### `OrbitYawBehavior`

Yaw behaviour during orbit flight.

| Value | Description |
|-------|-------------|
| `HOLD_FRONT_TO_CIRCLE_CENTER` (0) | |
| `HOLD_INITIAL_HEADING` (1) | |
| `UNCONTROLLED` (2) | |
| `HOLD_FRONT_TANGENT_TO_CIRCLE` (3) | |
| `RC_CONTROLLED` (4) | |

### `RelayCommand`

Commanded values for relays

| Value | Description |
|-------|-------------|
| `ON` (0) | |
| `OFF` (1) | |

### `ActionResult`

Possible results returned for action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `BUSY` (4) | |
| `COMMAND_DENIED` (5) | |
| `COMMAND_DENIED_LANDED_STATE_UNKNOWN` (6) | |
| `COMMAND_DENIED_NOT_LANDED` (7) | |
| `TIMEOUT` (8) | |
| `VTOL_TRANSITION_SUPPORT_UNKNOWN` (9) | |
| `NO_VTOL_TRANSITION_SUPPORT` (10) | |
| `PARAMETER_ERROR` (11) | |
| `UNSUPPORTED` (12) | |
| `FAILED` (13) | |
| `INVALID_ARGUMENT` (14) | |

## `Action`

Enable simple actions such as arming, taking off, and landing.

### `arm_async`

```python
def arm_async(callback: Callable, user_data: Any = None)
```

Send command to arm the drone.

Arming a drone normally causes motors to spin at idle.
Before arming take all safety precautions and stand clear of the drone!

### `arm`

```python
def arm()
```

Get arm (blocking)

### `arm_force_async`

```python
def arm_force_async(callback: Callable, user_data: Any = None)
```

Send command to force-arm the drone without any checks.

Attention: this is not to be used for normal flying but only bench tests!

Arming a drone normally causes motors to spin at idle.
Before arming take all safety precautions and stand clear of the drone!

### `arm_force`

```python
def arm_force()
```

Get arm_force (blocking)

### `disarm_async`

```python
def disarm_async(callback: Callable, user_data: Any = None)
```

Send command to disarm the drone.

This will disarm a drone that considers itself landed. If flying, the drone should
reject the disarm command. Disarming means that all motors will stop.

### `disarm`

```python
def disarm()
```

Get disarm (blocking)

### `takeoff_async`

```python
def takeoff_async(callback: Callable, user_data: Any = None)
```

Send command to take off and hover.

This switches the drone into position control mode and commands
it to take off and hover at the takeoff altitude.

Note that the vehicle must be armed before it can take off.

### `takeoff`

```python
def takeoff()
```

Get takeoff (blocking)

### `land_async`

```python
def land_async(callback: Callable, user_data: Any = None)
```

Send command to land at the current position.

This switches the drone to 'Land' flight mode.

### `land`

```python
def land()
```

Get land (blocking)

### `reboot_async`

```python
def reboot_async(callback: Callable, user_data: Any = None)
```

Send command to reboot the drone components.

This will reboot the autopilot, companion computer, camera and gimbal.

### `reboot`

```python
def reboot()
```

Get reboot (blocking)

### `shutdown_async`

```python
def shutdown_async(callback: Callable, user_data: Any = None)
```

Send command to shut down the drone components.

This will shut down the autopilot, onboard computer, camera and gimbal.
This command should only be used when the autopilot is disarmed and autopilots commonly
reject it if they are not already ready to shut down.

### `shutdown`

```python
def shutdown()
```

Get shutdown (blocking)

### `terminate_async`

```python
def terminate_async(callback: Callable, user_data: Any = None)
```

Send command to terminate the drone.

This will run the terminate routine as configured on the drone (e.g. disarm and open the parachute).

### `terminate`

```python
def terminate()
```

Get terminate (blocking)

### `kill_async`

```python
def kill_async(callback: Callable, user_data: Any = None)
```

Send command to kill the drone.

This will disarm a drone irrespective of whether it is landed or flying.
Note that the drone will fall out of the sky if this command is used while flying.

### `kill`

```python
def kill()
```

Get kill (blocking)

### `return_to_launch_async`

```python
def return_to_launch_async(callback: Callable, user_data: Any = None)
```

Send command to return to the launch (takeoff) position and land.

This switches the drone into [Return mode](https://docs.px4.io/master/en/flight_modes/return.html) which
generally means it will rise up to a certain altitude to clear any obstacles before heading
back to the launch (takeoff) position and land there.

### `return_to_launch`

```python
def return_to_launch()
```

Get return_to_launch (blocking)

### `goto_location_async`

```python
def goto_location_async(latitude_deg, longitude_deg, absolute_altitude_m, yaw_deg, callback: Callable, user_data: Any = None)
```

Send command to move the vehicle to a specific global position.

The latitude and longitude are given in degrees (WGS84 frame) and the altitude
in meters AMSL (above mean sea level).

The yaw angle is in degrees (frame is NED, 0 is North, positive is clockwise).

### `goto_location`

```python
def goto_location(latitude_deg, longitude_deg, absolute_altitude_m, yaw_deg)
```

Get goto_location (blocking)

### `do_orbit_async`

```python
def do_orbit_async(radius_m, velocity_ms, yaw_behavior, latitude_deg, longitude_deg, absolute_altitude_m, callback: Callable, user_data: Any = None)
```

Send command do orbit to the drone.

This will run the orbit routine with the given parameters.

### `do_orbit`

```python
def do_orbit(radius_m, velocity_ms, yaw_behavior, latitude_deg, longitude_deg, absolute_altitude_m)
```

Get do_orbit (blocking)

### `hold_async`

```python
def hold_async(callback: Callable, user_data: Any = None)
```

Send command to hold position (a.k.a. "Loiter").

Sends a command to drone to change to Hold flight mode, causing the
vehicle to stop and maintain its current GPS position and altitude.

Note: this command is specific to the PX4 Autopilot flight stack as
it implies a change to a PX4-specific mode.

### `hold`

```python
def hold()
```

Get hold (blocking)

### `set_actuator_async`

```python
def set_actuator_async(index, value, callback: Callable, user_data: Any = None)
```

Send command to set the value of an actuator.

Note that the index of the actuator starts at 1 and that the value goes from -1 to 1.

### `set_actuator`

```python
def set_actuator(index, value)
```

Get set_actuator (blocking)

### `set_relay_async`

```python
def set_relay_async(index, setting, callback: Callable, user_data: Any = None)
```

Send command to set the value of a relay.

The index of the relay starts at 0.
For the relay value, 1=on, 0=off, others possible depending on system hardware

### `set_relay`

```python
def set_relay(index, setting)
```

Get set_relay (blocking)

### `transition_to_fixedwing_async`

```python
def transition_to_fixedwing_async(callback: Callable, user_data: Any = None)
```

Send command to transition the drone to fixedwing.

The associated action will only be executed for VTOL vehicles (on other vehicle types the
command will fail). The command will succeed if called when the vehicle
is already in fixedwing mode.

### `transition_to_fixedwing`

```python
def transition_to_fixedwing()
```

Get transition_to_fixedwing (blocking)

### `transition_to_multicopter_async`

```python
def transition_to_multicopter_async(callback: Callable, user_data: Any = None)
```

Send command to transition the drone to multicopter.

The associated action will only be executed for VTOL vehicles (on other vehicle types the
command will fail). The command will succeed if called when the vehicle
is already in multicopter mode.

### `transition_to_multicopter`

```python
def transition_to_multicopter()
```

Get transition_to_multicopter (blocking)

### `get_takeoff_altitude_async`

```python
def get_takeoff_altitude_async(callback: Callable, user_data: Any = None)
```

Get the takeoff altitude (in meters above ground).

### `get_takeoff_altitude`

```python
def get_takeoff_altitude()
```

Get get_takeoff_altitude (blocking)

### `set_takeoff_altitude_async`

```python
def set_takeoff_altitude_async(altitude, callback: Callable, user_data: Any = None)
```

Set takeoff altitude (in meters above ground).

### `set_takeoff_altitude`

```python
def set_takeoff_altitude(altitude)
```

Get set_takeoff_altitude (blocking)

### `get_return_to_launch_altitude_async`

```python
def get_return_to_launch_altitude_async(callback: Callable, user_data: Any = None)
```

Get the return to launch minimum return altitude (in meters).

### `get_return_to_launch_altitude`

```python
def get_return_to_launch_altitude()
```

Get get_return_to_launch_altitude (blocking)

### `set_return_to_launch_altitude_async`

```python
def set_return_to_launch_altitude_async(relative_altitude_m, callback: Callable, user_data: Any = None)
```

Set the return to launch minimum return altitude (in meters).

### `set_return_to_launch_altitude`

```python
def set_return_to_launch_altitude(relative_altitude_m)
```

Get set_return_to_launch_altitude (blocking)

### `set_current_speed_async`

```python
def set_current_speed_async(speed_m_s, callback: Callable, user_data: Any = None)
```

Set current speed.

This will set the speed during a mission, reposition, and similar.
It is ephemeral, so not stored on the drone and does not survive a reboot.

### `set_current_speed`

```python
def set_current_speed(speed_m_s)
```

Get set_current_speed (blocking)

### `set_gps_global_origin`

```python
def set_gps_global_origin(latitude_deg, longitude_deg, absolute_altitude_m)
```

Get set_gps_global_origin (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
