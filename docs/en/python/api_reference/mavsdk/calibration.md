# Calibration

Enable to calibrate sensors of a drone such as gyro, accelerometer, and magnetometer.

## Enums

### `CalibrationResult`

Possible results returned for calibration commands

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NEXT` (2) | |
| `FAILED` (3) | |
| `NO_SYSTEM` (4) | |
| `CONNECTION_ERROR` (5) | |
| `BUSY` (6) | |
| `COMMAND_DENIED` (7) | |
| `TIMEOUT` (8) | |
| `CANCELLED` (9) | |
| `FAILED_ARMED` (10) | |
| `UNSUPPORTED` (11) | |

## Structs

### `ProgressDataCStruct`

Internal C structure for ProgressData.
Used only for C library communication.

### `ProgressData`

Progress data coming from calibration.

Can be a progress percentage, or an instruction text.

**Fields:**

- `has_progress`
- `progress`
- `has_status_text`
- `status_text`

## `Calibration`

Enable to calibrate sensors of a drone such as gyro, accelerometer, and magnetometer.

### `calibrate_gyro_async`

```python
def calibrate_gyro_async(callback: Callable, user_data: Any = None)
```

Perform gyro calibration.

### `calibrate_accelerometer_async`

```python
def calibrate_accelerometer_async(callback: Callable, user_data: Any = None)
```

Perform accelerometer calibration.

### `calibrate_magnetometer_async`

```python
def calibrate_magnetometer_async(callback: Callable, user_data: Any = None)
```

Perform magnetometer calibration.

### `calibrate_level_horizon_async`

```python
def calibrate_level_horizon_async(callback: Callable, user_data: Any = None)
```

Perform board level horizon calibration.

### `calibrate_gimbal_accelerometer_async`

```python
def calibrate_gimbal_accelerometer_async(callback: Callable, user_data: Any = None)
```

Perform gimbal accelerometer calibration.

### `cancel`

```python
def cancel()
```

Get cancel (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
