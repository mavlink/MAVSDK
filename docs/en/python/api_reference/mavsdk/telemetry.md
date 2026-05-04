# Telemetry

Allow users to get vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.
Certain Telemetry Topics such as, Position or Velocity_Ned require GPS Fix before data gets published.

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

### `BatteryFunction`

Battery function type.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `ALL` (1) | |
| `PROPULSION` (2) | |
| `AVIONICS` (3) | |
| `PAYLOAD` (4) | |

### `FlightMode`

Flight modes.

For more information about flight modes, check out
https://docs.px4.io/master/en/config/flight_mode.html.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `READY` (1) | |
| `TAKEOFF` (2) | |
| `HOLD` (3) | |
| `MISSION` (4) | |
| `RETURN_TO_LAUNCH` (5) | |
| `LAND` (6) | |
| `OFFBOARD` (7) | |
| `FOLLOW_ME` (8) | |
| `MANUAL` (9) | |
| `ALTCTL` (10) | |
| `POSCTL` (11) | |
| `ACRO` (12) | |
| `STABILIZED` (13) | |
| `RATTITUDE` (14) | |

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

### `VtolState`

VTOL State enumeration

| Value | Description |
|-------|-------------|
| `UNDEFINED` (0) | |
| `TRANSITION_TO_FW` (1) | |
| `TRANSITION_TO_MC` (2) | |
| `MC` (3) | |
| `FW` (4) | |

### `TelemetryResult`

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

- `id`
- `temperature_degc`
- `voltage_v`
- `current_battery_a`
- `capacity_consumed_ah`
- `remaining_percent`
- `time_remaining_s`
- `battery_function`

### `Health`

Health type.

**Fields:**

- `is_gyrometer_calibration_ok`
- `is_accelerometer_calibration_ok`
- `is_magnetometer_calibration_ok`
- `is_local_position_ok`
- `is_global_position_ok`
- `is_home_position_ok`
- `is_armable`

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
- `orientation`

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

### `GpsGlobalOrigin`

Gps global origin type.

**Fields:**

- `latitude_deg`
- `longitude_deg`
- `altitude_m`

### `Altitude`

Altitude message type

**Fields:**

- `altitude_monotonic_m`
- `altitude_amsl_m`
- `altitude_local_m`
- `altitude_relative_m`
- `altitude_terrain_m`
- `bottom_clearance_m`
- `timestamp_us`

### `Wind`

Wind message type

**Fields:**

- `wind_x_ned_m_s`
- `wind_y_ned_m_s`
- `wind_z_ned_m_s`
- `horizontal_variability_stddev_m_s`
- `vertical_variability_stddev_m_s`
- `wind_altitude_msl_m`
- `horizontal_wind_speed_accuracy_m_s`
- `vertical_wind_speed_accuracy_m_s`

## `Telemetry`

Allow users to get vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.
Certain Telemetry Topics such as, Position or Velocity_Ned require GPS Fix before data gets published.

### `subscribe_position`

```python
def subscribe_position(callback: Callable, user_data: Any = None)
```

Subscribe to 'position' updates.

### `unsubscribe_position`

```python
def unsubscribe_position(handle: Handle)
```

Unsubscribe from position

### `position`

```python
def position()
```

Get position (blocking)

### `subscribe_home`

```python
def subscribe_home(callback: Callable, user_data: Any = None)
```

Subscribe to 'home position' updates.

### `unsubscribe_home`

```python
def unsubscribe_home(handle: Handle)
```

Unsubscribe from home

### `home`

```python
def home()
```

Get home (blocking)

### `subscribe_in_air`

```python
def subscribe_in_air(callback: Callable, user_data: Any = None)
```

Subscribe to in-air updates.

### `unsubscribe_in_air`

```python
def unsubscribe_in_air(handle: Handle)
```

Unsubscribe from in_air

### `in_air`

```python
def in_air()
```

Get in_air (blocking)

### `subscribe_landed_state`

```python
def subscribe_landed_state(callback: Callable, user_data: Any = None)
```

Subscribe to landed state updates

### `unsubscribe_landed_state`

```python
def unsubscribe_landed_state(handle: Handle)
```

Unsubscribe from landed_state

### `landed_state`

```python
def landed_state()
```

Get landed_state (blocking)

### `subscribe_armed`

```python
def subscribe_armed(callback: Callable, user_data: Any = None)
```

Subscribe to armed updates.

### `unsubscribe_armed`

```python
def unsubscribe_armed(handle: Handle)
```

