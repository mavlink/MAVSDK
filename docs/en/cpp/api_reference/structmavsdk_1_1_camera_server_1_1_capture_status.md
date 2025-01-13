# mavsdk::CameraServer::CaptureStatus Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [ImageStatus](#structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657cc) | 
enum [VideoStatus](#structmavsdk_1_1_camera_server_1_1_capture_status_1aa5805c521b56d734ab6d78be2084397a) | 

## Data Fields


float [image_interval_s](#structmavsdk_1_1_camera_server_1_1_capture_status_1a89f9eb4eddb220da0c29f7da2452ab81) {} - Image capture interval (in s)

float [recording_time_s](#structmavsdk_1_1_camera_server_1_1_capture_status_1afcbb06ba085b5c7d85779712301edfda) {} - Elapsed time since recording started (in s)

float [available_capacity_mib](#structmavsdk_1_1_camera_server_1_1_capture_status_1a1c6911a20d88dfbfd7ec46c97a490ea3) {} - Available storage capacity. (in MiB)

[ImageStatus](structmavsdk_1_1_camera_server_1_1_capture_status.md#structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657cc) [image_status](#structmavsdk_1_1_camera_server_1_1_capture_status_1a89d5deb5ae03057ac7275ce4278e4db9) {} - Current status of image capturing.

[VideoStatus](structmavsdk_1_1_camera_server_1_1_capture_status.md#structmavsdk_1_1_camera_server_1_1_capture_status_1aa5805c521b56d734ab6d78be2084397a) [video_status](#structmavsdk_1_1_camera_server_1_1_capture_status_1ac8317772ba04bb716f96c1227a1cd2d4) {} - Current status of video capturing.

int32_t [image_count](#structmavsdk_1_1_camera_server_1_1_capture_status_1a64feb9833b8f6df4ff6858109112ebbe) {} - Total number of images captured ('forever', or until reset using MAV_CMD_STORAGE_FORMAT)


## Member Enumeration Documentation


### enum ImageStatus {#structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657cc}


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657ccae599161956d626eda4cb0a5ffb85271c"></span> `Idle` | idle. 
<span id="structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657cca8c7d049850f3989c571a22b4275dc28a"></span> `CaptureInProgress` | capture in progress. 
<span id="structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657cca0e4a71d453c1c14e09154a3fa101ec50"></span> `IntervalIdle` | interval set but idle. 
<span id="structmavsdk_1_1_camera_server_1_1_capture_status_1ab9567d4fc7a19f6549f233684f2657cca485df5a54846e4125c2e1ca9dc2f1f1b"></span> `IntervalInProgress` | interval set and capture in progress). 

### enum VideoStatus {#structmavsdk_1_1_camera_server_1_1_capture_status_1aa5805c521b56d734ab6d78be2084397a}


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_server_1_1_capture_status_1aa5805c521b56d734ab6d78be2084397aae599161956d626eda4cb0a5ffb85271c"></span> `Idle` | idle. 
<span id="structmavsdk_1_1_camera_server_1_1_capture_status_1aa5805c521b56d734ab6d78be2084397aa8c7d049850f3989c571a22b4275dc28a"></span> `CaptureInProgress` | capture in progress. 

## Field Documentation


### image_interval_s {#structmavsdk_1_1_camera_server_1_1_capture_status_1a89f9eb4eddb220da0c29f7da2452ab81}

```cpp
float mavsdk::CameraServer::CaptureStatus::image_interval_s {}
```


Image capture interval (in s)


### recording_time_s {#structmavsdk_1_1_camera_server_1_1_capture_status_1afcbb06ba085b5c7d85779712301edfda}

```cpp
float mavsdk::CameraServer::CaptureStatus::recording_time_s {}
```


Elapsed time since recording started (in s)


### available_capacity_mib {#structmavsdk_1_1_camera_server_1_1_capture_status_1a1c6911a20d88dfbfd7ec46c97a490ea3}

```cpp
float mavsdk::CameraServer::CaptureStatus::available_capacity_mib {}
```


Available storage capacity. (in MiB)


### image_status {#structmavsdk_1_1_camera_server_1_1_capture_status_1a89d5deb5ae03057ac7275ce4278e4db9}

```cpp
ImageStatus mavsdk::CameraServer::CaptureStatus::image_status {}
```


Current status of image capturing.


### video_status {#structmavsdk_1_1_camera_server_1_1_capture_status_1ac8317772ba04bb716f96c1227a1cd2d4}

```cpp
VideoStatus mavsdk::CameraServer::CaptureStatus::video_status {}
```


Current status of video capturing.


### image_count {#structmavsdk_1_1_camera_server_1_1_capture_status_1a64feb9833b8f6df4ff6858109112ebbe}

```cpp
int32_t mavsdk::CameraServer::CaptureStatus::image_count {}
```


Total number of images captured ('forever', or until reset using MAV_CMD_STORAGE_FORMAT)

