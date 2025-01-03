# mavsdk::CameraServer::StorageInformation Struct Reference
`#include: camera_server.h`

----


[Information](structmavsdk_1_1_camera_server_1_1_information.md) about the camera storage. 


## Public Types


Type | Description
--- | ---
enum [StorageStatus](#structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910f) | Storage status type.
enum [StorageType](#structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7) | Storage type.

## Data Fields


float [used_storage_mib](#structmavsdk_1_1_camera_server_1_1_storage_information_1a94040916ab463c2ddba0d68852ea382a) {} - Used storage (in MiB)

float [available_storage_mib](#structmavsdk_1_1_camera_server_1_1_storage_information_1ae781ac7b52fa2c140f2633e13dacf4b2) {} - Available storage (in MiB)

float [total_storage_mib](#structmavsdk_1_1_camera_server_1_1_storage_information_1a72aa03291a20f7985ab73f933348d8cc) {} - Total storage (in MiB)

[StorageStatus](structmavsdk_1_1_camera_server_1_1_storage_information.md#structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910f) [storage_status](#structmavsdk_1_1_camera_server_1_1_storage_information_1a78f55db7073e7d96c4a606b65c304f2a) {} - Storage status.

uint32_t [storage_id](#structmavsdk_1_1_camera_server_1_1_storage_information_1a3ba45bcbdd1bfd6402d09ee82828e56d) {} - Storage ID starting at 1.

[StorageType](structmavsdk_1_1_camera_server_1_1_storage_information.md#structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7) [storage_type](#structmavsdk_1_1_camera_server_1_1_storage_information_1a12f4d2dfa4429adebe0a38749f052794) {} - Storage type.

float [read_speed_mib_s](#structmavsdk_1_1_camera_server_1_1_storage_information_1a8df6bf66e2d436c388cb834a1e5bbe98) {} - Read speed [MiB/s].

float [write_speed_mib_s](#structmavsdk_1_1_camera_server_1_1_storage_information_1ac7aeae4770a810cd226cb05f4d6db982) {} - Write speed [MiB/s].


## Member Enumeration Documentation


### enum StorageStatus {#structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910f}


Storage status type.


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910fa534ceac854da4ba59c4dc41b7ab732dc"></span> `NotAvailable` | Storage not available. 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910facbe526fde94ab97f641ac0cb6d4b624b"></span> `Unformatted` | Storage is not formatted (i.e. has no recognized file system). 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910fa550ed376863f4dfb07120a0aa2d249db"></span> `Formatted` | Storage is formatted (i.e. has recognized a file system). 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1a82d24373786c582c377ac9671282910fa9ed2d871602556951e39f3cebd08d6cb"></span> `NotSupported` | Storage status is not supported. 

### enum StorageType {#structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7}


Storage type.


Value | Description
--- | ---
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Storage type unknown. 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7abde95cc0c5bed9c67d73515b8d5d7532"></span> `UsbStick` | Storage type USB stick. 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7a05c89bdbbeeae48a6daccce0d05257fd"></span> `Sd` | Storage type SD card. 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7ad837d75833970e2240058444912a5f9a"></span> `Microsd` | Storage type MicroSD card. 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7a9bae1e7e6957fa30f9258d6b590d936a"></span> `Hd` | Storage type HD mass storage. 
<span id="structmavsdk_1_1_camera_server_1_1_storage_information_1af3105ba56bf0475a5c9c0530270341d7a6311ae17c1ee52b36e68aaf4ad066387"></span> `Other` | Storage type other, not listed. 

## Field Documentation


### used_storage_mib {#structmavsdk_1_1_camera_server_1_1_storage_information_1a94040916ab463c2ddba0d68852ea382a}

```cpp
float mavsdk::CameraServer::StorageInformation::used_storage_mib {}
```


Used storage (in MiB)


### available_storage_mib {#structmavsdk_1_1_camera_server_1_1_storage_information_1ae781ac7b52fa2c140f2633e13dacf4b2}

```cpp
float mavsdk::CameraServer::StorageInformation::available_storage_mib {}
```


Available storage (in MiB)


### total_storage_mib {#structmavsdk_1_1_camera_server_1_1_storage_information_1a72aa03291a20f7985ab73f933348d8cc}

```cpp
float mavsdk::CameraServer::StorageInformation::total_storage_mib {}
```


Total storage (in MiB)


### storage_status {#structmavsdk_1_1_camera_server_1_1_storage_information_1a78f55db7073e7d96c4a606b65c304f2a}

```cpp
StorageStatus mavsdk::CameraServer::StorageInformation::storage_status {}
```


Storage status.


### storage_id {#structmavsdk_1_1_camera_server_1_1_storage_information_1a3ba45bcbdd1bfd6402d09ee82828e56d}

```cpp
uint32_t mavsdk::CameraServer::StorageInformation::storage_id {}
```


Storage ID starting at 1.


### storage_type {#structmavsdk_1_1_camera_server_1_1_storage_information_1a12f4d2dfa4429adebe0a38749f052794}

```cpp
StorageType mavsdk::CameraServer::StorageInformation::storage_type {}
```


Storage type.


### read_speed_mib_s {#structmavsdk_1_1_camera_server_1_1_storage_information_1a8df6bf66e2d436c388cb834a1e5bbe98}

```cpp
float mavsdk::CameraServer::StorageInformation::read_speed_mib_s {}
```


Read speed [MiB/s].


### write_speed_mib_s {#structmavsdk_1_1_camera_server_1_1_storage_information_1ac7aeae4770a810cd226cb05f4d6db982}

```cpp
float mavsdk::CameraServer::StorageInformation::write_speed_mib_s {}
```


Write speed [MiB/s].