Unsubscribe from armed

### `armed`

```python
def armed()
```

Get armed (blocking)

### `subscribe_vtol_state`

```python
def subscribe_vtol_state(callback: Callable, user_data: Any = None)
```

subscribe to vtol state Updates

### `unsubscribe_vtol_state`

```python
def unsubscribe_vtol_state(handle: Handle)
```

Unsubscribe from vtol_state

### `vtol_state`

```python
def vtol_state()
```

Get vtol_state (blocking)

### `subscribe_attitude_quaternion`

```python
def subscribe_attitude_quaternion(callback: Callable, user_data: Any = None)
```

Subscribe to 'attitude' updates (quaternion).

### `unsubscribe_attitude_quaternion`

```python
def unsubscribe_attitude_quaternion(handle: Handle)
```

Unsubscribe from attitude_quaternion

### `attitude_quaternion`

```python
def attitude_quaternion()
```

Get attitude_quaternion (blocking)

### `subscribe_attitude_euler`

```python
def subscribe_attitude_euler(callback: Callable, user_data: Any = None)
```

Subscribe to 'attitude' updates (Euler).

### `unsubscribe_attitude_euler`

```python
def unsubscribe_attitude_euler(handle: Handle)
```

Unsubscribe from attitude_euler

### `attitude_euler`

```python
def attitude_euler()
```

Get attitude_euler (blocking)

### `subscribe_attitude_angular_velocity_body`

```python
def subscribe_attitude_angular_velocity_body(callback: Callable, user_data: Any = None)
```

Subscribe to 'attitude' updates (angular velocity)

### `unsubscribe_attitude_angular_velocity_body`

```python
def unsubscribe_attitude_angular_velocity_body(handle: Handle)
```

Unsubscribe from attitude_angular_velocity_body

### `attitude_angular_velocity_body`

```python
def attitude_angular_velocity_body()
```

Get attitude_angular_velocity_body (blocking)

### `subscribe_velocity_ned`

```python
def subscribe_velocity_ned(callback: Callable, user_data: Any = None)
```

Subscribe to 'ground speed' updates (NED).

### `unsubscribe_velocity_ned`

```python
def unsubscribe_velocity_ned(handle: Handle)
```

Unsubscribe from velocity_ned

### `velocity_ned`

```python
def velocity_ned()
```

Get velocity_ned (blocking)

### `subscribe_gps_info`

```python
def subscribe_gps_info(callback: Callable, user_data: Any = None)
```

Subscribe to 'GPS info' updates.

### `unsubscribe_gps_info`

```python
def unsubscribe_gps_info(handle: Handle)
```

Unsubscribe from gps_info

### `gps_info`

```python
def gps_info()
```

Get gps_info (blocking)

### `subscribe_raw_gps`

```python
def subscribe_raw_gps(callback: Callable, user_data: Any = None)
```

Subscribe to 'Raw GPS' updates.

### `unsubscribe_raw_gps`

```python
def unsubscribe_raw_gps(handle: Handle)
```

Unsubscribe from raw_gps

### `raw_gps`

```python
def raw_gps()
```

Get raw_gps (blocking)

### `subscribe_battery`

```python
def subscribe_battery(callback: Callable, user_data: Any = None)
```

Subscribe to 'battery' updates.

### `unsubscribe_battery`

```python
def unsubscribe_battery(handle: Handle)
```

Unsubscribe from battery

### `battery`

```python
def battery()
```

Get battery (blocking)

### `subscribe_flight_mode`

```python
def subscribe_flight_mode(callback: Callable, user_data: Any = None)
```

Subscribe to 'flight mode' updates.

### `unsubscribe_flight_mode`

```python
def unsubscribe_flight_mode(handle: Handle)
```

Unsubscribe from flight_mode

### `flight_mode`

```python
def flight_mode()
```

Get flight_mode (blocking)

### `subscribe_health`

```python
def subscribe_health(callback: Callable, user_data: Any = None)
```

Subscribe to 'health' updates.

### `unsubscribe_health`

```python
def unsubscribe_health(handle: Handle)
```

Unsubscribe from health

### `health`

```python
def health()
```

Get health (blocking)

### `subscribe_rc_status`

```python
def subscribe_rc_status(callback: Callable, user_data: Any = None)
```

Subscribe to 'RC status' updates.

### `unsubscribe_rc_status`

```python
def unsubscribe_rc_status(handle: Handle)
```

Unsubscribe from rc_status

### `rc_status`

