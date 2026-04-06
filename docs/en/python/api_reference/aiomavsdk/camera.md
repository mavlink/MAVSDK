# Camera

Can be used to manage cameras that implement the MAVLink
Camera Protocol: https://mavlink.io/en/services/camera.html.

Currently only a single camera is supported.
When multiple cameras are supported the plugin will need to be
instantiated separately for every camera and the camera selected using
`select_camera`.

## `CameraAsync`

Can be used to manage cameras that implement the MAVLink
Camera Protocol: https://mavlink.io/en/services/camera.html.

Currently only a single camera is supported.
When multiple cameras are supported the plugin will need to be
instantiated separately for every camera and the camera selected using
`select_camera`.

Async wrapper around :class:`Camera` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `take_photo`

```python
async def take_photo(component_id)
```

Take one photo.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `start_photo_interval`

```python
async def start_photo_interval(component_id, interval_s)
```

Start photo timelapse with a given interval.

Parameters
----------
component_id : int
interval_s : float
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `stop_photo_interval`

```python
async def stop_photo_interval(component_id)
```

Stop a running photo timelapse.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `start_video`

```python
async def start_video(component_id)
```

Start a video recording.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `stop_video`

```python
async def stop_video(component_id)
```

Stop a running video recording.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `start_video_streaming`

```python
async def start_video_streaming(component_id, stream_id)
```

Start video streaming.

Parameters
----------
component_id : int
stream_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `stop_video_streaming`

```python
async def stop_video_streaming(component_id, stream_id)
```

Stop current video streaming.

Parameters
----------
component_id : int
stream_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `set_mode`

```python
async def set_mode(component_id, mode)
```

Set camera mode.

Parameters
----------
component_id : int
mode : Mode
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `list_photos`

```python
async def list_photos(component_id, photos_range)
```

List photos available on the camera.

Note that this might need to be called initially to set the PhotosRange accordingly.
Once set to 'all' rather than 'since connection', it will try to request the previous
images over time.

Parameters
----------
component_id : int
photos_range : PhotosRange
Returns
-------
capture_infos : CaptureInfo
Raises
------
CameraError
    If the request fails. The error contains the reason for the failure.

### `subscribe_camera_list`

```python
async def subscribe_camera_list() → AsyncGenerator[CameraList, None]
```

Subscribe to list of cameras.

This allows to find out what cameras are connected to the system.
Based on the camera ID, we can then address a specific camera.

Yields
------
camera_list : CameraList
     The next update

### `camera_list`

```python
async def camera_list()
```

Subscribe to list of cameras.

This allows to find out what cameras are connected to the system.
Based on the camera ID, we can then address a specific camera.

Returns
-------
camera_list : CameraList
Raises
------
CameraError
    If the request fails. The error contains the reason for the failure.

### `subscribe_mode`

```python
async def subscribe_mode() → AsyncGenerator[ModeUpdate, None]
```

Subscribe to camera mode updates.

Yields
------
mode_update : ModeUpdate
The next update

### `get_mode`

```python
async def get_mode(component_id)
```

Get camera mode.

Parameters
----------
component_id : int
Returns
-------
mode : Mode
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `subscribe_video_stream_info`

```python
async def subscribe_video_stream_info() → AsyncGenerator[VideoStreamUpdate, None]
```

Subscribe to video stream info updates.

Yields
------
video_stream_update : VideoStreamUpdate
The next update

### `get_video_stream_info`

```python
async def get_video_stream_info(component_id)
```

Get video stream info.

Parameters
----------
component_id : int
Returns
-------
video_stream_info : VideoStreamInfo
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `subscribe_capture_info`

```python
async def subscribe_capture_info() → AsyncGenerator[CaptureInfo, None]
```

Subscribe to capture info updates.

Yields
------
capture_info : CaptureInfo
The next update

### `subscribe_storage`

```python
async def subscribe_storage() → AsyncGenerator[StorageUpdate, None]
```

Subscribe to camera's storage status updates.

Yields
------
storage_update : StorageUpdate
The next update

### `get_storage`

```python
async def get_storage(component_id)
```

Get camera's storage status.

Parameters
----------
component_id : int
Returns
-------
storage : Storage
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `subscribe_current_settings`

```python
async def subscribe_current_settings() → AsyncGenerator[CurrentSettingsUpdate, None]
```

Get the list of current camera settings.

Yields
------
current_settings_update : CurrentSettingsUpdate
The next update

### `get_current_settings`

```python
async def get_current_settings(component_id)
```

Get current settings.

Parameters
----------
component_id : int
Returns
-------
current_settings : Setting
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `subscribe_possible_setting_options`

```python
async def subscribe_possible_setting_options() → AsyncGenerator[PossibleSettingOptionsUpdate, None]
```

Get the list of settings that can be changed.

Yields
------
possible_setting_options_update : PossibleSettingOptionsUpdate
The next update

### `get_possible_setting_options`

```python
async def get_possible_setting_options(component_id)
```

Get possible setting options.

Parameters
----------
component_id : int
Returns
-------
setting_options : SettingOptions
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `set_setting`

```python
async def set_setting(component_id, setting)
```

Set a setting to some value.

Only setting_id of setting and option_id of option needs to be set.

Parameters
----------
component_id : int
setting : Setting
Raises
------
CameraError
    If the request fails. The error contains the reason for the failure.

### `get_setting`

```python
async def get_setting(component_id, setting)
```

Get a setting.

Only setting_id of setting needs to be set.

Parameters
----------
component_id : int
setting : Setting
Returns
-------
setting : Setting
Raises
------
CameraError
    If the request fails. The error contains the reason for the failure.

### `format_storage`

```python
async def format_storage(component_id, storage_id)
```

Format storage (e.g. SD card) in camera.

This will delete all content of the camera storage!

Parameters
----------
component_id : int
storage_id : int
Raises
------
CameraError
    If the request fails. The error contains the reason for the failure.

### `reset_settings`

```python
async def reset_settings(component_id)
```

Reset all settings in camera.

This will reset all camera settings to default value

Parameters
----------
component_id : int
Raises
------
CameraError
    If the request fails. The error contains the reason for the failure.

### `zoom_in_start`

```python
async def zoom_in_start(component_id)
```

Start zooming in.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `zoom_out_start`

```python
async def zoom_out_start(component_id)
```

Start zooming out.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `zoom_stop`

```python
async def zoom_stop(component_id)
```

Stop zooming.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `zoom_range`

```python
async def zoom_range(component_id, range)
```

Zoom to value as proportion of full camera range (percentage between 0.0 and 100.0).

Parameters
----------
component_id : int
range : float
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `track_point`

```python
async def track_point(component_id, point_x, point_y, radius)
```

Track point.

Parameters
----------
component_id : int
point_x : float
point_y : float
radius : float
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `track_rectangle`

```python
async def track_rectangle(component_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y)
```

Track rectangle.

Parameters
----------
component_id : int
top_left_x : float
top_left_y : float
bottom_right_x : float
bottom_right_y : float
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `track_stop`

```python
async def track_stop(component_id)
```

Stop tracking.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `focus_in_start`

```python
async def focus_in_start(component_id)
```

Start focusing in.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `focus_out_start`

```python
async def focus_out_start(component_id)
```

Start focusing out.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `focus_stop`

```python
async def focus_stop(component_id)
```

Stop focus.

Parameters
----------
component_id : int
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `focus_range`

```python
async def focus_range(component_id, range)
```

Focus with range value of full range (value between 0.0 and 100.0).

Parameters
----------
component_id : int
range : float
Raises
------
CameraError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
