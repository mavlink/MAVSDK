# mavsdk::Camera::VideoStreamSettings Struct Reference
`#include: camera.h`

----


Type for video stream settings. 


## Data Fields


float [frame_rate_hz](#structmavsdk_1_1_camera_1_1_video_stream_settings_1ae638dd09c73fb7b375871c37eac54959) {} - Frames per second.

uint32_t [horizontal_resolution_pix](#structmavsdk_1_1_camera_1_1_video_stream_settings_1a4e0d6c3ee9a7c9798a63cabd63f97daf) {} - Horizontal resolution (in pixels)

uint32_t [vertical_resolution_pix](#structmavsdk_1_1_camera_1_1_video_stream_settings_1abaf114324d371b1adeec18cfa6cf243a) {} - Vertical resolution (in pixels)

uint32_t [bit_rate_b_s](#structmavsdk_1_1_camera_1_1_video_stream_settings_1af32e3577263d4a94cdc92839e3074d42) {} - Bit rate (in bits per second)

uint32_t [rotation_deg](#structmavsdk_1_1_camera_1_1_video_stream_settings_1ac96202a9dabb5699322986d526d48d0a) {} - Video image rotation (clockwise, 0-359 degrees)

std::string [uri](#structmavsdk_1_1_camera_1_1_video_stream_settings_1add3b459a45527b64b21eaf64757d7bf2) {} - Video stream URI.

float [horizontal_fov_deg](#structmavsdk_1_1_camera_1_1_video_stream_settings_1a8375b4e6ff5a44ff0af61b8997de2897) {} - Horizontal fov in degrees.


## Field Documentation


### frame_rate_hz {#structmavsdk_1_1_camera_1_1_video_stream_settings_1ae638dd09c73fb7b375871c37eac54959}

```cpp
float mavsdk::Camera::VideoStreamSettings::frame_rate_hz {}
```


Frames per second.


### horizontal_resolution_pix {#structmavsdk_1_1_camera_1_1_video_stream_settings_1a4e0d6c3ee9a7c9798a63cabd63f97daf}

```cpp
uint32_t mavsdk::Camera::VideoStreamSettings::horizontal_resolution_pix {}
```


Horizontal resolution (in pixels)


### vertical_resolution_pix {#structmavsdk_1_1_camera_1_1_video_stream_settings_1abaf114324d371b1adeec18cfa6cf243a}

```cpp
uint32_t mavsdk::Camera::VideoStreamSettings::vertical_resolution_pix {}
```


Vertical resolution (in pixels)


### bit_rate_b_s {#structmavsdk_1_1_camera_1_1_video_stream_settings_1af32e3577263d4a94cdc92839e3074d42}

```cpp
uint32_t mavsdk::Camera::VideoStreamSettings::bit_rate_b_s {}
```


Bit rate (in bits per second)


### rotation_deg {#structmavsdk_1_1_camera_1_1_video_stream_settings_1ac96202a9dabb5699322986d526d48d0a}

```cpp
uint32_t mavsdk::Camera::VideoStreamSettings::rotation_deg {}
```


Video image rotation (clockwise, 0-359 degrees)


### uri {#structmavsdk_1_1_camera_1_1_video_stream_settings_1add3b459a45527b64b21eaf64757d7bf2}

```cpp
std::string mavsdk::Camera::VideoStreamSettings::uri {}
```


Video stream URI.


### horizontal_fov_deg {#structmavsdk_1_1_camera_1_1_video_stream_settings_1a8375b4e6ff5a44ff0af61b8997de2897}

```cpp
float mavsdk::Camera::VideoStreamSettings::horizontal_fov_deg {}
```


Horizontal fov in degrees.

