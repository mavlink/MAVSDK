# Offboard

Control a drone with position, velocity, attitude or motor commands.

The module is called offboard because the commands can be sent from external sources
as opposed to onboard control right inside the autopilot "board".

Client code must specify a setpoint before starting offboard mode.
Mavsdk automatically sends setpoints at 20Hz (PX4 Offboard mode requires that setpoints
are minimally sent at 2Hz).

## Enums

### `OffboardResult`

Possible results returned for offboard requests

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `BUSY` (4) | |
| `COMMAND_DENIED` (5) | |
| `TIMEOUT` (6) | |
| `NO_SETPOINT_SET` (7) | |
| `FAILED` (8) | |

## Structs

### `AttitudeCStruct`

Internal C structure for Attitude.
Used only for C library communication.

### `ActuatorControlGroupCStruct`

Internal C structure for ActuatorControlGroup.
Used only for C library communication.

### `ActuatorControlCStruct`

Internal C structure for ActuatorControl.
Used only for C library communication.

### `AttitudeRateCStruct`

Internal C structure for AttitudeRate.
Used only for C library communication.

### `PositionNedYawCStruct`

Internal C structure for PositionNedYaw.
Used only for C library communication.

### `PositionGlobalYawCStruct`

Internal C structure for PositionGlobalYaw.
Used only for C library communication.

### `VelocityBodyYawspeedCStruct`

Internal C structure for VelocityBodyYawspeed.
Used only for C library communication.

### `VelocityNedYawCStruct`

Internal C structure for VelocityNedYaw.
Used only for C library communication.

### `AccelerationNedCStruct`

Internal C structure for AccelerationNed.
Used only for C library communication.

### `Attitude`

Type for attitude body angles in NED reference frame (roll, pitch, yaw and thrust)

**Fields:**

- `roll_deg`
- `pitch_deg`
- `yaw_deg`
- `thrust_value`

### `ActuatorControlGroup`

Eight controls that will be given to the group. Each control is a normalized
(-1..+1) command value, which will be mapped and scaled through the mixer.

**Fields:**

- `controls`

### `ActuatorControl`

Type for actuator control.

Control members should be normed to -1..+1 where 0 is neutral position.
Throttle for single rotation direction motors is 0..1, negative range for reverse direction.

One group support eight controls.

Up to 16 actuator controls can be set. To ignore an output group, set all it controls to NaN.
If one or more controls in group is not NaN, then all NaN controls will sent as zero.
The first 8 actuator controls internally map to control group 0, the latter 8 actuator
controls map to control group 1. Depending on what controls are set (instead of NaN) 1 or 2
MAVLink messages are actually sent.

In PX4 v1.9.0 Only first four Control Groups are supported
(https://github.com/PX4/Firmware/blob/v1.9.0/src/modules/mavlink/mavlink_receiver.cpp#L980).

**Fields:**

- `groups`

### `AttitudeRate`

Type for attitude rate commands in body coordinates (roll, pitch, yaw angular rate and thrust)

**Fields:**

- `roll_deg_s`
- `pitch_deg_s`
- `yaw_deg_s`
- `thrust_value`

### `PositionNedYaw`

Type for position commands in NED (North East Down) coordinates and yaw.

**Fields:**

- `north_m`
- `east_m`
- `down_m`
- `yaw_deg`

### `PositionGlobalYaw`

Type for position commands in Global (Latitude, Longitude, Altitude) coordinates and yaw.

**Fields:**

- `lat_deg`
- `lon_deg`
- `alt_m`
- `yaw_deg`
- `altitude_type`

### `VelocityBodyYawspeed`

Type for velocity commands in body coordinates.

**Fields:**

- `forward_m_s`
- `right_m_s`
- `down_m_s`
- `yawspeed_deg_s`

### `VelocityNedYaw`

Type for velocity commands in NED (North East Down) coordinates and yaw.

**Fields:**

- `north_m_s`
- `east_m_s`
- `down_m_s`
- `yaw_deg`

### `AccelerationNed`

Type for acceleration commands in NED (North East Down) coordinates.

**Fields:**

- `north_m_s2`
- `east_m_s2`
- `down_m_s2`

## `Offboard`

Control a drone with position, velocity, attitude or motor commands.

The module is called offboard because the commands can be sent from external sources
as opposed to onboard control right inside the autopilot "board".

Client code must specify a setpoint before starting offboard mode.
Mavsdk automatically sends setpoints at 20Hz (PX4 Offboard mode requires that setpoints
are minimally sent at 2Hz).

### `start_async`

```python
def start_async(callback: Callable, user_data: Any = None)
```

Start offboard control.

### `start`

```python
def start()
```

Get start (blocking)

### `stop_async`

```python
def stop_async(callback: Callable, user_data: Any = None)
```

Stop offboard control.

The vehicle will be put into Hold mode: https://docs.px4.io/en/flight_modes/hold.html

### `stop`

```python
def stop()
```

Get stop (blocking)

### `is_active`

```python
def is_active()
```

Get is_active (blocking)

### `set_attitude`

```python
def set_attitude(attitude)
```

Get set_attitude (blocking)

### `set_actuator_control`

```python
def set_actuator_control(actuator_control)
```

Get set_actuator_control (blocking)

### `set_attitude_rate`

```python
def set_attitude_rate(attitude_rate)
```

Get set_attitude_rate (blocking)

### `set_position_ned`

```python
def set_position_ned(position_ned_yaw)
```

Get set_position_ned (blocking)

### `set_position_global`

```python
def set_position_global(position_global_yaw)
```

Get set_position_global (blocking)

### `set_velocity_body`

```python
def set_velocity_body(velocity_body_yawspeed)
```

Get set_velocity_body (blocking)

### `set_velocity_ned`

```python
def set_velocity_ned(velocity_ned_yaw)
```

Get set_velocity_ned (blocking)

### `set_position_velocity_ned`

```python
def set_position_velocity_ned(position_ned_yaw, velocity_ned_yaw)
```

Get set_position_velocity_ned (blocking)

### `set_position_velocity_acceleration_ned`

```python
def set_position_velocity_acceleration_ned(position_ned_yaw, velocity_ned_yaw, acceleration_ned)
```

Get set_position_velocity_acceleration_ned (blocking)

### `set_acceleration_ned`

```python
def set_acceleration_ned(acceleration_ned)
```

Get set_acceleration_ned (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
