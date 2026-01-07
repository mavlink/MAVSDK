# mavsdk Namespace Reference

----

Namespace for all mavsdk types.


## Data Structures

* [mavsdk::Action](classmavsdk_1_1_action.md)
* [mavsdk::ActionServer](classmavsdk_1_1_action_server.md)
* [mavsdk::ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md)
* [mavsdk::Calibration](classmavsdk_1_1_calibration.md)
* [mavsdk::Camera](classmavsdk_1_1_camera.md)
* [mavsdk::CameraServer](classmavsdk_1_1_camera_server.md)
* [mavsdk::ComponentMetadata](classmavsdk_1_1_component_metadata.md)
* [mavsdk::ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md)
* [mavsdk::Events](classmavsdk_1_1_events.md)
* [mavsdk::Failure](classmavsdk_1_1_failure.md)
* [mavsdk::FakeHandle](classmavsdk_1_1_fake_handle.md)
* [mavsdk::FollowMe](classmavsdk_1_1_follow_me.md)
* [mavsdk::Ftp](classmavsdk_1_1_ftp.md)
* [mavsdk::FtpServer](classmavsdk_1_1_ftp_server.md)
* [mavsdk::Geofence](classmavsdk_1_1_geofence.md)
* [mavsdk::Gimbal](classmavsdk_1_1_gimbal.md)
* [mavsdk::Gripper](classmavsdk_1_1_gripper.md)
* [mavsdk::Handle](classmavsdk_1_1_handle.md)
* [mavsdk::HandleFactory](classmavsdk_1_1_handle_factory.md)
* [mavsdk::Info](classmavsdk_1_1_info.md)
* [mavsdk::LogFiles](classmavsdk_1_1_log_files.md)
* [mavsdk::LogStreaming](classmavsdk_1_1_log_streaming.md)
* [mavsdk::ManualControl](classmavsdk_1_1_manual_control.md)
* [mavsdk::MavlinkDirect](classmavsdk_1_1_mavlink_direct.md)
* [mavsdk::MavlinkPassthrough](classmavsdk_1_1_mavlink_passthrough.md)
* [mavsdk::Mavsdk](classmavsdk_1_1_mavsdk.md)
* [mavsdk::Mission](classmavsdk_1_1_mission.md)
* [mavsdk::MissionRaw](classmavsdk_1_1_mission_raw.md)
* [mavsdk::MissionRawServer](classmavsdk_1_1_mission_raw_server.md)
* [mavsdk::Mocap](classmavsdk_1_1_mocap.md)
* [mavsdk::Offboard](classmavsdk_1_1_offboard.md)
* [mavsdk::overloaded](structmavsdk_1_1overloaded.md)
* [mavsdk::Param](classmavsdk_1_1_param.md)
* [mavsdk::ParamServer](classmavsdk_1_1_param_server.md)
* [mavsdk::PluginBase](classmavsdk_1_1_plugin_base.md)
* [mavsdk::Rtk](classmavsdk_1_1_rtk.md)
* [mavsdk::ServerComponent](classmavsdk_1_1_server_component.md)
* [mavsdk::ServerPluginBase](classmavsdk_1_1_server_plugin_base.md)
* [mavsdk::ServerUtility](classmavsdk_1_1_server_utility.md)
* [mavsdk::Shell](classmavsdk_1_1_shell.md)
* [mavsdk::System](classmavsdk_1_1_system.md)
* [mavsdk::Telemetry](classmavsdk_1_1_telemetry.md)
* [mavsdk::TelemetryServer](classmavsdk_1_1_telemetry_server.md)
* [mavsdk::Transponder](classmavsdk_1_1_transponder.md)
* [mavsdk::Tune](classmavsdk_1_1_tune.md)
* [mavsdk::Winch](classmavsdk_1_1_winch.md)

## Enumerations