```python
def rc_status()
```

Get rc_status (blocking)

### `subscribe_status_text`

```python
def subscribe_status_text(callback: Callable, user_data: Any = None)
```

Subscribe to 'status text' updates.

### `unsubscribe_status_text`

```python
def unsubscribe_status_text(handle: Handle)
```

Unsubscribe from status_text

### `status_text`

```python
def status_text()
```

Get status_text (blocking)

### `subscribe_actuator_control_target`

```python
def subscribe_actuator_control_target(callback: Callable, user_data: Any = None)
```

Subscribe to 'actuator control target' updates.

### `unsubscribe_actuator_control_target`

```python
def unsubscribe_actuator_control_target(handle: Handle)
```

Unsubscribe from actuator_control_target

### `actuator_control_target`

```python
def actuator_control_target()
```

Get actuator_control_target (blocking)

### `subscribe_actuator_output_status`

```python
def subscribe_actuator_output_status(callback: Callable, user_data: Any = None)
```

Subscribe to 'actuator output status' updates.

### `unsubscribe_actuator_output_status`

```python
def unsubscribe_actuator_output_status(handle: Handle)
```

Unsubscribe from actuator_output_status

### `actuator_output_status`

```python
def actuator_output_status()
```

Get actuator_output_status (blocking)

### `subscribe_odometry`

```python
def subscribe_odometry(callback: Callable, user_data: Any = None)
```

Subscribe to 'odometry' updates.

### `unsubscribe_odometry`

```python
def unsubscribe_odometry(handle: Handle)
```

Unsubscribe from odometry

### `odometry`

```python
def odometry()
```

Get odometry (blocking)

### `subscribe_position_velocity_ned`

```python
def subscribe_position_velocity_ned(callback: Callable, user_data: Any = None)
```

Subscribe to 'position velocity' updates.

### `unsubscribe_position_velocity_ned`

```python
def unsubscribe_position_velocity_ned(handle: Handle)
```

Unsubscribe from position_velocity_ned

### `position_velocity_ned`

```python
def position_velocity_ned()
```

Get position_velocity_ned (blocking)

### `subscribe_ground_truth`

```python
def subscribe_ground_truth(callback: Callable, user_data: Any = None)
```

Subscribe to 'ground truth' updates.

### `unsubscribe_ground_truth`

```python
def unsubscribe_ground_truth(handle: Handle)
```

Unsubscribe from ground_truth

### `ground_truth`

```python
def ground_truth()
```

Get ground_truth (blocking)

### `subscribe_fixedwing_metrics`

```python
def subscribe_fixedwing_metrics(callback: Callable, user_data: Any = None)
```

Subscribe to 'fixedwing metrics' updates.

### `unsubscribe_fixedwing_metrics`

```python
def unsubscribe_fixedwing_metrics(handle: Handle)
```

Unsubscribe from fixedwing_metrics

### `fixedwing_metrics`

```python
def fixedwing_metrics()
```

Get fixedwing_metrics (blocking)

### `subscribe_imu`

```python
def subscribe_imu(callback: Callable, user_data: Any = None)
```

Subscribe to 'IMU' updates (in SI units in NED body frame).

### `unsubscribe_imu`

```python
def unsubscribe_imu(handle: Handle)
```

Unsubscribe from imu

### `imu`

```python
def imu()
```

Get imu (blocking)

### `subscribe_scaled_imu`

```python
def subscribe_scaled_imu(callback: Callable, user_data: Any = None)
```

Subscribe to 'Scaled IMU' updates.

### `unsubscribe_scaled_imu`

```python
def unsubscribe_scaled_imu(handle: Handle)
```

Unsubscribe from scaled_imu

### `scaled_imu`

```python
def scaled_imu()
```

Get scaled_imu (blocking)

### `subscribe_raw_imu`

```python
def subscribe_raw_imu(callback: Callable, user_data: Any = None)
```

Subscribe to 'Raw IMU' updates (note that units are are incorrect and "raw" as provided by the sensor)

### `unsubscribe_raw_imu`

```python
def unsubscribe_raw_imu(handle: Handle)
```

Unsubscribe from raw_imu

### `raw_imu`

```python
def raw_imu()
```

Get raw_imu (blocking)

### `subscribe_health_all_ok`

```python
def subscribe_health_all_ok(callback: Callable, user_data: Any = None)
```

Subscribe to 'HealthAllOk' updates.

### `unsubscribe_health_all_ok`

```python
def unsubscribe_health_all_ok(handle: Handle)
```

