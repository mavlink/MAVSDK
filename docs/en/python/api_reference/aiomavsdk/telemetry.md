# Telemetry

Allow users to get vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.
Certain Telemetry Topics such as, Position or Velocity_Ned require GPS Fix before data gets published.

## `TelemetryAsync`

Allow users to get vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.
Certain Telemetry Topics such as, Position or Velocity_Ned require GPS Fix before data gets published.

Async wrapper around `Telemetry` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `subscribe_position`

```python
async def subscribe_position() → AsyncGenerator[Position, None]
```

Subscribe to 'position' updates.

Yields
------

position : Position
The next update

### `position`

```python
async def position()
```

Subscribe to 'position' updates.

Returns
-------

position : Position
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_home`

```python
async def subscribe_home() → AsyncGenerator[Position, None]
```

Subscribe to 'home position' updates.

Yields
------

position : Position
The next update

### `home`

```python
async def home()
```

Subscribe to 'home position' updates.

Returns
-------

home : Position
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_in_air`

```python
async def subscribe_in_air() → AsyncGenerator[bool, None]
```

Subscribe to in-air updates.

Yields
------

: bool
    The next update

### `in_air`

```python
async def in_air()
```

Subscribe to in-air updates.

Returns
-------

is_in_air : bool
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_landed_state`

```python
async def subscribe_landed_state() → AsyncGenerator[LandedState, None]
```

Subscribe to landed state updates

Yields
------

landed_state : LandedState
The next update

### `landed_state`

```python
async def landed_state()
```

Subscribe to landed state updates

Returns
-------

landed_state : LandedState
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_armed`

```python
async def subscribe_armed() → AsyncGenerator[bool, None]
```

Subscribe to armed updates.

Yields
------

: bool
    The next update

### `armed`

```python
async def armed()
```

Subscribe to armed updates.

Returns
-------

is_armed : bool
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_vtol_state`

```python
async def subscribe_vtol_state() → AsyncGenerator[VtolState, None]
```

subscribe to vtol state Updates

Yields
------

vtol_state : VtolState
The next update

### `vtol_state`

```python
async def vtol_state()
```

subscribe to vtol state Updates

Returns
-------

vtol_state : VtolState
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_attitude_quaternion`

```python
async def subscribe_attitude_quaternion() → AsyncGenerator[Quaternion, None]
```

Subscribe to 'attitude' updates (quaternion).

Yields
------

quaternion : Quaternion
The next update

### `attitude_quaternion`

```python
async def attitude_quaternion()
```

Subscribe to 'attitude' updates (quaternion).

Returns
-------

attitude_quaternion : Quaternion
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_attitude_euler`

```python
async def subscribe_attitude_euler() → AsyncGenerator[EulerAngle, None]
```

Subscribe to 'attitude' updates (Euler).

Yields
------

euler_angle : EulerAngle
The next update

### `attitude_euler`

```python
async def attitude_euler()
```

Subscribe to 'attitude' updates (Euler).

Returns
-------

attitude_euler : EulerAngle
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_attitude_angular_velocity_body`

```python
async def subscribe_attitude_angular_velocity_body() → AsyncGenerator[AngularVelocityBody, None]
```

Subscribe to 'attitude' updates (angular velocity)

Yields
------

angular_velocity_body : AngularVelocityBody
The next update

### `attitude_angular_velocity_body`

```python
async def attitude_angular_velocity_body()
```

Subscribe to 'attitude' updates (angular velocity)

Returns
-------

attitude_angular_velocity_body : AngularVelocityBody
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_velocity_ned`

```python
async def subscribe_velocity_ned() → AsyncGenerator[VelocityNed, None]
```

Subscribe to 'ground speed' updates (NED).

Yields
------

velocity_ned : VelocityNed
The next update

### `velocity_ned`

```python
async def velocity_ned()
```

Subscribe to 'ground speed' updates (NED).

Returns
-------

velocity_ned : VelocityNed
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_gps_info`