Type | Description
--- | ---
enum [Autopilot](#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) | Autopilot type.
enum [CompatibilityMode](#namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00) | Compatibility mode for MAVLink protocol behavior.
enum [ComponentType](#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) | ComponentType of configurations, used for automatic ID setting.
enum [ConnectionResult](#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | Result type returned when adding a connection.
enum [ForwardingOption](#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) | ForwardingOption for Connection, used to set message forwarding option.
enum [Vehicle](#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b) | Vehicle type.

## Functions

Type | Name | Description
--- | --- | ---
std::ostream & | [operator<<](#namespacemavsdk_1ad782054ca8c33116d0210acca8c55ce6) (std::ostream & os, const [Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) & autopilot) | Stream operator to print information about an `Autopilot`.
std::string | [base64_encode](#namespacemavsdk_1a57a9962be22a61e5c36a66bc17e6a2a7) (std::vector< uint8_t > & raw) | Encode raw bytes to a base64 string.
std::vector< uint8_t > | [base64_decode](#namespacemavsdk_1a34e7609c9e2ddcc72a74bbc79daf9c19) (const std::string & str) | Decode a base64 string into raw bytes.
std::ostream & | [operator<<](#namespacemavsdk_1aab2fde9b1e274959eb378afef9e0747f) (std::ostream & os, const [CompatibilityMode](namespacemavsdk.md#namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00) & mode) | Stream operator to print information about a `CompatibilityMode`.
std::ostream & | [operator<<](#namespacemavsdk_1a2aa91d8b846b07fe7f305b399375ce5f) (std::ostream & str, const [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) & result) | Stream operator to print information about a `ConnectionResult`.
&nbsp; | [overloaded](#namespacemavsdk_1a724e321aaff91eb2ba28279e0292e552) (Ts...)-> overloaded< Ts... > | Template deduction helper for `overloaded`
std::ostream & | [operator<<](#namespacemavsdk_1a3e7a55e89629afd2a079d79c047e8dbd) (std::ostream & os, const [Vehicle](namespacemavsdk.md#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b) & vehicle) | Stream operator to print information about a `Vehicle`.
[Vehicle](namespacemavsdk.md#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b) | [to_vehicle_from_mav_type](#namespacemavsdk_1a4dede924df915e32b4807aa87a98b5bb) (MAV_TYPE type) | Convert a 'MAV_TYPE' to a `Vehicle`.

## Enumeration Type Documentation


### enum Autopilot {#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297}

```
#include: autopilot.h
```


Autopilot type.


Value | Description
--- | ---
<span id="namespacemavsdk_1aba05635d1785223a4d7b457ae0407297a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` |  
<span id="namespacemavsdk_1aba05635d1785223a4d7b457ae0407297a31e54e53a62995611ff9ccc0dd9cb510"></span> `Px4` |  
<span id="namespacemavsdk_1aba05635d1785223a4d7b457ae0407297a1f79341042063f30812f2942a48d19ed"></span> `ArduPilot` |  

### enum CompatibilityMode {#namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00}

```
#include: compatibility_mode.h
```


Compatibility mode for MAVLink protocol behavior.

This setting determines which autopilot-specific quirks and behaviors are used when communicating via MAVLink.

Value | Description
--- | ---
<span id="namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00a06b9281e396db002010bde1de57262eb"></span> `Auto` | Use detected autopilot (default, current behavior) 
<span id="namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00ad30973809f2e4238b7eb2bac9241674c"></span> `Pure` | Pure standard MAVLink - no autopilot-specific quirks. 
<span id="namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00a31e54e53a62995611ff9ccc0dd9cb510"></span> `Px4` | Force PX4 quirks regardless of detection. 
<span id="namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00a1f79341042063f30812f2942a48d19ed"></span> `ArduPilot` | Force ArduPilot quirks regardless of detection. 

### enum ComponentType {#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12}

```
#include: component_type.h
```


ComponentType of configurations, used for automatic ID setting.


Value | Description
--- | ---
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12a6ca1d2b081cc474f42cb95e3d04e6e68"></span> `Autopilot` | SDK is used as an autopilot. 
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12af64f82089eddc6133add8c55c65d6687"></span> `GroundStation` | SDK is used as a ground station. 
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12a8f2f82e1a7aa48819e9530d5c4977477"></span> `CompanionComputer` | SDK is used as a companion computer on board the MAV. 
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12a967d35e40f3f95b1f538bd248640bf3b"></span> `Camera` |  
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12acda0f2848103fd4a833697ac64b9ad8d"></span> `Gimbal` | SDK is used as a camera. <
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12a9b61d83cce82349f154b47131680be37"></span> `RemoteId` | SDK is used as a gimbal. <
<span id="namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12a90589c47f06eb971d548591f23c285af"></span> `Custom` | SDK is used as a RemoteId system. <


the SDK is used in a custom configuration, no automatic ID will be provided

### enum ConnectionResult {#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992}

```
#include: connection_result.h
```


Result type returned when adding a connection.

**Note**: [Mavsdk](classmavsdk_1_1_mavsdk.md) does not throw exceptions. Instead a result of this type will be returned.

Value | Description
--- | ---
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Connection succeeded. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Connection timed out. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992ad4a2b05c3c61121e1ce151feb647a0c6"></span> `SocketError` | Socket error. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a0e4f8eb16eec43ffd4fa88d49195664a"></span> `BindError` | Bind error. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a0e6a105b658bcf46443616f4ca8f51ee"></span> `SocketConnectionError` | Socket connection error. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a997ca4ce119685f40f03a9a8a6c5346e"></span> `NotImplemented` | Connection type not implemented. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a581519b813a8e38c88e0aeb9edcf4535"></span> `SystemNotConnected` | No system is connected. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a1c18a1a383bcd5a039b68b3751ac6d71"></span> `SystemBusy` | System is busy. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command is denied. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a5a36b75b17476943413af4c3f5b75ce3"></span> `DestinationIpUnknown` | Connection IP is unknown. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a63e76f04ccb26b3da08fc20176c8577c"></span> `ConnectionsExhausted` | Connections exhausted. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a4ffb87ccb1fd17d4d54d882a06885f49"></span> `ConnectionUrlInvalid` | URL invalid. 
<span id="namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992a94a6b3fbdd01834703b52a5493535b1d"></span> `BaudrateUnknown` | Baudrate unknown. 

### enum ForwardingOption {#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9}

```
#include: mavsdk.h
```


ForwardingOption for Connection, used to set message forwarding option.


Value | Description
--- | ---
<span id="namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9a67cc131236b099b681048d4812558fca"></span> `ForwardingOff` |  
<span id="namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9ac07f34e9a31f4a26b48711122e89cdb8"></span> `ForwardingOn` |  

### enum Vehicle {#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b}

```
#include: vehicle.h
```


Vehicle type.


Value | Description
--- | ---
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba8045a0a6c688b0635e3caccc408a1446"></span> `Generic` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95bafd01c0a8cba34e17735a9878e76d485d"></span> `FixedWing` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba623a7997ee9493420463eb52b88de935"></span> `Quadrotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95badc6f3fe82dfec3326452006d58dd4aad"></span> `Coaxial` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba2ab8b43468e8b92b0fc5c81e70e35a2d"></span> `Helicopter` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95bade750545706d10adc6a3b6ca1b0f7c3f"></span> `Airship` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba3dd340796c8755fa272c9b776f7b1058"></span> `FreeBalloon` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba15e8bfff2c10ca22da244d85b2bf7d94"></span> `Rocket` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba6c985fc3261cb4ccf6d72c725780807e"></span> `GroundRover` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba5e17c124ec97ee9ed25907042ef61f35"></span> `SurfaceBoat` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba0d234e5bcf75c768eb0db6bf96e3c22d"></span> `Submarine` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95baceee3c48890aa63467460b92940d24fe"></span> `Hexarotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba782cacf1683e035f039e607f34e4e945"></span> `Octorotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba7f5a1551acfeb83770c0665760132e17"></span> `Tricopter` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95bab80c55e3e8ba28a29ace551cf3390763"></span> `FlappingWing` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba65c449ad44e2c0b6c1fffc5c5cea7413"></span> `Kite` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba06b101d8e4475605ca9d1e2217c7296a"></span> `VtolTailsitterDuorotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba8b5a7606cd877542d59f2edc0618d06a"></span> `VtolTailsitterQuadrotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95baac9f48fd3bb7a7e42f5ae811fec2781e"></span> `VtolTiltrotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95bad755986e4ea01f23bdf03c2653676d75"></span> `VtolFixedrotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95babf28f759e1d46a1198943c8882d652b2"></span> `VtolTailsitter` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba278136ab9f897733648f230a9a6f6384"></span> `VtolTiltwing` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba88cbbac22ccc3e79b5adc1af44d7b8ec"></span> `Parafoil` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba4786f3d8d1faa285ddecf2a74f16c922"></span> `Dodecarotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba27005abdd874048c19c30c061eb1d9ba"></span> `Decarotor` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba889d6c668249b544803bafc6c18dbf34"></span> `Parachute` |  
<span id="namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95ba35a7a97619a60e154205a06207e18b60"></span> `GenericMultirotor` |  

## Function Documentation


### operator<<() {#namespacemavsdk_1ad782054ca8c33116d0210acca8c55ce6}

```
#include: autopilot.h
```
```cpp
std::ostream & mavsdk::operator<<(std::ostream &os, const Autopilot &autopilot)
```


Stream operator to print information about an `Autopilot`.


**Parameters**

* std::ostream& **os** - 
* const [Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297)& **autopilot** - 

**Returns**

&emsp;std::ostream & - A reference to the stream.

### base64_encode() {#namespacemavsdk_1a57a9962be22a61e5c36a66bc17e6a2a7}

```
#include: base64.h
```
```cpp
std::string mavsdk::base64_encode(std::vector< uint8_t > &raw)
```


Encode raw bytes to a base64 string.


**Parameters**

* std::vector< uint8_t >& **raw** - Raw bytes

**Returns**

&emsp;std::string - Base64 string

### base64_decode() {#namespacemavsdk_1a34e7609c9e2ddcc72a74bbc79daf9c19}

```
#include: base64.h
```
```cpp
std::vector< uint8_t > mavsdk::base64_decode(const std::string &str)
```


Decode a base64 string into raw bytes.


**Parameters**

* const std::string& **str** - Base64 string

**Returns**

&emsp;std::vector< uint8_t > - Raw bytes

### operator<<() {#namespacemavsdk_1aab2fde9b1e274959eb378afef9e0747f}

```
#include: compatibility_mode.h
```
```cpp
std::ostream & mavsdk::operator<<(std::ostream &os, const CompatibilityMode &mode)
```


Stream operator to print information about a `CompatibilityMode`.


**Parameters**

* std::ostream& **os** - 
* const [CompatibilityMode](namespacemavsdk.md#namespacemavsdk_1af0f9146b2086797ae56671d20bd29d00)& **mode** - 

**Returns**

&emsp;std::ostream & - A reference to the stream.

### operator<<() {#namespacemavsdk_1a2aa91d8b846b07fe7f305b399375ce5f}

```
#include: connection_result.h
```
```cpp
std::ostream & mavsdk::operator<<(std::ostream &str, const ConnectionResult &result)
```


Stream operator to print information about a `ConnectionResult`.


**Parameters**

* std::ostream& **str** - 
* const [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992)& **result** - 

**Returns**

&emsp;std::ostream & - A reference to the stream.

### overloaded() {#namespacemavsdk_1a724e321aaff91eb2ba28279e0292e552}

```
#include: overloaded.h
```
```cpp
mavsdk::overloaded(Ts...) -> overloaded< Ts... >
```


Template deduction helper for `overloaded`


**Parameters**

* Ts...  - 

### operator<<() {#namespacemavsdk_1a3e7a55e89629afd2a079d79c047e8dbd}

```
#include: vehicle.h
```
```cpp
std::ostream & mavsdk::operator<<(std::ostream &os, const Vehicle &vehicle)
```


Stream operator to print information about a `Vehicle`.


**Parameters**

* std::ostream& **os** - 
* const [Vehicle](namespacemavsdk.md#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b)& **vehicle** - 

**Returns**

&emsp;std::ostream & - A reference to the stream.

### to_vehicle_from_mav_type() {#namespacemavsdk_1a4dede924df915e32b4807aa87a98b5bb}

```
#include: vehicle.h
```
```cpp
Vehicle mavsdk::to_vehicle_from_mav_type(MAV_TYPE type)
```


Convert a 'MAV_TYPE' to a `Vehicle`.


**Parameters**

* MAV_TYPE **type** - 

**Returns**

&emsp;[Vehicle](namespacemavsdk.md#namespacemavsdk_1a9e3a3a502dc8313cb931a8a44cc6f95b) - The corresponding `Vehicle`.