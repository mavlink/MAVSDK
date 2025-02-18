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
enum [ComponentType](#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) | ComponentType of configurations, used for automatic ID setting.
enum [ConnectionResult](#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) | Result type returned when adding a connection.
enum [ForwardingOption](#namespacemavsdk_1a7066729108eae8a605d4dd169e4581b9) | ForwardingOption for Connection, used to set message forwarding option.

## Functions

Type | Name | Description
--- | --- | ---
std::string | [base64_encode](#namespacemavsdk_1a57a9962be22a61e5c36a66bc17e6a2a7) (std::vector< uint8_t > & raw) | Encode raw bytes to a base64 string.
std::vector< uint8_t > | [base64_decode](#namespacemavsdk_1a34e7609c9e2ddcc72a74bbc79daf9c19) (const std::string & str) | Decode a base64 string into raw bytes.
std::ostream & | [operator<<](#namespacemavsdk_1a2aa91d8b846b07fe7f305b399375ce5f) (std::ostream & str, const [ConnectionResult](namespacemavsdk.md#namespacemavsdk_1a0bad93f6d037051ac3906a0bcc09f992) & result) | Stream operator to print information about a `ConnectionResult`.
&nbsp; | [overloaded](#namespacemavsdk_1a724e321aaff91eb2ba28279e0292e552) (Ts...)-> overloaded< Ts... > | Template deduction helper for `overloaded`

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

## Function Documentation


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
#include: plugin_base.h
```
```cpp
mavsdk::overloaded(Ts...) -> overloaded< Ts... >
```


Template deduction helper for `overloaded`


**Parameters**

* Ts...  - 