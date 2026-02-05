# mavsdk::CameraServer Class Reference
`#include: camera_server.h`

----


Provides handling of camera interface. 


## Data Structures


struct [CaptureInfo](structmavsdk_1_1_camera_server_1_1_capture_info.md)

struct [CaptureStatus](structmavsdk_1_1_camera_server_1_1_capture_status.md)

struct [Information](structmavsdk_1_1_camera_server_1_1_information.md)

struct [Position](structmavsdk_1_1_camera_server_1_1_position.md)

struct [Quaternion](structmavsdk_1_1_camera_server_1_1_quaternion.md)

struct [StorageInformation](structmavsdk_1_1_camera_server_1_1_storage_information.md)

struct [TrackPoint](structmavsdk_1_1_camera_server_1_1_track_point.md)

struct [TrackRectangle](structmavsdk_1_1_camera_server_1_1_track_rectangle.md)

struct [VideoStreaming](structmavsdk_1_1_camera_server_1_1_video_streaming.md)

## Public Types


Type | Description
--- | ---
enum [CameraFeedback](#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) | Possible feedback results for camera respond command.
enum [Mode](#classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155) | [Camera](classmavsdk_1_1_camera.md) mode type.
enum [Result](#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | Possible results returned for action requests.
std::function< void([Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d))> [ResultCallback](#classmavsdk_1_1_camera_server_1a06aae7383f055f54a659cfce45432207) | Callback type for asynchronous [CameraServer](classmavsdk_1_1_camera_server.md) calls.
std::function< void(int32_t)> [TakePhotoCallback](#classmavsdk_1_1_camera_server_1a947529ac03c8dfac60c7c798db60a2d0) | Callback type for subscribe_take_photo.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [TakePhotoHandle](#classmavsdk_1_1_camera_server_1a2c79367a301d3b3093963222ac2cccbe) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_take_photo.
std::function< void(int32_t)> [StartVideoCallback](#classmavsdk_1_1_camera_server_1a01810f7bdc3ab4a59c633c7e11d02d0f) | Callback type for subscribe_start_video.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [StartVideoHandle](#classmavsdk_1_1_camera_server_1a2ec6dce94dfca9b5d102306df53072a1) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_start_video.
std::function< void(int32_t)> [StopVideoCallback](#classmavsdk_1_1_camera_server_1a37a4bd54432b5ffa866edd6cd9310c7f) | Callback type for subscribe_stop_video.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [StopVideoHandle](#classmavsdk_1_1_camera_server_1a7ca737d29d078f98770b7e2de6a93e8c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_stop_video.
std::function< void(int32_t)> [StartVideoStreamingCallback](#classmavsdk_1_1_camera_server_1ae67fa13502562dea8c3e955213cd14b9) | Callback type for subscribe_start_video_streaming.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [StartVideoStreamingHandle](#classmavsdk_1_1_camera_server_1ae6c608c1345fa1e1111c7bb2ed56cb85) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_start_video_streaming.
std::function< void(int32_t)> [StopVideoStreamingCallback](#classmavsdk_1_1_camera_server_1a111ed05ec69596127b9ac197c8fa2940) | Callback type for subscribe_stop_video_streaming.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [StopVideoStreamingHandle](#classmavsdk_1_1_camera_server_1a2b1a56ba4c349a59c3c62fccc5d249ea) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_stop_video_streaming.
std::function< void([Mode](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155))> [SetModeCallback](#classmavsdk_1_1_camera_server_1a0e023e2c618adcb2ad75527353dc8acd) | Callback type for subscribe_set_mode.
[Handle](classmavsdk_1_1_handle.md)< [Mode](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155) > [SetModeHandle](#classmavsdk_1_1_camera_server_1ae73af6f5c0e21e1ad89b75706f2d782c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_set_mode.
std::function< void(int32_t)> [StorageInformationCallback](#classmavsdk_1_1_camera_server_1ac5ad2dd65155ce1e7a6c0ebd2f0d20cf) | Callback type for subscribe_storage_information.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [StorageInformationHandle](#classmavsdk_1_1_camera_server_1a20edc2d4386999873dcbf74eafba6966) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_storage_information.
std::function< void(int32_t)> [CaptureStatusCallback](#classmavsdk_1_1_camera_server_1af288de066938d13c02b4f1364cd66403) | Callback type for subscribe_capture_status.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [CaptureStatusHandle](#classmavsdk_1_1_camera_server_1af67bf159a7f2173364182bff2cdd56bd) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_capture_status.
std::function< void(int32_t)> [FormatStorageCallback](#classmavsdk_1_1_camera_server_1a6242b427d4f29202ef931e8c6a7bdc46) | Callback type for subscribe_format_storage.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [FormatStorageHandle](#classmavsdk_1_1_camera_server_1a71b5531c91f7fba1c74296e55ecd3f45) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_format_storage.
std::function< void(int32_t)> [ResetSettingsCallback](#classmavsdk_1_1_camera_server_1ac88b7c640c76e5ff2bdc9afb243ceac2) | Callback type for subscribe_reset_settings.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [ResetSettingsHandle](#classmavsdk_1_1_camera_server_1aec973cf0e67863272e63e12902094f25) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_reset_settings.
std::function< void(int32_t)> [ZoomInStartCallback](#classmavsdk_1_1_camera_server_1a7b6c98f6051a80539ef891f65311aa71) | Callback type for subscribe_zoom_in_start.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [ZoomInStartHandle](#classmavsdk_1_1_camera_server_1a3e67fff49fcc4557662826305531e879) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_in_start.
std::function< void(int32_t)> [ZoomOutStartCallback](#classmavsdk_1_1_camera_server_1aa07c374347ed42ec8d2b6727c0d60f5b) | Callback type for subscribe_zoom_out_start.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [ZoomOutStartHandle](#classmavsdk_1_1_camera_server_1afdfa5f070466f4bcd52af5af989d5226) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_out_start.
std::function< void(int32_t)> [ZoomStopCallback](#classmavsdk_1_1_camera_server_1a6fc78f44226f6a326afb935a1dca5c86) | Callback type for subscribe_zoom_stop.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [ZoomStopHandle](#classmavsdk_1_1_camera_server_1a8bb9a99dfb5089332be6241230d54d34) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_stop.
std::function< void(float)> [ZoomRangeCallback](#classmavsdk_1_1_camera_server_1aa23f7ca9c7bf5eee8a12846743bed3a2) | Callback type for subscribe_zoom_range.
[Handle](classmavsdk_1_1_handle.md)< float > [ZoomRangeHandle](#classmavsdk_1_1_camera_server_1a37067e1804927f01921079dfa1e133c2) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_range.
std::function< void([TrackPoint](structmavsdk_1_1_camera_server_1_1_track_point.md))> [TrackingPointCommandCallback](#classmavsdk_1_1_camera_server_1afcb7cf46d38b362562653e9648191ef4) | Callback type for subscribe_tracking_point_command.
[Handle](classmavsdk_1_1_handle.md)< [TrackPoint](structmavsdk_1_1_camera_server_1_1_track_point.md) > [TrackingPointCommandHandle](#classmavsdk_1_1_camera_server_1a38c4148c1c717351865787b1529e9617) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_tracking_point_command.
std::function< void([TrackRectangle](structmavsdk_1_1_camera_server_1_1_track_rectangle.md))> [TrackingRectangleCommandCallback](#classmavsdk_1_1_camera_server_1aa7541523c37bb4b24246e40deb0e35a6) | Callback type for subscribe_tracking_rectangle_command.
[Handle](classmavsdk_1_1_handle.md)< [TrackRectangle](structmavsdk_1_1_camera_server_1_1_track_rectangle.md) > [TrackingRectangleCommandHandle](#classmavsdk_1_1_camera_server_1a2e6b6f5ec287bec05357cc9dc9259566) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_tracking_rectangle_command.
std::function< void(int32_t)> [TrackingOffCommandCallback](#classmavsdk_1_1_camera_server_1ae46621448c7e035ec4da57338d898109) | Callback type for subscribe_tracking_off_command.
[Handle](classmavsdk_1_1_handle.md)< int32_t > [TrackingOffCommandHandle](#classmavsdk_1_1_camera_server_1a5948e4523cdeff02791277df0d059557) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_tracking_off_command.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [CameraServer](#classmavsdk_1_1_camera_server_1ac8575ed408d1c5f317aeec458402ccc3) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~CameraServer](#classmavsdk_1_1_camera_server_1a6e387b798274d5e4dadb01499d91daff) () override | Destructor (internal use only).
&nbsp; | [CameraServer](#classmavsdk_1_1_camera_server_1acf1f1fcd248cf4942cd061a2345c89b9) (const [CameraServer](classmavsdk_1_1_camera_server.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [set_information](#classmavsdk_1_1_camera_server_1a81b6b957f11cc8764ea5bddfae88fc64) ([Information](structmavsdk_1_1_camera_server_1_1_information.md) information)const | Sets the camera information. This must be called as soon as the camera server is created.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [set_video_streaming](#classmavsdk_1_1_camera_server_1a8de695275fc2bee579005b4638250bb1) ([VideoStreaming](structmavsdk_1_1_camera_server_1_1_video_streaming.md) video_streaming)const | Sets video streaming settings.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [set_in_progress](#classmavsdk_1_1_camera_server_1a9f4c5012412b889017c9f47d01044651) (bool in_progress)const | Sets image capture in progress status flags. This should be set to true when the camera is busy taking a photo and false when it is done.
[TakePhotoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2c79367a301d3b3093963222ac2cccbe) | [subscribe_take_photo](#classmavsdk_1_1_camera_server_1a8a982fbf278d1521f6f80a7527f5f85c) (const [TakePhotoCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a947529ac03c8dfac60c7c798db60a2d0) & callback) | Subscribe to image capture requests. Each request received should respond to using RespondTakePhoto.
void | [unsubscribe_take_photo](#classmavsdk_1_1_camera_server_1a9b6c0b59f8990b4259b9a2fa134228c9) ([TakePhotoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2c79367a301d3b3093963222ac2cccbe) handle) | Unsubscribe from subscribe_take_photo.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_take_photo](#classmavsdk_1_1_camera_server_1aef1d7b26221a1e98b9d6ff099a6f1299) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) take_photo_feedback, [CaptureInfo](structmavsdk_1_1_camera_server_1_1_capture_info.md) capture_info)const | Respond to an image capture request from SubscribeTakePhoto.
[StartVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2ec6dce94dfca9b5d102306df53072a1) | [subscribe_start_video](#classmavsdk_1_1_camera_server_1a11e48131fac66f1f54a7f433a18cb000) (const [StartVideoCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a01810f7bdc3ab4a59c633c7e11d02d0f) & callback) | Subscribe to start video requests. Each request received should respond to using RespondStartVideo.
void | [unsubscribe_start_video](#classmavsdk_1_1_camera_server_1a408195cfa869e1146c054320c8bf74c2) ([StartVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2ec6dce94dfca9b5d102306df53072a1) handle) | Unsubscribe from subscribe_start_video.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_start_video](#classmavsdk_1_1_camera_server_1aae8ca562aed415a803e5cf116d3860ab) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) start_video_feedback)const | Subscribe to stop video requests. Each request received should respond using StopVideoResponse.
[StopVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a7ca737d29d078f98770b7e2de6a93e8c) | [subscribe_stop_video](#classmavsdk_1_1_camera_server_1a1f9bf41790c13bfac00e83a07a0aabc2) (const [StopVideoCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a37a4bd54432b5ffa866edd6cd9310c7f) & callback) | Subscribe to stop video requests. Each request received should response to using RespondStopVideo.
void | [unsubscribe_stop_video](#classmavsdk_1_1_camera_server_1a46087c9b9131d5b56c937a7a0f9b82cd) ([StopVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a7ca737d29d078f98770b7e2de6a93e8c) handle) | Unsubscribe from subscribe_stop_video.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_stop_video](#classmavsdk_1_1_camera_server_1a72504e0a97587dbd4e92cbdf2aedd36f) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) stop_video_feedback)const | Respond to stop video request from SubscribeStopVideo.
[StartVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae6c608c1345fa1e1111c7bb2ed56cb85) | [subscribe_start_video_streaming](#classmavsdk_1_1_camera_server_1a7528dfe116272cdef1584c0709e9d554) (const [StartVideoStreamingCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae67fa13502562dea8c3e955213cd14b9) & callback) | Subscribe to start video streaming requests. Each request received should response to using RespondStartVideoStreaming.
void | [unsubscribe_start_video_streaming](#classmavsdk_1_1_camera_server_1a0e85bc6405cb04be6d14e385b5c9b6f0) ([StartVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae6c608c1345fa1e1111c7bb2ed56cb85) handle) | Unsubscribe from subscribe_start_video_streaming.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_start_video_streaming](#classmavsdk_1_1_camera_server_1a32bf585f316c0990fd3c792930eb430c) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) start_video_streaming_feedback)const | Respond to start video streaming from SubscribeStartVideoStreaming.
[StopVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2b1a56ba4c349a59c3c62fccc5d249ea) | [subscribe_stop_video_streaming](#classmavsdk_1_1_camera_server_1aed9dd6e4e5d4b9aa24254933cfdb68d6) (const [StopVideoStreamingCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a111ed05ec69596127b9ac197c8fa2940) & callback) | Subscribe to stop video streaming requests. Each request received should response to using RespondStopVideoStreaming.
void | [unsubscribe_stop_video_streaming](#classmavsdk_1_1_camera_server_1a0ae66e34730d2a9ebea39bff190b432c) ([StopVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2b1a56ba4c349a59c3c62fccc5d249ea) handle) | Unsubscribe from subscribe_stop_video_streaming.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_stop_video_streaming](#classmavsdk_1_1_camera_server_1a8a452852fd4ba6a97ab7613dfbe3d3d5) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) stop_video_streaming_feedback)const | Respond to stop video streaming from SubscribeStopVideoStreaming.
[SetModeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae73af6f5c0e21e1ad89b75706f2d782c) | [subscribe_set_mode](#classmavsdk_1_1_camera_server_1a7aefc230a1c9688c6ffce0563970363d) (const [SetModeCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a0e023e2c618adcb2ad75527353dc8acd) & callback) | Subscribe to set camera mode requests. Each request received should response to using RespondSetMode.
void | [unsubscribe_set_mode](#classmavsdk_1_1_camera_server_1a4ebe9158300707532374492e664911b7) ([SetModeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae73af6f5c0e21e1ad89b75706f2d782c) handle) | Unsubscribe from subscribe_set_mode.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_set_mode](#classmavsdk_1_1_camera_server_1add075055b138c38ef17502ef049b2508) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) set_mode_feedback)const | Respond to set camera mode from SubscribeSetMode.
[StorageInformationHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a20edc2d4386999873dcbf74eafba6966) | [subscribe_storage_information](#classmavsdk_1_1_camera_server_1a05fbd42c88962fccad1b2a31810dade7) (const [StorageInformationCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ac5ad2dd65155ce1e7a6c0ebd2f0d20cf) & callback) | Subscribe to camera storage information requests. Each request received should response to using RespondStorageInformation.
void | [unsubscribe_storage_information](#classmavsdk_1_1_camera_server_1afe6fbc5c8f5f39f204c2d6c57ac5161d) ([StorageInformationHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a20edc2d4386999873dcbf74eafba6966) handle) | Unsubscribe from subscribe_storage_information.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_storage_information](#classmavsdk_1_1_camera_server_1a6dee290ca68414ee8ff9744e261b5ac3) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) storage_information_feedback, [StorageInformation](structmavsdk_1_1_camera_server_1_1_storage_information.md) storage_information)const | Respond to camera storage information from SubscribeStorageInformation.
[CaptureStatusHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1af67bf159a7f2173364182bff2cdd56bd) | [subscribe_capture_status](#classmavsdk_1_1_camera_server_1a783ac1e37cf9be240e660fe3ba4eced9) (const [CaptureStatusCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1af288de066938d13c02b4f1364cd66403) & callback) | Subscribe to camera capture status requests. Each request received should response to using RespondCaptureStatus.
void | [unsubscribe_capture_status](#classmavsdk_1_1_camera_server_1af26523420d48b629f1758da304c9d156) ([CaptureStatusHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1af67bf159a7f2173364182bff2cdd56bd) handle) | Unsubscribe from subscribe_capture_status.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_capture_status](#classmavsdk_1_1_camera_server_1ae091608e278807026e114d257d25407a) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) capture_status_feedback, [CaptureStatus](structmavsdk_1_1_camera_server_1_1_capture_status.md) capture_status)const | Respond to camera capture status from SubscribeCaptureStatus.
[FormatStorageHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a71b5531c91f7fba1c74296e55ecd3f45) | [subscribe_format_storage](#classmavsdk_1_1_camera_server_1ab788a20175966f8d5ddab973f176a3b4) (const [FormatStorageCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a6242b427d4f29202ef931e8c6a7bdc46) & callback) | Subscribe to format storage requests. Each request received should response to using RespondFormatStorage.
void | [unsubscribe_format_storage](#classmavsdk_1_1_camera_server_1a9b51f14ffa8c65518d59a5fed406be0c) ([FormatStorageHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a71b5531c91f7fba1c74296e55ecd3f45) handle) | Unsubscribe from subscribe_format_storage.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_format_storage](#classmavsdk_1_1_camera_server_1a18bb9a800df91f74efa0a35eb5218a14) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) format_storage_feedback)const | Respond to format storage from SubscribeFormatStorage.
[ResetSettingsHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aec973cf0e67863272e63e12902094f25) | [subscribe_reset_settings](#classmavsdk_1_1_camera_server_1ab1022684bc503e1d6294495de8c68165) (const [ResetSettingsCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ac88b7c640c76e5ff2bdc9afb243ceac2) & callback) | Subscribe to reset settings requests. Each request received should response to using RespondResetSettings.
void | [unsubscribe_reset_settings](#classmavsdk_1_1_camera_server_1a09c70966fa6f98215c11d8257d1b9608) ([ResetSettingsHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aec973cf0e67863272e63e12902094f25) handle) | Unsubscribe from subscribe_reset_settings.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_reset_settings](#classmavsdk_1_1_camera_server_1a1d7b2793d899c9052038f4e9569eb32b) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) reset_settings_feedback)const | Respond to reset settings from SubscribeResetSettings.
[ZoomInStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a3e67fff49fcc4557662826305531e879) | [subscribe_zoom_in_start](#classmavsdk_1_1_camera_server_1a4d5f9d60ad7e19cea32b4af040727d46) (const [ZoomInStartCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a7b6c98f6051a80539ef891f65311aa71) & callback) | Subscribe to zoom in start command.
void | [unsubscribe_zoom_in_start](#classmavsdk_1_1_camera_server_1a5ab3f69f82b3b848decf5067c26a66fa) ([ZoomInStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a3e67fff49fcc4557662826305531e879) handle) | Unsubscribe from subscribe_zoom_in_start.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_zoom_in_start](#classmavsdk_1_1_camera_server_1a059b221e9dc918ea4a3c17371ae8833f) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) zoom_in_start_feedback)const | Respond to zoom in start.
[ZoomOutStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1afdfa5f070466f4bcd52af5af989d5226) | [subscribe_zoom_out_start](#classmavsdk_1_1_camera_server_1a46244b254bce6fd53ca1c0db3c0966a1) (const [ZoomOutStartCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa07c374347ed42ec8d2b6727c0d60f5b) & callback) | Subscribe to zoom out start command.
void | [unsubscribe_zoom_out_start](#classmavsdk_1_1_camera_server_1a1aa244c9eeaef416cd67ef2896264959) ([ZoomOutStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1afdfa5f070466f4bcd52af5af989d5226) handle) | Unsubscribe from subscribe_zoom_out_start.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_zoom_out_start](#classmavsdk_1_1_camera_server_1aaf52d7ad70cb10219db015e17a657632) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) zoom_out_start_feedback)const | Respond to zoom out start.
[ZoomStopHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a8bb9a99dfb5089332be6241230d54d34) | [subscribe_zoom_stop](#classmavsdk_1_1_camera_server_1a2be8527911fe1f4dafc7cd87f9be60cd) (const [ZoomStopCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a6fc78f44226f6a326afb935a1dca5c86) & callback) | Subscribe to zoom stop command.
void | [unsubscribe_zoom_stop](#classmavsdk_1_1_camera_server_1aab7d43df6c39266ccdcb80ee6f5621fb) ([ZoomStopHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a8bb9a99dfb5089332be6241230d54d34) handle) | Unsubscribe from subscribe_zoom_stop.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_zoom_stop](#classmavsdk_1_1_camera_server_1a968890afb4b3b33a7a6965dd4b32aa1f) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) zoom_stop_feedback)const | Respond to zoom stop.
[ZoomRangeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a37067e1804927f01921079dfa1e133c2) | [subscribe_zoom_range](#classmavsdk_1_1_camera_server_1a6dc95f635c88749b4e2bbfa07bd5b0fd) (const [ZoomRangeCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa23f7ca9c7bf5eee8a12846743bed3a2) & callback) | Subscribe to zoom range command.
void | [unsubscribe_zoom_range](#classmavsdk_1_1_camera_server_1a9fc5f5daed094bea620011bce222bb88) ([ZoomRangeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a37067e1804927f01921079dfa1e133c2) handle) | Unsubscribe from subscribe_zoom_range.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_zoom_range](#classmavsdk_1_1_camera_server_1a5ea9dd6521f43d91671b0fa1eef6d0f6) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) zoom_range_feedback)const | Respond to zoom range.
void | [set_tracking_rectangle_status](#classmavsdk_1_1_camera_server_1a262b7ec85fee9aa1b83eebd3ffb9abaa) ([TrackRectangle](structmavsdk_1_1_camera_server_1_1_track_rectangle.md) tracked_rectangle)const | Set/update the current rectangle tracking status.
void | [set_tracking_off_status](#classmavsdk_1_1_camera_server_1a22ba37443a3bc69c826261964949a8d4) () const | Set the current tracking status to off.
[TrackingPointCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a38c4148c1c717351865787b1529e9617) | [subscribe_tracking_point_command](#classmavsdk_1_1_camera_server_1a474c49eff4fe7a1b707df2c610197b3d) (const [TrackingPointCommandCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1afcb7cf46d38b362562653e9648191ef4) & callback) | Subscribe to incoming tracking point command.
void | [unsubscribe_tracking_point_command](#classmavsdk_1_1_camera_server_1a8cd6c774f5cebb423cb32782d213f4de) ([TrackingPointCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a38c4148c1c717351865787b1529e9617) handle) | Unsubscribe from subscribe_tracking_point_command.
[TrackingRectangleCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2e6b6f5ec287bec05357cc9dc9259566) | [subscribe_tracking_rectangle_command](#classmavsdk_1_1_camera_server_1a752751ac6064456d0139592697bfd3a8) (const [TrackingRectangleCommandCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa7541523c37bb4b24246e40deb0e35a6) & callback) | Subscribe to incoming tracking rectangle command.
void | [unsubscribe_tracking_rectangle_command](#classmavsdk_1_1_camera_server_1a6abfa99ac94653930aa1e558d7c7beaa) ([TrackingRectangleCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2e6b6f5ec287bec05357cc9dc9259566) handle) | Unsubscribe from subscribe_tracking_rectangle_command.
[TrackingOffCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a5948e4523cdeff02791277df0d059557) | [subscribe_tracking_off_command](#classmavsdk_1_1_camera_server_1ac976e22ee5831c8144a4b917ab615946) (const [TrackingOffCommandCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae46621448c7e035ec4da57338d898109) & callback) | Subscribe to incoming tracking off command.
void | [unsubscribe_tracking_off_command](#classmavsdk_1_1_camera_server_1a13d8edd63c57c53c3d17893dc31e3bc3) ([TrackingOffCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a5948e4523cdeff02791277df0d059557) handle) | Unsubscribe from subscribe_tracking_off_command.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_tracking_point_command](#classmavsdk_1_1_camera_server_1a8749824a1326cc0a7c441fb7c59b9507) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) stop_video_feedback)const | Respond to an incoming tracking point command.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_tracking_rectangle_command](#classmavsdk_1_1_camera_server_1a2de79ab19b5d51e6760eb3173e9c6f77) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) stop_video_feedback)const | Respond to an incoming tracking rectangle command.
[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) | [respond_tracking_off_command](#classmavsdk_1_1_camera_server_1a31e8a3a8996ae48afbacbef38d31e209) ([CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) stop_video_feedback)const | Respond to an incoming tracking off command.
const [CameraServer](classmavsdk_1_1_camera_server.md) & | [operator=](#classmavsdk_1_1_camera_server_1a6b516bbddcbaa2867d1a11be8eda14bd) (const [CameraServer](classmavsdk_1_1_camera_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### CameraServer() {#classmavsdk_1_1_camera_server_1ac8575ed408d1c5f317aeec458402ccc3}
```cpp
mavsdk::CameraServer::CameraServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto camera_server = CameraServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~CameraServer() {#classmavsdk_1_1_camera_server_1a6e387b798274d5e4dadb01499d91daff}
```cpp
mavsdk::CameraServer::~CameraServer() override
```


Destructor (internal use only).


### CameraServer() {#classmavsdk_1_1_camera_server_1acf1f1fcd248cf4942cd061a2345c89b9}
```cpp
mavsdk::CameraServer::CameraServer(const CameraServer &other)
```


Copy constructor.


**Parameters**

* const [CameraServer](classmavsdk_1_1_camera_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_camera_server_1a06aae7383f055f54a659cfce45432207}

```cpp
using mavsdk::CameraServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [CameraServer](classmavsdk_1_1_camera_server.md) calls.


### typedef TakePhotoCallback {#classmavsdk_1_1_camera_server_1a947529ac03c8dfac60c7c798db60a2d0}

```cpp
using mavsdk::CameraServer::TakePhotoCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_take_photo.


### typedef TakePhotoHandle {#classmavsdk_1_1_camera_server_1a2c79367a301d3b3093963222ac2cccbe}

```cpp
using mavsdk::CameraServer::TakePhotoHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_take_photo.


### typedef StartVideoCallback {#classmavsdk_1_1_camera_server_1a01810f7bdc3ab4a59c633c7e11d02d0f}

```cpp
using mavsdk::CameraServer::StartVideoCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_start_video.


### typedef StartVideoHandle {#classmavsdk_1_1_camera_server_1a2ec6dce94dfca9b5d102306df53072a1}

```cpp
using mavsdk::CameraServer::StartVideoHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_start_video.


### typedef StopVideoCallback {#classmavsdk_1_1_camera_server_1a37a4bd54432b5ffa866edd6cd9310c7f}

```cpp
using mavsdk::CameraServer::StopVideoCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_stop_video.


### typedef StopVideoHandle {#classmavsdk_1_1_camera_server_1a7ca737d29d078f98770b7e2de6a93e8c}

```cpp
using mavsdk::CameraServer::StopVideoHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_stop_video.


### typedef StartVideoStreamingCallback {#classmavsdk_1_1_camera_server_1ae67fa13502562dea8c3e955213cd14b9}

```cpp
using mavsdk::CameraServer::StartVideoStreamingCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_start_video_streaming.


### typedef StartVideoStreamingHandle {#classmavsdk_1_1_camera_server_1ae6c608c1345fa1e1111c7bb2ed56cb85}

```cpp
using mavsdk::CameraServer::StartVideoStreamingHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_start_video_streaming.


### typedef StopVideoStreamingCallback {#classmavsdk_1_1_camera_server_1a111ed05ec69596127b9ac197c8fa2940}

```cpp
using mavsdk::CameraServer::StopVideoStreamingCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_stop_video_streaming.


### typedef StopVideoStreamingHandle {#classmavsdk_1_1_camera_server_1a2b1a56ba4c349a59c3c62fccc5d249ea}

```cpp
using mavsdk::CameraServer::StopVideoStreamingHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_stop_video_streaming.


### typedef SetModeCallback {#classmavsdk_1_1_camera_server_1a0e023e2c618adcb2ad75527353dc8acd}

```cpp
using mavsdk::CameraServer::SetModeCallback =  std::function<void(Mode)>
```


Callback type for subscribe_set_mode.


### typedef SetModeHandle {#classmavsdk_1_1_camera_server_1ae73af6f5c0e21e1ad89b75706f2d782c}

```cpp
using mavsdk::CameraServer::SetModeHandle =  Handle<Mode>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_set_mode.


### typedef StorageInformationCallback {#classmavsdk_1_1_camera_server_1ac5ad2dd65155ce1e7a6c0ebd2f0d20cf}

```cpp
using mavsdk::CameraServer::StorageInformationCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_storage_information.


### typedef StorageInformationHandle {#classmavsdk_1_1_camera_server_1a20edc2d4386999873dcbf74eafba6966}

```cpp
using mavsdk::CameraServer::StorageInformationHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_storage_information.


### typedef CaptureStatusCallback {#classmavsdk_1_1_camera_server_1af288de066938d13c02b4f1364cd66403}

```cpp
using mavsdk::CameraServer::CaptureStatusCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_capture_status.


### typedef CaptureStatusHandle {#classmavsdk_1_1_camera_server_1af67bf159a7f2173364182bff2cdd56bd}

```cpp
using mavsdk::CameraServer::CaptureStatusHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_capture_status.


### typedef FormatStorageCallback {#classmavsdk_1_1_camera_server_1a6242b427d4f29202ef931e8c6a7bdc46}

```cpp
using mavsdk::CameraServer::FormatStorageCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_format_storage.


### typedef FormatStorageHandle {#classmavsdk_1_1_camera_server_1a71b5531c91f7fba1c74296e55ecd3f45}

```cpp
using mavsdk::CameraServer::FormatStorageHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_format_storage.


### typedef ResetSettingsCallback {#classmavsdk_1_1_camera_server_1ac88b7c640c76e5ff2bdc9afb243ceac2}

```cpp
using mavsdk::CameraServer::ResetSettingsCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_reset_settings.


### typedef ResetSettingsHandle {#classmavsdk_1_1_camera_server_1aec973cf0e67863272e63e12902094f25}

```cpp
using mavsdk::CameraServer::ResetSettingsHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_reset_settings.


### typedef ZoomInStartCallback {#classmavsdk_1_1_camera_server_1a7b6c98f6051a80539ef891f65311aa71}

```cpp
using mavsdk::CameraServer::ZoomInStartCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_zoom_in_start.


### typedef ZoomInStartHandle {#classmavsdk_1_1_camera_server_1a3e67fff49fcc4557662826305531e879}

```cpp
using mavsdk::CameraServer::ZoomInStartHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_in_start.


### typedef ZoomOutStartCallback {#classmavsdk_1_1_camera_server_1aa07c374347ed42ec8d2b6727c0d60f5b}

```cpp
using mavsdk::CameraServer::ZoomOutStartCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_zoom_out_start.


### typedef ZoomOutStartHandle {#classmavsdk_1_1_camera_server_1afdfa5f070466f4bcd52af5af989d5226}

```cpp
using mavsdk::CameraServer::ZoomOutStartHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_out_start.


### typedef ZoomStopCallback {#classmavsdk_1_1_camera_server_1a6fc78f44226f6a326afb935a1dca5c86}

```cpp
using mavsdk::CameraServer::ZoomStopCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_zoom_stop.


### typedef ZoomStopHandle {#classmavsdk_1_1_camera_server_1a8bb9a99dfb5089332be6241230d54d34}

```cpp
using mavsdk::CameraServer::ZoomStopHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_stop.


### typedef ZoomRangeCallback {#classmavsdk_1_1_camera_server_1aa23f7ca9c7bf5eee8a12846743bed3a2}

```cpp
using mavsdk::CameraServer::ZoomRangeCallback =  std::function<void(float)>
```


Callback type for subscribe_zoom_range.


### typedef ZoomRangeHandle {#classmavsdk_1_1_camera_server_1a37067e1804927f01921079dfa1e133c2}

```cpp
using mavsdk::CameraServer::ZoomRangeHandle =  Handle<float>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_zoom_range.


### typedef TrackingPointCommandCallback {#classmavsdk_1_1_camera_server_1afcb7cf46d38b362562653e9648191ef4}

```cpp
using mavsdk::CameraServer::TrackingPointCommandCallback =  std::function<void(TrackPoint)>
```


Callback type for subscribe_tracking_point_command.


### typedef TrackingPointCommandHandle {#classmavsdk_1_1_camera_server_1a38c4148c1c717351865787b1529e9617}

```cpp
using mavsdk::CameraServer::TrackingPointCommandHandle =  Handle<TrackPoint>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_tracking_point_command.


### typedef TrackingRectangleCommandCallback {#classmavsdk_1_1_camera_server_1aa7541523c37bb4b24246e40deb0e35a6}

```cpp
using mavsdk::CameraServer::TrackingRectangleCommandCallback =  std::function<void(TrackRectangle)>
```


Callback type for subscribe_tracking_rectangle_command.


### typedef TrackingRectangleCommandHandle {#classmavsdk_1_1_camera_server_1a2e6b6f5ec287bec05357cc9dc9259566}

```cpp
using mavsdk::CameraServer::TrackingRectangleCommandHandle =  Handle<TrackRectangle>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_tracking_rectangle_command.


### typedef TrackingOffCommandCallback {#classmavsdk_1_1_camera_server_1ae46621448c7e035ec4da57338d898109}

```cpp
using mavsdk::CameraServer::TrackingOffCommandCallback =  std::function<void(int32_t)>
```


Callback type for subscribe_tracking_off_command.


### typedef TrackingOffCommandHandle {#classmavsdk_1_1_camera_server_1a5948e4523cdeff02791277df0d059557}

```cpp
using mavsdk::CameraServer::TrackingOffCommandHandle =  Handle<int32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_tracking_off_command.


## Member Enumeration Documentation


### enum CameraFeedback {#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72}


Possible feedback results for camera respond command.


Value | Description
--- | ---
<span id="classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown. 
<span id="classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72aa60852f204ed8028c1c58808b746d115"></span> `Ok` | Ok. 
<span id="classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Busy. 
<span id="classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Failed. 

### enum Mode {#classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155}


[Camera](classmavsdk_1_1_camera.md) mode type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown mode. 
<span id="classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155ac03d53b70feba4ea842510abecd6c45e"></span> `Photo` | Photo mode. 
<span id="classmavsdk_1_1_camera_server_1acf066a6b06ea67f66e59263a15869155a34e2d1989a1dbf75cd631596133ee5ee"></span> `Video` | Video mode. 

### enum Result {#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d}


Possible results returned for action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Command executed successfully. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da12d868c18cb29bf58f02b504be9033fd"></span> `InProgress` | Command in progress. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8dad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | [Camera](classmavsdk_1_1_camera.md) is busy and rejected command. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | [Camera](classmavsdk_1_1_camera.md) denied the command. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | An error has occurred while executing the command. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8dac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Command timed out. 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da5a738160747f39c20e9f65416931c974"></span> `WrongArgument` | Command has wrong argument(s). 
<span id="classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8da1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 

## Member Function Documentation


### set_information() {#classmavsdk_1_1_camera_server_1a81b6b957f11cc8764ea5bddfae88fc64}
```cpp
Result mavsdk::CameraServer::set_information(Information information) const
```


Sets the camera information. This must be called as soon as the camera server is created.

This function is blocking.

**Parameters**

* [Information](structmavsdk_1_1_camera_server_1_1_information.md) **information** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### set_video_streaming() {#classmavsdk_1_1_camera_server_1a8de695275fc2bee579005b4638250bb1}
```cpp
Result mavsdk::CameraServer::set_video_streaming(VideoStreaming video_streaming) const
```


Sets video streaming settings.

This function is blocking.

**Parameters**

* [VideoStreaming](structmavsdk_1_1_camera_server_1_1_video_streaming.md) **video_streaming** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### set_in_progress() {#classmavsdk_1_1_camera_server_1a9f4c5012412b889017c9f47d01044651}
```cpp
Result mavsdk::CameraServer::set_in_progress(bool in_progress) const
```


Sets image capture in progress status flags. This should be set to true when the camera is busy taking a photo and false when it is done.

This function is blocking.

**Parameters**

* bool **in_progress** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_take_photo() {#classmavsdk_1_1_camera_server_1a8a982fbf278d1521f6f80a7527f5f85c}
```cpp
TakePhotoHandle mavsdk::CameraServer::subscribe_take_photo(const TakePhotoCallback &callback)
```


Subscribe to image capture requests. Each request received should respond to using RespondTakePhoto.


**Parameters**

* const [TakePhotoCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a947529ac03c8dfac60c7c798db60a2d0)& **callback** - 

**Returns**

&emsp;[TakePhotoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2c79367a301d3b3093963222ac2cccbe) - 

### unsubscribe_take_photo() {#classmavsdk_1_1_camera_server_1a9b6c0b59f8990b4259b9a2fa134228c9}
```cpp
void mavsdk::CameraServer::unsubscribe_take_photo(TakePhotoHandle handle)
```


Unsubscribe from subscribe_take_photo.


**Parameters**

* [TakePhotoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2c79367a301d3b3093963222ac2cccbe) **handle** - 

### respond_take_photo() {#classmavsdk_1_1_camera_server_1aef1d7b26221a1e98b9d6ff099a6f1299}
```cpp
Result mavsdk::CameraServer::respond_take_photo(CameraFeedback take_photo_feedback, CaptureInfo capture_info) const
```


Respond to an image capture request from SubscribeTakePhoto.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **take_photo_feedback** - 
* [CaptureInfo](structmavsdk_1_1_camera_server_1_1_capture_info.md) **capture_info** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_start_video() {#classmavsdk_1_1_camera_server_1a11e48131fac66f1f54a7f433a18cb000}
```cpp
StartVideoHandle mavsdk::CameraServer::subscribe_start_video(const StartVideoCallback &callback)
```


Subscribe to start video requests. Each request received should respond to using RespondStartVideo.


**Parameters**

* const [StartVideoCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a01810f7bdc3ab4a59c633c7e11d02d0f)& **callback** - 

**Returns**

&emsp;[StartVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2ec6dce94dfca9b5d102306df53072a1) - 

### unsubscribe_start_video() {#classmavsdk_1_1_camera_server_1a408195cfa869e1146c054320c8bf74c2}
```cpp
void mavsdk::CameraServer::unsubscribe_start_video(StartVideoHandle handle)
```


Unsubscribe from subscribe_start_video.


**Parameters**

* [StartVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2ec6dce94dfca9b5d102306df53072a1) **handle** - 

### respond_start_video() {#classmavsdk_1_1_camera_server_1aae8ca562aed415a803e5cf116d3860ab}
```cpp
Result mavsdk::CameraServer::respond_start_video(CameraFeedback start_video_feedback) const
```


Subscribe to stop video requests. Each request received should respond using StopVideoResponse.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **start_video_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_stop_video() {#classmavsdk_1_1_camera_server_1a1f9bf41790c13bfac00e83a07a0aabc2}
```cpp
StopVideoHandle mavsdk::CameraServer::subscribe_stop_video(const StopVideoCallback &callback)
```


Subscribe to stop video requests. Each request received should response to using RespondStopVideo.


**Parameters**

* const [StopVideoCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a37a4bd54432b5ffa866edd6cd9310c7f)& **callback** - 

**Returns**

&emsp;[StopVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a7ca737d29d078f98770b7e2de6a93e8c) - 

### unsubscribe_stop_video() {#classmavsdk_1_1_camera_server_1a46087c9b9131d5b56c937a7a0f9b82cd}
```cpp
void mavsdk::CameraServer::unsubscribe_stop_video(StopVideoHandle handle)
```


Unsubscribe from subscribe_stop_video.


**Parameters**

* [StopVideoHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a7ca737d29d078f98770b7e2de6a93e8c) **handle** - 

### respond_stop_video() {#classmavsdk_1_1_camera_server_1a72504e0a97587dbd4e92cbdf2aedd36f}
```cpp
Result mavsdk::CameraServer::respond_stop_video(CameraFeedback stop_video_feedback) const
```


Respond to stop video request from SubscribeStopVideo.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **stop_video_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_start_video_streaming() {#classmavsdk_1_1_camera_server_1a7528dfe116272cdef1584c0709e9d554}
```cpp
StartVideoStreamingHandle mavsdk::CameraServer::subscribe_start_video_streaming(const StartVideoStreamingCallback &callback)
```


Subscribe to start video streaming requests. Each request received should response to using RespondStartVideoStreaming.


**Parameters**

* const [StartVideoStreamingCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae67fa13502562dea8c3e955213cd14b9)& **callback** - 

**Returns**

&emsp;[StartVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae6c608c1345fa1e1111c7bb2ed56cb85) - 

### unsubscribe_start_video_streaming() {#classmavsdk_1_1_camera_server_1a0e85bc6405cb04be6d14e385b5c9b6f0}
```cpp
void mavsdk::CameraServer::unsubscribe_start_video_streaming(StartVideoStreamingHandle handle)
```


Unsubscribe from subscribe_start_video_streaming.


**Parameters**

* [StartVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae6c608c1345fa1e1111c7bb2ed56cb85) **handle** - 

### respond_start_video_streaming() {#classmavsdk_1_1_camera_server_1a32bf585f316c0990fd3c792930eb430c}
```cpp
Result mavsdk::CameraServer::respond_start_video_streaming(CameraFeedback start_video_streaming_feedback) const
```


Respond to start video streaming from SubscribeStartVideoStreaming.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **start_video_streaming_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_stop_video_streaming() {#classmavsdk_1_1_camera_server_1aed9dd6e4e5d4b9aa24254933cfdb68d6}
```cpp
StopVideoStreamingHandle mavsdk::CameraServer::subscribe_stop_video_streaming(const StopVideoStreamingCallback &callback)
```


Subscribe to stop video streaming requests. Each request received should response to using RespondStopVideoStreaming.


**Parameters**

* const [StopVideoStreamingCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a111ed05ec69596127b9ac197c8fa2940)& **callback** - 

**Returns**

&emsp;[StopVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2b1a56ba4c349a59c3c62fccc5d249ea) - 

### unsubscribe_stop_video_streaming() {#classmavsdk_1_1_camera_server_1a0ae66e34730d2a9ebea39bff190b432c}
```cpp
void mavsdk::CameraServer::unsubscribe_stop_video_streaming(StopVideoStreamingHandle handle)
```


Unsubscribe from subscribe_stop_video_streaming.


**Parameters**

* [StopVideoStreamingHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2b1a56ba4c349a59c3c62fccc5d249ea) **handle** - 

### respond_stop_video_streaming() {#classmavsdk_1_1_camera_server_1a8a452852fd4ba6a97ab7613dfbe3d3d5}
```cpp
Result mavsdk::CameraServer::respond_stop_video_streaming(CameraFeedback stop_video_streaming_feedback) const
```


Respond to stop video streaming from SubscribeStopVideoStreaming.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **stop_video_streaming_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_set_mode() {#classmavsdk_1_1_camera_server_1a7aefc230a1c9688c6ffce0563970363d}
```cpp
SetModeHandle mavsdk::CameraServer::subscribe_set_mode(const SetModeCallback &callback)
```


Subscribe to set camera mode requests. Each request received should response to using RespondSetMode.


**Parameters**

* const [SetModeCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a0e023e2c618adcb2ad75527353dc8acd)& **callback** - 

**Returns**

&emsp;[SetModeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae73af6f5c0e21e1ad89b75706f2d782c) - 

### unsubscribe_set_mode() {#classmavsdk_1_1_camera_server_1a4ebe9158300707532374492e664911b7}
```cpp
void mavsdk::CameraServer::unsubscribe_set_mode(SetModeHandle handle)
```


Unsubscribe from subscribe_set_mode.


**Parameters**

* [SetModeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae73af6f5c0e21e1ad89b75706f2d782c) **handle** - 

### respond_set_mode() {#classmavsdk_1_1_camera_server_1add075055b138c38ef17502ef049b2508}
```cpp
Result mavsdk::CameraServer::respond_set_mode(CameraFeedback set_mode_feedback) const
```


Respond to set camera mode from SubscribeSetMode.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **set_mode_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_storage_information() {#classmavsdk_1_1_camera_server_1a05fbd42c88962fccad1b2a31810dade7}
```cpp
StorageInformationHandle mavsdk::CameraServer::subscribe_storage_information(const StorageInformationCallback &callback)
```


Subscribe to camera storage information requests. Each request received should response to using RespondStorageInformation.


**Parameters**

* const [StorageInformationCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ac5ad2dd65155ce1e7a6c0ebd2f0d20cf)& **callback** - 

**Returns**

&emsp;[StorageInformationHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a20edc2d4386999873dcbf74eafba6966) - 

### unsubscribe_storage_information() {#classmavsdk_1_1_camera_server_1afe6fbc5c8f5f39f204c2d6c57ac5161d}
```cpp
void mavsdk::CameraServer::unsubscribe_storage_information(StorageInformationHandle handle)
```


Unsubscribe from subscribe_storage_information.


**Parameters**

* [StorageInformationHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a20edc2d4386999873dcbf74eafba6966) **handle** - 

### respond_storage_information() {#classmavsdk_1_1_camera_server_1a6dee290ca68414ee8ff9744e261b5ac3}
```cpp
Result mavsdk::CameraServer::respond_storage_information(CameraFeedback storage_information_feedback, StorageInformation storage_information) const
```


Respond to camera storage information from SubscribeStorageInformation.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **storage_information_feedback** - 
* [StorageInformation](structmavsdk_1_1_camera_server_1_1_storage_information.md) **storage_information** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_capture_status() {#classmavsdk_1_1_camera_server_1a783ac1e37cf9be240e660fe3ba4eced9}
```cpp
CaptureStatusHandle mavsdk::CameraServer::subscribe_capture_status(const CaptureStatusCallback &callback)
```


Subscribe to camera capture status requests. Each request received should response to using RespondCaptureStatus.


**Parameters**

* const [CaptureStatusCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1af288de066938d13c02b4f1364cd66403)& **callback** - 

**Returns**

&emsp;[CaptureStatusHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1af67bf159a7f2173364182bff2cdd56bd) - 

### unsubscribe_capture_status() {#classmavsdk_1_1_camera_server_1af26523420d48b629f1758da304c9d156}
```cpp
void mavsdk::CameraServer::unsubscribe_capture_status(CaptureStatusHandle handle)
```


Unsubscribe from subscribe_capture_status.


**Parameters**

* [CaptureStatusHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1af67bf159a7f2173364182bff2cdd56bd) **handle** - 

### respond_capture_status() {#classmavsdk_1_1_camera_server_1ae091608e278807026e114d257d25407a}
```cpp
Result mavsdk::CameraServer::respond_capture_status(CameraFeedback capture_status_feedback, CaptureStatus capture_status) const
```


Respond to camera capture status from SubscribeCaptureStatus.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **capture_status_feedback** - 
* [CaptureStatus](structmavsdk_1_1_camera_server_1_1_capture_status.md) **capture_status** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_format_storage() {#classmavsdk_1_1_camera_server_1ab788a20175966f8d5ddab973f176a3b4}
```cpp
FormatStorageHandle mavsdk::CameraServer::subscribe_format_storage(const FormatStorageCallback &callback)
```


Subscribe to format storage requests. Each request received should response to using RespondFormatStorage.


**Parameters**

* const [FormatStorageCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a6242b427d4f29202ef931e8c6a7bdc46)& **callback** - 

**Returns**

&emsp;[FormatStorageHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a71b5531c91f7fba1c74296e55ecd3f45) - 

### unsubscribe_format_storage() {#classmavsdk_1_1_camera_server_1a9b51f14ffa8c65518d59a5fed406be0c}
```cpp
void mavsdk::CameraServer::unsubscribe_format_storage(FormatStorageHandle handle)
```


Unsubscribe from subscribe_format_storage.


**Parameters**

* [FormatStorageHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a71b5531c91f7fba1c74296e55ecd3f45) **handle** - 

### respond_format_storage() {#classmavsdk_1_1_camera_server_1a18bb9a800df91f74efa0a35eb5218a14}
```cpp
Result mavsdk::CameraServer::respond_format_storage(CameraFeedback format_storage_feedback) const
```


Respond to format storage from SubscribeFormatStorage.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **format_storage_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_reset_settings() {#classmavsdk_1_1_camera_server_1ab1022684bc503e1d6294495de8c68165}
```cpp
ResetSettingsHandle mavsdk::CameraServer::subscribe_reset_settings(const ResetSettingsCallback &callback)
```


Subscribe to reset settings requests. Each request received should response to using RespondResetSettings.


**Parameters**

* const [ResetSettingsCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ac88b7c640c76e5ff2bdc9afb243ceac2)& **callback** - 

**Returns**

&emsp;[ResetSettingsHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aec973cf0e67863272e63e12902094f25) - 

### unsubscribe_reset_settings() {#classmavsdk_1_1_camera_server_1a09c70966fa6f98215c11d8257d1b9608}
```cpp
void mavsdk::CameraServer::unsubscribe_reset_settings(ResetSettingsHandle handle)
```


Unsubscribe from subscribe_reset_settings.


**Parameters**

* [ResetSettingsHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aec973cf0e67863272e63e12902094f25) **handle** - 

### respond_reset_settings() {#classmavsdk_1_1_camera_server_1a1d7b2793d899c9052038f4e9569eb32b}
```cpp
Result mavsdk::CameraServer::respond_reset_settings(CameraFeedback reset_settings_feedback) const
```


Respond to reset settings from SubscribeResetSettings.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **reset_settings_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_zoom_in_start() {#classmavsdk_1_1_camera_server_1a4d5f9d60ad7e19cea32b4af040727d46}
```cpp
ZoomInStartHandle mavsdk::CameraServer::subscribe_zoom_in_start(const ZoomInStartCallback &callback)
```


Subscribe to zoom in start command.


**Parameters**

* const [ZoomInStartCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a7b6c98f6051a80539ef891f65311aa71)& **callback** - 

**Returns**

&emsp;[ZoomInStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a3e67fff49fcc4557662826305531e879) - 

### unsubscribe_zoom_in_start() {#classmavsdk_1_1_camera_server_1a5ab3f69f82b3b848decf5067c26a66fa}
```cpp
void mavsdk::CameraServer::unsubscribe_zoom_in_start(ZoomInStartHandle handle)
```


Unsubscribe from subscribe_zoom_in_start.


**Parameters**

* [ZoomInStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a3e67fff49fcc4557662826305531e879) **handle** - 

### respond_zoom_in_start() {#classmavsdk_1_1_camera_server_1a059b221e9dc918ea4a3c17371ae8833f}
```cpp
Result mavsdk::CameraServer::respond_zoom_in_start(CameraFeedback zoom_in_start_feedback) const
```


Respond to zoom in start.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **zoom_in_start_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_zoom_out_start() {#classmavsdk_1_1_camera_server_1a46244b254bce6fd53ca1c0db3c0966a1}
```cpp
ZoomOutStartHandle mavsdk::CameraServer::subscribe_zoom_out_start(const ZoomOutStartCallback &callback)
```


Subscribe to zoom out start command.


**Parameters**

* const [ZoomOutStartCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa07c374347ed42ec8d2b6727c0d60f5b)& **callback** - 

**Returns**

&emsp;[ZoomOutStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1afdfa5f070466f4bcd52af5af989d5226) - 

### unsubscribe_zoom_out_start() {#classmavsdk_1_1_camera_server_1a1aa244c9eeaef416cd67ef2896264959}
```cpp
void mavsdk::CameraServer::unsubscribe_zoom_out_start(ZoomOutStartHandle handle)
```


Unsubscribe from subscribe_zoom_out_start.


**Parameters**

* [ZoomOutStartHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1afdfa5f070466f4bcd52af5af989d5226) **handle** - 

### respond_zoom_out_start() {#classmavsdk_1_1_camera_server_1aaf52d7ad70cb10219db015e17a657632}
```cpp
Result mavsdk::CameraServer::respond_zoom_out_start(CameraFeedback zoom_out_start_feedback) const
```


Respond to zoom out start.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **zoom_out_start_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_zoom_stop() {#classmavsdk_1_1_camera_server_1a2be8527911fe1f4dafc7cd87f9be60cd}
```cpp
ZoomStopHandle mavsdk::CameraServer::subscribe_zoom_stop(const ZoomStopCallback &callback)
```


Subscribe to zoom stop command.


**Parameters**

* const [ZoomStopCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a6fc78f44226f6a326afb935a1dca5c86)& **callback** - 

**Returns**

&emsp;[ZoomStopHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a8bb9a99dfb5089332be6241230d54d34) - 

### unsubscribe_zoom_stop() {#classmavsdk_1_1_camera_server_1aab7d43df6c39266ccdcb80ee6f5621fb}
```cpp
void mavsdk::CameraServer::unsubscribe_zoom_stop(ZoomStopHandle handle)
```


Unsubscribe from subscribe_zoom_stop.


**Parameters**

* [ZoomStopHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a8bb9a99dfb5089332be6241230d54d34) **handle** - 

### respond_zoom_stop() {#classmavsdk_1_1_camera_server_1a968890afb4b3b33a7a6965dd4b32aa1f}
```cpp
Result mavsdk::CameraServer::respond_zoom_stop(CameraFeedback zoom_stop_feedback) const
```


Respond to zoom stop.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **zoom_stop_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### subscribe_zoom_range() {#classmavsdk_1_1_camera_server_1a6dc95f635c88749b4e2bbfa07bd5b0fd}
```cpp
ZoomRangeHandle mavsdk::CameraServer::subscribe_zoom_range(const ZoomRangeCallback &callback)
```


Subscribe to zoom range command.


**Parameters**

* const [ZoomRangeCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa23f7ca9c7bf5eee8a12846743bed3a2)& **callback** - 

**Returns**

&emsp;[ZoomRangeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a37067e1804927f01921079dfa1e133c2) - 

### unsubscribe_zoom_range() {#classmavsdk_1_1_camera_server_1a9fc5f5daed094bea620011bce222bb88}
```cpp
void mavsdk::CameraServer::unsubscribe_zoom_range(ZoomRangeHandle handle)
```


Unsubscribe from subscribe_zoom_range.


**Parameters**

* [ZoomRangeHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a37067e1804927f01921079dfa1e133c2) **handle** - 

### respond_zoom_range() {#classmavsdk_1_1_camera_server_1a5ea9dd6521f43d91671b0fa1eef6d0f6}
```cpp
Result mavsdk::CameraServer::respond_zoom_range(CameraFeedback zoom_range_feedback) const
```


Respond to zoom range.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **zoom_range_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### set_tracking_rectangle_status() {#classmavsdk_1_1_camera_server_1a262b7ec85fee9aa1b83eebd3ffb9abaa}
```cpp
void mavsdk::CameraServer::set_tracking_rectangle_status(TrackRectangle tracked_rectangle) const
```


Set/update the current rectangle tracking status.

This function is blocking.

**Parameters**

* [TrackRectangle](structmavsdk_1_1_camera_server_1_1_track_rectangle.md) **tracked_rectangle** - 

### set_tracking_off_status() {#classmavsdk_1_1_camera_server_1a22ba37443a3bc69c826261964949a8d4}
```cpp
void mavsdk::CameraServer::set_tracking_off_status() const
```


Set the current tracking status to off.

This function is blocking.

### subscribe_tracking_point_command() {#classmavsdk_1_1_camera_server_1a474c49eff4fe7a1b707df2c610197b3d}
```cpp
TrackingPointCommandHandle mavsdk::CameraServer::subscribe_tracking_point_command(const TrackingPointCommandCallback &callback)
```


Subscribe to incoming tracking point command.


**Parameters**

* const [TrackingPointCommandCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1afcb7cf46d38b362562653e9648191ef4)& **callback** - 

**Returns**

&emsp;[TrackingPointCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a38c4148c1c717351865787b1529e9617) - 

### unsubscribe_tracking_point_command() {#classmavsdk_1_1_camera_server_1a8cd6c774f5cebb423cb32782d213f4de}
```cpp
void mavsdk::CameraServer::unsubscribe_tracking_point_command(TrackingPointCommandHandle handle)
```


Unsubscribe from subscribe_tracking_point_command.


**Parameters**

* [TrackingPointCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a38c4148c1c717351865787b1529e9617) **handle** - 

### subscribe_tracking_rectangle_command() {#classmavsdk_1_1_camera_server_1a752751ac6064456d0139592697bfd3a8}
```cpp
TrackingRectangleCommandHandle mavsdk::CameraServer::subscribe_tracking_rectangle_command(const TrackingRectangleCommandCallback &callback)
```


Subscribe to incoming tracking rectangle command.


**Parameters**

* const [TrackingRectangleCommandCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa7541523c37bb4b24246e40deb0e35a6)& **callback** - 

**Returns**

&emsp;[TrackingRectangleCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2e6b6f5ec287bec05357cc9dc9259566) - 

### unsubscribe_tracking_rectangle_command() {#classmavsdk_1_1_camera_server_1a6abfa99ac94653930aa1e558d7c7beaa}
```cpp
void mavsdk::CameraServer::unsubscribe_tracking_rectangle_command(TrackingRectangleCommandHandle handle)
```


Unsubscribe from subscribe_tracking_rectangle_command.


**Parameters**

* [TrackingRectangleCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a2e6b6f5ec287bec05357cc9dc9259566) **handle** - 

### subscribe_tracking_off_command() {#classmavsdk_1_1_camera_server_1ac976e22ee5831c8144a4b917ab615946}
```cpp
TrackingOffCommandHandle mavsdk::CameraServer::subscribe_tracking_off_command(const TrackingOffCommandCallback &callback)
```


Subscribe to incoming tracking off command.


**Parameters**

* const [TrackingOffCommandCallback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1ae46621448c7e035ec4da57338d898109)& **callback** - 

**Returns**

&emsp;[TrackingOffCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a5948e4523cdeff02791277df0d059557) - 

### unsubscribe_tracking_off_command() {#classmavsdk_1_1_camera_server_1a13d8edd63c57c53c3d17893dc31e3bc3}
```cpp
void mavsdk::CameraServer::unsubscribe_tracking_off_command(TrackingOffCommandHandle handle)
```


Unsubscribe from subscribe_tracking_off_command.


**Parameters**

* [TrackingOffCommandHandle](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a5948e4523cdeff02791277df0d059557) **handle** - 

### respond_tracking_point_command() {#classmavsdk_1_1_camera_server_1a8749824a1326cc0a7c441fb7c59b9507}
```cpp
Result mavsdk::CameraServer::respond_tracking_point_command(CameraFeedback stop_video_feedback) const
```


Respond to an incoming tracking point command.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **stop_video_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### respond_tracking_rectangle_command() {#classmavsdk_1_1_camera_server_1a2de79ab19b5d51e6760eb3173e9c6f77}
```cpp
Result mavsdk::CameraServer::respond_tracking_rectangle_command(CameraFeedback stop_video_feedback) const
```


Respond to an incoming tracking rectangle command.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **stop_video_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### respond_tracking_off_command() {#classmavsdk_1_1_camera_server_1a31e8a3a8996ae48afbacbef38d31e209}
```cpp
Result mavsdk::CameraServer::respond_tracking_off_command(CameraFeedback stop_video_feedback) const
```


Respond to an incoming tracking off command.

This function is blocking.

**Parameters**

* [CameraFeedback](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1a088cdcd9da37b84f075d20d5b7458a72) **stop_video_feedback** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_camera_server.md#classmavsdk_1_1_camera_server_1aa625af622ca91165c737cffebfe57f8d) - Result of request.

### operator=() {#classmavsdk_1_1_camera_server_1a6b516bbddcbaa2867d1a11be8eda14bd}
```cpp
const CameraServer & mavsdk::CameraServer::operator=(const CameraServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [CameraServer](classmavsdk_1_1_camera_server.md)&  - 

**Returns**

&emsp;const [CameraServer](classmavsdk_1_1_camera_server.md) & - 