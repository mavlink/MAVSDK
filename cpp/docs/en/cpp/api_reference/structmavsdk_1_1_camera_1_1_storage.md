# mavsdk::Camera::Storage Struct Reference
`#include: camera.h`

----


[Information](structmavsdk_1_1_camera_1_1_information.md) about the camera's storage status. 


## Public Types


Type | Description
--- | ---
enum [StorageStatus](#structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49a) | [Storage](structmavsdk_1_1_camera_1_1_storage.md) status type.
enum [StorageType](#structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290) | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type.

## Data Fields


int32_t [component_id](#structmavsdk_1_1_camera_1_1_storage_1a3a8040ef921dec7b5b77e9bfb2406036) {} - Component ID.

bool [video_on](#structmavsdk_1_1_camera_1_1_storage_1aee007d646be2f4c6d32d8c23e1a83dc4) {} - Whether video recording is currently in process.

bool [photo_interval_on](#structmavsdk_1_1_camera_1_1_storage_1a91d5c2e653642b949889f5236ff7e1ae) {} - Whether a photo interval is currently in process.

float [used_storage_mib](#structmavsdk_1_1_camera_1_1_storage_1a524b19eb489baf9ac1eea908baf9cdd9) {} - Used storage (in MiB)

float [available_storage_mib](#structmavsdk_1_1_camera_1_1_storage_1a0f529192063ce6888c7411cce720ecc1) {} - Available storage (in MiB)

float [total_storage_mib](#structmavsdk_1_1_camera_1_1_storage_1a40b4ca56b8bd923fea7c50b43d1ae891) {} - Total storage (in MiB)

float [recording_time_s](#structmavsdk_1_1_camera_1_1_storage_1a903999c7001fe1f025369b35a3908a20) {} - Elapsed time since starting the video recording (in seconds)

std::string [media_folder_name](#structmavsdk_1_1_camera_1_1_storage_1a210b8451c96810b54bcb7359a934a663) {} - Current folder name where media are saved.

[StorageStatus](structmavsdk_1_1_camera_1_1_storage.md#structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49a) [storage_status](#structmavsdk_1_1_camera_1_1_storage_1a7547bb3cb47e22f64f21026ebbca890e) {} - [Storage](structmavsdk_1_1_camera_1_1_storage.md) status.

uint32_t [storage_id](#structmavsdk_1_1_camera_1_1_storage_1aa534f7cec458d9cbb075675e91db4127) {} - [Storage](structmavsdk_1_1_camera_1_1_storage.md) ID starting at 1.

[StorageType](structmavsdk_1_1_camera_1_1_storage.md#structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290) [storage_type](#structmavsdk_1_1_camera_1_1_storage_1a193dc7d75ea3278f81e73793d61ba8a5) {} - [Storage](structmavsdk_1_1_camera_1_1_storage.md) type.


## Member Enumeration Documentation


### enum StorageStatus {#structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49a}


[Storage](structmavsdk_1_1_camera_1_1_storage.md) status type.


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49aa534ceac854da4ba59c4dc41b7ab732dc"></span> `NotAvailable` | Status not available. 
<span id="structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49aacbe526fde94ab97f641ac0cb6d4b624b"></span> `Unformatted` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) is not formatted (i.e. has no recognized file system). 
<span id="structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49aa550ed376863f4dfb07120a0aa2d249db"></span> `Formatted` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) is formatted (i.e. has recognized a file system). 
<span id="structmavsdk_1_1_camera_1_1_storage_1a57ec40da608871d2a23fc6460a4bc49aa9ed2d871602556951e39f3cebd08d6cb"></span> `NotSupported` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) status is not supported. 

### enum StorageType {#structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290}


[Storage](structmavsdk_1_1_camera_1_1_storage.md) type.


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type unknown. 
<span id="structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290abde95cc0c5bed9c67d73515b8d5d7532"></span> `UsbStick` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type USB stick. 
<span id="structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290a05c89bdbbeeae48a6daccce0d05257fd"></span> `Sd` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type SD card. 
<span id="structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290ad837d75833970e2240058444912a5f9a"></span> `Microsd` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type MicroSD card. 
<span id="structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290a9bae1e7e6957fa30f9258d6b590d936a"></span> `Hd` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type HD mass storage. 
<span id="structmavsdk_1_1_camera_1_1_storage_1a404236fdf8dbab2570b3fc9cbeb4e290a6311ae17c1ee52b36e68aaf4ad066387"></span> `Other` | [Storage](structmavsdk_1_1_camera_1_1_storage.md) type other, not listed. 

## Field Documentation


### component_id {#structmavsdk_1_1_camera_1_1_storage_1a3a8040ef921dec7b5b77e9bfb2406036}

```cpp
int32_t mavsdk::Camera::Storage::component_id {}
```


Component ID.


### video_on {#structmavsdk_1_1_camera_1_1_storage_1aee007d646be2f4c6d32d8c23e1a83dc4}

```cpp
bool mavsdk::Camera::Storage::video_on {}
```


Whether video recording is currently in process.


### photo_interval_on {#structmavsdk_1_1_camera_1_1_storage_1a91d5c2e653642b949889f5236ff7e1ae}

```cpp
bool mavsdk::Camera::Storage::photo_interval_on {}
```


Whether a photo interval is currently in process.


### used_storage_mib {#structmavsdk_1_1_camera_1_1_storage_1a524b19eb489baf9ac1eea908baf9cdd9}

```cpp
float mavsdk::Camera::Storage::used_storage_mib {}
```


Used storage (in MiB)


### available_storage_mib {#structmavsdk_1_1_camera_1_1_storage_1a0f529192063ce6888c7411cce720ecc1}

```cpp
float mavsdk::Camera::Storage::available_storage_mib {}
```


Available storage (in MiB)


### total_storage_mib {#structmavsdk_1_1_camera_1_1_storage_1a40b4ca56b8bd923fea7c50b43d1ae891}

```cpp
float mavsdk::Camera::Storage::total_storage_mib {}
```


Total storage (in MiB)


### recording_time_s {#structmavsdk_1_1_camera_1_1_storage_1a903999c7001fe1f025369b35a3908a20}

```cpp
float mavsdk::Camera::Storage::recording_time_s {}
```


Elapsed time since starting the video recording (in seconds)


### media_folder_name {#structmavsdk_1_1_camera_1_1_storage_1a210b8451c96810b54bcb7359a934a663}

```cpp
std::string mavsdk::Camera::Storage::media_folder_name {}
```


Current folder name where media are saved.


### storage_status {#structmavsdk_1_1_camera_1_1_storage_1a7547bb3cb47e22f64f21026ebbca890e}

```cpp
StorageStatus mavsdk::Camera::Storage::storage_status {}
```


[Storage](structmavsdk_1_1_camera_1_1_storage.md) status.


### storage_id {#structmavsdk_1_1_camera_1_1_storage_1aa534f7cec458d9cbb075675e91db4127}

```cpp
uint32_t mavsdk::Camera::Storage::storage_id {}
```


[Storage](structmavsdk_1_1_camera_1_1_storage.md) ID starting at 1.


### storage_type {#structmavsdk_1_1_camera_1_1_storage_1a193dc7d75ea3278f81e73793d61ba8a5}

```cpp
StorageType mavsdk::Camera::Storage::storage_type {}
```


[Storage](structmavsdk_1_1_camera_1_1_storage.md) type.

