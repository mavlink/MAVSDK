# Camera

Can be used to manage cameras that implement the MAVLink
Camera Protocol: https://mavlink.io/en/services/camera.html.

Currently only a single camera is supported.
When multiple cameras are supported the plugin will need to be
instantiated separately for every camera and the camera selected using
`select_camera`.

## Enums

### `Mode`

Camera mode type.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `PHOTO` (1) | |
| `VIDEO` (2) | |

### `PhotosRange`

Photos range type.

| Value | Description |
|-------|-------------|
| `ALL` (0) | |
| `SINCE_CONNECTION` (1) | |

### `CameraResult`

Possible results returned for camera commands

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `IN_PROGRESS` (2) | |
| `BUSY` (3) | |
| `DENIED` (4) | |
| `ERROR` (5) | |
| `TIMEOUT` (6) | |
| `WRONG_ARGUMENT` (7) | |
| `NO_SYSTEM` (8) | |
| `PROTOCOL_UNSUPPORTED` (9) | |
| `UNAVAILABLE` (10) | |
| `CAMERA_ID_INVALID` (11) | |
| `ACTION_UNSUPPORTED` (12) | |

## Structs

### `Option`

Type to represent a setting option.

**Fields:**

- `option_id`
- `option_description`

### `Setting`

Type to represent a setting with a selected option.

**Fields:**

- `setting_id`
- `setting_description`
- `option`
- `is_range`

### `SettingOptions`

Type to represent a setting with a list of options to choose from.

**Fields:**

- `component_id`
- `setting_id`
- `setting_description`
- `options`
- `is_range`

### `VideoStreamSettings`

Type for video stream settings.

**Fields:**

- `frame_rate_hz`
- `horizontal_resolution_pix`
- `vertical_resolution_pix`
- `bit_rate_b_s`
- `rotation_deg`
- `uri`
- `horizontal_fov_deg`

### `VideoStreamInfo`

Information about the video stream.

**Fields:**

- `stream_id`
- `settings`
- `status`
- `spectrum`

### `ModeUpdate`

An update about the current mode

**Fields:**

- `component_id`
- `mode`

### `VideoStreamUpdate`

An update about a video stream

**Fields:**

- `component_id`
- `video_stream_info`

### `Storage`

Information about the camera's storage status.

**Fields:**

- `component_id`
- `video_on`
- `photo_interval_on`
- `used_storage_mib`
- `available_storage_mib`
- `total_storage_mib`
- `recording_time_s`
- `media_folder_name`
- `storage_status`
- `storage_id`
- `storage_type`

### `StorageUpdate`

An update about storage

**Fields:**

- `component_id`
- `storage`

### `CurrentSettingsUpdate`

An update about a current setting

**Fields:**

- `component_id`
- `current_settings`

### `PossibleSettingOptionsUpdate`

An update about possible setting options

**Fields:**

- `component_id`
- `setting_options`

### `Position`

Position type in global coordinates.

**Fields:**

- `latitude_deg`
- `longitude_deg`
- `absolute_altitude_m`
- `relative_altitude_m`

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

### `EulerAngle`

Euler angle type.

All rotations and axis systems follow the right-hand rule.
The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.

For more info see https://en.wikipedia.org/wiki/Euler_angles

**Fields:**

- `roll_deg`
- `pitch_deg`
- `yaw_deg`

### `CaptureInfo`

Information about a picture just captured.

**Fields:**

- `component_id`
- `position`
- `attitude_quaternion`
- `attitude_euler_angle`
- `time_utc_us`
- `is_success`
- `index`
- `file_url`

### `Information`

Type to represent a camera information.

**Fields:**

- `component_id`
- `vendor_name`
- `model_name`
- `focal_length_mm`
- `horizontal_sensor_size_mm`
- `vertical_sensor_size_mm`
- `horizontal_resolution_px`
- `vertical_resolution_px`

## `CameraList`

Camera list

## `Camera`

Can be used to manage cameras that implement the MAVLink
Camera Protocol: https://mavlink.io/en/services/camera.html.

Currently only a single camera is supported.
When multiple cameras are supported the plugin will need to be
instantiated separately for every camera and the camera selected using
`select_camera`.

### `take_photo_async`

```python
def take_photo_async(component_id, callback: Callable, user_data: Any = None)
```

Take one photo.

### `take_photo`

