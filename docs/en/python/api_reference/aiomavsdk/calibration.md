# Calibration

Enable to calibrate sensors of a drone such as gyro, accelerometer, and magnetometer.

## `CalibrationAsync`

Enable to calibrate sensors of a drone such as gyro, accelerometer, and magnetometer.

Async wrapper around `Calibration` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `calibrate_gyro`

```python
async def calibrate_gyro() → AsyncGenerator
```

Perform gyro calibration.

### `calibrate_accelerometer`

```python
async def calibrate_accelerometer() → AsyncGenerator
```

Perform accelerometer calibration.

### `calibrate_magnetometer`

```python
async def calibrate_magnetometer() → AsyncGenerator
```

Perform magnetometer calibration.

### `calibrate_level_horizon`

```python
async def calibrate_level_horizon() → AsyncGenerator
```

Perform board level horizon calibration.

### `calibrate_gimbal_accelerometer`

```python
async def calibrate_gimbal_accelerometer() → AsyncGenerator
```

Perform gimbal accelerometer calibration.

### `cancel`

```python
async def cancel()
```

Cancel ongoing calibration process.

Raises
------

CalibrationError
If the request fails. The error contains the reason for the failure.
