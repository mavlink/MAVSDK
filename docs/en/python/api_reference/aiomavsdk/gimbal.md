# Gimbal

Provide control over a gimbal within the MAVLink
Gimbal Protocol: https://mavlink.io/en/services/gimbal_v2.html

## `GimbalAsync`

Provide control over a gimbal within the MAVLink
Gimbal Protocol: https://mavlink.io/en/services/gimbal_v2.html

Async wrapper around :class:`Gimbal` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `set_angles`

```python
async def set_angles(gimbal_id, roll_deg, pitch_deg, yaw_deg, gimbal_mode, send_mode)
```

Set gimbal roll, pitch and yaw angles.

This sets the desired roll, pitch and yaw angles of a gimbal.
Will return when the command is accepted, however, it might
take the gimbal longer to actually be set to the new angles.

Note that the roll angle needs to be set to 0 when send_mode is Once.

Parameters
----------
gimbal_id : int
roll_deg : float
pitch_deg : float
yaw_deg : float
gimbal_mode : GimbalMode
send_mode : SendMode
Raises
------
GimbalError
    If the request fails. The error contains the reason for the failure.

### `set_angular_rates`

```python
async def set_angular_rates(gimbal_id, roll_rate_deg_s, pitch_rate_deg_s, yaw_rate_deg_s, gimbal_mode, send_mode)
```

Set gimbal angular rates.

This sets the desired angular rates around roll, pitch and yaw axes of a gimbal.
Will return when the command is accepted, however, it might
take the gimbal longer to actually reach the angular rate.

Note that the roll angle needs to be set to 0 when send_mode is Once.

Parameters
----------
gimbal_id : int
roll_rate_deg_s : float
pitch_rate_deg_s : float
yaw_rate_deg_s : float
gimbal_mode : GimbalMode
send_mode : SendMode
Raises
------
GimbalError
    If the request fails. The error contains the reason for the failure.

### `set_roi_location`

```python
async def set_roi_location(gimbal_id, latitude_deg, longitude_deg, altitude_m)
```

Set gimbal region of interest (ROI).

This sets a region of interest that the gimbal will point to.
The gimbal will continue to point to the specified region until it
receives a new command.
The function will return when the command is accepted, however, it might
take the gimbal longer to actually rotate to the ROI.

Parameters
----------
gimbal_id : int
latitude_deg : float
longitude_deg : float
altitude_m : float
Raises
------
GimbalError
    If the request fails. The error contains the reason for the failure.

### `take_control`

```python
async def take_control(gimbal_id, control_mode)
```

Take control.

There can be only two components in control of a gimbal at any given time.
One with "primary" control, and one with "secondary" control. The way the
secondary control is implemented is not specified and hence depends on the
vehicle.

Components are expected to be cooperative, which means that they can
override each other and should therefore do it carefully.

Parameters
----------
gimbal_id : int
control_mode : ControlMode
Raises
------
GimbalError
    If the request fails. The error contains the reason for the failure.

### `release_control`

```python
async def release_control(gimbal_id)
```

Release control.

Release control, such that other components can control the gimbal.

Parameters
----------
gimbal_id : int
Raises
------
GimbalError
    If the request fails. The error contains the reason for the failure.

### `subscribe_gimbal_list`

```python
async def subscribe_gimbal_list() → AsyncGenerator[GimbalList, None]
```

Subscribe to list of gimbals.

This allows to find out what gimbals are connected to the system.
Based on the gimbal ID, we can then address a specific gimbal.

Yields
------
gimbal_list : GimbalList
     The next update

### `gimbal_list`

```python
async def gimbal_list()
```

Subscribe to list of gimbals.

This allows to find out what gimbals are connected to the system.
Based on the gimbal ID, we can then address a specific gimbal.

Returns
-------
gimbal_list : GimbalList
Raises
------
GimbalError
    If the request fails. The error contains the reason for the failure.

### `subscribe_control_status`

```python
async def subscribe_control_status() → AsyncGenerator[ControlStatus, None]
```

Subscribe to control status updates.

This allows a component to know if it has primary, secondary or
no control over the gimbal. Also, it gives the system and component ids
of the other components in control (if any).

Yields
------
control_status : ControlStatus
     The next update

### `get_control_status`

```python
async def get_control_status(gimbal_id)
```

Get control status for specific gimbal.

Parameters
----------
gimbal_id : int
Returns
-------
control_status : ControlStatus
Raises
------
GimbalError
If the request fails. The error contains the reason for the failure.

### `subscribe_attitude`

```python
async def subscribe_attitude() → AsyncGenerator[Attitude, None]
```

Subscribe to attitude updates.

This gets you the gimbal's attitude and angular rate.

Yields
------
attitude : Attitude
     The next update

### `get_attitude`

```python
async def get_attitude(gimbal_id)
```

Get attitude for specific gimbal.

Parameters
----------
gimbal_id : int
Returns
-------
attitude : Attitude
Raises
------
GimbalError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