```python
def take_photo(component_id)
```

Get take_photo (blocking)

### `start_photo_interval_async`

```python
def start_photo_interval_async(component_id, interval_s, callback: Callable, user_data: Any = None)
```

Start photo timelapse with a given interval.

### `start_photo_interval`

```python
def start_photo_interval(component_id, interval_s)
```

Get start_photo_interval (blocking)

### `stop_photo_interval_async`

```python
def stop_photo_interval_async(component_id, callback: Callable, user_data: Any = None)
```

Stop a running photo timelapse.

### `stop_photo_interval`

```python
def stop_photo_interval(component_id)
```

Get stop_photo_interval (blocking)

### `start_video_async`

```python
def start_video_async(component_id, callback: Callable, user_data: Any = None)
```

Start a video recording.

### `start_video`

```python
def start_video(component_id)
```

Get start_video (blocking)

### `stop_video_async`

```python
def stop_video_async(component_id, callback: Callable, user_data: Any = None)
```

Stop a running video recording.

### `stop_video`

```python
def stop_video(component_id)
```

Get stop_video (blocking)

### `start_video_streaming`

```python
def start_video_streaming(component_id, stream_id)
```

Get start_video_streaming (blocking)

### `stop_video_streaming`

```python
def stop_video_streaming(component_id, stream_id)
```

Get stop_video_streaming (blocking)

### `set_mode_async`

```python
def set_mode_async(component_id, mode, callback: Callable, user_data: Any = None)
```

Set camera mode.

### `set_mode`

```python
def set_mode(component_id, mode)
```

Get set_mode (blocking)

### `list_photos_async`

```python
def list_photos_async(component_id, photos_range, callback: Callable, user_data: Any = None)
```

List photos available on the camera.

Note that this might need to be called initially to set the PhotosRange accordingly.
Once set to 'all' rather than 'since connection', it will try to request the previous
images over time.

### `list_photos`

```python
def list_photos(component_id, photos_range)
```

Get list_photos (blocking)

### `subscribe_camera_list`

```python
def subscribe_camera_list(callback: Callable, user_data: Any = None)
```

Subscribe to list of cameras.

This allows to find out what cameras are connected to the system.
Based on the camera ID, we can then address a specific camera.

### `unsubscribe_camera_list`

```python
def unsubscribe_camera_list(handle: Handle)
```

Unsubscribe from camera_list

### `camera_list`

```python
def camera_list()
```

Get camera_list (blocking)

### `subscribe_mode`

```python
def subscribe_mode(callback: Callable, user_data: Any = None)
```

Subscribe to camera mode updates.

### `unsubscribe_mode`

```python
def unsubscribe_mode(handle: Handle)
```

Unsubscribe from mode

### `get_mode`

```python
def get_mode(component_id)
```

Get get_mode (blocking)

### `subscribe_video_stream_info`

```python
def subscribe_video_stream_info(callback: Callable, user_data: Any = None)
```

Subscribe to video stream info updates.

### `unsubscribe_video_stream_info`

```python
def unsubscribe_video_stream_info(handle: Handle)
```

Unsubscribe from video_stream_info

### `get_video_stream_info`

```python
def get_video_stream_info(component_id)
```

Get get_video_stream_info (blocking)

### `subscribe_capture_info`

```python
def subscribe_capture_info(callback: Callable, user_data: Any = None)
```

Subscribe to capture info updates.

### `unsubscribe_capture_info`

```python
def unsubscribe_capture_info(handle: Handle)
```

Unsubscribe from capture_info

### `subscribe_storage`

```python
def subscribe_storage(callback: Callable, user_data: Any = None)
```

Subscribe to camera's storage status updates.

### `unsubscribe_storage`

```python
def unsubscribe_storage(handle: Handle)
```

Unsubscribe from storage

### `get_storage`

```python
def get_storage(component_id)
```

Get get_storage (blocking)

### `subscribe_current_settings`

```python
def subscribe_current_settings(callback: Callable, user_data: Any = None)
```

Get the list of current camera settings.

### `unsubscribe_current_settings`

```python
def unsubscribe_current_settings(handle: Handle)
```

Unsubscribe from current_settings

### `get_current_settings`

```python
def get_current_settings(component_id)
```

Get get_current_settings (blocking)

### `subscribe_possible_setting_options`

