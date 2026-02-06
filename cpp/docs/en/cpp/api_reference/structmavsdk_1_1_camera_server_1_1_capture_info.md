# mavsdk::CameraServer::CaptureInfo Struct Reference
`#include: camera_server.h`

----


[Information](structmavsdk_1_1_camera_server_1_1_information.md) about a picture just captured. 


## Data Fields


[Position](structmavsdk_1_1_camera_server_1_1_position.md) [position](#structmavsdk_1_1_camera_server_1_1_capture_info_1a102ac2967a54137616bd1f80275a39d2) {} - Location where the picture was taken.

[Quaternion](structmavsdk_1_1_camera_server_1_1_quaternion.md) [attitude_quaternion](#structmavsdk_1_1_camera_server_1_1_capture_info_1a4a94c6c925f09106212b4af3ba3bde97) {} - Attitude of the camera when the picture was taken (quaternion)

uint64_t [time_utc_us](#structmavsdk_1_1_camera_server_1_1_capture_info_1afbc9b59d9f231ae420fd36c0a847f466) {} - Timestamp in UTC (since UNIX epoch) in microseconds.

bool [is_success](#structmavsdk_1_1_camera_server_1_1_capture_info_1a5622147d5467314a0b1e30a28b097484) {} - True if the capture was successful.

int32_t [index](#structmavsdk_1_1_camera_server_1_1_capture_info_1acd2187b662182cc5f9b5d8b5b56b2ac0) {} - Index from TakePhotoResponse.

std::string [file_url](#structmavsdk_1_1_camera_server_1_1_capture_info_1aed11ca12c1a2458b9b9b43d49d9581e9) {} - Download URL of this image.


## Field Documentation


### position {#structmavsdk_1_1_camera_server_1_1_capture_info_1a102ac2967a54137616bd1f80275a39d2}

```cpp
Position mavsdk::CameraServer::CaptureInfo::position {}
```


Location where the picture was taken.


### attitude_quaternion {#structmavsdk_1_1_camera_server_1_1_capture_info_1a4a94c6c925f09106212b4af3ba3bde97}

```cpp
Quaternion mavsdk::CameraServer::CaptureInfo::attitude_quaternion {}
```


Attitude of the camera when the picture was taken (quaternion)


### time_utc_us {#structmavsdk_1_1_camera_server_1_1_capture_info_1afbc9b59d9f231ae420fd36c0a847f466}

```cpp
uint64_t mavsdk::CameraServer::CaptureInfo::time_utc_us {}
```


Timestamp in UTC (since UNIX epoch) in microseconds.


### is_success {#structmavsdk_1_1_camera_server_1_1_capture_info_1a5622147d5467314a0b1e30a28b097484}

```cpp
bool mavsdk::CameraServer::CaptureInfo::is_success {}
```


True if the capture was successful.


### index {#structmavsdk_1_1_camera_server_1_1_capture_info_1acd2187b662182cc5f9b5d8b5b56b2ac0}

```cpp
int32_t mavsdk::CameraServer::CaptureInfo::index {}
```


Index from TakePhotoResponse.


### file_url {#structmavsdk_1_1_camera_server_1_1_capture_info_1aed11ca12c1a2458b9b9b43d49d9581e9}

```cpp
std::string mavsdk::CameraServer::CaptureInfo::file_url {}
```


Download URL of this image.