```python
async def subscribe_gps_info() → AsyncGenerator[GpsInfo, None]
```

Subscribe to 'GPS info' updates.

Yields
------

gps_info : GpsInfo
The next update

### `gps_info`

```python
async def gps_info()
```

Subscribe to 'GPS info' updates.

Returns
-------

gps_info : GpsInfo
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_raw_gps`

```python
async def subscribe_raw_gps() → AsyncGenerator[RawGps, None]
```

Subscribe to 'Raw GPS' updates.

Yields
------

raw_gps : RawGps
The next update

### `raw_gps`

```python
async def raw_gps()
```

Subscribe to 'Raw GPS' updates.

Returns
-------

raw_gps : RawGps
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_battery`

```python
async def subscribe_battery() → AsyncGenerator[Battery, None]
```

Subscribe to 'battery' updates.

Yields
------

battery : Battery
The next update

### `battery`

```python
async def battery()
```

Subscribe to 'battery' updates.

Returns
-------

battery : Battery
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_flight_mode`

```python
async def subscribe_flight_mode() → AsyncGenerator[FlightMode, None]
```

Subscribe to 'flight mode' updates.

Yields
------

flight_mode : FlightMode
The next update

### `flight_mode`

```python
async def flight_mode()
```

Subscribe to 'flight mode' updates.

Returns
-------

flight_mode : FlightMode
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_health`

```python
async def subscribe_health() → AsyncGenerator[Health, None]
```

Subscribe to 'health' updates.

Yields
------

health : Health
The next update

### `health`

```python
async def health()
```

Subscribe to 'health' updates.

Returns
-------

health : Health
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_rc_status`

```python
async def subscribe_rc_status() → AsyncGenerator[RcStatus, None]
```

Subscribe to 'RC status' updates.

Yields
------

rc_status : RcStatus
The next update

### `rc_status`

```python
async def rc_status()
```

Subscribe to 'RC status' updates.

Returns
-------

rc_status : RcStatus
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_status_text`

```python
async def subscribe_status_text() → AsyncGenerator[StatusText, None]
```

Subscribe to 'status text' updates.

Yields
------

status_text : StatusText
The next update

### `status_text`

```python
async def status_text()
```

Subscribe to 'status text' updates.

Returns
-------

status_text : StatusText
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_actuator_control_target`

```python
async def subscribe_actuator_control_target() → AsyncGenerator[ActuatorControlTarget, None]
```

Subscribe to 'actuator control target' updates.

Yields
------

actuator_control_target : ActuatorControlTarget
The next update

### `actuator_control_target`

```python
async def actuator_control_target()
```

Subscribe to 'actuator control target' updates.

Returns
-------

actuator_control_target : ActuatorControlTarget
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_actuator_output_status`

```python
async def subscribe_actuator_output_status() → AsyncGenerator[ActuatorOutputStatus, None]
```

Subscribe to 'actuator output status' updates.

Yields
------

actuator_output_status : ActuatorOutputStatus
The next update

### `actuator_output_status`

```python
async def actuator_output_status()
```

Subscribe to 'actuator output status' updates.

Returns
-------

actuator_output_status : ActuatorOutputStatus
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_odometry`

```python
async def subscribe_odometry() → AsyncGenerator[Odometry, None]
```

Subscribe to 'odometry' updates.

Yields
------

odometry : Odometry
The next update

### `odometry`

```python
async def odometry()
```

Subscribe to 'odometry' updates.

Returns
-------

odometry : Odometry
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_position_velocity_ned`

```python
async def subscribe_position_velocity_ned() → AsyncGenerator[PositionVelocityNed, None]
```

Subscribe to 'position velocity' updates.

Yields
------

position_velocity_ned : PositionVelocityNed
The next update

### `position_velocity_ned`

```python
async def position_velocity_ned()
```

Subscribe to 'position velocity' updates.

Returns
-------