Unsubscribe from health_all_ok

### `health_all_ok`

```python
def health_all_ok()
```

Get health_all_ok (blocking)

### `subscribe_unix_epoch_time`

```python
def subscribe_unix_epoch_time(callback: Callable, user_data: Any = None)
```

Subscribe to 'unix epoch time' updates.

### `unsubscribe_unix_epoch_time`

```python
def unsubscribe_unix_epoch_time(handle: Handle)
```

Unsubscribe from unix_epoch_time

### `unix_epoch_time`

```python
def unix_epoch_time()
```

Get unix_epoch_time (blocking)

### `subscribe_distance_sensor`

```python
def subscribe_distance_sensor(callback: Callable, user_data: Any = None)
```

Subscribe to 'Distance Sensor' updates.

### `unsubscribe_distance_sensor`

```python
def unsubscribe_distance_sensor(handle: Handle)
```

Unsubscribe from distance_sensor

### `distance_sensor`

```python
def distance_sensor()
```

Get distance_sensor (blocking)

### `subscribe_scaled_pressure`

```python
def subscribe_scaled_pressure(callback: Callable, user_data: Any = None)
```

Subscribe to 'Scaled Pressure' updates.

### `unsubscribe_scaled_pressure`

```python
def unsubscribe_scaled_pressure(handle: Handle)
```

Unsubscribe from scaled_pressure

### `scaled_pressure`

```python
def scaled_pressure()
```

Get scaled_pressure (blocking)

### `subscribe_heading`

```python
def subscribe_heading(callback: Callable, user_data: Any = None)
```

Subscribe to 'Heading' updates.

### `unsubscribe_heading`

```python
def unsubscribe_heading(handle: Handle)
```

Unsubscribe from heading

### `heading`

```python
def heading()
```

Get heading (blocking)

### `subscribe_altitude`

```python
def subscribe_altitude(callback: Callable, user_data: Any = None)
```

Subscribe to 'Altitude' updates.

### `unsubscribe_altitude`

```python
def unsubscribe_altitude(handle: Handle)
```

Unsubscribe from altitude

### `altitude`

```python
def altitude()
```

Get altitude (blocking)

### `subscribe_wind`

```python
def subscribe_wind(callback: Callable, user_data: Any = None)
```

Subscribe to 'Wind Estimated' updates.

### `unsubscribe_wind`

```python
def unsubscribe_wind(handle: Handle)
```

Unsubscribe from wind

### `wind`

```python
def wind()
```

Get wind (blocking)

### `set_rate_position_async`

```python
def set_rate_position_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'position' updates.

### `set_rate_position`

```python
def set_rate_position(rate_hz)
```

Get set_rate_position (blocking)

### `set_rate_home_async`

```python
def set_rate_home_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'home position' updates.

### `set_rate_home`

```python
def set_rate_home(rate_hz)
```

Get set_rate_home (blocking)

### `set_rate_in_air_async`

```python
def set_rate_in_air_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to in-air updates.

### `set_rate_in_air`

```python
def set_rate_in_air(rate_hz)
```

Get set_rate_in_air (blocking)

### `set_rate_landed_state_async`

```python
def set_rate_landed_state_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to landed state updates

### `set_rate_landed_state`

```python
def set_rate_landed_state(rate_hz)
```

Get set_rate_landed_state (blocking)

### `set_rate_vtol_state_async`

```python
def set_rate_vtol_state_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to VTOL state updates

### `set_rate_vtol_state`

```python
def set_rate_vtol_state(rate_hz)
```

Get set_rate_vtol_state (blocking)

### `set_rate_attitude_quaternion_async`

```python
def set_rate_attitude_quaternion_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'attitude euler angle' updates.

### `set_rate_attitude_quaternion`

```python
def set_rate_attitude_quaternion(rate_hz)
```

Get set_rate_attitude_quaternion (blocking)

### `set_rate_attitude_euler_async`

```python
def set_rate_attitude_euler_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'attitude quaternion' updates.

### `set_rate_attitude_euler`

```python
def set_rate_attitude_euler(rate_hz)
```

Get set_rate_attitude_euler (blocking)

### `set_rate_velocity_ned_async`

```python
def set_rate_velocity_ned_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate of camera attitude updates.
Set rate to 'ground speed' updates (NED).

### `set_rate_velocity_ned`

```python
def set_rate_velocity_ned(rate_hz)
```

Get set_rate_velocity_ned (blocking)

### `set_rate_gps_info_async`

