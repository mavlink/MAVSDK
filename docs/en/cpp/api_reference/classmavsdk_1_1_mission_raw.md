# mavsdk::MissionRaw Class Reference
`#include: mission_raw.h`

----


Enable raw missions as exposed by MAVLink. 


## Data Structures


struct [MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md)

struct [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md)

struct [MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | Possible results returned for action requests.
std::function< void([Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf))> [ResultCallback](#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) | Callback type for asynchronous [MissionRaw](classmavsdk_1_1_mission_raw.md) calls.
std::function< void([Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) >)> [DownloadMissionCallback](#classmavsdk_1_1_mission_raw_1a016633e6338744da02ac7cb6da28880a) | Callback type for download_mission_async.
std::function< void([Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) >)> [DownloadGeofenceCallback](#classmavsdk_1_1_mission_raw_1a3e75d493ec08fec9d9faa597238c11be) | Callback type for download_geofence_async.
std::function< void([Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) >)> [DownloadRallypointsCallback](#classmavsdk_1_1_mission_raw_1a473abce54f89efe05d5ac051c9d8a5dc) | Callback type for download_rallypoints_async.
std::function< void([MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md))> [MissionProgressCallback](#classmavsdk_1_1_mission_raw_1a9dd594878925da494b4add6acc3184fc) | Callback type for subscribe_mission_progress.
[Handle](classmavsdk_1_1_handle.md)< [MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md) > [MissionProgressHandle](#classmavsdk_1_1_mission_raw_1a34e0eaf9922daa5d27d2b044eae7885c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_mission_progress.
std::function< void(bool)> [MissionChangedCallback](#classmavsdk_1_1_mission_raw_1ac22d81eefc5e883cdb6baf792a7487e6) | Callback type for subscribe_mission_changed.
[Handle](classmavsdk_1_1_handle.md)< bool > [MissionChangedHandle](#classmavsdk_1_1_mission_raw_1a46da6d8a53822fd5fbd7b2a414624c5c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_mission_changed.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [MissionRaw](#classmavsdk_1_1_mission_raw_1ad03476f12988a12808a8c4385c7a7344) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [MissionRaw](#classmavsdk_1_1_mission_raw_1aec90f3e32d7366f3e27f025bb62c1e78) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~MissionRaw](#classmavsdk_1_1_mission_raw_1a1c7e90261ab67337d6b570cb3f9bfdb9) () override | Destructor (internal use only).
&nbsp; | [MissionRaw](#classmavsdk_1_1_mission_raw_1a91216322f0b6139d5ba83127f2ef1f12) (const [MissionRaw](classmavsdk_1_1_mission_raw.md) & other) | Copy constructor.
void | [upload_mission_async](#classmavsdk_1_1_mission_raw_1a77cc5df3362b7ab4cbc94e5bc9707609) (std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > mission_items, const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Upload a list of raw mission items to the system.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [upload_mission](#classmavsdk_1_1_mission_raw_1ad4f5c2ccfb2249f6e11c9533c263926a) (std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > mission_items)const | Upload a list of raw mission items to the system.
void | [upload_geofence_async](#classmavsdk_1_1_mission_raw_1abae3246480b334c319cfb8a9c0d2d8c6) (std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > mission_items, const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Upload a list of geofence items to the system.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [upload_geofence](#classmavsdk_1_1_mission_raw_1ac65fe0675332c318c5c07be5a8d22ac7) (std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > mission_items)const | Upload a list of geofence items to the system.
void | [upload_rally_points_async](#classmavsdk_1_1_mission_raw_1a2c5d52246a8ad13151fcfedeab2859e5) (std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > mission_items, const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Upload a list of rally point items to the system.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [upload_rally_points](#classmavsdk_1_1_mission_raw_1a02fc4f293a8094df5dbd7ea0d2184739) (std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > mission_items)const | Upload a list of rally point items to the system.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [cancel_mission_upload](#classmavsdk_1_1_mission_raw_1aa353e3fa6e836305248be131dbe19273) () const | Cancel an ongoing mission upload.
void | [download_mission_async](#classmavsdk_1_1_mission_raw_1a7e27b0fb58889ca5cb1202276c0e0669) (const [DownloadMissionCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a016633e6338744da02ac7cb6da28880a) callback) | Download a list of raw mission items from the system (asynchronous).
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionRaw::MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > > | [download_mission](#classmavsdk_1_1_mission_raw_1a18138bc7cbc6c6c16fde44e2fa53a459) () const | Download a list of raw mission items from the system (asynchronous).
void | [download_geofence_async](#classmavsdk_1_1_mission_raw_1acc681495c318043334691ab9ccd6778d) (const [DownloadGeofenceCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a3e75d493ec08fec9d9faa597238c11be) callback) | Download a list of raw geofence items from the system (asynchronous).
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionRaw::MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > > | [download_geofence](#classmavsdk_1_1_mission_raw_1ae2d3d743e00312e07ca3893ab27f0138) () const | Download a list of raw geofence items from the system (asynchronous).
void | [download_rallypoints_async](#classmavsdk_1_1_mission_raw_1a38e58cdecc1737936bec5d046c8eb511) (const [DownloadRallypointsCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a473abce54f89efe05d5ac051c9d8a5dc) callback) | Download a list of raw rallypoint items from the system (asynchronous).
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionRaw::MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > > | [download_rallypoints](#classmavsdk_1_1_mission_raw_1a0cb3895f91bb2042c9d3282b09c442af) () const | Download a list of raw rallypoint items from the system (asynchronous).
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [cancel_mission_download](#classmavsdk_1_1_mission_raw_1a7c554999ca66c5434ef1fa334d949e5a) () const | Cancel an ongoing mission download.
void | [start_mission_async](#classmavsdk_1_1_mission_raw_1acca64e0a08978f5721be8fa955b1bb0f) (const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Start the mission.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [start_mission](#classmavsdk_1_1_mission_raw_1af1b010b0f28b284a94eba88198ee15f8) () const | Start the mission.
void | [pause_mission_async](#classmavsdk_1_1_mission_raw_1aae0eedbe4216266eb6e2115cd03c61a1) (const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Pause the mission.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [pause_mission](#classmavsdk_1_1_mission_raw_1abda483b0659a6c0397c588341688bb39) () const | Pause the mission.
void | [clear_mission_async](#classmavsdk_1_1_mission_raw_1acf6bf293facbd45fa1126e52e99248a2) (const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Clear the mission saved on the vehicle.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [clear_mission](#classmavsdk_1_1_mission_raw_1ab10f8fcaa0f6d3e0f844b7430d8d14c2) () const | Clear the mission saved on the vehicle.
void | [set_current_mission_item_async](#classmavsdk_1_1_mission_raw_1a5540d6ca691d60ef19b66e303bae7f87) (int32_t index, const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) callback) | Sets the raw mission item index to go to.
[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) | [set_current_mission_item](#classmavsdk_1_1_mission_raw_1ada9aa2abf79ebfc8e1d10de8e85e91ae) (int32_t index)const | Sets the raw mission item index to go to.
[MissionProgressHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a34e0eaf9922daa5d27d2b044eae7885c) | [subscribe_mission_progress](#classmavsdk_1_1_mission_raw_1a88a3c4b26418e734a547f251706988d2) (const [MissionProgressCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a9dd594878925da494b4add6acc3184fc) & callback) | Subscribe to mission progress updates.
void | [unsubscribe_mission_progress](#classmavsdk_1_1_mission_raw_1ac46f08b52706f45956cf3b01df381835) ([MissionProgressHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a34e0eaf9922daa5d27d2b044eae7885c) handle) | Unsubscribe from subscribe_mission_progress.
[MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md) | [mission_progress](#classmavsdk_1_1_mission_raw_1a3200dea1094926a4dd54f079f21b94e1) () const | Poll for '[MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md)' (blocking).
[MissionChangedHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a46da6d8a53822fd5fbd7b2a414624c5c) | [subscribe_mission_changed](#classmavsdk_1_1_mission_raw_1ad4a2991e1a8f9423270af4220309edfb) (const [MissionChangedCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1ac22d81eefc5e883cdb6baf792a7487e6) & callback) | Subscribes to mission changed.
void | [unsubscribe_mission_changed](#classmavsdk_1_1_mission_raw_1ac6cd7602b2e5b46ad0ea1cf8bf602a0c) ([MissionChangedHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a46da6d8a53822fd5fbd7b2a414624c5c) handle) | Unsubscribe from subscribe_mission_changed.
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > | [import_qgroundcontrol_mission](#classmavsdk_1_1_mission_raw_1a2a4ca261c37737e691c6954693d6d0a5) (std::string qgc_plan_path)const | Import a QGroundControl missions in JSON .plan format, from a file.
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > | [import_qgroundcontrol_mission_from_string](#classmavsdk_1_1_mission_raw_1a4a1b55650120d8af0ce7fa037f6b5ce9) (std::string qgc_plan)const | Import a QGroundControl missions in JSON .plan format, from a string.
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > | [import_mission_planner_mission](#classmavsdk_1_1_mission_raw_1abf8ac0f813d4dff4f0fe3acce0b39163) (std::string mission_planner_path)const | Import a [Mission](classmavsdk_1_1_mission.md) Planner mission in QGC WPL 110 format, from a file.
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > | [import_mission_planner_mission_from_string](#classmavsdk_1_1_mission_raw_1a25e5f8e8c7f6d778f7ab6e91b39a6ec9) (std::string mission_planner_mission)const | Import a [Mission](classmavsdk_1_1_mission.md) Planner mission in QGC WPL 110 format, from a string.
std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), bool > | [is_mission_finished](#classmavsdk_1_1_mission_raw_1a998db451c2718d9728276ca2d01ce315) () const | Check if the mission is finished.
const [MissionRaw](classmavsdk_1_1_mission_raw.md) & | [operator=](#classmavsdk_1_1_mission_raw_1a2b8cdc1fbee72224a9ef6eb9266b2e2a) (const [MissionRaw](classmavsdk_1_1_mission_raw.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### MissionRaw() {#classmavsdk_1_1_mission_raw_1ad03476f12988a12808a8c4385c7a7344}
```cpp
mavsdk::MissionRaw::MissionRaw(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mission_raw = MissionRaw(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### MissionRaw() {#classmavsdk_1_1_mission_raw_1aec90f3e32d7366f3e27f025bb62c1e78}
```cpp
mavsdk::MissionRaw::MissionRaw(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto mission_raw = MissionRaw(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~MissionRaw() {#classmavsdk_1_1_mission_raw_1a1c7e90261ab67337d6b570cb3f9bfdb9}
```cpp
mavsdk::MissionRaw::~MissionRaw() override
```


Destructor (internal use only).


### MissionRaw() {#classmavsdk_1_1_mission_raw_1a91216322f0b6139d5ba83127f2ef1f12}
```cpp
mavsdk::MissionRaw::MissionRaw(const MissionRaw &other)
```


Copy constructor.


**Parameters**

* const [MissionRaw](classmavsdk_1_1_mission_raw.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564}

```cpp
using mavsdk::MissionRaw::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [MissionRaw](classmavsdk_1_1_mission_raw.md) calls.


### typedef DownloadMissionCallback {#classmavsdk_1_1_mission_raw_1a016633e6338744da02ac7cb6da28880a}

```cpp
using mavsdk::MissionRaw::DownloadMissionCallback =  std::function<void(Result, std::vector<MissionItem>)>
```


Callback type for download_mission_async.


### typedef DownloadGeofenceCallback {#classmavsdk_1_1_mission_raw_1a3e75d493ec08fec9d9faa597238c11be}

```cpp
using mavsdk::MissionRaw::DownloadGeofenceCallback =  std::function<void(Result, std::vector<MissionItem>)>
```


Callback type for download_geofence_async.


### typedef DownloadRallypointsCallback {#classmavsdk_1_1_mission_raw_1a473abce54f89efe05d5ac051c9d8a5dc}

```cpp
using mavsdk::MissionRaw::DownloadRallypointsCallback =  std::function<void(Result, std::vector<MissionItem>)>
```


Callback type for download_rallypoints_async.


### typedef MissionProgressCallback {#classmavsdk_1_1_mission_raw_1a9dd594878925da494b4add6acc3184fc}

```cpp
using mavsdk::MissionRaw::MissionProgressCallback =  std::function<void(MissionProgress)>
```


Callback type for subscribe_mission_progress.


### typedef MissionProgressHandle {#classmavsdk_1_1_mission_raw_1a34e0eaf9922daa5d27d2b044eae7885c}

```cpp
using mavsdk::MissionRaw::MissionProgressHandle =  Handle<MissionProgress>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_mission_progress.


### typedef MissionChangedCallback {#classmavsdk_1_1_mission_raw_1ac22d81eefc5e883cdb6baf792a7487e6}

```cpp
using mavsdk::MissionRaw::MissionChangedCallback =  std::function<void(bool)>
```


Callback type for subscribe_mission_changed.


### typedef MissionChangedHandle {#classmavsdk_1_1_mission_raw_1a46da6d8a53822fd5fbd7b2a414624c5c}

```cpp
using mavsdk::MissionRaw::MissionChangedHandle =  Handle<bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_mission_changed.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf}


Possible results returned for action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafaecc3de82f27e8e7f65807c69a114efbe"></span> `TooManyMissionItems` | Too many mission items in the mission. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa253ca7dd096ee0956cccee4d376cab8b"></span> `InvalidArgument` | Invalid argument. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | [Mission](classmavsdk_1_1_mission.md) downloaded from the system is not supported. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa6b0ce476dfc17eed72967386f52ede78"></span> `NoMissionAvailable` | No mission available on the system. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa3465fd31285ebd60597cf59bff9db01a"></span> `TransferCancelled` | [Mission](classmavsdk_1_1_mission.md) transfer (upload or download) has been cancelled. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafac73901782ccb07eeaf03f1a27e323e4f"></span> `FailedToOpenQgcPlan` | Failed to open the QGroundControl plan. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafabd0579c3163a37a4bc4fe181903cc1e9"></span> `FailedToParseQgcPlan` | Failed to parse the QGroundControl plan. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | Request denied. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa43dd1d7bd7daf88c044f7463ccdb1f0c"></span> `MissionTypeNotConsistent` | [Mission](classmavsdk_1_1_mission.md) type is not consistent. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafaff5c3eeddd1c7a19875b5686efd52fdb"></span> `InvalidSequence` | The mission item sequences are not increasing correctly. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa80038ba51e64057d4a00429697a3368f"></span> `CurrentInvalid` | The current item is not set correctly. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafaca3da8f495e4e628912a7798655da6c2"></span> `ProtocolError` | There was a protocol error. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa3f5f527103f3b11c1c271363c542432b"></span> `IntMessagesNotSupported` | The system does not support the MISSION_INT protocol. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafa6d80e75c1ced1d8667d7ba55a2b67429"></span> `FailedToOpenMissionPlannerPlan` | Failed to open the [Mission](classmavsdk_1_1_mission.md) Planner plan. 
<span id="classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eafad56bf0f57cf6420ed771f852543a07eb"></span> `FailedToParseMissionPlannerPlan` | Failed to parse the [Mission](classmavsdk_1_1_mission.md) Planner plan. 

## Member Function Documentation


### upload_mission_async() {#classmavsdk_1_1_mission_raw_1a77cc5df3362b7ab4cbc94e5bc9707609}
```cpp
void mavsdk::MissionRaw::upload_mission_async(std::vector< MissionItem > mission_items, const ResultCallback callback)
```


Upload a list of raw mission items to the system.

The raw mission items are uploaded to a drone. Once uploaded the mission can be started and executed even if the connection is lost.


This function is non-blocking. See 'upload_mission' for the blocking counterpart.

**Parameters**

* std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > **mission_items** - 
* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### upload_mission() {#classmavsdk_1_1_mission_raw_1ad4f5c2ccfb2249f6e11c9533c263926a}
```cpp
Result mavsdk::MissionRaw::upload_mission(std::vector< MissionItem > mission_items) const
```


Upload a list of raw mission items to the system.

The raw mission items are uploaded to a drone. Once uploaded the mission can be started and executed even if the connection is lost.


This function is blocking. See 'upload_mission_async' for the non-blocking counterpart.

**Parameters**

* std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > **mission_items** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### upload_geofence_async() {#classmavsdk_1_1_mission_raw_1abae3246480b334c319cfb8a9c0d2d8c6}
```cpp
void mavsdk::MissionRaw::upload_geofence_async(std::vector< MissionItem > mission_items, const ResultCallback callback)
```


Upload a list of geofence items to the system.

This function is non-blocking. See 'upload_geofence' for the blocking counterpart.

**Parameters**

* std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > **mission_items** - 
* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### upload_geofence() {#classmavsdk_1_1_mission_raw_1ac65fe0675332c318c5c07be5a8d22ac7}
```cpp
Result mavsdk::MissionRaw::upload_geofence(std::vector< MissionItem > mission_items) const
```


Upload a list of geofence items to the system.

This function is blocking. See 'upload_geofence_async' for the non-blocking counterpart.

**Parameters**

* std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > **mission_items** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### upload_rally_points_async() {#classmavsdk_1_1_mission_raw_1a2c5d52246a8ad13151fcfedeab2859e5}
```cpp
void mavsdk::MissionRaw::upload_rally_points_async(std::vector< MissionItem > mission_items, const ResultCallback callback)
```


Upload a list of rally point items to the system.

This function is non-blocking. See 'upload_rally_points' for the blocking counterpart.

**Parameters**

* std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > **mission_items** - 
* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### upload_rally_points() {#classmavsdk_1_1_mission_raw_1a02fc4f293a8094df5dbd7ea0d2184739}
```cpp
Result mavsdk::MissionRaw::upload_rally_points(std::vector< MissionItem > mission_items) const
```


Upload a list of rally point items to the system.

This function is blocking. See 'upload_rally_points_async' for the non-blocking counterpart.

**Parameters**

* std::vector< [MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > **mission_items** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### cancel_mission_upload() {#classmavsdk_1_1_mission_raw_1aa353e3fa6e836305248be131dbe19273}
```cpp
Result mavsdk::MissionRaw::cancel_mission_upload() const
```


Cancel an ongoing mission upload.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### download_mission_async() {#classmavsdk_1_1_mission_raw_1a7e27b0fb58889ca5cb1202276c0e0669}
```cpp
void mavsdk::MissionRaw::download_mission_async(const DownloadMissionCallback callback)
```


Download a list of raw mission items from the system (asynchronous).

This function is non-blocking. See 'download_mission' for the blocking counterpart.

**Parameters**

* const [DownloadMissionCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a016633e6338744da02ac7cb6da28880a) **callback** - 

### download_mission() {#classmavsdk_1_1_mission_raw_1a18138bc7cbc6c6c16fde44e2fa53a459}
```cpp
std::pair< Result, std::vector< MissionRaw::MissionItem > > mavsdk::MissionRaw::download_mission() const
```


Download a list of raw mission items from the system (asynchronous).

This function is blocking. See 'download_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionRaw::MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > > - Result of request.

### download_geofence_async() {#classmavsdk_1_1_mission_raw_1acc681495c318043334691ab9ccd6778d}
```cpp
void mavsdk::MissionRaw::download_geofence_async(const DownloadGeofenceCallback callback)
```


Download a list of raw geofence items from the system (asynchronous).

This function is non-blocking. See 'download_geofence' for the blocking counterpart.

**Parameters**

* const [DownloadGeofenceCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a3e75d493ec08fec9d9faa597238c11be) **callback** - 

### download_geofence() {#classmavsdk_1_1_mission_raw_1ae2d3d743e00312e07ca3893ab27f0138}
```cpp
std::pair< Result, std::vector< MissionRaw::MissionItem > > mavsdk::MissionRaw::download_geofence() const
```


Download a list of raw geofence items from the system (asynchronous).

This function is blocking. See 'download_geofence_async' for the non-blocking counterpart.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionRaw::MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > > - Result of request.

### download_rallypoints_async() {#classmavsdk_1_1_mission_raw_1a38e58cdecc1737936bec5d046c8eb511}
```cpp
void mavsdk::MissionRaw::download_rallypoints_async(const DownloadRallypointsCallback callback)
```


Download a list of raw rallypoint items from the system (asynchronous).

This function is non-blocking. See 'download_rallypoints' for the blocking counterpart.

**Parameters**

* const [DownloadRallypointsCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a473abce54f89efe05d5ac051c9d8a5dc) **callback** - 

### download_rallypoints() {#classmavsdk_1_1_mission_raw_1a0cb3895f91bb2042c9d3282b09c442af}
```cpp
std::pair< Result, std::vector< MissionRaw::MissionItem > > mavsdk::MissionRaw::download_rallypoints() const
```


Download a list of raw rallypoint items from the system (asynchronous).

This function is blocking. See 'download_rallypoints_async' for the non-blocking counterpart.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), std::vector< [MissionRaw::MissionItem](structmavsdk_1_1_mission_raw_1_1_mission_item.md) > > - Result of request.

### cancel_mission_download() {#classmavsdk_1_1_mission_raw_1a7c554999ca66c5434ef1fa334d949e5a}
```cpp
Result mavsdk::MissionRaw::cancel_mission_download() const
```


Cancel an ongoing mission download.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### start_mission_async() {#classmavsdk_1_1_mission_raw_1acca64e0a08978f5721be8fa955b1bb0f}
```cpp
void mavsdk::MissionRaw::start_mission_async(const ResultCallback callback)
```


Start the mission.

A mission must be uploaded to the vehicle before this can be called.


This function is non-blocking. See 'start_mission' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### start_mission() {#classmavsdk_1_1_mission_raw_1af1b010b0f28b284a94eba88198ee15f8}
```cpp
Result mavsdk::MissionRaw::start_mission() const
```


Start the mission.

A mission must be uploaded to the vehicle before this can be called.


This function is blocking. See 'start_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### pause_mission_async() {#classmavsdk_1_1_mission_raw_1aae0eedbe4216266eb6e2115cd03c61a1}
```cpp
void mavsdk::MissionRaw::pause_mission_async(const ResultCallback callback)
```


Pause the mission.

Pausing the mission puts the vehicle into [HOLD mode](https://docs.px4.io/en/flight_modes/hold.html). A multicopter should just hover at the spot while a fixedwing vehicle should loiter around the location where it paused.


This function is non-blocking. See 'pause_mission' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### pause_mission() {#classmavsdk_1_1_mission_raw_1abda483b0659a6c0397c588341688bb39}
```cpp
Result mavsdk::MissionRaw::pause_mission() const
```


Pause the mission.

Pausing the mission puts the vehicle into [HOLD mode](https://docs.px4.io/en/flight_modes/hold.html). A multicopter should just hover at the spot while a fixedwing vehicle should loiter around the location where it paused.


This function is blocking. See 'pause_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### clear_mission_async() {#classmavsdk_1_1_mission_raw_1acf6bf293facbd45fa1126e52e99248a2}
```cpp
void mavsdk::MissionRaw::clear_mission_async(const ResultCallback callback)
```


Clear the mission saved on the vehicle.

This function is non-blocking. See 'clear_mission' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### clear_mission() {#classmavsdk_1_1_mission_raw_1ab10f8fcaa0f6d3e0f844b7430d8d14c2}
```cpp
Result mavsdk::MissionRaw::clear_mission() const
```


Clear the mission saved on the vehicle.

This function is blocking. See 'clear_mission_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### set_current_mission_item_async() {#classmavsdk_1_1_mission_raw_1a5540d6ca691d60ef19b66e303bae7f87}
```cpp
void mavsdk::MissionRaw::set_current_mission_item_async(int32_t index, const ResultCallback callback)
```


Sets the raw mission item index to go to.

By setting the current index to 0, the mission is restarted from the beginning. If it is set to a specific index of a raw mission item, the mission will be set to this item.


This function is non-blocking. See 'set_current_mission_item' for the blocking counterpart.

**Parameters**

* int32_t **index** - 
* const [ResultCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a1a36a84f17dca07e1da49c13abbc9564) **callback** - 

### set_current_mission_item() {#classmavsdk_1_1_mission_raw_1ada9aa2abf79ebfc8e1d10de8e85e91ae}
```cpp
Result mavsdk::MissionRaw::set_current_mission_item(int32_t index) const
```


Sets the raw mission item index to go to.

By setting the current index to 0, the mission is restarted from the beginning. If it is set to a specific index of a raw mission item, the mission will be set to this item.


This function is blocking. See 'set_current_mission_item_async' for the non-blocking counterpart.

**Parameters**

* int32_t **index** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf) - Result of request.

### subscribe_mission_progress() {#classmavsdk_1_1_mission_raw_1a88a3c4b26418e734a547f251706988d2}
```cpp
MissionProgressHandle mavsdk::MissionRaw::subscribe_mission_progress(const MissionProgressCallback &callback)
```


Subscribe to mission progress updates.


**Parameters**

* const [MissionProgressCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a9dd594878925da494b4add6acc3184fc)& **callback** - 

**Returns**

&emsp;[MissionProgressHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a34e0eaf9922daa5d27d2b044eae7885c) - 

### unsubscribe_mission_progress() {#classmavsdk_1_1_mission_raw_1ac46f08b52706f45956cf3b01df381835}
```cpp
void mavsdk::MissionRaw::unsubscribe_mission_progress(MissionProgressHandle handle)
```


Unsubscribe from subscribe_mission_progress.


**Parameters**

* [MissionProgressHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a34e0eaf9922daa5d27d2b044eae7885c) **handle** - 

### mission_progress() {#classmavsdk_1_1_mission_raw_1a3200dea1094926a4dd54f079f21b94e1}
```cpp
MissionProgress mavsdk::MissionRaw::mission_progress() const
```


Poll for '[MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md)' (blocking).


**Returns**

&emsp;[MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md) - One [MissionProgress](structmavsdk_1_1_mission_raw_1_1_mission_progress.md) update.

### subscribe_mission_changed() {#classmavsdk_1_1_mission_raw_1ad4a2991e1a8f9423270af4220309edfb}
```cpp
MissionChangedHandle mavsdk::MissionRaw::subscribe_mission_changed(const MissionChangedCallback &callback)
```


Subscribes to mission changed.

This notification can be used to be informed if a ground station has been uploaded or changed by a ground station or companion computer.

**Parameters**

* const [MissionChangedCallback](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1ac22d81eefc5e883cdb6baf792a7487e6)& **callback** - Callback to notify about change.

**Returns**

&emsp;[MissionChangedHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a46da6d8a53822fd5fbd7b2a414624c5c) - 

### unsubscribe_mission_changed() {#classmavsdk_1_1_mission_raw_1ac6cd7602b2e5b46ad0ea1cf8bf602a0c}
```cpp
void mavsdk::MissionRaw::unsubscribe_mission_changed(MissionChangedHandle handle)
```


Unsubscribe from subscribe_mission_changed.


**Parameters**

* [MissionChangedHandle](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a46da6d8a53822fd5fbd7b2a414624c5c) **handle** - 

### import_qgroundcontrol_mission() {#classmavsdk_1_1_mission_raw_1a2a4ca261c37737e691c6954693d6d0a5}
```cpp
std::pair< Result, MissionRaw::MissionImportData > mavsdk::MissionRaw::import_qgroundcontrol_mission(std::string qgc_plan_path) const
```


Import a QGroundControl missions in JSON .plan format, from a file.

Supported:
<ul>
<li><p>Waypoints</p>
</li>
<li><p>Survey Not supported:</p>
</li>
<li><p>Structure Scan</p>
</li>
</ul>


This function is blocking.

**Parameters**

* std::string **qgc_plan_path** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > - Result of request.

### import_qgroundcontrol_mission_from_string() {#classmavsdk_1_1_mission_raw_1a4a1b55650120d8af0ce7fa037f6b5ce9}
```cpp
std::pair< Result, MissionRaw::MissionImportData > mavsdk::MissionRaw::import_qgroundcontrol_mission_from_string(std::string qgc_plan) const
```


Import a QGroundControl missions in JSON .plan format, from a string.

Supported:
<ul>
<li><p>Waypoints</p>
</li>
<li><p>Survey Not supported:</p>
</li>
<li><p>Structure Scan</p>
</li>
</ul>


This function is blocking.

**Parameters**

* std::string **qgc_plan** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > - Result of request.

### import_mission_planner_mission() {#classmavsdk_1_1_mission_raw_1abf8ac0f813d4dff4f0fe3acce0b39163}
```cpp
std::pair< Result, MissionRaw::MissionImportData > mavsdk::MissionRaw::import_mission_planner_mission(std::string mission_planner_path) const
```


Import a [Mission](classmavsdk_1_1_mission.md) Planner mission in QGC WPL 110 format, from a file.

Supported:
<ul>
<li><p>Waypoints</p>
</li>
<li><p>ArduPilot home position handling</p>
</li>
</ul>


This function is blocking.

**Parameters**

* std::string **mission_planner_path** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > - Result of request.

### import_mission_planner_mission_from_string() {#classmavsdk_1_1_mission_raw_1a25e5f8e8c7f6d778f7ab6e91b39a6ec9}
```cpp
std::pair< Result, MissionRaw::MissionImportData > mavsdk::MissionRaw::import_mission_planner_mission_from_string(std::string mission_planner_mission) const
```


Import a [Mission](classmavsdk_1_1_mission.md) Planner mission in QGC WPL 110 format, from a string.

Supported:
<ul>
<li><p>Waypoints</p>
</li>
<li><p>ArduPilot home position handling</p>
</li>
</ul>


This function is blocking.

**Parameters**

* std::string **mission_planner_mission** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), [MissionRaw::MissionImportData](structmavsdk_1_1_mission_raw_1_1_mission_import_data.md) > - Result of request.

### is_mission_finished() {#classmavsdk_1_1_mission_raw_1a998db451c2718d9728276ca2d01ce315}
```cpp
std::pair< Result, bool > mavsdk::MissionRaw::is_mission_finished() const
```


Check if the mission is finished.

Returns true if the mission is finished, false otherwise.


This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_mission_raw.md#classmavsdk_1_1_mission_raw_1a7ea2a624818ebb5a3e209cc275d58eaf), bool > - Result of request.

### operator=() {#classmavsdk_1_1_mission_raw_1a2b8cdc1fbee72224a9ef6eb9266b2e2a}
```cpp
const MissionRaw & mavsdk::MissionRaw::operator=(const MissionRaw &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [MissionRaw](classmavsdk_1_1_mission_raw.md)&  - 

**Returns**

&emsp;const [MissionRaw](classmavsdk_1_1_mission_raw.md) & - 