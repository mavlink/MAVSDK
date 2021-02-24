#import "plugin-camera.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>


MAVSDKCameraMode translateFromCppMode(mavsdk::Camera::Mode mode)
{
    switch (mode) {
        default:
            NSLog(@"Unknown mode enum value: %d", static_cast<int>(mode));
        // FALLTHROUGH
        case mavsdk::Camera::Mode::Unknown:
            return MAVSDKCameraModeUnknown;
        case mavsdk::Camera::Mode::Photo:
            return MAVSDKCameraModePhoto;
        case mavsdk::Camera::Mode::Video:
            return MAVSDKCameraModeVideo;
    }
}

mavsdk::Camera::Mode translateToCppMode(MAVSDKCameraMode mode)
{
    switch (mode) {
        default:
            NSLog(@"Unknown Mode enum value: %d", static_cast<int>(mode));
        // FALLTHROUGH
        case MAVSDKCameraModeUnknown:
            return mavsdk::Camera::Mode::Unknown;
        case MAVSDKCameraModePhoto:
            return mavsdk::Camera::Mode::Photo;
        case MAVSDKCameraModeVideo:
            return mavsdk::Camera::Mode::Video;
    }
}

MAVSDKCameraPhotosRange translateFromCppPhotosRange(mavsdk::Camera::PhotosRange photos_range)
{
    switch (photos_range) {
        default:
            NSLog(@"Unknown photos_range enum value: %d", static_cast<int>(photos_range));
        // FALLTHROUGH
        case mavsdk::Camera::PhotosRange::All:
            return MAVSDKCameraPhotosRangeAll;
        case mavsdk::Camera::PhotosRange::SinceConnection:
            return MAVSDKCameraPhotosRangeSinceConnection;
    }
}

mavsdk::Camera::PhotosRange translateToCppPhotosRange(MAVSDKCameraPhotosRange photosRange)
{
    switch (photosRange) {
        default:
            NSLog(@"Unknown PhotosRange enum value: %d", static_cast<int>(photosRange));
        // FALLTHROUGH
        case MAVSDKCameraPhotosRangeAll:
            return mavsdk::Camera::PhotosRange::All;
        case MAVSDKCameraPhotosRangeSinceConnection:
            return mavsdk::Camera::PhotosRange::SinceConnection;
    }
}







MAVSDKCameraPosition* translateFromCppPosition(mavsdk::Camera::Position position)
{
    MAVSDKCameraPosition *obj = [[MAVSDKCameraPosition alloc] init];


        
    obj.latitudeDeg = position.latitude_deg;
        
    
        
    obj.longitudeDeg = position.longitude_deg;
        
    
        
    obj.absoluteAltitudeM = position.absolute_altitude_m;
        
    
        
    obj.relativeAltitudeM = position.relative_altitude_m;
        
    
    return obj;
}

mavsdk::Camera::Position translateToCppPosition(MAVSDKCameraPosition* position)
{
    mavsdk::Camera::Position obj;


        
    obj.latitude_deg = position.latitudeDeg;
        
    
        
    obj.longitude_deg = position.longitudeDeg;
        
    
        
    obj.absolute_altitude_m = position.absoluteAltitudeM;
        
    
        
    obj.relative_altitude_m = position.relativeAltitudeM;
        
    
    return obj;
}



@implementation MAVSDKCameraPosition
@end



MAVSDKCameraQuaternion* translateFromCppQuaternion(mavsdk::Camera::Quaternion quaternion)
{
    MAVSDKCameraQuaternion *obj = [[MAVSDKCameraQuaternion alloc] init];


        
    obj.w = quaternion.w;
        
    
        
    obj.x = quaternion.x;
        
    
        
    obj.y = quaternion.y;
        
    
        
    obj.z = quaternion.z;
        
    
    return obj;
}

mavsdk::Camera::Quaternion translateToCppQuaternion(MAVSDKCameraQuaternion* quaternion)
{
    mavsdk::Camera::Quaternion obj;


        
    obj.w = quaternion.w;
        
    
        
    obj.x = quaternion.x;
        
    
        
    obj.y = quaternion.y;
        
    
        
    obj.z = quaternion.z;
        
    
    return obj;
}



@implementation MAVSDKCameraQuaternion
@end



MAVSDKCameraEulerAngle* translateFromCppEulerAngle(mavsdk::Camera::EulerAngle euler_angle)
{
    MAVSDKCameraEulerAngle *obj = [[MAVSDKCameraEulerAngle alloc] init];


        
    obj.rollDeg = euler_angle.roll_deg;
        
    
        
    obj.pitchDeg = euler_angle.pitch_deg;
        
    
        
    obj.yawDeg = euler_angle.yaw_deg;
        
    
    return obj;
}

