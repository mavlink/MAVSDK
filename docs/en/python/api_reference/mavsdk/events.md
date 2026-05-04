# Events

Get event notifications, such as takeoff, or arming checks

## Enums

### `LogLevel`

Log level type

| Value | Description |
|-------|-------------|
| `EMERGENCY` (0) | |
| `ALERT` (1) | |
| `CRITICAL` (2) | |
| `ERROR` (3) | |
| `WARNING` (4) | |
| `NOTICE` (5) | |
| `INFO` (6) | |
| `DEBUG` (7) | |

### `EventsResult`

Possible results returned

| Value | Description |
|-------|-------------|
| `SUCCESS` (0) | |
| `NOT_AVAILABLE` (1) | |
| `CONNECTION_ERROR` (2) | |
| `UNSUPPORTED` (3) | |
| `DENIED` (4) | |
| `FAILED` (5) | |
| `TIMEOUT` (6) | |
| `NO_SYSTEM` (7) | |
| `UNKNOWN` (8) | |

## Structs

### `Event`

Event type

**Fields:**

- `compid`
- `message`
- `description`
- `log_level`
- `event_namespace`
- `event_name`

### `HealthAndArmingCheckProblem`

Health and arming check problem type

**Fields:**

- `message`
- `description`
- `log_level`
- `health_component`

### `HealthAndArmingCheckMode`

Arming checks for a specific mode

**Fields:**

- `mode_name`
- `can_arm_or_run`
- `problems`

### `HealthComponentReport`

Health component report type

**Fields:**

- `name`
- `label`
- `is_present`
- `has_error`
- `has_warning`

### `HealthAndArmingCheckReport`

Health and arming check report type

**Fields:**

- `current_mode_intention`
- `health_components`
- `all_problems`

## `Events`

Get event notifications, such as takeoff, or arming checks

### `subscribe_events`

```python
def subscribe_events(callback: Callable, user_data: Any = None)
```

Subscribe to event updates.

### `unsubscribe_events`

```python
def unsubscribe_events(handle: Handle)
```

Unsubscribe from events

### `subscribe_health_and_arming_checks`

```python
def subscribe_health_and_arming_checks(callback: Callable, user_data: Any = None)
```

Subscribe to arming check updates.

### `unsubscribe_health_and_arming_checks`

```python
def unsubscribe_health_and_arming_checks(handle: Handle)
```

Unsubscribe from health_and_arming_checks

### `get_health_and_arming_checks_report`

```python
def get_health_and_arming_checks_report()
```

Get get_health_and_arming_checks_report (blocking)
