# mavsdk::RemoteId::SystemId Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [OperatorLocationType](#structmavsdk_1_1_remote_id_1_1_system_id_1aaf0efa1b25f9775668f975494c59fd09) | 
enum [ClassificationType](#structmavsdk_1_1_remote_id_1_1_system_id_1a65d669332e60e9fa559cd6cf4bced264) | 
enum [CategoryEu](#structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736) | 
enum [ClassEu](#structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfc) | 

## Data Fields


[OperatorLocationType](structmavsdk_1_1_remote_id_1_1_system_id.md#structmavsdk_1_1_remote_id_1_1_system_id_1aaf0efa1b25f9775668f975494c59fd09) [operator_location_type](#structmavsdk_1_1_remote_id_1_1_system_id_1a51662a9b4bf39dc00e3be9c4cbf76cd7) {} - Specifies the operator location type.

[ClassificationType](structmavsdk_1_1_remote_id_1_1_system_id.md#structmavsdk_1_1_remote_id_1_1_system_id_1a65d669332e60e9fa559cd6cf4bced264) [classification_type](#structmavsdk_1_1_remote_id_1_1_system_id_1a58bc106670c306e7ae9f39991d71fd27) {} - Specifies the classification type of the UA.

double [operator_latitude_deg](#structmavsdk_1_1_remote_id_1_1_system_id_1a8c49eabf8bb6ce398a6520dee6ef0ca3) {} - Latitude of the operator.

double [operator_longitude_deg](#structmavsdk_1_1_remote_id_1_1_system_id_1aa1be7b9c32fd055f16f1b7f47233261d) {} - Longitude of the operator.

uint32_t [area_count](#structmavsdk_1_1_remote_id_1_1_system_id_1af9c1a95a0f0b4d0eeb31a08d75fcb5c8) {} - Number of aircraft in the area.

uint32_t [area_radius_m](#structmavsdk_1_1_remote_id_1_1_system_id_1a85384066656e1955ebc5e612f9c1bf36) {} - Radius of the cylindrical area of the group or formation.

float [area_ceiling_m](#structmavsdk_1_1_remote_id_1_1_system_id_1a9722e4085446724e04538473465e330b) {} - Area Operations Ceiling relative to WGS84.

float [area_floor_m](#structmavsdk_1_1_remote_id_1_1_system_id_1a81a5e44a16223390501d4a0840b44d0e) {} - Area Operations Floor relative to WGS84.

[CategoryEu](structmavsdk_1_1_remote_id_1_1_system_id.md#structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736) [category_eu](#structmavsdk_1_1_remote_id_1_1_system_id_1a6ec88b9d069b742344b64e8bbf5fc594) {} - When classification_type is MAV_ODID_CLASSIFICATION_TYPE_EU, specifies the category of the UA.

[ClassEu](structmavsdk_1_1_remote_id_1_1_system_id.md#structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfc) [class_eu](#structmavsdk_1_1_remote_id_1_1_system_id_1a955290e5c82ec19d9f738dc1b63645ba) {} - When classification_type is MAV_ODID_CLASSIFICATION_TYPE_EU, specifies the class of the UA.

float [operator_altitude_geo_m](#structmavsdk_1_1_remote_id_1_1_system_id_1a5a72096fffd0e131a6dd971e02525c73) {} - Geodetic altitude of the operator relative to WGS84. If unknown: -1000 m.

uint64_t [time_utc_us](#structmavsdk_1_1_remote_id_1_1_system_id_1acfdfc7367dbbecce85c66f8195af9c43) {} - Timestamp in UTC (since UNIX epoch) in microseconds.


## Member Enumeration Documentation


### enum OperatorLocationType {#structmavsdk_1_1_remote_id_1_1_system_id_1aaf0efa1b25f9775668f975494c59fd09}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1aaf0efa1b25f9775668f975494c59fd09a56373a80447c41b9a29e500e62d6884e"></span> `Takeoff` | The location/altitude of the operator is the same as the take-off location.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1aaf0efa1b25f9775668f975494c59fd09a2f8e5a60326bc09e2615e740745530a1"></span> `LiveGnss` | The location/altitude of the operator is dynamic. E.g. based on live GNSS data.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1aaf0efa1b25f9775668f975494c59fd09a4457d440870ad6d42bab9082d9bf9b61"></span> `Fixed` | The location/altitude of the operator are fixed values.. 

### enum ClassificationType {#structmavsdk_1_1_remote_id_1_1_system_id_1a65d669332e60e9fa559cd6cf4bced264}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a65d669332e60e9fa559cd6cf4bced264a2af457eff1694fff8c7ef6c3a8974ce6"></span> `Undeclared` | The classification type for the UA is undeclared.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a65d669332e60e9fa559cd6cf4bced264adbf59bd164fe55695cc7d960da4f1516"></span> `Eu` | The classification type for the UA follows EU (European Union) specifications.. 

### enum CategoryEu {#structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736a2af457eff1694fff8c7ef6c3a8974ce6"></span> `Undeclared` | The category for the UA, according to the EU specification, is undeclared.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736ac3bf447eabe632720a3aa1a7ce401274"></span> `Open` | The category for the UA, according to the EU specification, is the Open category.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736afe0619d5f4999e09c622e32da7e7566a"></span> `Specific` | The category for the UA, according to the EU specification, is the Specific category.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a099a56ae4c804a5fef0a494d8c1f3736a2e768a9e3370f4f3dc13869f71c7c456"></span> `Certified` | The category for the UA, according to the EU specification, is the Certified category.. 

### enum ClassEu {#structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfc}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfca2af457eff1694fff8c7ef6c3a8974ce6"></span> `Undeclared` | The class for the UA, according to the EU specification, is undeclared.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfcadb4073c49e90e2cf7bcf55cfbeb1a408"></span> `Class0` | The class for the UA, according to the EU specification, is Class 0.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfca1a853a1f8ccec546f0ba357cf8d33aa3"></span> `Class1` | The class for the UA, according to the EU specification, is Class 1.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfcacc5c776dab07ee285f226495b48c6f88"></span> `Class2` | The class for the UA, according to the EU specification, is Class 2.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfca1d392bcd8a38f07c0749608b230785a0"></span> `Class3` | The class for the UA, according to the EU specification, is Class 3.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfcabc2ffbdc04f67e17a417da4f3a029db6"></span> `Class4` | The class for the UA, according to the EU specification, is Class 4.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfca7b7a530cd4038d55de3401c35a535169"></span> `Class5` | The class for the UA, according to the EU specification, is Class 5.. 
<span id="structmavsdk_1_1_remote_id_1_1_system_id_1a2fe3542dfe8ea108cb343d7e89e96cfcad28a21fe2ad478c011dd6e2d963727bb"></span> `Class6` | The class for the UA, according to the EU specification, is Class 6.. 

## Field Documentation


### operator_location_type {#structmavsdk_1_1_remote_id_1_1_system_id_1a51662a9b4bf39dc00e3be9c4cbf76cd7}

```cpp
OperatorLocationType mavsdk::RemoteId::SystemId::operator_location_type {}
```


Specifies the operator location type.


### classification_type {#structmavsdk_1_1_remote_id_1_1_system_id_1a58bc106670c306e7ae9f39991d71fd27}

```cpp
ClassificationType mavsdk::RemoteId::SystemId::classification_type {}
```


Specifies the classification type of the UA.


### operator_latitude_deg {#structmavsdk_1_1_remote_id_1_1_system_id_1a8c49eabf8bb6ce398a6520dee6ef0ca3}

```cpp
double mavsdk::RemoteId::SystemId::operator_latitude_deg {}
```


Latitude of the operator.


### operator_longitude_deg {#structmavsdk_1_1_remote_id_1_1_system_id_1aa1be7b9c32fd055f16f1b7f47233261d}

```cpp
double mavsdk::RemoteId::SystemId::operator_longitude_deg {}
```


Longitude of the operator.


### area_count {#structmavsdk_1_1_remote_id_1_1_system_id_1af9c1a95a0f0b4d0eeb31a08d75fcb5c8}

```cpp
uint32_t mavsdk::RemoteId::SystemId::area_count {}
```


Number of aircraft in the area.


### area_radius_m {#structmavsdk_1_1_remote_id_1_1_system_id_1a85384066656e1955ebc5e612f9c1bf36}

```cpp
uint32_t mavsdk::RemoteId::SystemId::area_radius_m {}
```


Radius of the cylindrical area of the group or formation.


### area_ceiling_m {#structmavsdk_1_1_remote_id_1_1_system_id_1a9722e4085446724e04538473465e330b}

```cpp
float mavsdk::RemoteId::SystemId::area_ceiling_m {}
```


Area Operations Ceiling relative to WGS84.


### area_floor_m {#structmavsdk_1_1_remote_id_1_1_system_id_1a81a5e44a16223390501d4a0840b44d0e}

```cpp
float mavsdk::RemoteId::SystemId::area_floor_m {}
```


Area Operations Floor relative to WGS84.


### category_eu {#structmavsdk_1_1_remote_id_1_1_system_id_1a6ec88b9d069b742344b64e8bbf5fc594}

```cpp
CategoryEu mavsdk::RemoteId::SystemId::category_eu {}
```


When classification_type is MAV_ODID_CLASSIFICATION_TYPE_EU, specifies the category of the UA.


### class_eu {#structmavsdk_1_1_remote_id_1_1_system_id_1a955290e5c82ec19d9f738dc1b63645ba}

```cpp
ClassEu mavsdk::RemoteId::SystemId::class_eu {}
```


When classification_type is MAV_ODID_CLASSIFICATION_TYPE_EU, specifies the class of the UA.


### operator_altitude_geo_m {#structmavsdk_1_1_remote_id_1_1_system_id_1a5a72096fffd0e131a6dd971e02525c73}

```cpp
float mavsdk::RemoteId::SystemId::operator_altitude_geo_m {}
```


Geodetic altitude of the operator relative to WGS84. If unknown: -1000 m.


### time_utc_us {#structmavsdk_1_1_remote_id_1_1_system_id_1acfdfc7367dbbecce85c66f8195af9c43}

```cpp
uint64_t mavsdk::RemoteId::SystemId::time_utc_us {}
```


Timestamp in UTC (since UNIX epoch) in microseconds.

