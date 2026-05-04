# Gimbal

Provide control over a gimbal within the MAVLink
Gimbal Protocol: https://mavlink.io/en/services/gimbal_v2.html

## Enums

### `GimbalMode`

Gimbal mode type.

| Value | Description |
|-------|-------------|
| `YAW_FOLLOW` (0) | |
| `YAW_LOCK` (1) | |

### `ControlMode`

Control mode

| Value | Description |
|-------|-------------|
| `NONE` (0) | |
| `PRIMARY` (1) | |
| `SECONDARY` (2) | |

### `SendMode`

The send mode type

| Value | Description |
|-------|-------------|
| `ONCE` (0) | |
| `STREAM` (1) | |

### `GimbalResult`

Possible results returned for gimbal commands.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `ERROR` (2) | |
| `TIMEOUT` (3) | |
| `UNSUPPORTED` (4) | |
| `NO_SYSTEM` (5) | |
| `INVALID_ARGUMENT` (6) | |

## Structs

### `Quaternion`

Quaternion type.

All rotations and axis systems follow the right-hand rule.
The Hamilton quaternion product definition is used.
A zero-rotation quaternion is represented by (1,0,0,0).
The quaternion could also be written as w + xi + yj + zk.

For more info see: https://en.wikipedia.org/wiki/Quaternion

**Fields:**

- `w`
- `x`
- `y`
- `z`

### `EulerAngle`

Euler angle type.

All rotations and axis systems follow the right-hand rule.
The Euler angles are converted using the 3-1-2 sequence instead of standard 3-2-1 in order
to avoid the gimbal lock at 90 degrees down.

For more info see https://en.wikipedia.org/wiki/Euler_angles

**Fields:**

- `roll_deg`
- `pitch_deg`
- `yaw_deg`

### `AngularVelocityBody`

Gimbal angular rate type

**Fields:**

- `roll_rad_s`
- `pitch_rad_s`
- `yaw_rad_s`

### `Attitude`

Gimbal attitude type

**Fields:**

- `gimbal_id`
- `euler_angle_forward`
- `quaternion_forward`
- `euler_angle_north`
- `quaternion_north`
- `angular_velocity`
- `timestamp_us`

### `ControlStatus`

Control status

**Fields:**

- `gimbal_id`
- `control_mode`
- `sysid_primary_control`
- `compid_primary_control`
- `sysid_secondary_control`
- `compid_secondary_control`

## `GimbalItem`

Gimbal list item

## `GimbalList`

Gimbal list

## `Gimbal`

Provide control over a gimbal within the MAVLink
Gimbal Protocol: https://mavlink.io/en/services/gimbal_v2.html

### `set_angles_async`

```python
def set_angles_async(gimbal_id, roll_deg, pitch_deg, yaw_deg, gimbal_mode, send_mode, callback: Callable, user_data: Any = None)
```

Set gimbal roll, pitch and yaw angles.

This sets the desired roll, pitch and yaw angles of a gimbal.
Will return when the command is accepted, however, it might
take the gimbal longer to actually be set to the new angles.

Note that the roll angle needs to be set to 0 when send_mode is Once.

### `set_angles`

```python
def set_angles(gimbal_id, roll_deg, pitch_deg, yaw_deg, gimbal_mode, send_mode)
```

Get set_angles (blocking)

### `set_angular_rates_async`

```python
def set_angular_rates_async(gimbal_id, roll_rate_deg_s, pitch_rate_deg_s, yaw_rate_deg_s, gimbal_mode, send_mode, callback: Callable, user_data: Any = None)
```

Set gimbal angular rates.

This sets the desired angular rates around roll, pitch and yaw axes of a gimbal.
Will return when the command is accepted, however, it might
take the gimbal longer to actually reach the angular rate.

Note that the roll angle needs to be set to 0 when send_mode is Once.

### `set_angular_rates`

```python
def set_angular_rates(gimbal_id, roll_rate_deg_s, pitch_rate_deg_s, yaw_rate_deg_s, gimbal_mode, send_mode)
```

Get set_angular_rates (blocking)

### `set_roi_location_async`

```python
def set_roi_location_async(gimbal_id, latitude_deg, longitude_deg, altitude_m, callback: Callable, user_data: Any = None)
```

Set gimbal region of interest (ROI).

This sets a region of interest that the gimbal will point to.
The gimbal will continue to point to the specified region until it
receives a new command.
The function will return when the command is accepted, however, it might
take the gimbal longer to actually rotate to the ROI.

### `set_roi_location`

```python
def set_roi_location(gimbal_id, latitude_deg, longitude_deg, altitude_m)
```

Get set_roi_location (blocking)

### `take_control_async`

```python
def take_control_async(gimbal_id, control_mode, callback: Callable, user_data: Any = None)
```

Take control.

There can be only two components in control of a gimbal at any given time.
One with "primary" control, and one with "secondary" control. The way the
secondary control is implemented is not specified and hence depends on the
vehicle.

Components are expected to be cooperative, which means that they can
override each other and should therefore do it carefully.

### `take_control`

```python
def take_control(gimbal_id, control_mode)
```

Get take_control (blocking)

### `release_control_async`

```python
def release_control_async(gimbal_id, callback: Callable, user_data: Any = None)
```

Release control.

Release control, such that other components can control the gimbal.

### `release_control`

```python
def release_control(gimbal_id)
```

Get release_control (blocking)

### `subscribe_gimbal_list`

```python
def subscribe_gimbal_list(callback: Callable, user_data: Any = None)
```

Subscribe to list of gimbals.

This allows to find out what gimbals are connected to the system.
Based on the gimbal ID, we can then address a specific gimbal.

### `unsubscribe_gimbal_list`

```python
def unsubscribe_gimbal_list(handle: Handle)
```

Unsubscribe from gimbal_list

### `gimbal_list`

```python
def gimbal_list()
```

Get gimbal_list (blocking)

### `subscribe_control_status`

```python
def subscribe_control_status(callback: Callable, user_data: Any = None)
```

Subscribe to control status updates.

This allows a component to know if it has primary, secondary or
no control over the gimbal. Also, it gives the system and component ids
of the other components in control (if any).

### `unsubscribe_control_status`

```python
def unsubscribe_control_status(handle: Handle)
```

Unsubscribe from control_status

### `get_control_status`

```python
def get_control_status(gimbal_id)
```

Get get_control_status (blocking)

### `subscribe_attitude`

```python
def subscribe_attitude(callback: Callable, user_data: Any = None)
```

Subscribe to attitude updates.

This gets you the gimbal's attitude and angular rate.

### `unsubscribe_attitude`

```python
def unsubscribe_attitude(handle: Handle)
```

Unsubscribe from attitude

### `get_attitude`

```python
def get_attitude(gimbal_id)
```

Get get_attitude (blocking)