position_velocity_ned : PositionVelocityNed
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_ground_truth`

```python
async def subscribe_ground_truth() → AsyncGenerator[GroundTruth, None]
```

Subscribe to 'ground truth' updates.

Yields
------

ground_truth : GroundTruth
The next update

### `ground_truth`

```python
async def ground_truth()
```

Subscribe to 'ground truth' updates.

Returns
-------

ground_truth : GroundTruth
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_fixedwing_metrics`

```python
async def subscribe_fixedwing_metrics() → AsyncGenerator[FixedwingMetrics, None]
```

Subscribe to 'fixedwing metrics' updates.

Yields
------

fixedwing_metrics : FixedwingMetrics
The next update

### `fixedwing_metrics`

```python
async def fixedwing_metrics()
```

Subscribe to 'fixedwing metrics' updates.

Returns
-------

fixedwing_metrics : FixedwingMetrics
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_imu`

```python
async def subscribe_imu() → AsyncGenerator[Imu, None]
```

Subscribe to 'IMU' updates (in SI units in NED body frame).

Yields
------

imu : Imu
The next update

### `imu`

```python
async def imu()
```

Subscribe to 'IMU' updates (in SI units in NED body frame).

Returns
-------

imu : Imu
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_scaled_imu`

```python
async def subscribe_scaled_imu() → AsyncGenerator[Imu, None]
```

Subscribe to 'Scaled IMU' updates.

Yields
------

imu : Imu
The next update

### `scaled_imu`

```python
async def scaled_imu()
```

Subscribe to 'Scaled IMU' updates.

Returns
-------

imu : Imu
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_raw_imu`

```python
async def subscribe_raw_imu() → AsyncGenerator[Imu, None]
```

Subscribe to 'Raw IMU' updates (note that units are are incorrect and "raw" as provided by the sensor)

Yields
------

imu : Imu
The next update

### `raw_imu`

```python
async def raw_imu()
```

Subscribe to 'Raw IMU' updates (note that units are are incorrect and "raw" as provided by the sensor)

Returns
-------

imu : Imu
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_health_all_ok`

```python
async def subscribe_health_all_ok() → AsyncGenerator[bool, None]
```

Subscribe to 'HealthAllOk' updates.

Yields
------

: bool
    The next update

### `health_all_ok`

```python
async def health_all_ok()
```

Subscribe to 'HealthAllOk' updates.

Returns
-------

is_health_all_ok : bool
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_unix_epoch_time`

```python
async def subscribe_unix_epoch_time() → AsyncGenerator[int, None]
```

Subscribe to 'unix epoch time' updates.

Yields
------

: int
    The next update

### `unix_epoch_time`

```python
async def unix_epoch_time()
```

Subscribe to 'unix epoch time' updates.

Returns
-------

time_us : int
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_distance_sensor`

```python
async def subscribe_distance_sensor() → AsyncGenerator[DistanceSensor, None]
```

Subscribe to 'Distance Sensor' updates.

Yields
------

distance_sensor : DistanceSensor
The next update

### `distance_sensor`

```python
async def distance_sensor()
```

Subscribe to 'Distance Sensor' updates.

Returns
-------

distance_sensor : DistanceSensor
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_scaled_pressure`

```python
async def subscribe_scaled_pressure() → AsyncGenerator[ScaledPressure, None]
```

Subscribe to 'Scaled Pressure' updates.

Yields
------

scaled_pressure : ScaledPressure
The next update

### `scaled_pressure`

```python
async def scaled_pressure()
```

Subscribe to 'Scaled Pressure' updates.

Returns
-------

scaled_pressure : ScaledPressure
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_heading`

```python
async def subscribe_heading() → AsyncGenerator[Heading, None]
```

Subscribe to 'Heading' updates.

Yields
------

heading : Heading
The next update

### `heading`

```python
async def heading()
```

Subscribe to 'Heading' updates.

Returns
-------

