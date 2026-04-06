# Mocap

Allows interfacing a vehicle with a motion capture system in
order to allow navigation without global positioning sources available
(e.g. indoors, or when flying under a bridge. etc.).

## Enums

### `MocapResult`

Possible results returned for mocap requests

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `INVALID_REQUEST_DATA` (4) | |
| `UNSUPPORTED` (5) | |

## Structs

### `PositionBodyCStruct`

Internal C structure for PositionBody.
Used only for C library communication.

### `AngleBodyCStruct`

Internal C structure for AngleBody.
Used only for C library communication.

### `SpeedBodyCStruct`

Internal C structure for SpeedBody.
Used only for C library communication.

### `SpeedNedCStruct`

Internal C structure for SpeedNed.
Used only for C library communication.

### `AngularVelocityBodyCStruct`

Internal C structure for AngularVelocityBody.
Used only for C library communication.

### `CovarianceCStruct`

Internal C structure for Covariance.
Used only for C library communication.

### `QuaternionCStruct`

Internal C structure for Quaternion.
Used only for C library communication.

### `VisionPositionEstimateCStruct`

Internal C structure for VisionPositionEstimate.
Used only for C library communication.

### `VisionSpeedEstimateCStruct`

Internal C structure for VisionSpeedEstimate.
Used only for C library communication.

### `AttitudePositionMocapCStruct`

Internal C structure for AttitudePositionMocap.
Used only for C library communication.

### `OdometryCStruct`

Internal C structure for Odometry.
Used only for C library communication.

### `PositionBody`

Body position type

**Fields:**

- `x_m`
- `y_m`
- `z_m`

### `AngleBody`

Body angle type

**Fields:**

- `roll_rad`
- `pitch_rad`
- `yaw_rad`

### `SpeedBody`

Speed type, represented in the Body (X Y Z) frame and in metres/second.

**Fields:**

- `x_m_s`
- `y_m_s`
- `z_m_s`

### `SpeedNed`

Speed type, represented in NED (North East Down) coordinates.

**Fields:**

- `north_m_s`
- `east_m_s`
- `down_m_s`

### `AngularVelocityBody`

Angular velocity type

**Fields:**

- `roll_rad_s`
- `pitch_rad_s`
- `yaw_rad_s`

### `Covariance`

Covariance type.
Row-major representation of a 6x6 cross-covariance matrix upper
right triangle.
Needs to be 21 entries or 1 entry with NaN if unknown.

**Fields:**

- `covariance_matrix`

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

### `VisionPositionEstimate`

Global position/attitude estimate from a vision source.

**Fields:**

- `time_usec`
- `position_body`
- `angle_body`
- `pose_covariance`
- `reset_counter`

### `VisionSpeedEstimate`

Global speed estimate from a vision source.

**Fields:**

- `time_usec`
- `speed_ned`
- `speed_covariance`
- `reset_counter`

### `AttitudePositionMocap`

Motion capture attitude and position

**Fields:**

- `time_usec`
- `q`
- `position_body`
- `pose_covariance`

### `Odometry`

Odometry message to communicate odometry information with an external interface.

**Fields:**

- `time_usec`
- `frame_id`
- `position_body`
- `q`
- `speed_body`
- `angular_velocity_body`
- `pose_covariance`
- `velocity_covariance`
- `reset_counter`
- `estimator_type`
- `quality_percent`

## `Mocap`

Allows interfacing a vehicle with a motion capture system in
order to allow navigation without global positioning sources available
(e.g. indoors, or when flying under a bridge. etc.).

### `set_vision_position_estimate`

```python
def set_vision_position_estimate(vision_position_estimate)
```

Get set_vision_position_estimate (blocking)

### `set_vision_speed_estimate`

```python
def set_vision_speed_estimate(vision_speed_estimate)
```

Get set_vision_speed_estimate (blocking)

### `set_attitude_position_mocap`

```python
def set_attitude_position_mocap(attitude_position_mocap)
```

Get set_attitude_position_mocap (blocking)

### `set_odometry`

```python
def set_odometry(odometry)
```

Get set_odometry (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
