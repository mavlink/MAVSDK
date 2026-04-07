# Telemetry Server

Allow users to provide vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.

## Enums

### `FixType`

GPS fix type.

| Value | Description |
|-------|-------------|
| `NO_GPS` (0) | |
| `NO_FIX` (1) | |
| `FIX_2D` (2) | |
| `FIX_3D` (3) | |
| `FIX_DGPS` (4) | |
| `RTK_FLOAT` (5) | |
| `RTK_FIXED` (6) | |

### `VtolState`

Maps to MAV_VTOL_STATE

| Value | Description |
|-------|-------------|
| `UNDEFINED` (0) | |
| `TRANSITION_TO_FW` (1) | |
| `TRANSITION_TO_MC` (2) | |
| `MC` (3) | |
| `FW` (4) | |

### `StatusTextType`

Status types.

| Value | Description |
|-------|-------------|
| `DEBUG` (0) | |
| `INFO` (1) | |
| `NOTICE` (2) | |
| `WARNING` (3) | |
| `ERROR` (4) | |
| `CRITICAL` (5) | |
| `ALERT` (6) | |
| `EMERGENCY` (7) | |

### `LandedState`

Landed State enumeration.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `ON_GROUND` (1) | |
| `IN_AIR` (2) | |
| `TAKING_OFF` (3) | |
| `LANDING` (4) | |

### `TelemetryServerResult`

Possible results returned for telemetry requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `BUSY` (4) | |
| `COMMAND_DENIED` (5) | |
| `TIMEOUT` (6) | |
| `UNSUPPORTED` (7) | |

## Structs

### `Position`

Position type in global coordinates.

**Fields:**

- `latitude_deg`
- `longitude_deg`
- `absolute_altitude_m`
- `relative_altitude_m`

### `Heading`

Heading type used for global position

**Fields:**

- `heading_deg`

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
- `timestamp_us`

### `EulerAngle`

Euler angle type.

All rotations and axis systems follow the right-hand rule.
The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.

For more info see https://en.wikipedia.org/wiki/Euler_angles

**Fields:**

- `roll_deg`
- `pitch_deg`
- `yaw_deg`
- `timestamp_us`

### `AngularVelocityBody`

Angular velocity type.

**Fields:**

- `roll_rad_s`
- `pitch_rad_s`
- `yaw_rad_s`

### `GpsInfo`

GPS information type.

**Fields:**

- `num_satellites`
- `fix_type`

### `RawGps`

Raw GPS information type.

Warning: this is an advanced type! If you want the location of the drone, use
the position instead. This message exposes the raw values of the GNSS sensor.

**Fields:**

- `timestamp_us`
- `latitude_deg`
- `longitude_deg`
- `absolute_altitude_m`
- `hdop`
- `vdop`
- `velocity_m_s`
- `cog_deg`
- `altitude_ellipsoid_m`
- `horizontal_uncertainty_m`
- `vertical_uncertainty_m`
- `velocity_uncertainty_m_s`
- `heading_uncertainty_deg`
- `yaw_deg`

### `Battery`

Battery type.

**Fields:**

- `voltage_v`
- `remaining_percent`

### `RcStatus`

Remote control status type.

**Fields:**

- `was_available_once`
- `is_available`
- `signal_strength_percent`

### `StatusText`

StatusText information type.

**Fields:**

- `type`
- `text`

### `ActuatorControlTarget`

Actuator control target type.

**Fields:**

- `group`
- `controls`

### `ActuatorOutputStatus`

Actuator output status type.

**Fields:**

- `active`
- `actuator`

### `Covariance`

Covariance type.

Row-major representation of a 6x6 cross-covariance matrix
upper right triangle.
Set first to NaN if unknown.

**Fields:**

- `covariance_matrix`

### `VelocityBody`

Velocity type, represented in the Body (X Y Z) frame and in metres/second.

**Fields:**

- `x_m_s`
- `y_m_s`
- `z_m_s`

### `PositionBody`

Position type, represented in the Body (X Y Z) frame

**Fields:**

- `x_m`
- `y_m`
- `z_m`

### `Odometry`

Odometry message type.

**Fields:**

- `time_usec`
- `frame_id`
- `child_frame_id`
- `position_body`
- `q`
- `velocity_body`
- `angular_velocity_body`
- `pose_covariance`
- `velocity_covariance`

### `DistanceSensor`

DistanceSensor message type.

**Fields:**

- `minimum_distance_m`
- `maximum_distance_m`
- `current_distance_m`

### `ScaledPressure`

Scaled Pressure message type.

**Fields:**

- `timestamp_us`
- `absolute_pressure_hpa`
- `differential_pressure_hpa`
- `temperature_deg`
- `differential_pressure_temperature_deg`

### `PositionNed`

PositionNed message type.

**Fields:**

- `north_m`
- `east_m`
- `down_m`

### `VelocityNed`

VelocityNed message type.

**Fields:**

- `north_m_s`
- `east_m_s`
- `down_m_s`

### `PositionVelocityNed`

PositionVelocityNed message type.

**Fields:**

- `position`
- `velocity`

### `GroundTruth`

GroundTruth message type.

**Fields:**

- `latitude_deg`
- `longitude_deg`
- `absolute_altitude_m`
- `timestamp_us`

### `FixedwingMetrics`

FixedwingMetrics message type.

**Fields:**

- `airspeed_m_s`
- `throttle_percentage`
- `climb_rate_m_s`
- `groundspeed_m_s`
- `heading_deg`
- `absolute_altitude_m`

### `AccelerationFrd`

AccelerationFrd message type.

**Fields:**

- `forward_m_s2`
- `right_m_s2`
- `down_m_s2`

### `AngularVelocityFrd`

AngularVelocityFrd message type.

**Fields:**

- `forward_rad_s`
- `right_rad_s`
- `down_rad_s`

### `MagneticFieldFrd`

MagneticFieldFrd message type.

**Fields:**

- `forward_gauss`
- `right_gauss`
- `down_gauss`

### `Imu`

Imu message type.

**Fields:**

- `acceleration_frd`
- `angular_velocity_frd`
- `magnetic_field_frd`
- `temperature_degc`
- `timestamp_us`

### `TelemetryServer`

Allow users to provide vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.

**Fields:**

- `server_component`
