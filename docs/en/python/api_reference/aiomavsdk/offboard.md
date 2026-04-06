# Offboard

Control a drone with position, velocity, attitude or motor commands.

The module is called offboard because the commands can be sent from external sources
as opposed to onboard control right inside the autopilot "board".

Client code must specify a setpoint before starting offboard mode.
Mavsdk automatically sends setpoints at 20Hz (PX4 Offboard mode requires that setpoints
are minimally sent at 2Hz).

## `OffboardAsync`

Control a drone with position, velocity, attitude or motor commands.

The module is called offboard because the commands can be sent from external sources
as opposed to onboard control right inside the autopilot "board".

Client code must specify a setpoint before starting offboard mode.
Mavsdk automatically sends setpoints at 20Hz (PX4 Offboard mode requires that setpoints
are minimally sent at 2Hz).

Async wrapper around :class:`Offboard` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `start`

```python
async def start()
```

Start offboard control.

Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `stop`

```python
async def stop()
```

Stop offboard control.

The vehicle will be put into Hold mode: https://docs.px4.io/en/flight_modes/hold.html

Raises
------
OffboardError
    If the request fails. The error contains the reason for the failure.

### `is_active`

```python
async def is_active()
```

Check if offboard control is active.

True means that the vehicle is in offboard mode and we are actively sending
setpoints.

Returns
-------
is_active : bool
Raises
------
OffboardError
    If the request fails. The error contains the reason for the failure.

### `set_attitude`

```python
async def set_attitude(attitude)
```

Set the attitude in terms of roll, pitch and yaw in degrees with thrust.

Parameters
----------
attitude : Attitude
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_actuator_control`

```python
async def set_actuator_control(actuator_control)
```

Set direct actuator control values to groups #0 and #1.

First 8 controls will go to control group 0, the following 8 controls to control group 1 (if
actuator_control.num_controls more than 8).

Parameters
----------
actuator_control : ActuatorControl
Raises
------
OffboardError
    If the request fails. The error contains the reason for the failure.

### `set_attitude_rate`

```python
async def set_attitude_rate(attitude_rate)
```

Set the attitude rate in terms of pitch, roll and yaw angular rate along with thrust.

Parameters
----------
attitude_rate : AttitudeRate
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_position_ned`

```python
async def set_position_ned(position_ned_yaw)
```

Set the position in NED coordinates and yaw.

Parameters
----------
position_ned_yaw : PositionNedYaw
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_position_global`

```python
async def set_position_global(position_global_yaw)
```

Set the position in Global coordinates (latitude, longitude, altitude) and yaw

Parameters
----------
position_global_yaw : PositionGlobalYaw
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_velocity_body`

```python
async def set_velocity_body(velocity_body_yawspeed)
```

Set the velocity in body coordinates and yaw angular rate. Not available for fixed-wing aircraft.

Parameters
----------
velocity_body_yawspeed : VelocityBodyYawspeed
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_velocity_ned`

```python
async def set_velocity_ned(velocity_ned_yaw)
```

Set the velocity in NED coordinates and yaw. Not available for fixed-wing aircraft.

Parameters
----------
velocity_ned_yaw : VelocityNedYaw
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_position_velocity_ned`

```python
async def set_position_velocity_ned(position_ned_yaw, velocity_ned_yaw)
```

Set the position in NED coordinates, with the velocity to be used as feed-forward.

Parameters
----------
position_ned_yaw : PositionNedYaw
velocity_ned_yaw : VelocityNedYaw
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_position_velocity_acceleration_ned`

```python
async def set_position_velocity_acceleration_ned(position_ned_yaw, velocity_ned_yaw, acceleration_ned)
```

Set the position, velocity and acceleration in NED coordinates, with velocity and acceleration used as feed-forward.

Parameters
----------
position_ned_yaw : PositionNedYaw
velocity_ned_yaw : VelocityNedYaw
acceleration_ned : AccelerationNed
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `set_acceleration_ned`

```python
async def set_acceleration_ned(acceleration_ned)
```

Set the acceleration in NED coordinates.

Parameters
----------
acceleration_ned : AccelerationNed
Raises
------
OffboardError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
