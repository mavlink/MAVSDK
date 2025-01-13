# mavsdk::Camera::CaptureInfo Struct Reference
`#include: camera.h`

----


[Information](structmavsdk_1_1_camera_1_1_information.md) about a picture just captured. 


## Data Fields


[Position](structmavsdk_1_1_camera_1_1_position.md) [position](#structmavsdk_1_1_camera_1_1_capture_info_1a570779eaff94d3cf3c1a160938096d0f) {} - Location where the picture was taken.

[Quaternion](structmavsdk_1_1_camera_1_1_quaternion.md) [attitude_quaternion](#structmavsdk_1_1_camera_1_1_capture_info_1add20201828d0e0a82a6ee6366022abc4) {} - Attitude of the camera when the picture was taken (quaternion)

[EulerAngle](structmavsdk_1_1_camera_1_1_euler_angle.md) [attitude_euler_angle](#structmavsdk_1_1_camera_1_1_capture_info_1a6265295864a8f4e7081a34edb070a531) {} - Attitude of the camera when the picture was taken (euler angle)

uint64_t [time_utc_us](#structmavsdk_1_1_camera_1_1_capture_info_1ad0e92389ab305025ce3cebe0e35d0b51) {} - Timestamp in UTC (since UNIX epoch) in microseconds.

bool [is_success](#structmavsdk_1_1_camera_1_1_capture_info_1a654b17853337aaa3e46b9e70394642d0) {} - True if the capture was successful.

int32_t [index](#structmavsdk_1_1_camera_1_1_capture_info_1ad8d31a8640a824a6e8c449a49bdd4f82) {} - Zero-based index of this image since vehicle was armed.

std::string [file_url](#structmavsdk_1_1_camera_1_1_capture_info_1a6fa362c6a13fe4fafe40d8cb5043407f) {} - Download URL of this image.


## Field Documentation


### position {#structmavsdk_1_1_camera_1_1_capture_info_1a570779eaff94d3cf3c1a160938096d0f}

```cpp
Position mavsdk::Camera::CaptureInfo::position {}
```


Location where the picture was taken.


### attitude_quaternion {#structmavsdk_1_1_camera_1_1_capture_info_1add20201828d0e0a82a6ee6366022abc4}

```cpp
Quaternion mavsdk::Camera::CaptureInfo::attitude_quaternion {}
```


Attitude of the camera when the picture was taken (quaternion)


### attitude_euler_angle {#structmavsdk_1_1_camera_1_1_capture_info_1a6265295864a8f4e7081a34edb070a531}

```cpp
EulerAngle mavsdk::Camera::CaptureInfo::attitude_euler_angle {}
```


Attitude of the camera when the picture was taken (euler angle)


### time_utc_us {#structmavsdk_1_1_camera_1_1_capture_info_1ad0e92389ab305025ce3cebe0e35d0b51}

```cpp
uint64_t mavsdk::Camera::CaptureInfo::time_utc_us {}
```


Timestamp in UTC (since UNIX epoch) in microseconds.


### is_success {#structmavsdk_1_1_camera_1_1_capture_info_1a654b17853337aaa3e46b9e70394642d0}

```cpp
bool mavsdk::Camera::CaptureInfo::is_success {}
```


True if the capture was successful.


### index {#structmavsdk_1_1_camera_1_1_capture_info_1ad8d31a8640a824a6e8c449a49bdd4f82}

```cpp
int32_t mavsdk::Camera::CaptureInfo::index {}
```


Zero-based index of this image since vehicle was armed.


### file_url {#structmavsdk_1_1_camera_1_1_capture_info_1a6fa362c6a13fe4fafe40d8cb5043407f}

```cpp
std::string mavsdk::Camera::CaptureInfo::file_url {}
```


Download URL of this image.

