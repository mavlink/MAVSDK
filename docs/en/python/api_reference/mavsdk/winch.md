# Winch

Allows users to send winch actions, as well as receive status information from winch systems.

## Enums

### `WinchAction`

Winch Action type.

| Value | Description |
|-------|-------------|
| `RELAXED` (0) | |
| `RELATIVE_LENGTH_CONTROL` (1) | |
| `RATE_CONTROL` (2) | |
| `LOCK` (3) | |
| `DELIVER` (4) | |
| `HOLD` (5) | |
| `RETRACT` (6) | |
| `LOAD_LINE` (7) | |
| `ABANDON_LINE` (8) | |
| `LOAD_PAYLOAD` (9) | |

### `WinchResult`

Possible results returned for winch action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `BUSY` (3) | |
| `TIMEOUT` (4) | |
| `UNSUPPORTED` (5) | |
| `FAILED` (6) | |

## Structs

### `StatusFlags`

Winch Status Flags.

The status flags are defined in mavlink
https://mavlink.io/en/messages/common.html#MAV_WINCH_STATUS_FLAG.

Multiple status fields can be set simultaneously. Mavlink does
not specify which states are mutually exclusive.

**Fields:**

- `healthy`
- `fully_retracted`
- `moving`
- `clutch_engaged`
- `locked`
- `dropping`
- `arresting`
- `ground_sense`
- `retracting`
- `redeliver`
- `abandon_line`
- `locking`
- `load_line`
- `load_payload`

### `Status`

Status type.

**Fields:**

- `time_usec`
- `line_length_m`
- `speed_m_s`
- `tension_kg`
- `voltage_v`
- `current_a`
- `temperature_c`
- `status_flags`

## `Winch`

Allows users to send winch actions, as well as receive status information from winch systems.

### `subscribe_status`

```python
def subscribe_status(callback: Callable, user_data: Any = None)
```

Subscribe to 'winch status' updates.

### `unsubscribe_status`

```python
def unsubscribe_status(handle: Handle)
```

Unsubscribe from status

### `status`

```python
def status()
```

Get status (blocking)

### `relax_async`

```python
def relax_async(instance, callback: Callable, user_data: Any = None)
```

Allow motor to freewheel.

### `relax`

```python
def relax(instance)
```

Get relax (blocking)

### `relative_length_control_async`

```python
def relative_length_control_async(instance, length_m, rate_m_s, callback: Callable, user_data: Any = None)
```

Wind or unwind specified length of line, optionally using specified rate.

### `relative_length_control`

```python
def relative_length_control(instance, length_m, rate_m_s)
```

Get relative_length_control (blocking)

### `rate_control_async`

```python
def rate_control_async(instance, rate_m_s, callback: Callable, user_data: Any = None)
```

Wind or unwind line at specified rate.

### `rate_control`

```python
def rate_control(instance, rate_m_s)
```

Get rate_control (blocking)

### `lock_async`

```python
def lock_async(instance, callback: Callable, user_data: Any = None)
```

Perform the locking sequence to relieve motor while in the fully retracted position.

### `lock`

```python
def lock(instance)
```

Get lock (blocking)

### `deliver_async`

```python
def deliver_async(instance, callback: Callable, user_data: Any = None)
```

Sequence of drop, slow down, touch down, reel up, lock.

### `deliver`

```python
def deliver(instance)
```

Get deliver (blocking)

### `hold_async`

```python
def hold_async(instance, callback: Callable, user_data: Any = None)
```

Engage motor and hold current position.

### `hold`

```python
def hold(instance)
```

Get hold (blocking)

### `retract_async`

```python
def retract_async(instance, callback: Callable, user_data: Any = None)
```

Return the reel to the fully retracted position.

### `retract`

```python
def retract(instance)
```

Get retract (blocking)

### `load_line_async`

```python
def load_line_async(instance, callback: Callable, user_data: Any = None)
```

Load the reel with line.

The winch will calculate the total loaded length and stop when the tension exceeds a threshold.

### `load_line`

```python
def load_line(instance)
```

Get load_line (blocking)

### `abandon_line_async`

```python
def abandon_line_async(instance, callback: Callable, user_data: Any = None)
```

Spool out the entire length of the line.

### `abandon_line`

```python
def abandon_line(instance)
```

Get abandon_line (blocking)

### `load_payload_async`

```python
def load_payload_async(instance, callback: Callable, user_data: Any = None)
```

Spools out just enough to present the hook to the user to load the payload.

### `load_payload`

```python
def load_payload(instance)
```

Get load_payload (blocking)
