# Mission

Enable waypoint missions.

## `MissionAsync`

Enable waypoint missions.

Async wrapper around `Mission` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `upload_mission`

```python
async def upload_mission(mission_plan)
```

Upload a list of mission items to the system.

The mission items are uploaded to a drone. Once uploaded the mission can be started and
executed even if the connection is lost.

Parameters
----------

mission_plan : MissionPlan
Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `upload_mission_with_progress`

```python
async def upload_mission_with_progress(mission_plan) → AsyncGenerator
```

Upload a list of mission items to the system and report upload progress.

The mission items are uploaded to a drone. Once uploaded the mission can be started and
executed even if the connection is lost.

### `cancel_mission_upload`

```python
async def cancel_mission_upload()
```

Cancel an ongoing mission upload.

Raises
------

MissionError
If the request fails. The error contains the reason for the failure.

### `download_mission`

```python
async def download_mission()
```

Download a list of mission items from the system (asynchronous).

Will fail if any of the downloaded mission items are not supported
by the MAVSDK API.

Returns
-------

mission_plan : MissionPlan
Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `download_mission_with_progress`

```python
async def download_mission_with_progress() → AsyncGenerator
```

Download a list of mission items from the system (asynchronous) and report progress.

Will fail if any of the downloaded mission items are not supported
by the MAVSDK API.

### `cancel_mission_download`

```python
async def cancel_mission_download()
```

Cancel an ongoing mission download.

Raises
------

MissionError
If the request fails. The error contains the reason for the failure.

### `start_mission`

```python
async def start_mission()
```

Start the mission.

A mission must be uploaded to the vehicle before this can be called.

Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `pause_mission`

```python
async def pause_mission()
```

Pause the mission.

Pausing the mission puts the vehicle into
[HOLD mode](https://docs.px4.io/en/flight_modes/hold.html).
A multicopter should just hover at the spot while a fixedwing vehicle should loiter
around the location where it paused.

Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `clear_mission`

```python
async def clear_mission()
```

Clear the mission saved on the vehicle.

Raises
------

MissionError
If the request fails. The error contains the reason for the failure.

### `set_current_mission_item`

```python
async def set_current_mission_item(index)
```

Sets the mission item index to go to.

By setting the current index to 0, the mission is restarted from the beginning. If it is set
to a specific index of a mission item, the mission will be set to this item.

Note that this is not necessarily true for general missions using MAVLink if loop counters
are used.

Parameters
----------

index : int
Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `is_mission_finished`

```python
async def is_mission_finished()
```

Check if the mission has been finished.

Returns
-------

is_finished : bool
Raises
------

MissionError
If the request fails. The error contains the reason for the failure.

### `subscribe_mission_progress`

```python
async def subscribe_mission_progress() → AsyncGenerator[MissionProgress, None]
```

Subscribe to mission progress updates.

Yields
------

mission_progress : MissionProgress
The next update

### `mission_progress`

```python
async def mission_progress()
```

Subscribe to mission progress updates.

Returns
-------

mission_progress : MissionProgress
Raises
------

MissionError
If the request fails. The error contains the reason for the failure.

### `get_return_to_launch_after_mission`

```python
async def get_return_to_launch_after_mission()
```

Get whether to trigger Return-to-Launch (RTL) after mission is complete.

Before getting this option, it needs to be set, or a mission
needs to be downloaded.

Returns
-------

enable : bool
Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `set_return_to_launch_after_mission`

```python
async def set_return_to_launch_after_mission(enable)
```

Set whether to trigger Return-to-Launch (RTL) after the mission is complete.

This will only take effect for the next mission upload, meaning that
the mission may have to be uploaded again.

Parameters
----------

enable : bool
Raises
------

MissionError
    If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