mavsdk::Camera::EulerAngle translateToCppEulerAngle(MAVSDKCameraEulerAngle* eulerAngle)
{
    mavsdk::Camera::EulerAngle obj;


        
    obj.roll_deg = eulerAngle.rollDeg;
        
    
        
    obj.pitch_deg = eulerAngle.pitchDeg;
        
    
        
    obj.yaw_deg = eulerAngle.yawDeg;
        
    
    return obj;
}



@implementation MAVSDKCameraEulerAngle
@end



MAVSDKCameraCaptureInfo* translateFromCppCaptureInfo(mavsdk::Camera::CaptureInfo capture_info)
{
    MAVSDKCameraCaptureInfo *obj = [[MAVSDKCameraCaptureInfo alloc] init];


        
    obj.position = translateFromCppPosition(capture_info.position);
        
    
        
    obj.attitudeQuaternion = translateFromCppQuaternion(capture_info.attitude_quaternion);
        
    
        
    obj.attitudeEulerAngle = translateFromCppEulerAngle(capture_info.attitude_euler_angle);
        
    
        
    obj.timeUtcUs = capture_info.time_utc_us;
        
    
        
    obj.isSuccess = capture_info.is_success;
        
    
        
    obj.index = capture_info.index;
        
    
        
    [NSString stringWithCString:capture_info.file_url.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Camera::CaptureInfo translateToCppCaptureInfo(MAVSDKCameraCaptureInfo* captureInfo)
{
    mavsdk::Camera::CaptureInfo obj;


        
    obj.position = translateToCppPosition(captureInfo.position);
        
    
        
    obj.attitude_quaternion = translateToCppQuaternion(captureInfo.attitudeQuaternion);
        
    
        
    obj.attitude_euler_angle = translateToCppEulerAngle(captureInfo.attitudeEulerAngle);
        
    
        
    obj.time_utc_us = captureInfo.timeUtcUs;
        
    
        
    obj.is_success = captureInfo.isSuccess;
        
    
        
    obj.index = captureInfo.index;
        
    
        
    obj.file_url = [captureInfo.fileUrl UTF8String];
        
    
    return obj;
}



@implementation MAVSDKCameraCaptureInfo
@end



MAVSDKCameraVideoStreamSettings* translateFromCppVideoStreamSettings(mavsdk::Camera::VideoStreamSettings video_stream_settings)
{
    MAVSDKCameraVideoStreamSettings *obj = [[MAVSDKCameraVideoStreamSettings alloc] init];


        
    obj.frameRateHz = video_stream_settings.frame_rate_hz;
        
    
        
    obj.horizontalResolutionPix = video_stream_settings.horizontal_resolution_pix;
        
    
        
    obj.verticalResolutionPix = video_stream_settings.vertical_resolution_pix;
        
    
        
    obj.bitRateBS = video_stream_settings.bit_rate_b_s;
        
    
        
    obj.rotationDeg = video_stream_settings.rotation_deg;
        
    
        
    [NSString stringWithCString:video_stream_settings.uri.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Camera::VideoStreamSettings translateToCppVideoStreamSettings(MAVSDKCameraVideoStreamSettings* videoStreamSettings)
{
    mavsdk::Camera::VideoStreamSettings obj;


        
    obj.frame_rate_hz = videoStreamSettings.frameRateHz;
        
    
        
    obj.horizontal_resolution_pix = videoStreamSettings.horizontalResolutionPix;
        
    
        
    obj.vertical_resolution_pix = videoStreamSettings.verticalResolutionPix;
        
    
        
    obj.bit_rate_b_s = videoStreamSettings.bitRateBS;
        
    
        
    obj.rotation_deg = videoStreamSettings.rotationDeg;
        
    
        
    obj.uri = [videoStreamSettings.uri UTF8String];
        
    
    return obj;
}



@implementation MAVSDKCameraVideoStreamSettings
@end




MAVSDKCameraVideoStreamStatus translateFromCppVideoStreamStatus(mavsdk::Camera::VideoStreamInfo::VideoStreamStatus video_stream_status)
{
    switch (video_stream_status) {
        default:
            NSLog(@"Unknown video_stream_status enum value: %d", static_cast<int>(video_stream_status));
        // FALLTHROUGH
        case mavsdk::Camera::VideoStreamInfo::VideoStreamStatus::NotRunning:
            return MAVSDKCameraVideoStreamStatusNotRunning;
        case mavsdk::Camera::VideoStreamInfo::VideoStreamStatus::InProgress:
            return MAVSDKCameraVideoStreamStatusInProgress;
    }
}

mavsdk::Camera::VideoStreamInfo::VideoStreamStatus translateToCppVideoStreamStatus(MAVSDKCameraVideoStreamStatus videoStreamStatus)
{
    switch (videoStreamStatus) {
        default:
            NSLog(@"Unknown VideoStreamStatus enum value: %d", static_cast<int>(videoStreamStatus));
        // FALLTHROUGH
        case MAVSDKCameraVideoStreamStatusNotRunning:
            return mavsdk::Camera::VideoStreamInfo::VideoStreamStatus::NotRunning;
        case MAVSDKCameraVideoStreamStatusInProgress:
            return mavsdk::Camera::VideoStreamInfo::VideoStreamStatus::InProgress;
    }
}
MAVSDKCameraVideoStreamInfo* translateFromCppVideoStreamInfo(mavsdk::Camera::VideoStreamInfo video_stream_info)
{
    MAVSDKCameraVideoStreamInfo *obj = [[MAVSDKCameraVideoStreamInfo alloc] init];


        
    obj.settings = translateFromCppVideoStreamSettings(video_stream_info.settings);
        
    
        
    obj.status = (MAVSDKCameraVideoStreamStatus)video_stream_info.status;
        
    
    return obj;
}

mavsdk::Camera::VideoStreamInfo translateToCppVideoStreamInfo(MAVSDKCameraVideoStreamInfo* videoStreamInfo)
{
    mavsdk::Camera::VideoStreamInfo obj;


        
    obj.settings = translateToCppVideoStreamSettings(videoStreamInfo.settings);
        
    
        
    obj.status = translateToCppVideoStreamStatus(videoStreamInfo.status);
        
    
    return obj;
}



@implementation MAVSDKCameraVideoStreamInfo
@end




MAVSDKCameraStorageStatus translateFromCppStorageStatus(mavsdk::Camera::Status::StorageStatus storage_status)
{
    switch (storage_status) {
        default:
            NSLog(@"Unknown storage_status enum value: %d", static_cast<int>(storage_status));
        // FALLTHROUGH
        case mavsdk::Camera::Status::StorageStatus::NotAvailable:
            return MAVSDKCameraStorageStatusNotAvailable;
        case mavsdk::Camera::Status::StorageStatus::Unformatted:
            return MAVSDKCameraStorageStatusUnformatted;
        case mavsdk::Camera::Status::StorageStatus::Formatted:
            return MAVSDKCameraStorageStatusFormatted;
    }
}

mavsdk::Camera::Status::StorageStatus translateToCppStorageStatus(MAVSDKCameraStorageStatus storageStatus)
{
    switch (storageStatus) {
        default:
            NSLog(@"Unknown StorageStatus enum value: %d", static_cast<int>(storageStatus));
        // FALLTHROUGH
        case MAVSDKCameraStorageStatusNotAvailable:
            return mavsdk::Camera::Status::StorageStatus::NotAvailable;
        case MAVSDKCameraStorageStatusUnformatted:
            return mavsdk::Camera::Status::StorageStatus::Unformatted;
        case MAVSDKCameraStorageStatusFormatted:
            return mavsdk::Camera::Status::StorageStatus::Formatted;
    }
}
MAVSDKCameraStatus* translateFromCppStatus(mavsdk::Camera::Status status)
{
    MAVSDKCameraStatus *obj = [[MAVSDKCameraStatus alloc] init];


        
    obj.videoOn = status.video_on;
        
    
        
    obj.photoIntervalOn = status.photo_interval_on;
        
    
        
    obj.usedStorageMib = status.used_storage_mib;
        
    
        
    obj.availableStorageMib = status.available_storage_mib;
        
    
        
    obj.totalStorageMib = status.total_storage_mib;
        
    
        
    obj.recordingTimeS = status.recording_time_s;
        
    
        
    [NSString stringWithCString:status.media_folder_name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    obj.storageStatus = (MAVSDKCameraStorageStatus)status.storage_status;
        
    
    return obj;
}

mavsdk::Camera::Status translateToCppStatus(MAVSDKCameraStatus* status)
{
    mavsdk::Camera::Status obj;


        
    obj.video_on = status.videoOn;
        
    
        
    obj.photo_interval_on = status.photoIntervalOn;
        
    
        
    obj.used_storage_mib = status.usedStorageMib;
        
    
        
    obj.available_storage_mib = status.availableStorageMib;
        
    
        
    obj.total_storage_mib = status.totalStorageMib;
        
    
        
    obj.recording_time_s = status.recordingTimeS;
        
    
        
    obj.media_folder_name = [status.mediaFolderName UTF8String];
        
    
        
    obj.storage_status = translateToCppStorageStatus(status.storageStatus);
        
    
    return obj;
}



@implementation MAVSDKCameraStatus
@end



MAVSDKCameraOption* translateFromCppOption(mavsdk::Camera::Option option)
{
    MAVSDKCameraOption *obj = [[MAVSDKCameraOption alloc] init];


        
    [NSString stringWithCString:option.option_id.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    [NSString stringWithCString:option.option_description.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Camera::Option translateToCppOption(MAVSDKCameraOption* option)
{
    mavsdk::Camera::Option obj;


        
    obj.option_id = [option.optionId UTF8String];
        
    
        
    obj.option_description = [option.optionDescription UTF8String];
        
    
    return obj;
}



@implementation MAVSDKCameraOption
@end



MAVSDKCameraSetting* translateFromCppSetting(mavsdk::Camera::Setting setting)
{
    MAVSDKCameraSetting *obj = [[MAVSDKCameraSetting alloc] init];


        
    [NSString stringWithCString:setting.setting_id.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    [NSString stringWithCString:setting.setting_description.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    obj.option = translateFromCppOption(setting.option);
        
    
        
    obj.isRange = setting.is_range;
        
    
    return obj;
}

mavsdk::Camera::Setting translateToCppSetting(MAVSDKCameraSetting* setting)
{
    mavsdk::Camera::Setting obj;


        
    obj.setting_id = [setting.settingId UTF8String];
        
    
        
    obj.setting_description = [setting.settingDescription UTF8String];
        
    
        
    obj.option = translateToCppOption(setting.option);
        
    
        
    obj.is_range = setting.isRange;
        
    
    return obj;
}



@implementation MAVSDKCameraSetting
@end



MAVSDKCameraSettingOptions* translateFromCppSettingOptions(mavsdk::Camera::SettingOptions setting_options)
{
    MAVSDKCameraSettingOptions *obj = [[MAVSDKCameraSettingOptions alloc] init];


        
    [NSString stringWithCString:setting_options.setting_id.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    [NSString stringWithCString:setting_options.setting_description.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
            
    for (const auto& elem : setting_options.options) {
        [obj.options addObject:translateFromCppOption(elem)];
    }
            
        
    
        
    obj.isRange = setting_options.is_range;
        
    
    return obj;
}

mavsdk::Camera::SettingOptions translateToCppSettingOptions(MAVSDKCameraSettingOptions* settingOptions)
{
    mavsdk::Camera::SettingOptions obj;


        
    obj.setting_id = [settingOptions.settingId UTF8String];
        
    
        
    obj.setting_description = [settingOptions.settingDescription UTF8String];
        
    
        
            
    for (MAVSDKCameraOption *elem in settingOptions.options) {
        obj.options.push_back(translateToCppOption(elem));
    }
            
        
    
        
    obj.is_range = settingOptions.isRange;
        
    
    return obj;
}



@implementation MAVSDKCameraSettingOptions
@end



MAVSDKCameraInformation* translateFromCppInformation(mavsdk::Camera::Information information)
{
    MAVSDKCameraInformation *obj = [[MAVSDKCameraInformation alloc] init];


        
    [NSString stringWithCString:information.vendor_name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
        
    [NSString stringWithCString:information.model_name.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Camera::Information translateToCppInformation(MAVSDKCameraInformation* information)
{
    mavsdk::Camera::Information obj;


        
    obj.vendor_name = [information.vendorName UTF8String];
        
    
        
    obj.model_name = [information.modelName UTF8String];
        
    
    return obj;
}



@implementation MAVSDKCameraInformation
@end




@implementation MAVSDKCamera

mavsdk::Camera *camera;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    camera = new mavsdk::Camera(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKCameraResult)takePhoto {
    return (MAVSDKCameraResult)camera->take_photo();
}
- (MAVSDKCameraResult)startPhotoInterval:(float)intervalS {
    return (MAVSDKCameraResult)camera->start_photo_interval(intervalS);
}
- (MAVSDKCameraResult)stopPhotoInterval {
    return (MAVSDKCameraResult)camera->stop_photo_interval();
}
- (MAVSDKCameraResult)startVideo {
    return (MAVSDKCameraResult)camera->start_video();
}
- (MAVSDKCameraResult)stopVideo {
    return (MAVSDKCameraResult)camera->stop_video();
}
- (MAVSDKCameraResult)startVideoStreaming {
    return (MAVSDKCameraResult)camera->start_video_streaming();
}
- (MAVSDKCameraResult)stopVideoStreaming {
    return (MAVSDKCameraResult)camera->stop_video_streaming();
}
- (MAVSDKCameraResult)setMode:(MAVSDKCameraMode)mode {
    return (MAVSDKCameraResult)camera->set_mode((mavsdk::Camera::Mode)mode);
}








- (MAVSDKCameraResult)setSetting:(MAVSDKCameraSetting*)setting {
    return (MAVSDKCameraResult)camera->set_setting(translateToCppSetting(setting));
}

- (MAVSDKCameraResult)formatStorage {
    return (MAVSDKCameraResult)camera->format_storage();
}


@end

