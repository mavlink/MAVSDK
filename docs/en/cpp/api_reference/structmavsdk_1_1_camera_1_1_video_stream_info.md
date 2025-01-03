# mavsdk::Camera::VideoStreamInfo Struct Reference
`#include: camera.h`

----


[Information](structmavsdk_1_1_camera_1_1_information.md) about the video stream. 


## Public Types


Type | Description
--- | ---
enum [VideoStreamStatus](#structmavsdk_1_1_camera_1_1_video_stream_info_1a1750d70ce75b862be98164b67026d85c) | Video stream status type.
enum [VideoStreamSpectrum](#structmavsdk_1_1_camera_1_1_video_stream_info_1a51a8994b04aae4b78b3ba3279344584f) | Video stream light spectrum type.

## Data Fields


[VideoStreamSettings](structmavsdk_1_1_camera_1_1_video_stream_settings.md) [settings](#structmavsdk_1_1_camera_1_1_video_stream_info_1a11d9b81db8dc7b666ff9cd37d3835ead) {} - Video stream settings.

[VideoStreamStatus](structmavsdk_1_1_camera_1_1_video_stream_info.md#structmavsdk_1_1_camera_1_1_video_stream_info_1a1750d70ce75b862be98164b67026d85c) [status](#structmavsdk_1_1_camera_1_1_video_stream_info_1a08c5a2c0f4109d9a61a7e0b7edffde96) {} - Current status of video streaming.

[VideoStreamSpectrum](structmavsdk_1_1_camera_1_1_video_stream_info.md#structmavsdk_1_1_camera_1_1_video_stream_info_1a51a8994b04aae4b78b3ba3279344584f) [spectrum](#structmavsdk_1_1_camera_1_1_video_stream_info_1acedc8becdc55b9893c38ef5da6de5bb4) {} - Light-spectrum of the video stream.


## Member Enumeration Documentation


### enum VideoStreamStatus {#structmavsdk_1_1_camera_1_1_video_stream_info_1a1750d70ce75b862be98164b67026d85c}


Video stream status type.


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_1_1_video_stream_info_1a1750d70ce75b862be98164b67026d85cae457ff7a83d0a8681fa483f898788515"></span> `NotRunning` | Video stream is not running. 
<span id="structmavsdk_1_1_camera_1_1_video_stream_info_1a1750d70ce75b862be98164b67026d85ca12d868c18cb29bf58f02b504be9033fd"></span> `InProgress` | Video stream is running. 

### enum VideoStreamSpectrum {#structmavsdk_1_1_camera_1_1_video_stream_info_1a51a8994b04aae4b78b3ba3279344584f}


Video stream light spectrum type.


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_1_1_video_stream_info_1a51a8994b04aae4b78b3ba3279344584fa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown. 
<span id="structmavsdk_1_1_camera_1_1_video_stream_info_1a51a8994b04aae4b78b3ba3279344584fa6bed35ab1d605887f9774a728d47a23a"></span> `VisibleLight` | Visible light. 
<span id="structmavsdk_1_1_camera_1_1_video_stream_info_1a51a8994b04aae4b78b3ba3279344584fade1c318db60bc972858577273c2d1ddd"></span> `Infrared` | Infrared. 

## Field Documentation


### settings {#structmavsdk_1_1_camera_1_1_video_stream_info_1a11d9b81db8dc7b666ff9cd37d3835ead}

```cpp
VideoStreamSettings mavsdk::Camera::VideoStreamInfo::settings {}
```


Video stream settings.


### status {#structmavsdk_1_1_camera_1_1_video_stream_info_1a08c5a2c0f4109d9a61a7e0b7edffde96}

```cpp
VideoStreamStatus mavsdk::Camera::VideoStreamInfo::status {}
```


Current status of video streaming.


### spectrum {#structmavsdk_1_1_camera_1_1_video_stream_info_1acedc8becdc55b9893c38ef5da6de5bb4}

```cpp
VideoStreamSpectrum mavsdk::Camera::VideoStreamInfo::spectrum {}
```


Light-spectrum of the video stream.