heading_deg : Heading
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_altitude`

```python
async def subscribe_altitude() → AsyncGenerator[Altitude, None]
```

Subscribe to 'Altitude' updates.

Yields
------

altitude : Altitude
The next update

### `altitude`

```python
async def altitude()
```

Subscribe to 'Altitude' updates.

Returns
-------

altitude : Altitude
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `subscribe_wind`

```python
async def subscribe_wind() → AsyncGenerator[Wind, None]
```

Subscribe to 'Wind Estimated' updates.

Yields
------

wind : Wind
The next update

### `wind`

```python
async def wind()
```

Subscribe to 'Wind Estimated' updates.

Returns
-------

wind : Wind
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_position`

```python
async def set_rate_position(rate_hz)
```

Set rate to 'position' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_home`

```python
async def set_rate_home(rate_hz)
```

Set rate to 'home position' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_in_air`

```python
async def set_rate_in_air(rate_hz)
```

Set rate to in-air updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_landed_state`

```python
async def set_rate_landed_state(rate_hz)
```

Set rate to landed state updates

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_vtol_state`

```python
async def set_rate_vtol_state(rate_hz)
```

Set rate to VTOL state updates

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_attitude_quaternion`

```python
async def set_rate_attitude_quaternion(rate_hz)
```

Set rate to 'attitude euler angle' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_attitude_euler`

```python
async def set_rate_attitude_euler(rate_hz)
```

Set rate to 'attitude quaternion' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_velocity_ned`

```python
async def set_rate_velocity_ned(rate_hz)
```

Set rate of camera attitude updates.
Set rate to 'ground speed' updates (NED).

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
    If the request fails. The error contains the reason for the failure.

### `set_rate_gps_info`

```python
async def set_rate_gps_info(rate_hz)
```

Set rate to 'GPS info' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_raw_gps`

```python
async def set_rate_raw_gps(rate_hz)
```

Set rate to 'Raw GPS' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_battery`

```python
async def set_rate_battery(rate_hz)
```

Set rate to 'battery' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_rc_status`

```python
async def set_rate_rc_status(rate_hz)
```

Set rate to 'RC status' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_actuator_control_target`

```python
async def set_rate_actuator_control_target(rate_hz)
```

Set rate to 'actuator control target' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_actuator_output_status`

```python
async def set_rate_actuator_output_status(rate_hz)
```

Set rate to 'actuator output status' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_odometry`

```python
async def set_rate_odometry(rate_hz)
```

Set rate to 'odometry' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_position_velocity_ned`

```python
async def set_rate_position_velocity_ned(rate_hz)
```

Set rate to 'position velocity' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_ground_truth`

```python
async def set_rate_ground_truth(rate_hz)
```

Set rate to 'ground truth' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_fixedwing_metrics`

```python
async def set_rate_fixedwing_metrics(rate_hz)
```

Set rate to 'fixedwing metrics' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_imu`

```python
async def set_rate_imu(rate_hz)
```

Set rate to 'IMU' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_scaled_imu`

```python
async def set_rate_scaled_imu(rate_hz)
```

Set rate to 'Scaled IMU' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_raw_imu`

```python
async def set_rate_raw_imu(rate_hz)
```

Set rate to 'Raw IMU' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_unix_epoch_time`

```python
async def set_rate_unix_epoch_time(rate_hz)
```

Set rate to 'unix epoch time' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_distance_sensor`

```python
async def set_rate_distance_sensor(rate_hz)
```

Set rate to 'Distance Sensor' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_altitude`

```python
async def set_rate_altitude(rate_hz)
```

Set rate to 'Altitude' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `set_rate_health`

```python
async def set_rate_health(rate_hz)
```

Set rate to 'Health' updates.

Parameters
----------

rate_hz : float
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.

### `get_gps_global_origin`

```python
async def get_gps_global_origin()
```

Get the GPS location of where the estimator has been initialized.

Returns
-------

gps_global_origin : GpsGlobalOrigin
Raises
------

TelemetryError
If the request fails. The error contains the reason for the failure.
