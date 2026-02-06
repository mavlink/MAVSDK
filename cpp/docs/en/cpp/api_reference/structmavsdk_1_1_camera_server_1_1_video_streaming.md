# mavsdk::CameraServer::VideoStreaming Struct Reference
`#include: camera_server.h`

----


Type to represent video streaming settings. 


## Data Fields


bool [has_rtsp_server](#structmavsdk_1_1_camera_server_1_1_video_streaming_1aae5b34470f3594cf7e46404d77dc8bee) {} - True if the capture was successful.

std::string [rtsp_uri](#structmavsdk_1_1_camera_server_1_1_video_streaming_1a29a2555577945dc6d64de15772c4a8ab) {} - RTSP URI (e.g. rtsp://192.168.1.42:8554/live)


## Field Documentation


### has_rtsp_server {#structmavsdk_1_1_camera_server_1_1_video_streaming_1aae5b34470f3594cf7e46404d77dc8bee}

```cpp
bool mavsdk::CameraServer::VideoStreaming::has_rtsp_server {}
```


True if the capture was successful.


### rtsp_uri {#structmavsdk_1_1_camera_server_1_1_video_streaming_1a29a2555577945dc6d64de15772c4a8ab}

```cpp
std::string mavsdk::CameraServer::VideoStreaming::rtsp_uri {}
```


RTSP URI (e.g. rtsp://192.168.1.42:8554/live)

