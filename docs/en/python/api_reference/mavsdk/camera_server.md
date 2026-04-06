# Camera Server

Provides handling of camera interface

## Enums

### `CameraFeedback`

Possible feedback results for camera respond command.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `OK` (1) | |
| `BUSY` (2) | |
| `FAILED` (3) | |

### `Mode`

Camera mode type.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `PHOTO` (1) | |
| `VIDEO` (2) | |

### `CameraServerResult`

Possible results returned for action requests.

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

## Structs

### `InformationCStruct`

Internal C structure for Information.
Used only for C library communication.

### `VideoStreamingCStruct`

Internal C structure for VideoStreaming.
Used only for C library communication.

### `PositionCStruct`

Internal C structure for Position.
Used only for C library communication.

### `QuaternionCStruct`

Internal C structure for Quaternion.
Used only for C library communication.

### `CaptureInfoCStruct`

Internal C structure for CaptureInfo.
Used only for C library communication.

### `StorageInformationCStruct`

Internal C structure for StorageInformation.
Used only for C library communication.

### `CaptureStatusCStruct`

Internal C structure for CaptureStatus.
Used only for C library communication.

### `TrackPointCStruct`

Internal C structure for TrackPoint.
Used only for C library communication.

### `TrackRectangleCStruct`

Internal C structure for TrackRectangle.
Used only for C library communication.

### `Information`

Type to represent a camera information.

**Fields:**

- `vendor_name`
- `model_name`
- `firmware_version`
- `focal_length_mm`
- `horizontal_sensor_size_mm`
- `vertical_sensor_size_mm`
- `horizontal_resolution_px`
- `vertical_resolution_px`
- `lens_id`
- `definition_file_version`
- `definition_file_uri`
- `image_in_video_mode_supported`
- `video_in_image_mode_supported`

### `VideoStreaming`

Type to represent video streaming settings

**Fields:**

- `has_rtsp_server`
- `rtsp_uri`

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

### `CaptureInfo`

Information about a picture just captured.

**Fields:**

- `position`
- `attitude_quaternion`
- `time_utc_us`
- `is_success`
- `index`
- `file_url`

### `StorageInformation`

Information about the camera storage.

**Fields:**

- `used_storage_mib`
- `available_storage_mib`
- `total_storage_mib`
- `storage_status`
- `storage_id`
- `storage_type`
- `read_speed_mib_s`
- `write_speed_mib_s`

### `CaptureStatus`

Capture status

**Fields:**

- `image_interval_s`
- `recording_time_s`
- `available_capacity_mib`
- `image_status`
- `video_status`
- `image_count`

### `TrackPoint`

Point description type

**Fields:**

- `point_x`
- `point_y`
- `radius`

### `TrackRectangle`

Rectangle description type

**Fields:**

- `top_left_corner_x`
- `top_left_corner_y`
- `bottom_right_corner_x`
- `bottom_right_corner_y`

### `CameraServer`

Provides handling of camera interface

**Fields:**

- `server_component`