```python
def set_rate_gps_info_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'GPS info' updates.

### `set_rate_gps_info`

```python
def set_rate_gps_info(rate_hz)
```

Get set_rate_gps_info (blocking)

### `set_rate_raw_gps_async`

```python
def set_rate_raw_gps_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'Raw GPS' updates.

### `set_rate_raw_gps`

```python
def set_rate_raw_gps(rate_hz)
```

Get set_rate_raw_gps (blocking)

### `set_rate_battery_async`

```python
def set_rate_battery_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'battery' updates.

### `set_rate_battery`

```python
def set_rate_battery(rate_hz)
```

Get set_rate_battery (blocking)

### `set_rate_rc_status_async`

```python
def set_rate_rc_status_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'RC status' updates.

### `set_rate_rc_status`

```python
def set_rate_rc_status(rate_hz)
```

Get set_rate_rc_status (blocking)

### `set_rate_actuator_control_target_async`

```python
def set_rate_actuator_control_target_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'actuator control target' updates.

### `set_rate_actuator_control_target`

```python
def set_rate_actuator_control_target(rate_hz)
```

Get set_rate_actuator_control_target (blocking)

### `set_rate_actuator_output_status_async`

```python
def set_rate_actuator_output_status_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'actuator output status' updates.

### `set_rate_actuator_output_status`

```python
def set_rate_actuator_output_status(rate_hz)
```

Get set_rate_actuator_output_status (blocking)

### `set_rate_odometry_async`

```python
def set_rate_odometry_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'odometry' updates.

### `set_rate_odometry`

```python
def set_rate_odometry(rate_hz)
```

Get set_rate_odometry (blocking)

### `set_rate_position_velocity_ned_async`

```python
def set_rate_position_velocity_ned_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'position velocity' updates.

### `set_rate_position_velocity_ned`

```python
def set_rate_position_velocity_ned(rate_hz)
```

Get set_rate_position_velocity_ned (blocking)

### `set_rate_ground_truth_async`

```python
def set_rate_ground_truth_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'ground truth' updates.

### `set_rate_ground_truth`

```python
def set_rate_ground_truth(rate_hz)
```

Get set_rate_ground_truth (blocking)

### `set_rate_fixedwing_metrics_async`

```python
def set_rate_fixedwing_metrics_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'fixedwing metrics' updates.

### `set_rate_fixedwing_metrics`

```python
def set_rate_fixedwing_metrics(rate_hz)
```

Get set_rate_fixedwing_metrics (blocking)

### `set_rate_imu_async`

```python
def set_rate_imu_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'IMU' updates.

### `set_rate_imu`

```python
def set_rate_imu(rate_hz)
```

Get set_rate_imu (blocking)

### `set_rate_scaled_imu_async`

```python
def set_rate_scaled_imu_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'Scaled IMU' updates.

### `set_rate_scaled_imu`

```python
def set_rate_scaled_imu(rate_hz)
```

Get set_rate_scaled_imu (blocking)

### `set_rate_raw_imu_async`

```python
def set_rate_raw_imu_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'Raw IMU' updates.

### `set_rate_raw_imu`

```python
def set_rate_raw_imu(rate_hz)
```

Get set_rate_raw_imu (blocking)

### `set_rate_unix_epoch_time_async`

```python
def set_rate_unix_epoch_time_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'unix epoch time' updates.

### `set_rate_unix_epoch_time`

```python
def set_rate_unix_epoch_time(rate_hz)
```

Get set_rate_unix_epoch_time (blocking)

### `set_rate_distance_sensor_async`

```python
def set_rate_distance_sensor_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'Distance Sensor' updates.

### `set_rate_distance_sensor`

```python
def set_rate_distance_sensor(rate_hz)
```

Get set_rate_distance_sensor (blocking)

### `set_rate_altitude_async`

```python
def set_rate_altitude_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'Altitude' updates.

### `set_rate_altitude`

```python
def set_rate_altitude(rate_hz)
```

Get set_rate_altitude (blocking)

### `set_rate_health_async`

```python
def set_rate_health_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'Health' updates.

### `set_rate_health`

```python
def set_rate_health(rate_hz)
```

Get set_rate_health (blocking)

### `get_gps_global_origin_async`

```python
def get_gps_global_origin_async(callback: Callable, user_data: Any = None)
```

Get the GPS location of where the estimator has been initialized.

### `get_gps_global_origin`

```python
def get_gps_global_origin()
```

Get get_gps_global_origin (blocking)
