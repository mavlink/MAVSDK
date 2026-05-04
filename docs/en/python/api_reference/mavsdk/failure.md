# Failure

Inject failures into system to test failsafes.

## Enums

### `FailureUnit`

A failure unit.

| Value | Description |
|-------|-------------|
| `SENSOR_GYRO` (0) | |
| `SENSOR_ACCEL` (1) | |
| `SENSOR_MAG` (2) | |
| `SENSOR_BARO` (3) | |
| `SENSOR_GPS` (4) | |
| `SENSOR_OPTICAL_FLOW` (5) | |
| `SENSOR_VIO` (6) | |
| `SENSOR_DISTANCE_SENSOR` (7) | |
| `SENSOR_AIRSPEED` (8) | |
| `SYSTEM_BATTERY` (9) | |
| `SYSTEM_MOTOR` (10) | |
| `SYSTEM_SERVO` (11) | |
| `SYSTEM_AVOIDANCE` (12) | |
| `SYSTEM_RC_SIGNAL` (13) | |
| `SYSTEM_MAVLINK_SIGNAL` (14) | |

### `FailureType`

A failure type

| Value | Description |
|-------|-------------|
| `OK` (0) | |
| `OFF` (1) | |
| `STUCK` (2) | |
| `GARBAGE` (3) | |
| `WRONG` (4) | |
| `SLOW` (5) | |
| `DELAYED` (6) | |
| `INTERMITTENT` (7) | |

### `FailureResult`

Possible results returned for failure requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `UNSUPPORTED` (4) | |
| `DENIED` (5) | |
| `DISABLED` (6) | |
| `TIMEOUT` (7) | |

## `Failure`

Inject failures into system to test failsafes.

### `inject`

```python
def inject(failure_unit, failure_type, instance)
```

Get inject (blocking)
