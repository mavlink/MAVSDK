# Mission

Enable waypoint missions.

## Enums

### `MissionResult`

Possible results returned for action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `ERROR` (2) | |
| `TOO_MANY_MISSION_ITEMS` (3) | |
| `BUSY` (4) | |
| `TIMEOUT` (5) | |
| `INVALID_ARGUMENT` (6) | |
| `UNSUPPORTED` (7) | |
| `NO_MISSION_AVAILABLE` (8) | |
| `UNSUPPORTED_MISSION_CMD` (9) | |
| `TRANSFER_CANCELLED` (10) | |
| `NO_SYSTEM` (11) | |
| `NEXT` (12) | |
| `DENIED` (13) | |
| `PROTOCOL_ERROR` (14) | |
| `INT_MESSAGES_NOT_SUPPORTED` (15) | |

## Structs

### `ProgressData`

Progress data coming from mission upload.

**Fields:**

- `progress`

### `ProgressDataOrMission`

Progress data coming from mission download, or the mission itself (if the transfer succeeds).

**Fields:**

- `has_progress`
- `progress`
- `has_mission`
- `mission_plan`

## `MissionItem`

Type representing a mission item.

A MissionItem can contain a position and/or actions.
Mission items are building blocks to assemble a mission,
which can be sent to (or received from) a system.
They cannot be used independently.

## `MissionPlan`

Mission plan type

## `MissionProgress`

Mission progress type.

## `Mission`

Enable waypoint missions.

### `upload_mission_async`

```python
def upload_mission_async(mission_plan, callback: Callable, user_data: Any = None)
```

Upload a list of mission items to the system.

The mission items are uploaded to a drone. Once uploaded the mission can be started and
executed even if the connection is lost.

### `upload_mission`

```python
def upload_mission(mission_plan)
```

Get upload_mission (blocking)

### `upload_mission_with_progress_async`

```python
def upload_mission_with_progress_async(mission_plan, callback: Callable, user_data: Any = None)
```

Upload a list of mission items to the system and report upload progress.

The mission items are uploaded to a drone. Once uploaded the mission can be started and
executed even if the connection is lost.

### `cancel_mission_upload`

```python
def cancel_mission_upload()
```

Get cancel_mission_upload (blocking)

### `download_mission_async`

```python
def download_mission_async(callback: Callable, user_data: Any = None)
```

Download a list of mission items from the system (asynchronous).

Will fail if any of the downloaded mission items are not supported
by the MAVSDK API.

### `download_mission`

```python
def download_mission()
```

Get download_mission (blocking)

### `download_mission_with_progress_async`

```python
def download_mission_with_progress_async(callback: Callable, user_data: Any = None)
```

Download a list of mission items from the system (asynchronous) and report progress.

Will fail if any of the downloaded mission items are not supported
by the MAVSDK API.

### `cancel_mission_download`

```python
def cancel_mission_download()
```

Get cancel_mission_download (blocking)

### `start_mission_async`

```python
def start_mission_async(callback: Callable, user_data: Any = None)
```

Start the mission.

A mission must be uploaded to the vehicle before this can be called.

### `start_mission`

```python
def start_mission()
```

Get start_mission (blocking)

### `pause_mission_async`

```python
def pause_mission_async(callback: Callable, user_data: Any = None)
```

Pause the mission.

Pausing the mission puts the vehicle into
[HOLD mode](https://docs.px4.io/en/flight_modes/hold.html).
A multicopter should just hover at the spot while a fixedwing vehicle should loiter
around the location where it paused.

### `pause_mission`

```python
def pause_mission()
```

Get pause_mission (blocking)

### `clear_mission_async`

```python
def clear_mission_async(callback: Callable, user_data: Any = None)
```

Clear the mission saved on the vehicle.

### `clear_mission`

```python
def clear_mission()
```

Get clear_mission (blocking)

### `set_current_mission_item_async`

```python
def set_current_mission_item_async(index, callback: Callable, user_data: Any = None)
```

Sets the mission item index to go to.

By setting the current index to 0, the mission is restarted from the beginning. If it is set
to a specific index of a mission item, the mission will be set to this item.

Note that this is not necessarily true for general missions using MAVLink if loop counters
are used.

### `set_current_mission_item`

```python
def set_current_mission_item(index)
```

Get set_current_mission_item (blocking)

### `is_mission_finished`

```python
def is_mission_finished()
```

Get is_mission_finished (blocking)

### `subscribe_mission_progress`

```python
def subscribe_mission_progress(callback: Callable, user_data: Any = None)
```

Subscribe to mission progress updates.

### `unsubscribe_mission_progress`

```python
def unsubscribe_mission_progress(handle: Handle)
```

Unsubscribe from mission_progress

### `mission_progress`

```python
def mission_progress()
```

Get mission_progress (blocking)

### `get_return_to_launch_after_mission`

```python
def get_return_to_launch_after_mission()
```

Get get_return_to_launch_after_mission (blocking)

### `set_return_to_launch_after_mission`

```python
def set_return_to_launch_after_mission(enable)
```

Get set_return_to_launch_after_mission (blocking)