```python
def subscribe_possible_setting_options(callback: Callable, user_data: Any = None)
```

Get the list of settings that can be changed.

### `unsubscribe_possible_setting_options`

```python
def unsubscribe_possible_setting_options(handle: Handle)
```

Unsubscribe from possible_setting_options

### `get_possible_setting_options`

```python
def get_possible_setting_options(component_id)
```

Get get_possible_setting_options (blocking)

### `set_setting_async`

```python
def set_setting_async(component_id, setting, callback: Callable, user_data: Any = None)
```

Set a setting to some value.

Only setting_id of setting and option_id of option needs to be set.

### `set_setting`

```python
def set_setting(component_id, setting)
```

Get set_setting (blocking)

### `get_setting_async`

```python
def get_setting_async(component_id, setting, callback: Callable, user_data: Any = None)
```

Get a setting.

Only setting_id of setting needs to be set.

### `get_setting`

```python
def get_setting(component_id, setting)
```

Get get_setting (blocking)

### `format_storage_async`

```python
def format_storage_async(component_id, storage_id, callback: Callable, user_data: Any = None)
```

Format storage (e.g. SD card) in camera.

This will delete all content of the camera storage!

### `format_storage`

```python
def format_storage(component_id, storage_id)
```

Get format_storage (blocking)

### `reset_settings_async`

```python
def reset_settings_async(component_id, callback: Callable, user_data: Any = None)
```

Reset all settings in camera.

This will reset all camera settings to default value

### `reset_settings`

```python
def reset_settings(component_id)
```

Get reset_settings (blocking)

### `zoom_in_start_async`

```python
def zoom_in_start_async(component_id, callback: Callable, user_data: Any = None)
```

Start zooming in.

### `zoom_in_start`

```python
def zoom_in_start(component_id)
```

Get zoom_in_start (blocking)

### `zoom_out_start_async`

```python
def zoom_out_start_async(component_id, callback: Callable, user_data: Any = None)
```

Start zooming out.

### `zoom_out_start`

```python
def zoom_out_start(component_id)
```

Get zoom_out_start (blocking)

### `zoom_stop_async`

```python
def zoom_stop_async(component_id, callback: Callable, user_data: Any = None)
```

Stop zooming.

### `zoom_stop`

```python
def zoom_stop(component_id)
```

Get zoom_stop (blocking)

### `zoom_range_async`

```python
def zoom_range_async(component_id, range, callback: Callable, user_data: Any = None)
```

Zoom to value as proportion of full camera range (percentage between 0.0 and 100.0).

### `zoom_range`

```python
def zoom_range(component_id, range)
```

Get zoom_range (blocking)

### `track_point_async`

```python
def track_point_async(component_id, point_x, point_y, radius, callback: Callable, user_data: Any = None)
```

Track point.

### `track_point`

```python
def track_point(component_id, point_x, point_y, radius)
```

Get track_point (blocking)

### `track_rectangle_async`

```python
def track_rectangle_async(component_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y, callback: Callable, user_data: Any = None)
```

Track rectangle.

### `track_rectangle`

```python
def track_rectangle(component_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y)
```

Get track_rectangle (blocking)

### `track_stop_async`

```python
def track_stop_async(component_id, callback: Callable, user_data: Any = None)
```

Stop tracking.

### `track_stop`

```python
def track_stop(component_id)
```

Get track_stop (blocking)

### `focus_in_start_async`

```python
def focus_in_start_async(component_id, callback: Callable, user_data: Any = None)
```

Start focusing in.

### `focus_in_start`

```python
def focus_in_start(component_id)
```

Get focus_in_start (blocking)

### `focus_out_start_async`

```python
def focus_out_start_async(component_id, callback: Callable, user_data: Any = None)
```

Start focusing out.

### `focus_out_start`

```python
def focus_out_start(component_id)
```

Get focus_out_start (blocking)

### `focus_stop_async`

```python
def focus_stop_async(component_id, callback: Callable, user_data: Any = None)
```

Stop focus.

### `focus_stop`

```python
def focus_stop(component_id)
```

Get focus_stop (blocking)

### `focus_range_async`

```python
def focus_range_async(component_id, range, callback: Callable, user_data: Any = None)
```

Focus with range value of full range (value between 0.0 and 100.0).

### `focus_range`

```python
def focus_range(component_id, range)
```

Get focus_range (blocking)
