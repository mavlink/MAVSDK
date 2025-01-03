# mavsdk::CameraServer::Information Struct Reference
`#include: camera_server.h`

----


Type to represent a camera information. 


## Data Fields


std::string [vendor_name](#structmavsdk_1_1_camera_server_1_1_information_1a3f9431423765da73f6bcb44be29a8f2f) {} - Name of the camera vendor.

std::string [model_name](#structmavsdk_1_1_camera_server_1_1_information_1ae1a12277b6b6bbe7c0fc7bedda3d985e) {} - Name of the camera model.

std::string [firmware_version](#structmavsdk_1_1_camera_server_1_1_information_1a90137fa1a2141eec7c9dd74833fde8c2) {} - [Camera](classmavsdk_1_1_camera.md) firmware version in major[.minor[.patch[.dev]]] format.

float [focal_length_mm](#structmavsdk_1_1_camera_server_1_1_information_1ac645588029a9106ec6aacb50cf45da8e) {} - Focal length.

float [horizontal_sensor_size_mm](#structmavsdk_1_1_camera_server_1_1_information_1ace5df86c5f6beb1bacb5713a21f80945) {} - Horizontal sensor size.

float [vertical_sensor_size_mm](#structmavsdk_1_1_camera_server_1_1_information_1aac983c253d311928cd97b2c939468074) {} - Vertical sensor size.

uint32_t [horizontal_resolution_px](#structmavsdk_1_1_camera_server_1_1_information_1a7975af21c8e492c9f0558886660e04d3) {} - Horizontal image resolution in pixels.

uint32_t [vertical_resolution_px](#structmavsdk_1_1_camera_server_1_1_information_1ae83a311d8fd37a16868803f9b65e1656) {} - Vertical image resolution in pixels.

uint32_t [lens_id](#structmavsdk_1_1_camera_server_1_1_information_1a59d0a7f30a748175399caf751745c5ac) {} - Lens ID.

uint32_t [definition_file_version](#structmavsdk_1_1_camera_server_1_1_information_1ad26af2024edddb500d5e1e340afb114c) {} - [Camera](classmavsdk_1_1_camera.md) definition file version (iteration)

std::string [definition_file_uri](#structmavsdk_1_1_camera_server_1_1_information_1ac45dbba688c93cafe5750fc352917797) {} - [Camera](classmavsdk_1_1_camera.md) definition URI (http or mavlink ftp)


## Field Documentation


### vendor_name {#structmavsdk_1_1_camera_server_1_1_information_1a3f9431423765da73f6bcb44be29a8f2f}

```cpp
std::string mavsdk::CameraServer::Information::vendor_name {}
```


Name of the camera vendor.


### model_name {#structmavsdk_1_1_camera_server_1_1_information_1ae1a12277b6b6bbe7c0fc7bedda3d985e}

```cpp
std::string mavsdk::CameraServer::Information::model_name {}
```


Name of the camera model.


### firmware_version {#structmavsdk_1_1_camera_server_1_1_information_1a90137fa1a2141eec7c9dd74833fde8c2}

```cpp
std::string mavsdk::CameraServer::Information::firmware_version {}
```


[Camera](classmavsdk_1_1_camera.md) firmware version in major[.minor[.patch[.dev]]] format.


### focal_length_mm {#structmavsdk_1_1_camera_server_1_1_information_1ac645588029a9106ec6aacb50cf45da8e}

```cpp
float mavsdk::CameraServer::Information::focal_length_mm {}
```


Focal length.


### horizontal_sensor_size_mm {#structmavsdk_1_1_camera_server_1_1_information_1ace5df86c5f6beb1bacb5713a21f80945}

```cpp
float mavsdk::CameraServer::Information::horizontal_sensor_size_mm {}
```


Horizontal sensor size.


### vertical_sensor_size_mm {#structmavsdk_1_1_camera_server_1_1_information_1aac983c253d311928cd97b2c939468074}

```cpp
float mavsdk::CameraServer::Information::vertical_sensor_size_mm {}
```


Vertical sensor size.


### horizontal_resolution_px {#structmavsdk_1_1_camera_server_1_1_information_1a7975af21c8e492c9f0558886660e04d3}

```cpp
uint32_t mavsdk::CameraServer::Information::horizontal_resolution_px {}
```


Horizontal image resolution in pixels.


### vertical_resolution_px {#structmavsdk_1_1_camera_server_1_1_information_1ae83a311d8fd37a16868803f9b65e1656}

```cpp
uint32_t mavsdk::CameraServer::Information::vertical_resolution_px {}
```


Vertical image resolution in pixels.


### lens_id {#structmavsdk_1_1_camera_server_1_1_information_1a59d0a7f30a748175399caf751745c5ac}

```cpp
uint32_t mavsdk::CameraServer::Information::lens_id {}
```


Lens ID.


### definition_file_version {#structmavsdk_1_1_camera_server_1_1_information_1ad26af2024edddb500d5e1e340afb114c}

```cpp
uint32_t mavsdk::CameraServer::Information::definition_file_version {}
```


[Camera](classmavsdk_1_1_camera.md) definition file version (iteration)


### definition_file_uri {#structmavsdk_1_1_camera_server_1_1_information_1ac45dbba688c93cafe5750fc352917797}

```cpp
std::string mavsdk::CameraServer::Information::definition_file_uri {}
```


[Camera](classmavsdk_1_1_camera.md) definition URI (http or mavlink ftp)

