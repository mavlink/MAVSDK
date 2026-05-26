# Mocap

Allows interfacing a vehicle with a motion capture system in
order to allow navigation without global positioning sources available
(e.g. indoors, or when flying under a bridge. etc.).

## `MocapAsync`

Allows interfacing a vehicle with a motion capture system in
order to allow navigation without global positioning sources available
(e.g. indoors, or when flying under a bridge. etc.).

Async wrapper around `Mocap` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `set_vision_position_estimate`

```python
async def set_vision_position_estimate(vision_position_estimate)
```

Send Global position/attitude estimate from a vision source.

Parameters
----------

vision_position_estimate : VisionPositionEstimate
Raises
------

MocapError
If the request fails. The error contains the reason for the failure.

### `set_vision_speed_estimate`

```python
async def set_vision_speed_estimate(vision_speed_estimate)
```

Send Global speed estimate from a vision source.

Parameters
----------

vision_speed_estimate : VisionSpeedEstimate
Raises
------

MocapError
If the request fails. The error contains the reason for the failure.

### `set_attitude_position_mocap`

```python
async def set_attitude_position_mocap(attitude_position_mocap)
```

Send motion capture attitude and position.

Parameters
----------

attitude_position_mocap : AttitudePositionMocap
Raises
------

MocapError
If the request fails. The error contains the reason for the failure.

### `set_odometry`

```python
async def set_odometry(odometry)
```

Send odometry information with an external interface.

Parameters
----------

odometry : Odometry
Raises
------

MocapError
If the request fails. The error contains the reason for the failure.
