# mavsdk::Camera Class Reference
`#include: camera.h`

----


Can be used to manage cameras that implement the MAVLink [Camera](classmavsdk_1_1_camera.md) Protocol: [https://mavlink.io/en/services/camera.html](https://mavlink.io/en/services/camera.html). 


Currently only a single camera is supported. When multiple cameras are supported the plugin will need to be instantiated separately for every camera and the camera selected using `select_camera`. 


## Data Structures


struct [CameraList](structmavsdk_1_1_camera_1_1_camera_list.md)

struct [CaptureInfo](structmavsdk_1_1_camera_1_1_capture_info.md)

struct [CurrentSettingsUpdate](structmavsdk_1_1_camera_1_1_current_settings_update.md)

struct [EulerAngle](structmavsdk_1_1_camera_1_1_euler_angle.md)

struct [Information](structmavsdk_1_1_camera_1_1_information.md)

struct [ModeUpdate](structmavsdk_1_1_camera_1_1_mode_update.md)

struct [Option](structmavsdk_1_1_camera_1_1_option.md)

struct [Position](structmavsdk_1_1_camera_1_1_position.md)

struct [PossibleSettingOptionsUpdate](structmavsdk_1_1_camera_1_1_possible_setting_options_update.md)

struct [Quaternion](structmavsdk_1_1_camera_1_1_quaternion.md)

struct [Setting](structmavsdk_1_1_camera_1_1_setting.md)

struct [SettingOptions](structmavsdk_1_1_camera_1_1_setting_options.md)

struct [Storage](structmavsdk_1_1_camera_1_1_storage.md)

struct [StorageUpdate](structmavsdk_1_1_camera_1_1_storage_update.md)

struct [VideoStreamInfo](structmavsdk_1_1_camera_1_1_video_stream_info.md)

struct [VideoStreamSettings](structmavsdk_1_1_camera_1_1_video_stream_settings.md)

struct [VideoStreamUpdate](structmavsdk_1_1_camera_1_1_video_stream_update.md)

## Public Types


Type | Description
--- | ---
enum [Mode](#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) | [Camera](classmavsdk_1_1_camera.md) mode type.
enum [PhotosRange](#classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22) | Photos range type.
enum [Result](#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | Possible results returned for camera commands.
std::function< void([Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf))> [ResultCallback](#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) | Callback type for asynchronous [Camera](classmavsdk_1_1_camera.md) calls.
std::function< void([Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [CaptureInfo](structmavsdk_1_1_camera_1_1_capture_info.md) >)> [ListPhotosCallback](#classmavsdk_1_1_camera_1a23240233586f7673c3a1b48f07623fe4) | Callback type for list_photos_async.
std::function< void([CameraList](structmavsdk_1_1_camera_1_1_camera_list.md))> [CameraListCallback](#classmavsdk_1_1_camera_1ac853b9f8da070d22b1cfe2680f7785d3) | Callback type for subscribe_camera_list.
[Handle](classmavsdk_1_1_handle.md)< [CameraList](structmavsdk_1_1_camera_1_1_camera_list.md) > [CameraListHandle](#classmavsdk_1_1_camera_1aeb7f6082a8f08509e8c45f23768a5a64) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_camera_list.
std::function< void([ModeUpdate](structmavsdk_1_1_camera_1_1_mode_update.md))> [ModeCallback](#classmavsdk_1_1_camera_1af6649645c4056d797b42de9418dc4226) | Callback type for subscribe_mode.
[Handle](classmavsdk_1_1_handle.md)< [ModeUpdate](structmavsdk_1_1_camera_1_1_mode_update.md) > [ModeHandle](#classmavsdk_1_1_camera_1a71c4937ed9dcc1766561370cfe0f523f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_mode.
std::function< void([VideoStreamUpdate](structmavsdk_1_1_camera_1_1_video_stream_update.md))> [VideoStreamInfoCallback](#classmavsdk_1_1_camera_1a5c9669b2652e37e067c6c5c300c9409b) | Callback type for subscribe_video_stream_info.
[Handle](classmavsdk_1_1_handle.md)< [VideoStreamUpdate](structmavsdk_1_1_camera_1_1_video_stream_update.md) > [VideoStreamInfoHandle](#classmavsdk_1_1_camera_1a093e5b2652791c703534f38ca329b198) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_video_stream_info.
std::function< void([CaptureInfo](structmavsdk_1_1_camera_1_1_capture_info.md))> [CaptureInfoCallback](#classmavsdk_1_1_camera_1a9bf5c20ea0b03ab019057829d1a3441e) | Callback type for subscribe_capture_info.
[Handle](classmavsdk_1_1_handle.md)< [CaptureInfo](structmavsdk_1_1_camera_1_1_capture_info.md) > [CaptureInfoHandle](#classmavsdk_1_1_camera_1a789f1fccde8eb44bc07694751b3a1b40) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_capture_info.
std::function< void([StorageUpdate](structmavsdk_1_1_camera_1_1_storage_update.md))> [StorageCallback](#classmavsdk_1_1_camera_1a7811502147e261b81f7f4ea2422fad03) | Callback type for subscribe_storage.
[Handle](classmavsdk_1_1_handle.md)< [StorageUpdate](structmavsdk_1_1_camera_1_1_storage_update.md) > [StorageHandle](#classmavsdk_1_1_camera_1a4707b885d81143f3cb24ca956c1f91c9) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_storage.
std::function< void([CurrentSettingsUpdate](structmavsdk_1_1_camera_1_1_current_settings_update.md))> [CurrentSettingsCallback](#classmavsdk_1_1_camera_1ac12104378b90908410ffe100e9fd264e) | Callback type for subscribe_current_settings.
[Handle](classmavsdk_1_1_handle.md)< [CurrentSettingsUpdate](structmavsdk_1_1_camera_1_1_current_settings_update.md) > [CurrentSettingsHandle](#classmavsdk_1_1_camera_1afa537975f7985f7f7697dc7b4477ccd9) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_current_settings.
std::function< void([PossibleSettingOptionsUpdate](structmavsdk_1_1_camera_1_1_possible_setting_options_update.md))> [PossibleSettingOptionsCallback](#classmavsdk_1_1_camera_1a7eec62e8cdab1a2a7e787bbfeb5205e2) | Callback type for subscribe_possible_setting_options.
[Handle](classmavsdk_1_1_handle.md)< [PossibleSettingOptionsUpdate](structmavsdk_1_1_camera_1_1_possible_setting_options_update.md) > [PossibleSettingOptionsHandle](#classmavsdk_1_1_camera_1a8ebb2da9f006cff11293e7e5a20d2f82) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_possible_setting_options.
std::function< void([Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Setting](structmavsdk_1_1_camera_1_1_setting.md))> [GetSettingCallback](#classmavsdk_1_1_camera_1ac233f5688f0b7f1e712bb31dfaeadd85) | Callback type for get_setting_async.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Camera](#classmavsdk_1_1_camera_1a186a2853440c879b99ed7e4a726969e9) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Camera](#classmavsdk_1_1_camera_1aecd55dc849bbb967a6b9dcfd36cc075e) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Camera](#classmavsdk_1_1_camera_1afc23a58baf97868b1d9a9b983d7c2087) () override | Destructor (internal use only).
&nbsp; | [Camera](#classmavsdk_1_1_camera_1a1b83c38c360d70b56222a94999d862fd) (const [Camera](classmavsdk_1_1_camera.md) & other) | Copy constructor.
void | [take_photo_async](#classmavsdk_1_1_camera_1ae7a59d768c597fbee8eff47073dd8690) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Take one photo.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [take_photo](#classmavsdk_1_1_camera_1ae06036414213ef3af9dd1e63401b043e) (int32_t component_id)const | Take one photo.
void | [start_photo_interval_async](#classmavsdk_1_1_camera_1af57a8398e1f2d4ca79368d04bb7c211f) (int32_t component_id, float interval_s, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Start photo timelapse with a given interval.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [start_photo_interval](#classmavsdk_1_1_camera_1afb346b7103eebf4b71846c17cda3b5d9) (int32_t component_id, float interval_s)const | Start photo timelapse with a given interval.
void | [stop_photo_interval_async](#classmavsdk_1_1_camera_1af63064187baaa598d1b38545ec28e173) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Stop a running photo timelapse.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [stop_photo_interval](#classmavsdk_1_1_camera_1a1284b8db67fd8444704680eeb5136915) (int32_t component_id)const | Stop a running photo timelapse.
void | [start_video_async](#classmavsdk_1_1_camera_1af88ebe8c37534ce6cdbc2956a5b7422f) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Start a video recording.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [start_video](#classmavsdk_1_1_camera_1a962e0a2b69bedfffc34cdb87dca6ce15) (int32_t component_id)const | Start a video recording.
void | [stop_video_async](#classmavsdk_1_1_camera_1a35e4a079f3f82e936eb592ca926808b4) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Stop a running video recording.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [stop_video](#classmavsdk_1_1_camera_1a0f5c985d7c2b2d4e32af3ee2d16a2727) (int32_t component_id)const | Stop a running video recording.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [start_video_streaming](#classmavsdk_1_1_camera_1a91530da971787bc240f50171ebbf756d) (int32_t component_id, int32_t stream_id)const | Start video streaming.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [stop_video_streaming](#classmavsdk_1_1_camera_1a3992c4fc3f133904d039e3a9ab035818) (int32_t component_id, int32_t stream_id)const | Stop current video streaming.
void | [set_mode_async](#classmavsdk_1_1_camera_1a46e3a1929048d5ebc3bad7787e0ff72e) (int32_t component_id, [Mode](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) mode, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Set camera mode.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [set_mode](#classmavsdk_1_1_camera_1a0f7421bda8f0a7c7e50865e315a35439) (int32_t component_id, [Mode](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) mode)const | Set camera mode.
void | [list_photos_async](#classmavsdk_1_1_camera_1a8e9e1364079a503c705255b36cd41ff8) (int32_t component_id, [PhotosRange](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22) photos_range, const [ListPhotosCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a23240233586f7673c3a1b48f07623fe4) callback) | List photos available on the camera.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [Camera::CaptureInfo](structmavsdk_1_1_camera_1_1_capture_info.md) > > | [list_photos](#classmavsdk_1_1_camera_1a775bbe8873c57d789f42b5f59bf2bff0) (int32_t component_id, [PhotosRange](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22) photos_range)const | List photos available on the camera.
[CameraListHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1aeb7f6082a8f08509e8c45f23768a5a64) | [subscribe_camera_list](#classmavsdk_1_1_camera_1af7de7e428b7d1f95535d27e246be615a) (const [CameraListCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1ac853b9f8da070d22b1cfe2680f7785d3) & callback) | Subscribe to list of cameras.
void | [unsubscribe_camera_list](#classmavsdk_1_1_camera_1a686168126d1d9b7905f4b9c50504598d) ([CameraListHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1aeb7f6082a8f08509e8c45f23768a5a64) handle) | Unsubscribe from subscribe_camera_list.
[CameraList](structmavsdk_1_1_camera_1_1_camera_list.md) | [camera_list](#classmavsdk_1_1_camera_1ac6514ecf820f819076663401e8527f2d) () const | Poll for '[CameraList](structmavsdk_1_1_camera_1_1_camera_list.md)' (blocking).
[ModeHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a71c4937ed9dcc1766561370cfe0f523f) | [subscribe_mode](#classmavsdk_1_1_camera_1a1963bfcfcc8c9e96451648f96fd618cd) (const [ModeCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1af6649645c4056d797b42de9418dc4226) & callback) | Subscribe to camera mode updates.
void | [unsubscribe_mode](#classmavsdk_1_1_camera_1ab9437b41565d8957050e09cf4143f5d1) ([ModeHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a71c4937ed9dcc1766561370cfe0f523f) handle) | Unsubscribe from subscribe_mode.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::Mode](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) > | [get_mode](#classmavsdk_1_1_camera_1a5f6377a669bfb8ad4c0efb310ef9bfd2) (int32_t component_id)const | Get camera mode.
[VideoStreamInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a093e5b2652791c703534f38ca329b198) | [subscribe_video_stream_info](#classmavsdk_1_1_camera_1a4dcf0ebeb1f9f5e5b2b71f59c9b7eeb9) (const [VideoStreamInfoCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a5c9669b2652e37e067c6c5c300c9409b) & callback) | Subscribe to video stream info updates.
void | [unsubscribe_video_stream_info](#classmavsdk_1_1_camera_1abc370783800fb90dda20d2843f990036) ([VideoStreamInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a093e5b2652791c703534f38ca329b198) handle) | Unsubscribe from subscribe_video_stream_info.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::VideoStreamInfo](structmavsdk_1_1_camera_1_1_video_stream_info.md) > | [get_video_stream_info](#classmavsdk_1_1_camera_1a08b199c0a0cceb37f55a457d56a6797f) (int32_t component_id)const | Get video stream info.
[CaptureInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a789f1fccde8eb44bc07694751b3a1b40) | [subscribe_capture_info](#classmavsdk_1_1_camera_1a8822ab1d802984a1acc9b53cd9810e8f) (const [CaptureInfoCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a9bf5c20ea0b03ab019057829d1a3441e) & callback) | Subscribe to capture info updates.
void | [unsubscribe_capture_info](#classmavsdk_1_1_camera_1a9b7f729dfa30a6cb0d041533bd272793) ([CaptureInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a789f1fccde8eb44bc07694751b3a1b40) handle) | Unsubscribe from subscribe_capture_info.
[StorageHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a4707b885d81143f3cb24ca956c1f91c9) | [subscribe_storage](#classmavsdk_1_1_camera_1a3b2c8f2f9512ceb6d456f35e1bf7777f) (const [StorageCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a7811502147e261b81f7f4ea2422fad03) & callback) | Subscribe to camera's storage status updates.
void | [unsubscribe_storage](#classmavsdk_1_1_camera_1a925cab3ea7e34ff2ed5a698598c5edd3) ([StorageHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a4707b885d81143f3cb24ca956c1f91c9) handle) | Unsubscribe from subscribe_storage.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::Storage](structmavsdk_1_1_camera_1_1_storage.md) > | [get_storage](#classmavsdk_1_1_camera_1a2e35679fe372bb575a6b3b84dfc82c72) (int32_t component_id)const | Get camera's storage status.
[CurrentSettingsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1afa537975f7985f7f7697dc7b4477ccd9) | [subscribe_current_settings](#classmavsdk_1_1_camera_1a906c431b63dde866b691ab4c839858ff) (const [CurrentSettingsCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1ac12104378b90908410ffe100e9fd264e) & callback) | Get the list of current camera settings.
void | [unsubscribe_current_settings](#classmavsdk_1_1_camera_1a117e76672b39c9cd751fa64a42741c17) ([CurrentSettingsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1afa537975f7985f7f7697dc7b4477ccd9) handle) | Unsubscribe from subscribe_current_settings.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [Camera::Setting](structmavsdk_1_1_camera_1_1_setting.md) > > | [get_current_settings](#classmavsdk_1_1_camera_1a980512b264c8aff8584a12a749d00382) (int32_t component_id)const | Get current settings.
[PossibleSettingOptionsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8ebb2da9f006cff11293e7e5a20d2f82) | [subscribe_possible_setting_options](#classmavsdk_1_1_camera_1aef7c545aaa05e36b212255120557967b) (const [PossibleSettingOptionsCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a7eec62e8cdab1a2a7e787bbfeb5205e2) & callback) | Get the list of settings that can be changed.
void | [unsubscribe_possible_setting_options](#classmavsdk_1_1_camera_1a191cc00bb6b1c834fc93b19f4bff1c29) ([PossibleSettingOptionsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8ebb2da9f006cff11293e7e5a20d2f82) handle) | Unsubscribe from subscribe_possible_setting_options.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [Camera::SettingOptions](structmavsdk_1_1_camera_1_1_setting_options.md) > > | [get_possible_setting_options](#classmavsdk_1_1_camera_1a7c9aa933b7c93a5178cd9e478ee72170) (int32_t component_id)const | Get possible setting options.
void | [set_setting_async](#classmavsdk_1_1_camera_1a5861022cf053c2c347c994e0d0ae62a4) (int32_t component_id, [Setting](structmavsdk_1_1_camera_1_1_setting.md) setting, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Set a setting to some value.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [set_setting](#classmavsdk_1_1_camera_1ab34650789b55900716dd8ea3d81d5a05) (int32_t component_id, [Setting](structmavsdk_1_1_camera_1_1_setting.md) setting)const | Set a setting to some value.
void | [get_setting_async](#classmavsdk_1_1_camera_1a3364fd9b2f48388148620471bc598d85) (int32_t component_id, [Setting](structmavsdk_1_1_camera_1_1_setting.md) setting, const [GetSettingCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1ac233f5688f0b7f1e712bb31dfaeadd85) callback) | Get a setting.
std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::Setting](structmavsdk_1_1_camera_1_1_setting.md) > | [get_setting](#classmavsdk_1_1_camera_1a0e8376bf9c3e4d9367b9e296e808529a) (int32_t component_id, [Setting](structmavsdk_1_1_camera_1_1_setting.md) setting)const | Get a setting.
void | [format_storage_async](#classmavsdk_1_1_camera_1a5ca1864fd62f357187290f753a3e7b89) (int32_t component_id, int32_t storage_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Format storage (e.g. SD card) in camera.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [format_storage](#classmavsdk_1_1_camera_1ad239fe6b8dad6783d1f53c3e8e8212ae) (int32_t component_id, int32_t storage_id)const | Format storage (e.g. SD card) in camera.
void | [reset_settings_async](#classmavsdk_1_1_camera_1a6b558f5e8e6298584883a5622dd0d5eb) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Reset all settings in camera.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [reset_settings](#classmavsdk_1_1_camera_1a6b5ba6cd713705848585d2808ceb53fc) (int32_t component_id)const | Reset all settings in camera.
void | [zoom_in_start_async](#classmavsdk_1_1_camera_1a6ff5149c53d8acc6c4a5f17081b88a15) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Start zooming in.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [zoom_in_start](#classmavsdk_1_1_camera_1a71b6535b10a5cf00baf451e4b2e0b6c4) (int32_t component_id)const | Start zooming in.
void | [zoom_out_start_async](#classmavsdk_1_1_camera_1a088b21cec49f800ab666d7409a299937) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Start zooming out.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [zoom_out_start](#classmavsdk_1_1_camera_1a72456e28c67ba451921cedf7f11f5954) (int32_t component_id)const | Start zooming out.
void | [zoom_stop_async](#classmavsdk_1_1_camera_1a2a30851420c2d2ceb102692cf917327b) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Stop zooming.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [zoom_stop](#classmavsdk_1_1_camera_1a30fee70ce5bb42071fcce27841a8a01a) (int32_t component_id)const | Stop zooming.
void | [zoom_range_async](#classmavsdk_1_1_camera_1acce728b12b8e767ee9cc55afb1d4d6ee) (int32_t component_id, float range, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Zoom to value as proportion of full camera range (percentage between 0.0 and 100.0).
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [zoom_range](#classmavsdk_1_1_camera_1ac9a05ffb7a8520c9f9dcb29919878d11) (int32_t component_id, float range)const | Zoom to value as proportion of full camera range (percentage between 0.0 and 100.0).
void | [track_point_async](#classmavsdk_1_1_camera_1ac83433124165ec1b3fbe2629a9939617) (int32_t component_id, float point_x, float point_y, float radius, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Track point.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [track_point](#classmavsdk_1_1_camera_1aff70a9533d377b792ede73d1d3bec2fd) (int32_t component_id, float point_x, float point_y, float radius)const | Track point.
void | [track_rectangle_async](#classmavsdk_1_1_camera_1a4fcec80674d2de32934c67a3544130f6) (int32_t component_id, float top_left_x, float top_left_y, float bottom_right_x, float bottom_right_y, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Track rectangle.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [track_rectangle](#classmavsdk_1_1_camera_1a32c338bc723337dcf4229bac94073d03) (int32_t component_id, float top_left_x, float top_left_y, float bottom_right_x, float bottom_right_y)const | Track rectangle.
void | [track_stop_async](#classmavsdk_1_1_camera_1a7b1c720a0a346516af8c1bc3e49210cc) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Stop tracking.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [track_stop](#classmavsdk_1_1_camera_1a1f4a0c1c2cfcef55cdab94f3d726766f) (int32_t component_id)const | Stop tracking.
void | [focus_in_start_async](#classmavsdk_1_1_camera_1ad40616613df551c00a78099e7790b880) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Start focusing in.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [focus_in_start](#classmavsdk_1_1_camera_1a8be24bc7d5c21b94b388bc1eeed2e8fe) (int32_t component_id)const | Start focusing in.
void | [focus_out_start_async](#classmavsdk_1_1_camera_1ad7917d952b7d0e43dba631a152575765) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Start focusing out.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [focus_out_start](#classmavsdk_1_1_camera_1a25b5c34491cb17f40e7c9c4461c37723) (int32_t component_id)const | Start focusing out.
void | [focus_stop_async](#classmavsdk_1_1_camera_1a562ad9fa8f29b359785069a9dc5b8de2) (int32_t component_id, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Stop focus.
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [focus_stop](#classmavsdk_1_1_camera_1aed5b395a57b14da2d15395dfa2e56600) (int32_t component_id)const | Stop focus.
void | [focus_range_async](#classmavsdk_1_1_camera_1a44cebbb36a9bc9c203dc0233f8b52f7b) (int32_t component_id, float range, const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) callback) | Focus with range value of full range (value between 0.0 and 100.0).
[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) | [focus_range](#classmavsdk_1_1_camera_1aec7d7a0ed5fa2ba84124f9f363e8d94a) (int32_t component_id, float range)const | Focus with range value of full range (value between 0.0 and 100.0).
const [Camera](classmavsdk_1_1_camera.md) & | [operator=](#classmavsdk_1_1_camera_1a358026db44ff9a10cf14a166d8f1da78) (const [Camera](classmavsdk_1_1_camera.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Camera() {#classmavsdk_1_1_camera_1a186a2853440c879b99ed7e4a726969e9}
```cpp
mavsdk::Camera::Camera(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto camera = Camera(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Camera() {#classmavsdk_1_1_camera_1aecd55dc849bbb967a6b9dcfd36cc075e}
```cpp
mavsdk::Camera::Camera(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto camera = Camera(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Camera() {#classmavsdk_1_1_camera_1afc23a58baf97868b1d9a9b983d7c2087}
```cpp
mavsdk::Camera::~Camera() override
```


Destructor (internal use only).


### Camera() {#classmavsdk_1_1_camera_1a1b83c38c360d70b56222a94999d862fd}
```cpp
mavsdk::Camera::Camera(const Camera &other)
```


Copy constructor.


**Parameters**

* const [Camera](classmavsdk_1_1_camera.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f}

```cpp
using mavsdk::Camera::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Camera](classmavsdk_1_1_camera.md) calls.


### typedef ListPhotosCallback {#classmavsdk_1_1_camera_1a23240233586f7673c3a1b48f07623fe4}

```cpp
using mavsdk::Camera::ListPhotosCallback =  std::function<void(Result, std::vector<CaptureInfo>)>
```


Callback type for list_photos_async.


### typedef CameraListCallback {#classmavsdk_1_1_camera_1ac853b9f8da070d22b1cfe2680f7785d3}

```cpp
using mavsdk::Camera::CameraListCallback =  std::function<void(CameraList)>
```


Callback type for subscribe_camera_list.


### typedef CameraListHandle {#classmavsdk_1_1_camera_1aeb7f6082a8f08509e8c45f23768a5a64}

```cpp
using mavsdk::Camera::CameraListHandle =  Handle<CameraList>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_camera_list.


### typedef ModeCallback {#classmavsdk_1_1_camera_1af6649645c4056d797b42de9418dc4226}

```cpp
using mavsdk::Camera::ModeCallback =  std::function<void(ModeUpdate)>
```


Callback type for subscribe_mode.


### typedef ModeHandle {#classmavsdk_1_1_camera_1a71c4937ed9dcc1766561370cfe0f523f}

```cpp
using mavsdk::Camera::ModeHandle =  Handle<ModeUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_mode.


### typedef VideoStreamInfoCallback {#classmavsdk_1_1_camera_1a5c9669b2652e37e067c6c5c300c9409b}

```cpp
using mavsdk::Camera::VideoStreamInfoCallback =  std::function<void(VideoStreamUpdate)>
```


Callback type for subscribe_video_stream_info.


### typedef VideoStreamInfoHandle {#classmavsdk_1_1_camera_1a093e5b2652791c703534f38ca329b198}

```cpp
using mavsdk::Camera::VideoStreamInfoHandle =  Handle<VideoStreamUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_video_stream_info.


### typedef CaptureInfoCallback {#classmavsdk_1_1_camera_1a9bf5c20ea0b03ab019057829d1a3441e}

```cpp
using mavsdk::Camera::CaptureInfoCallback =  std::function<void(CaptureInfo)>
```


Callback type for subscribe_capture_info.


### typedef CaptureInfoHandle {#classmavsdk_1_1_camera_1a789f1fccde8eb44bc07694751b3a1b40}

```cpp
using mavsdk::Camera::CaptureInfoHandle =  Handle<CaptureInfo>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_capture_info.


### typedef StorageCallback {#classmavsdk_1_1_camera_1a7811502147e261b81f7f4ea2422fad03}

```cpp
using mavsdk::Camera::StorageCallback =  std::function<void(StorageUpdate)>
```


Callback type for subscribe_storage.


### typedef StorageHandle {#classmavsdk_1_1_camera_1a4707b885d81143f3cb24ca956c1f91c9}

```cpp
using mavsdk::Camera::StorageHandle =  Handle<StorageUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_storage.


### typedef CurrentSettingsCallback {#classmavsdk_1_1_camera_1ac12104378b90908410ffe100e9fd264e}

```cpp
using mavsdk::Camera::CurrentSettingsCallback =  std::function<void(CurrentSettingsUpdate)>
```


Callback type for subscribe_current_settings.


### typedef CurrentSettingsHandle {#classmavsdk_1_1_camera_1afa537975f7985f7f7697dc7b4477ccd9}

```cpp
using mavsdk::Camera::CurrentSettingsHandle =  Handle<CurrentSettingsUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_current_settings.


### typedef PossibleSettingOptionsCallback {#classmavsdk_1_1_camera_1a7eec62e8cdab1a2a7e787bbfeb5205e2}

```cpp
using mavsdk::Camera::PossibleSettingOptionsCallback =  std::function<void(PossibleSettingOptionsUpdate)>
```


Callback type for subscribe_possible_setting_options.


### typedef PossibleSettingOptionsHandle {#classmavsdk_1_1_camera_1a8ebb2da9f006cff11293e7e5a20d2f82}

```cpp
using mavsdk::Camera::PossibleSettingOptionsHandle =  Handle<PossibleSettingOptionsUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_possible_setting_options.


### typedef GetSettingCallback {#classmavsdk_1_1_camera_1ac233f5688f0b7f1e712bb31dfaeadd85}

```cpp
using mavsdk::Camera::GetSettingCallback =  std::function<void(Result, Setting)>
```


Callback type for get_setting_async.


## Member Enumeration Documentation


### enum Mode {#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65}


[Camera](classmavsdk_1_1_camera.md) mode type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown. 
<span id="classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65ac03d53b70feba4ea842510abecd6c45e"></span> `Photo` | Photo mode. 
<span id="classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65a34e2d1989a1dbf75cd631596133ee5ee"></span> `Video` | Video mode. 

### enum PhotosRange {#classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22}


Photos range type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22ab1c94ca2fbc3e78fc30069c8d0f01680"></span> `All` | All the photos present on the camera. 
<span id="classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22a1d4c76f33a690094071f239ada313ceb"></span> `SinceConnection` | Photos taken since MAVSDK got connected. 

### enum Result {#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf}


Possible results returned for camera commands.


Value | Description
--- | ---
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Command executed successfully. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa12d868c18cb29bf58f02b504be9033fd"></span> `InProgress` | Command in progress. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | [Camera](classmavsdk_1_1_camera.md) is busy and rejected command. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | [Camera](classmavsdk_1_1_camera.md) denied the command. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | An error has occurred while executing the command. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Command timed out. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa5a738160747f39c20e9f65416931c974"></span> `WrongArgument` | Command has wrong argument(s). 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfacb4221c88cb5b30c3380aad8569a80c1"></span> `ProtocolUnsupported` | Definition file protocol not supported. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa453e6aa38d87b28ccae545967c53004f"></span> `Unavailable` | Not available (yet). 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfa7949960ae6f23c2b4fef10e8868198fc"></span> `CameraIdInvalid` | [Camera](classmavsdk_1_1_camera.md) with camera ID not found. 
<span id="classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcfaef156e757c0ac9911d4734ca7601b716"></span> `ActionUnsupported` | [Camera](classmavsdk_1_1_camera.md) action not supported. 

## Member Function Documentation


### take_photo_async() {#classmavsdk_1_1_camera_1ae7a59d768c597fbee8eff47073dd8690}
```cpp
void mavsdk::Camera::take_photo_async(int32_t component_id, const ResultCallback callback)
```


Take one photo.

This function is non-blocking. See 'take_photo' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### take_photo() {#classmavsdk_1_1_camera_1ae06036414213ef3af9dd1e63401b043e}
```cpp
Result mavsdk::Camera::take_photo(int32_t component_id) const
```


Take one photo.

This function is blocking. See 'take_photo_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### start_photo_interval_async() {#classmavsdk_1_1_camera_1af57a8398e1f2d4ca79368d04bb7c211f}
```cpp
void mavsdk::Camera::start_photo_interval_async(int32_t component_id, float interval_s, const ResultCallback callback)
```


Start photo timelapse with a given interval.

This function is non-blocking. See 'start_photo_interval' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **interval_s** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### start_photo_interval() {#classmavsdk_1_1_camera_1afb346b7103eebf4b71846c17cda3b5d9}
```cpp
Result mavsdk::Camera::start_photo_interval(int32_t component_id, float interval_s) const
```


Start photo timelapse with a given interval.

This function is blocking. See 'start_photo_interval_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **interval_s** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### stop_photo_interval_async() {#classmavsdk_1_1_camera_1af63064187baaa598d1b38545ec28e173}
```cpp
void mavsdk::Camera::stop_photo_interval_async(int32_t component_id, const ResultCallback callback)
```


Stop a running photo timelapse.

This function is non-blocking. See 'stop_photo_interval' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### stop_photo_interval() {#classmavsdk_1_1_camera_1a1284b8db67fd8444704680eeb5136915}
```cpp
Result mavsdk::Camera::stop_photo_interval(int32_t component_id) const
```


Stop a running photo timelapse.

This function is blocking. See 'stop_photo_interval_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### start_video_async() {#classmavsdk_1_1_camera_1af88ebe8c37534ce6cdbc2956a5b7422f}
```cpp
void mavsdk::Camera::start_video_async(int32_t component_id, const ResultCallback callback)
```


Start a video recording.

This function is non-blocking. See 'start_video' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### start_video() {#classmavsdk_1_1_camera_1a962e0a2b69bedfffc34cdb87dca6ce15}
```cpp
Result mavsdk::Camera::start_video(int32_t component_id) const
```


Start a video recording.

This function is blocking. See 'start_video_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### stop_video_async() {#classmavsdk_1_1_camera_1a35e4a079f3f82e936eb592ca926808b4}
```cpp
void mavsdk::Camera::stop_video_async(int32_t component_id, const ResultCallback callback)
```


Stop a running video recording.

This function is non-blocking. See 'stop_video' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### stop_video() {#classmavsdk_1_1_camera_1a0f5c985d7c2b2d4e32af3ee2d16a2727}
```cpp
Result mavsdk::Camera::stop_video(int32_t component_id) const
```


Stop a running video recording.

This function is blocking. See 'stop_video_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### start_video_streaming() {#classmavsdk_1_1_camera_1a91530da971787bc240f50171ebbf756d}
```cpp
Result mavsdk::Camera::start_video_streaming(int32_t component_id, int32_t stream_id) const
```


Start video streaming.

This function is blocking.

**Parameters**

* int32_t **component_id** - 
* int32_t **stream_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### stop_video_streaming() {#classmavsdk_1_1_camera_1a3992c4fc3f133904d039e3a9ab035818}
```cpp
Result mavsdk::Camera::stop_video_streaming(int32_t component_id, int32_t stream_id) const
```


Stop current video streaming.

This function is blocking.

**Parameters**

* int32_t **component_id** - 
* int32_t **stream_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### set_mode_async() {#classmavsdk_1_1_camera_1a46e3a1929048d5ebc3bad7787e0ff72e}
```cpp
void mavsdk::Camera::set_mode_async(int32_t component_id, Mode mode, const ResultCallback callback)
```


Set camera mode.

This function is non-blocking. See 'set_mode' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [Mode](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) **mode** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### set_mode() {#classmavsdk_1_1_camera_1a0f7421bda8f0a7c7e50865e315a35439}
```cpp
Result mavsdk::Camera::set_mode(int32_t component_id, Mode mode) const
```


Set camera mode.

This function is blocking. See 'set_mode_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [Mode](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) **mode** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### list_photos_async() {#classmavsdk_1_1_camera_1a8e9e1364079a503c705255b36cd41ff8}
```cpp
void mavsdk::Camera::list_photos_async(int32_t component_id, PhotosRange photos_range, const ListPhotosCallback callback)
```


List photos available on the camera.

Note that this might need to be called initially to set the PhotosRange accordingly. Once set to 'all' rather than 'since connection', it will try to request the previous images over time.


This function is non-blocking. See 'list_photos' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [PhotosRange](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22) **photos_range** - 
* const [ListPhotosCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a23240233586f7673c3a1b48f07623fe4) **callback** - 

### list_photos() {#classmavsdk_1_1_camera_1a775bbe8873c57d789f42b5f59bf2bff0}
```cpp
std::pair< Result, std::vector< Camera::CaptureInfo > > mavsdk::Camera::list_photos(int32_t component_id, PhotosRange photos_range) const
```


List photos available on the camera.

Note that this might need to be called initially to set the PhotosRange accordingly. Once set to 'all' rather than 'since connection', it will try to request the previous images over time.


This function is blocking. See 'list_photos_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [PhotosRange](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a1211ea6664aa9c1d4ef4aede363c7c22) **photos_range** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [Camera::CaptureInfo](structmavsdk_1_1_camera_1_1_capture_info.md) > > - Result of request.

### subscribe_camera_list() {#classmavsdk_1_1_camera_1af7de7e428b7d1f95535d27e246be615a}
```cpp
CameraListHandle mavsdk::Camera::subscribe_camera_list(const CameraListCallback &callback)
```


Subscribe to list of cameras.

This allows to find out what cameras are connected to the system. Based on the camera ID, we can then address a specific camera.

**Parameters**

* const [CameraListCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1ac853b9f8da070d22b1cfe2680f7785d3)& **callback** - 

**Returns**

&emsp;[CameraListHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1aeb7f6082a8f08509e8c45f23768a5a64) - 

### unsubscribe_camera_list() {#classmavsdk_1_1_camera_1a686168126d1d9b7905f4b9c50504598d}
```cpp
void mavsdk::Camera::unsubscribe_camera_list(CameraListHandle handle)
```


Unsubscribe from subscribe_camera_list.


**Parameters**

* [CameraListHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1aeb7f6082a8f08509e8c45f23768a5a64) **handle** - 

### camera_list() {#classmavsdk_1_1_camera_1ac6514ecf820f819076663401e8527f2d}
```cpp
CameraList mavsdk::Camera::camera_list() const
```


Poll for '[CameraList](structmavsdk_1_1_camera_1_1_camera_list.md)' (blocking).


**Returns**

&emsp;[CameraList](structmavsdk_1_1_camera_1_1_camera_list.md) - One [CameraList](structmavsdk_1_1_camera_1_1_camera_list.md) update.

### subscribe_mode() {#classmavsdk_1_1_camera_1a1963bfcfcc8c9e96451648f96fd618cd}
```cpp
ModeHandle mavsdk::Camera::subscribe_mode(const ModeCallback &callback)
```


Subscribe to camera mode updates.


**Parameters**

* const [ModeCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1af6649645c4056d797b42de9418dc4226)& **callback** - 

**Returns**

&emsp;[ModeHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a71c4937ed9dcc1766561370cfe0f523f) - 

### unsubscribe_mode() {#classmavsdk_1_1_camera_1ab9437b41565d8957050e09cf4143f5d1}
```cpp
void mavsdk::Camera::unsubscribe_mode(ModeHandle handle)
```


Unsubscribe from subscribe_mode.


**Parameters**

* [ModeHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a71c4937ed9dcc1766561370cfe0f523f) **handle** - 

### get_mode() {#classmavsdk_1_1_camera_1a5f6377a669bfb8ad4c0efb310ef9bfd2}
```cpp
std::pair< Result, Camera::Mode > mavsdk::Camera::get_mode(int32_t component_id) const
```


Get camera mode.

This function is blocking.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::Mode](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a02bb5ce37d125ba4c65d43f172cc2d65) > - Result of request.

### subscribe_video_stream_info() {#classmavsdk_1_1_camera_1a4dcf0ebeb1f9f5e5b2b71f59c9b7eeb9}
```cpp
VideoStreamInfoHandle mavsdk::Camera::subscribe_video_stream_info(const VideoStreamInfoCallback &callback)
```


Subscribe to video stream info updates.


**Parameters**

* const [VideoStreamInfoCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a5c9669b2652e37e067c6c5c300c9409b)& **callback** - 

**Returns**

&emsp;[VideoStreamInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a093e5b2652791c703534f38ca329b198) - 

### unsubscribe_video_stream_info() {#classmavsdk_1_1_camera_1abc370783800fb90dda20d2843f990036}
```cpp
void mavsdk::Camera::unsubscribe_video_stream_info(VideoStreamInfoHandle handle)
```


Unsubscribe from subscribe_video_stream_info.


**Parameters**

* [VideoStreamInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a093e5b2652791c703534f38ca329b198) **handle** - 

### get_video_stream_info() {#classmavsdk_1_1_camera_1a08b199c0a0cceb37f55a457d56a6797f}
```cpp
std::pair< Result, Camera::VideoStreamInfo > mavsdk::Camera::get_video_stream_info(int32_t component_id) const
```


Get video stream info.

This function is blocking.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::VideoStreamInfo](structmavsdk_1_1_camera_1_1_video_stream_info.md) > - Result of request.

### subscribe_capture_info() {#classmavsdk_1_1_camera_1a8822ab1d802984a1acc9b53cd9810e8f}
```cpp
CaptureInfoHandle mavsdk::Camera::subscribe_capture_info(const CaptureInfoCallback &callback)
```


Subscribe to capture info updates.


**Parameters**

* const [CaptureInfoCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a9bf5c20ea0b03ab019057829d1a3441e)& **callback** - 

**Returns**

&emsp;[CaptureInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a789f1fccde8eb44bc07694751b3a1b40) - 

### unsubscribe_capture_info() {#classmavsdk_1_1_camera_1a9b7f729dfa30a6cb0d041533bd272793}
```cpp
void mavsdk::Camera::unsubscribe_capture_info(CaptureInfoHandle handle)
```


Unsubscribe from subscribe_capture_info.


**Parameters**

* [CaptureInfoHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a789f1fccde8eb44bc07694751b3a1b40) **handle** - 

### subscribe_storage() {#classmavsdk_1_1_camera_1a3b2c8f2f9512ceb6d456f35e1bf7777f}
```cpp
StorageHandle mavsdk::Camera::subscribe_storage(const StorageCallback &callback)
```


Subscribe to camera's storage status updates.


**Parameters**

* const [StorageCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a7811502147e261b81f7f4ea2422fad03)& **callback** - 

**Returns**

&emsp;[StorageHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a4707b885d81143f3cb24ca956c1f91c9) - 

### unsubscribe_storage() {#classmavsdk_1_1_camera_1a925cab3ea7e34ff2ed5a698598c5edd3}
```cpp
void mavsdk::Camera::unsubscribe_storage(StorageHandle handle)
```


Unsubscribe from subscribe_storage.


**Parameters**

* [StorageHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a4707b885d81143f3cb24ca956c1f91c9) **handle** - 

### get_storage() {#classmavsdk_1_1_camera_1a2e35679fe372bb575a6b3b84dfc82c72}
```cpp
std::pair< Result, Camera::Storage > mavsdk::Camera::get_storage(int32_t component_id) const
```


Get camera's storage status.

This function is blocking.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::Storage](structmavsdk_1_1_camera_1_1_storage.md) > - Result of request.

### subscribe_current_settings() {#classmavsdk_1_1_camera_1a906c431b63dde866b691ab4c839858ff}
```cpp
CurrentSettingsHandle mavsdk::Camera::subscribe_current_settings(const CurrentSettingsCallback &callback)
```


Get the list of current camera settings.


**Parameters**

* const [CurrentSettingsCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1ac12104378b90908410ffe100e9fd264e)& **callback** - 

**Returns**

&emsp;[CurrentSettingsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1afa537975f7985f7f7697dc7b4477ccd9) - 

### unsubscribe_current_settings() {#classmavsdk_1_1_camera_1a117e76672b39c9cd751fa64a42741c17}
```cpp
void mavsdk::Camera::unsubscribe_current_settings(CurrentSettingsHandle handle)
```


Unsubscribe from subscribe_current_settings.


**Parameters**

* [CurrentSettingsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1afa537975f7985f7f7697dc7b4477ccd9) **handle** - 

### get_current_settings() {#classmavsdk_1_1_camera_1a980512b264c8aff8584a12a749d00382}
```cpp
std::pair< Result, std::vector< Camera::Setting > > mavsdk::Camera::get_current_settings(int32_t component_id) const
```


Get current settings.

This function is blocking.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [Camera::Setting](structmavsdk_1_1_camera_1_1_setting.md) > > - Result of request.

### subscribe_possible_setting_options() {#classmavsdk_1_1_camera_1aef7c545aaa05e36b212255120557967b}
```cpp
PossibleSettingOptionsHandle mavsdk::Camera::subscribe_possible_setting_options(const PossibleSettingOptionsCallback &callback)
```


Get the list of settings that can be changed.


**Parameters**

* const [PossibleSettingOptionsCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a7eec62e8cdab1a2a7e787bbfeb5205e2)& **callback** - 

**Returns**

&emsp;[PossibleSettingOptionsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8ebb2da9f006cff11293e7e5a20d2f82) - 

### unsubscribe_possible_setting_options() {#classmavsdk_1_1_camera_1a191cc00bb6b1c834fc93b19f4bff1c29}
```cpp
void mavsdk::Camera::unsubscribe_possible_setting_options(PossibleSettingOptionsHandle handle)
```


Unsubscribe from subscribe_possible_setting_options.


**Parameters**

* [PossibleSettingOptionsHandle](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8ebb2da9f006cff11293e7e5a20d2f82) **handle** - 

### get_possible_setting_options() {#classmavsdk_1_1_camera_1a7c9aa933b7c93a5178cd9e478ee72170}
```cpp
std::pair< Result, std::vector< Camera::SettingOptions > > mavsdk::Camera::get_possible_setting_options(int32_t component_id) const
```


Get possible setting options.

This function is blocking.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), std::vector< [Camera::SettingOptions](structmavsdk_1_1_camera_1_1_setting_options.md) > > - Result of request.

### set_setting_async() {#classmavsdk_1_1_camera_1a5861022cf053c2c347c994e0d0ae62a4}
```cpp
void mavsdk::Camera::set_setting_async(int32_t component_id, Setting setting, const ResultCallback callback)
```


Set a setting to some value.

Only setting_id of setting and option_id of option needs to be set.


This function is non-blocking. See 'set_setting' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [Setting](structmavsdk_1_1_camera_1_1_setting.md) **setting** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### set_setting() {#classmavsdk_1_1_camera_1ab34650789b55900716dd8ea3d81d5a05}
```cpp
Result mavsdk::Camera::set_setting(int32_t component_id, Setting setting) const
```


Set a setting to some value.

Only setting_id of setting and option_id of option needs to be set.


This function is blocking. See 'set_setting_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [Setting](structmavsdk_1_1_camera_1_1_setting.md) **setting** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### get_setting_async() {#classmavsdk_1_1_camera_1a3364fd9b2f48388148620471bc598d85}
```cpp
void mavsdk::Camera::get_setting_async(int32_t component_id, Setting setting, const GetSettingCallback callback)
```


Get a setting.

Only setting_id of setting needs to be set.


This function is non-blocking. See 'get_setting' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [Setting](structmavsdk_1_1_camera_1_1_setting.md) **setting** - 
* const [GetSettingCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1ac233f5688f0b7f1e712bb31dfaeadd85) **callback** - 

### get_setting() {#classmavsdk_1_1_camera_1a0e8376bf9c3e4d9367b9e296e808529a}
```cpp
std::pair< Result, Camera::Setting > mavsdk::Camera::get_setting(int32_t component_id, Setting setting) const
```


Get a setting.

Only setting_id of setting needs to be set.


This function is blocking. See 'get_setting_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* [Setting](structmavsdk_1_1_camera_1_1_setting.md) **setting** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf), [Camera::Setting](structmavsdk_1_1_camera_1_1_setting.md) > - Result of request.

### format_storage_async() {#classmavsdk_1_1_camera_1a5ca1864fd62f357187290f753a3e7b89}
```cpp
void mavsdk::Camera::format_storage_async(int32_t component_id, int32_t storage_id, const ResultCallback callback)
```


Format storage (e.g. SD card) in camera.

This will delete all content of the camera storage!


This function is non-blocking. See 'format_storage' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* int32_t **storage_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### format_storage() {#classmavsdk_1_1_camera_1ad239fe6b8dad6783d1f53c3e8e8212ae}
```cpp
Result mavsdk::Camera::format_storage(int32_t component_id, int32_t storage_id) const
```


Format storage (e.g. SD card) in camera.

This will delete all content of the camera storage!


This function is blocking. See 'format_storage_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* int32_t **storage_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### reset_settings_async() {#classmavsdk_1_1_camera_1a6b558f5e8e6298584883a5622dd0d5eb}
```cpp
void mavsdk::Camera::reset_settings_async(int32_t component_id, const ResultCallback callback)
```


Reset all settings in camera.

This will reset all camera settings to default value


This function is non-blocking. See 'reset_settings' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### reset_settings() {#classmavsdk_1_1_camera_1a6b5ba6cd713705848585d2808ceb53fc}
```cpp
Result mavsdk::Camera::reset_settings(int32_t component_id) const
```


Reset all settings in camera.

This will reset all camera settings to default value


This function is blocking. See 'reset_settings_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### zoom_in_start_async() {#classmavsdk_1_1_camera_1a6ff5149c53d8acc6c4a5f17081b88a15}
```cpp
void mavsdk::Camera::zoom_in_start_async(int32_t component_id, const ResultCallback callback)
```


Start zooming in.

This function is non-blocking. See 'zoom_in_start' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### zoom_in_start() {#classmavsdk_1_1_camera_1a71b6535b10a5cf00baf451e4b2e0b6c4}
```cpp
Result mavsdk::Camera::zoom_in_start(int32_t component_id) const
```


Start zooming in.

This function is blocking. See 'zoom_in_start_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### zoom_out_start_async() {#classmavsdk_1_1_camera_1a088b21cec49f800ab666d7409a299937}
```cpp
void mavsdk::Camera::zoom_out_start_async(int32_t component_id, const ResultCallback callback)
```


Start zooming out.

This function is non-blocking. See 'zoom_out_start' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### zoom_out_start() {#classmavsdk_1_1_camera_1a72456e28c67ba451921cedf7f11f5954}
```cpp
Result mavsdk::Camera::zoom_out_start(int32_t component_id) const
```


Start zooming out.

This function is blocking. See 'zoom_out_start_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### zoom_stop_async() {#classmavsdk_1_1_camera_1a2a30851420c2d2ceb102692cf917327b}
```cpp
void mavsdk::Camera::zoom_stop_async(int32_t component_id, const ResultCallback callback)
```


Stop zooming.

This function is non-blocking. See 'zoom_stop' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### zoom_stop() {#classmavsdk_1_1_camera_1a30fee70ce5bb42071fcce27841a8a01a}
```cpp
Result mavsdk::Camera::zoom_stop(int32_t component_id) const
```


Stop zooming.

This function is blocking. See 'zoom_stop_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### zoom_range_async() {#classmavsdk_1_1_camera_1acce728b12b8e767ee9cc55afb1d4d6ee}
```cpp
void mavsdk::Camera::zoom_range_async(int32_t component_id, float range, const ResultCallback callback)
```


Zoom to value as proportion of full camera range (percentage between 0.0 and 100.0).

This function is non-blocking. See 'zoom_range' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **range** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### zoom_range() {#classmavsdk_1_1_camera_1ac9a05ffb7a8520c9f9dcb29919878d11}
```cpp
Result mavsdk::Camera::zoom_range(int32_t component_id, float range) const
```


Zoom to value as proportion of full camera range (percentage between 0.0 and 100.0).

This function is blocking. See 'zoom_range_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **range** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### track_point_async() {#classmavsdk_1_1_camera_1ac83433124165ec1b3fbe2629a9939617}
```cpp
void mavsdk::Camera::track_point_async(int32_t component_id, float point_x, float point_y, float radius, const ResultCallback callback)
```


Track point.

This function is non-blocking. See 'track_point' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **point_x** - 
* float **point_y** - 
* float **radius** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### track_point() {#classmavsdk_1_1_camera_1aff70a9533d377b792ede73d1d3bec2fd}
```cpp
Result mavsdk::Camera::track_point(int32_t component_id, float point_x, float point_y, float radius) const
```


Track point.

This function is blocking. See 'track_point_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **point_x** - 
* float **point_y** - 
* float **radius** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### track_rectangle_async() {#classmavsdk_1_1_camera_1a4fcec80674d2de32934c67a3544130f6}
```cpp
void mavsdk::Camera::track_rectangle_async(int32_t component_id, float top_left_x, float top_left_y, float bottom_right_x, float bottom_right_y, const ResultCallback callback)
```


Track rectangle.

This function is non-blocking. See 'track_rectangle' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **top_left_x** - 
* float **top_left_y** - 
* float **bottom_right_x** - 
* float **bottom_right_y** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### track_rectangle() {#classmavsdk_1_1_camera_1a32c338bc723337dcf4229bac94073d03}
```cpp
Result mavsdk::Camera::track_rectangle(int32_t component_id, float top_left_x, float top_left_y, float bottom_right_x, float bottom_right_y) const
```


Track rectangle.

This function is blocking. See 'track_rectangle_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **top_left_x** - 
* float **top_left_y** - 
* float **bottom_right_x** - 
* float **bottom_right_y** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### track_stop_async() {#classmavsdk_1_1_camera_1a7b1c720a0a346516af8c1bc3e49210cc}
```cpp
void mavsdk::Camera::track_stop_async(int32_t component_id, const ResultCallback callback)
```


Stop tracking.

This function is non-blocking. See 'track_stop' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### track_stop() {#classmavsdk_1_1_camera_1a1f4a0c1c2cfcef55cdab94f3d726766f}
```cpp
Result mavsdk::Camera::track_stop(int32_t component_id) const
```


Stop tracking.

This function is blocking. See 'track_stop_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### focus_in_start_async() {#classmavsdk_1_1_camera_1ad40616613df551c00a78099e7790b880}
```cpp
void mavsdk::Camera::focus_in_start_async(int32_t component_id, const ResultCallback callback)
```


Start focusing in.

This function is non-blocking. See 'focus_in_start' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### focus_in_start() {#classmavsdk_1_1_camera_1a8be24bc7d5c21b94b388bc1eeed2e8fe}
```cpp
Result mavsdk::Camera::focus_in_start(int32_t component_id) const
```


Start focusing in.

This function is blocking. See 'focus_in_start_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### focus_out_start_async() {#classmavsdk_1_1_camera_1ad7917d952b7d0e43dba631a152575765}
```cpp
void mavsdk::Camera::focus_out_start_async(int32_t component_id, const ResultCallback callback)
```


Start focusing out.

This function is non-blocking. See 'focus_out_start' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### focus_out_start() {#classmavsdk_1_1_camera_1a25b5c34491cb17f40e7c9c4461c37723}
```cpp
Result mavsdk::Camera::focus_out_start(int32_t component_id) const
```


Start focusing out.

This function is blocking. See 'focus_out_start_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### focus_stop_async() {#classmavsdk_1_1_camera_1a562ad9fa8f29b359785069a9dc5b8de2}
```cpp
void mavsdk::Camera::focus_stop_async(int32_t component_id, const ResultCallback callback)
```


Stop focus.

This function is non-blocking. See 'focus_stop' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### focus_stop() {#classmavsdk_1_1_camera_1aed5b395a57b14da2d15395dfa2e56600}
```cpp
Result mavsdk::Camera::focus_stop(int32_t component_id) const
```


Stop focus.

This function is blocking. See 'focus_stop_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### focus_range_async() {#classmavsdk_1_1_camera_1a44cebbb36a9bc9c203dc0233f8b52f7b}
```cpp
void mavsdk::Camera::focus_range_async(int32_t component_id, float range, const ResultCallback callback)
```


Focus with range value of full range (value between 0.0 and 100.0).

This function is non-blocking. See 'focus_range' for the blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **range** - 
* const [ResultCallback](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a8d6d59cd8d0a3584ef60b16255b6301f) **callback** - 

### focus_range() {#classmavsdk_1_1_camera_1aec7d7a0ed5fa2ba84124f9f363e8d94a}
```cpp
Result mavsdk::Camera::focus_range(int32_t component_id, float range) const
```


Focus with range value of full range (value between 0.0 and 100.0).

This function is blocking. See 'focus_range_async' for the non-blocking counterpart.

**Parameters**

* int32_t **component_id** - 
* float **range** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera.md#classmavsdk_1_1_camera_1a2a84df3938372f4f302576227b308bcf) - Result of request.

### operator=() {#classmavsdk_1_1_camera_1a358026db44ff9a10cf14a166d8f1da78}
```cpp
const Camera & mavsdk::Camera::operator=(const Camera &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Camera](classmavsdk_1_1_camera.md)&  - 

**Returns**

&emsp;const [Camera](classmavsdk_1_1_camera.md) & - 