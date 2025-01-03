# mavsdk::Mission Class Reference
`#include: mission.h`

----


Enable waypoint missions. 


## Data Structures


struct [MissionItem](structmavsdk_1_1_mission_1_1_mission_item.md)

struct [MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md)

struct [MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md)

struct [ProgressData](structmavsdk_1_1_mission_1_1_progress_data.md)

struct [ProgressDataOrMission](structmavsdk_1_1_mission_1_1_progress_data_or_mission.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | Possible results returned for action requests.
std::function< void([Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6))> [ResultCallback](#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) | Callback type for asynchronous [Mission](classmavsdk_1_1_mission.md) calls.
std::function< void([Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), [ProgressData](structmavsdk_1_1_mission_1_1_progress_data.md))> [UploadMissionWithProgressCallback](#classmavsdk_1_1_mission_1a559c82c81b3b7694973da8a65af24837) | Callback type for upload_mission_with_progress_async.
std::function< void([Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), [MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md))> [DownloadMissionCallback](#classmavsdk_1_1_mission_1af40f70b9b4c91aa280bf75fbfc333b3b) | Callback type for download_mission_async.
std::function< void([Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), [ProgressDataOrMission](structmavsdk_1_1_mission_1_1_progress_data_or_mission.md))> [DownloadMissionWithProgressCallback](#classmavsdk_1_1_mission_1ae73dbe775ceaba81183cebdaa1b6779e) | Callback type for download_mission_with_progress_async.
std::function< void([MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md))> [MissionProgressCallback](#classmavsdk_1_1_mission_1a67e8d00b1b20affca59fd4338c34c0e2) | Callback type for subscribe_mission_progress.
[Handle](classmavsdk_1_1_handle.md)< [MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md) > [MissionProgressHandle](#classmavsdk_1_1_mission_1aded0ba06c787ad2f30f401a30b240c8a) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_mission_progress.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Mission](#classmavsdk_1_1_mission_1ab02ae9aada51b483e45d88948fe62c29) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Mission](#classmavsdk_1_1_mission_1a5222f05ac0c65c7455f19e136b8fcb22) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Mission](#classmavsdk_1_1_mission_1ab8ddd47439bbc63a0a6b175fdad0368e) () override | Destructor (internal use only).
&nbsp; | [Mission](#classmavsdk_1_1_mission_1ab4a4a26254fdff0e3f3872f32479fe1c) (const [Mission](classmavsdk_1_1_mission.md) & other) | Copy constructor.
void | [upload_mission_async](#classmavsdk_1_1_mission_1a250fc4758d47ec12e025c327dc0eb482) ([MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) mission_plan, const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) callback) | Upload a list of mission items to the system.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [upload_mission](#classmavsdk_1_1_mission_1a38274b1c1509375a182c44711ee9f7b1) ([MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) mission_plan)const | Upload a list of mission items to the system.
void | [upload_mission_with_progress_async](#classmavsdk_1_1_mission_1a0faa587e80a59e8c40d7fc135650749c) ([MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) mission_plan, const [UploadMissionWithProgressCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a559c82c81b3b7694973da8a65af24837) & callback) | Upload a list of mission items to the system and report upload progress.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [cancel_mission_upload](#classmavsdk_1_1_mission_1ab82609426bef51202b2107d33412378c) () const | Cancel an ongoing mission upload.
void | [download_mission_async](#classmavsdk_1_1_mission_1a04e7e7074273b4591a820894c5c4ad43) (const [DownloadMissionCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1af40f70b9b4c91aa280bf75fbfc333b3b) callback) | Download a list of mission items from the system (asynchronous).
std::pair< [Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), [Mission::MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) > | [download_mission](#classmavsdk_1_1_mission_1a23e9f7da32f42bcce7ef16ea8044fe53) () const | Download a list of mission items from the system (asynchronous).
void | [download_mission_with_progress_async](#classmavsdk_1_1_mission_1a5b6b93482f2599c08d75d27a31a4f1d9) (const [DownloadMissionWithProgressCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ae73dbe775ceaba81183cebdaa1b6779e) & callback) | Download a list of mission items from the system (asynchronous) and report progress.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [cancel_mission_download](#classmavsdk_1_1_mission_1a0eaaeffe0354156b5abed892f0950bcc) () const | Cancel an ongoing mission download.
void | [start_mission_async](#classmavsdk_1_1_mission_1a31ca2fc6b9fe4802dbc3fbebad0bb5d7) (const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) callback) | Start the mission.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [start_mission](#classmavsdk_1_1_mission_1a927fecc7734810d01cc218153780e6e3) () const | Start the mission.
void | [pause_mission_async](#classmavsdk_1_1_mission_1a4c5679369e215ef21901fc7ffe1ce32b) (const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) callback) | Pause the mission.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [pause_mission](#classmavsdk_1_1_mission_1ab569500d992d6d859c1c35052db07315) () const | Pause the mission.
void | [clear_mission_async](#classmavsdk_1_1_mission_1a51d04a808743915e3cac7f35fbd5038f) (const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) callback) | Clear the mission saved on the vehicle.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [clear_mission](#classmavsdk_1_1_mission_1a3c3f5eac6e864873f4bb0390d1ee9306) () const | Clear the mission saved on the vehicle.
void | [set_current_mission_item_async](#classmavsdk_1_1_mission_1a81aa356215cb2131c2480dc121a6af7b) (int32_t index, const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) callback) | Sets the mission item index to go to.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [set_current_mission_item](#classmavsdk_1_1_mission_1a419397edcf63771ddd59a6af231bc8d2) (int32_t index)const | Sets the mission item index to go to.
std::pair< [Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), bool > | [is_mission_finished](#classmavsdk_1_1_mission_1a1ecf4f8798ab9ae96882dfbd34f23466) () const | Check if the mission has been finished.
[MissionProgressHandle](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1aded0ba06c787ad2f30f401a30b240c8a) | [subscribe_mission_progress](#classmavsdk_1_1_mission_1ac6d3e78de0ea2e6c1db2eaa5f3418660) (const [MissionProgressCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a67e8d00b1b20affca59fd4338c34c0e2) & callback) | Subscribe to mission progress updates.
void | [unsubscribe_mission_progress](#classmavsdk_1_1_mission_1a2b8bc4dd210506a703afa926f8406880) ([MissionProgressHandle](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1aded0ba06c787ad2f30f401a30b240c8a) handle) | Unsubscribe from subscribe_mission_progress.
[MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md) | [mission_progress](#classmavsdk_1_1_mission_1a5570443e7c1f08cff1759980ff44b40e) () const | Poll for '[MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md)' (blocking).
std::pair< [Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), bool > | [get_return_to_launch_after_mission](#classmavsdk_1_1_mission_1a38d17268541ba81d494976caa4a08661) () const | Get whether to trigger Return-to-Launch (RTL) after mission is complete.
[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) | [set_return_to_launch_after_mission](#classmavsdk_1_1_mission_1ab4adb09283b3fa64a8829f9a6e34cf37) (bool enable)const | Set whether to trigger Return-to-Launch (RTL) after the mission is complete.
const [Mission](classmavsdk_1_1_mission.md) & | [operator=](#classmavsdk_1_1_mission_1a30d49ea769f358cb4e4fe3056728838c) (const [Mission](classmavsdk_1_1_mission.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Mission() {#classmavsdk_1_1_mission_1ab02ae9aada51b483e45d88948fe62c29}
```cpp
mavsdk::Mission::Mission(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mission = Mission(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Mission() {#classmavsdk_1_1_mission_1a5222f05ac0c65c7455f19e136b8fcb22}
```cpp
mavsdk::Mission::Mission(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mission = Mission(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Mission() {#classmavsdk_1_1_mission_1ab8ddd47439bbc63a0a6b175fdad0368e}
```cpp
mavsdk::Mission::~Mission() override
```


Destructor (internal use only).


### Mission() {#classmavsdk_1_1_mission_1ab4a4a26254fdff0e3f3872f32479fe1c}
```cpp
mavsdk::Mission::Mission(const Mission &other)
```


Copy constructor.


**Parameters**

* const [Mission](classmavsdk_1_1_mission.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591}

```cpp
using mavsdk::Mission::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Mission](classmavsdk_1_1_mission.md) calls.


### typedef UploadMissionWithProgressCallback {#classmavsdk_1_1_mission_1a559c82c81b3b7694973da8a65af24837}

```cpp
using mavsdk::Mission::UploadMissionWithProgressCallback =  std::function<void(Result, ProgressData)>
```


Callback type for upload_mission_with_progress_async.


### typedef DownloadMissionCallback {#classmavsdk_1_1_mission_1af40f70b9b4c91aa280bf75fbfc333b3b}

```cpp
using mavsdk::Mission::DownloadMissionCallback =  std::function<void(Result, MissionPlan)>
```


Callback type for download_mission_async.


### typedef DownloadMissionWithProgressCallback {#classmavsdk_1_1_mission_1ae73dbe775ceaba81183cebdaa1b6779e}

```cpp
using mavsdk::Mission::DownloadMissionWithProgressCallback =  std::function<void(Result, ProgressDataOrMission)>
```


Callback type for download_mission_with_progress_async.


### typedef MissionProgressCallback {#classmavsdk_1_1_mission_1a67e8d00b1b20affca59fd4338c34c0e2}

```cpp
using mavsdk::Mission::MissionProgressCallback =  std::function<void(MissionProgress)>
```


Callback type for subscribe_mission_progress.


### typedef MissionProgressHandle {#classmavsdk_1_1_mission_1aded0ba06c787ad2f30f401a30b240c8a}

```cpp
using mavsdk::Mission::MissionProgressHandle =  Handle<MissionProgress>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_mission_progress.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6}


Possible results returned for action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6aecc3de82f27e8e7f65807c69a114efbe"></span> `TooManyMissionItems` | Too many mission items in the mission. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a253ca7dd096ee0956cccee4d376cab8b"></span> `InvalidArgument` | Invalid argument. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | [Mission](classmavsdk_1_1_mission.md) downloaded from the system is not supported. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a6b0ce476dfc17eed72967386f52ede78"></span> `NoMissionAvailable` | No mission available on the system. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6aefcaef698baace312f79a53019bd9cf4"></span> `UnsupportedMissionCmd` | Unsupported mission command. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a3465fd31285ebd60597cf59bff9db01a"></span> `TransferCancelled` | [Mission](classmavsdk_1_1_mission.md) transfer (upload or download) has been cancelled. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a10ac3d04253ef7e1ddc73e6091c0cd55"></span> `Next` | Intermediate message showing progress. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | Request denied. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6aca3da8f495e4e628912a7798655da6c2"></span> `ProtocolError` | There was a protocol error. 
<span id="classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6a3f5f527103f3b11c1c271363c542432b"></span> `IntMessagesNotSupported` | The system does not support the MISSION_INT protocol. 

## Member Function Documentation


### upload_mission_async() {#classmavsdk_1_1_mission_1a250fc4758d47ec12e025c327dc0eb482}
```cpp
void mavsdk::Mission::upload_mission_async(MissionPlan mission_plan, const ResultCallback callback)
```


Upload a list of mission items to the system.

The mission items are uploaded to a drone. Once uploaded the mission can be started and executed even if the connection is lost.


This function is non-blocking. See 'upload_mission' for the blocking counterpart.

**Parameters**

* [MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) **mission_plan** - 
* const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) **callback** - 

### upload_mission() {#classmavsdk_1_1_mission_1a38274b1c1509375a182c44711ee9f7b1}
```cpp
Result mavsdk::Mission::upload_mission(MissionPlan mission_plan) const
```


Upload a list of mission items to the system.

The mission items are uploaded to a drone. Once uploaded the mission can be started and executed even if the connection is lost.


This function is blocking. See 'upload_mission_async' for the non-blocking counterpart.

**Parameters**

* [MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) **mission_plan** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### upload_mission_with_progress_async() {#classmavsdk_1_1_mission_1a0faa587e80a59e8c40d7fc135650749c}
```cpp
void mavsdk::Mission::upload_mission_with_progress_async(MissionPlan mission_plan, const UploadMissionWithProgressCallback &callback)
```


Upload a list of mission items to the system and report upload progress.

The mission items are uploaded to a drone. Once uploaded the mission can be started and executed even if the connection is lost.

**Parameters**

* [MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) **mission_plan** - 
* const [UploadMissionWithProgressCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a559c82c81b3b7694973da8a65af24837)& **callback** - 

### cancel_mission_upload() {#classmavsdk_1_1_mission_1ab82609426bef51202b2107d33412378c}
```cpp
Result mavsdk::Mission::cancel_mission_upload() const
```


Cancel an ongoing mission upload.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### download_mission_async() {#classmavsdk_1_1_mission_1a04e7e7074273b4591a820894c5c4ad43}
```cpp
void mavsdk::Mission::download_mission_async(const DownloadMissionCallback callback)
```


Download a list of mission items from the system (asynchronous).

Will fail if any of the downloaded mission items are not supported by the MAVSDK API.


This function is non-blocking. See 'download_mission' for the blocking counterpart.

**Parameters**

* const [DownloadMissionCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1af40f70b9b4c91aa280bf75fbfc333b3b) **callback** - 

### download_mission() {#classmavsdk_1_1_mission_1a23e9f7da32f42bcce7ef16ea8044fe53}
```cpp
std::pair<Result, Mission::MissionPlan> mavsdk::Mission::download_mission() const
```


Download a list of mission items from the system (asynchronous).

Will fail if any of the downloaded mission items are not supported by the MAVSDK API.


This function is blocking. See 'download_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), [Mission::MissionPlan](structmavsdk_1_1_mission_1_1_mission_plan.md) > - Result of request.

### download_mission_with_progress_async() {#classmavsdk_1_1_mission_1a5b6b93482f2599c08d75d27a31a4f1d9}
```cpp
void mavsdk::Mission::download_mission_with_progress_async(const DownloadMissionWithProgressCallback &callback)
```


Download a list of mission items from the system (asynchronous) and report progress.

Will fail if any of the downloaded mission items are not supported by the MAVSDK API.

**Parameters**

* const [DownloadMissionWithProgressCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ae73dbe775ceaba81183cebdaa1b6779e)& **callback** - 

### cancel_mission_download() {#classmavsdk_1_1_mission_1a0eaaeffe0354156b5abed892f0950bcc}
```cpp
Result mavsdk::Mission::cancel_mission_download() const
```


Cancel an ongoing mission download.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### start_mission_async() {#classmavsdk_1_1_mission_1a31ca2fc6b9fe4802dbc3fbebad0bb5d7}
```cpp
void mavsdk::Mission::start_mission_async(const ResultCallback callback)
```


Start the mission.

A mission must be uploaded to the vehicle before this can be called.


This function is non-blocking. See 'start_mission' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) **callback** - 

### start_mission() {#classmavsdk_1_1_mission_1a927fecc7734810d01cc218153780e6e3}
```cpp
Result mavsdk::Mission::start_mission() const
```


Start the mission.

A mission must be uploaded to the vehicle before this can be called.


This function is blocking. See 'start_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### pause_mission_async() {#classmavsdk_1_1_mission_1a4c5679369e215ef21901fc7ffe1ce32b}
```cpp
void mavsdk::Mission::pause_mission_async(const ResultCallback callback)
```


Pause the mission.

Pausing the mission puts the vehicle into [HOLD mode](https://docs.px4.io/en/flight_modes/hold.html). A multicopter should just hover at the spot while a fixedwing vehicle should loiter around the location where it paused.


This function is non-blocking. See 'pause_mission' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) **callback** - 

### pause_mission() {#classmavsdk_1_1_mission_1ab569500d992d6d859c1c35052db07315}
```cpp
Result mavsdk::Mission::pause_mission() const
```


Pause the mission.

Pausing the mission puts the vehicle into [HOLD mode](https://docs.px4.io/en/flight_modes/hold.html). A multicopter should just hover at the spot while a fixedwing vehicle should loiter around the location where it paused.


This function is blocking. See 'pause_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### clear_mission_async() {#classmavsdk_1_1_mission_1a51d04a808743915e3cac7f35fbd5038f}
```cpp
void mavsdk::Mission::clear_mission_async(const ResultCallback callback)
```


Clear the mission saved on the vehicle.

This function is non-blocking. See 'clear_mission' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) **callback** - 

### clear_mission() {#classmavsdk_1_1_mission_1a3c3f5eac6e864873f4bb0390d1ee9306}
```cpp
Result mavsdk::Mission::clear_mission() const
```


Clear the mission saved on the vehicle.

This function is blocking. See 'clear_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### set_current_mission_item_async() {#classmavsdk_1_1_mission_1a81aa356215cb2131c2480dc121a6af7b}
```cpp
void mavsdk::Mission::set_current_mission_item_async(int32_t index, const ResultCallback callback)
```


Sets the mission item index to go to.

By setting the current index to 0, the mission is restarted from the beginning. If it is set to a specific index of a mission item, the mission will be set to this item.


Note that this is not necessarily true for general missions using MAVLink if loop counters are used.


This function is non-blocking. See 'set_current_mission_item' for the blocking counterpart.

**Parameters**

* int32_t **index** - 
* const [ResultCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a30091e79f5b67ade138e5be9d65b6591) **callback** - 

### set_current_mission_item() {#classmavsdk_1_1_mission_1a419397edcf63771ddd59a6af231bc8d2}
```cpp
Result mavsdk::Mission::set_current_mission_item(int32_t index) const
```


Sets the mission item index to go to.

By setting the current index to 0, the mission is restarted from the beginning. If it is set to a specific index of a mission item, the mission will be set to this item.


Note that this is not necessarily true for general missions using MAVLink if loop counters are used.


This function is blocking. See 'set_current_mission_item_async' for the non-blocking counterpart.

**Parameters**

* int32_t **index** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### is_mission_finished() {#classmavsdk_1_1_mission_1a1ecf4f8798ab9ae96882dfbd34f23466}
```cpp
std::pair<Result, bool> mavsdk::Mission::is_mission_finished() const
```


Check if the mission has been finished.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), bool > - Result of request.

### subscribe_mission_progress() {#classmavsdk_1_1_mission_1ac6d3e78de0ea2e6c1db2eaa5f3418660}
```cpp
MissionProgressHandle mavsdk::Mission::subscribe_mission_progress(const MissionProgressCallback &callback)
```


Subscribe to mission progress updates.


**Parameters**

* const [MissionProgressCallback](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a67e8d00b1b20affca59fd4338c34c0e2)& **callback** - 

**Returns**

&emsp;[MissionProgressHandle](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1aded0ba06c787ad2f30f401a30b240c8a) - 

### unsubscribe_mission_progress() {#classmavsdk_1_1_mission_1a2b8bc4dd210506a703afa926f8406880}
```cpp
void mavsdk::Mission::unsubscribe_mission_progress(MissionProgressHandle handle)
```


Unsubscribe from subscribe_mission_progress.


**Parameters**

* [MissionProgressHandle](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1aded0ba06c787ad2f30f401a30b240c8a) **handle** - 

### mission_progress() {#classmavsdk_1_1_mission_1a5570443e7c1f08cff1759980ff44b40e}
```cpp
MissionProgress mavsdk::Mission::mission_progress() const
```


Poll for '[MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md)' (blocking).


**Returns**

&emsp;[MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md) - One [MissionProgress](structmavsdk_1_1_mission_1_1_mission_progress.md) update.

### get_return_to_launch_after_mission() {#classmavsdk_1_1_mission_1a38d17268541ba81d494976caa4a08661}
```cpp
std::pair<Result, bool> mavsdk::Mission::get_return_to_launch_after_mission() const
```


Get whether to trigger Return-to-Launch (RTL) after mission is complete.

Before getting this option, it needs to be set, or a mission needs to be downloaded.


This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6), bool > - Result of request.

### set_return_to_launch_after_mission() {#classmavsdk_1_1_mission_1ab4adb09283b3fa64a8829f9a6e34cf37}
```cpp
Result mavsdk::Mission::set_return_to_launch_after_mission(bool enable) const
```


Set whether to trigger Return-to-Launch (RTL) after the mission is complete.

This will only take effect for the next mission upload, meaning that the mission may have to be uploaded again.


This function is blocking.

**Parameters**

* bool **enable** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab3114c63db76bdc37460939a1f3316f6) - Result of request.

### operator=() {#classmavsdk_1_1_mission_1a30d49ea769f358cb4e4fe3056728838c}
```cpp
const Mission& mavsdk::Mission::operator=(const Mission &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Mission](classmavsdk_1_1_mission.md)&  - 

**Returns**

&emsp;const [Mission](classmavsdk_1_1_mission.md) & - 