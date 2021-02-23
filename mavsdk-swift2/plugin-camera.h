#ifndef plugin_camera_h
#define plugin_camera_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKCameraMode)  {
    MAVSDKCameraModeUnknown,
    MAVSDKCameraModePhoto,
    MAVSDKCameraModeVideo,
};


typedef NS_ENUM(NSInteger, MAVSDKCameraPhotosRange)  {
    MAVSDKCameraPhotosRangeAll,
    MAVSDKCameraPhotosRangeSinceConnection,
};






typedef NS_ENUM(NSInteger, MAVSDKCameraResult)  {
    MAVSDKCameraResultUnknown,
    MAVSDKCameraResultSuccess,
    MAVSDKCameraResultInProgress,
    MAVSDKCameraResultBusy,
    MAVSDKCameraResultDenied,
    MAVSDKCameraResultError,
    MAVSDKCameraResultTimeout,
    MAVSDKCameraResultWrongArgument,
};



@interface MAVSDKCameraPosition : NSObject


@property (nonatomic, assign) double latitudeDeg;
@property (nonatomic, assign) double longitudeDeg;
@property (nonatomic, assign) float absoluteAltitudeM;
@property (nonatomic, assign) float relativeAltitudeM;


@end




@interface MAVSDKCameraQuaternion : NSObject


@property (nonatomic, assign) float w;
@property (nonatomic, assign) float x;
@property (nonatomic, assign) float y;
@property (nonatomic, assign) float z;


@end




@interface MAVSDKCameraEulerAngle : NSObject


@property (nonatomic, assign) float rollDeg;
@property (nonatomic, assign) float pitchDeg;
@property (nonatomic, assign) float yawDeg;


@end




@interface MAVSDKCameraCaptureInfo : NSObject


@property (nonatomic, strong) MAVSDKCameraPosition *position;
@property (nonatomic, strong) MAVSDKCameraQuaternion *attitudeQuaternion;
@property (nonatomic, strong) MAVSDKCameraEulerAngle *attitudeEulerAngle;
@property (nonatomic, assign) UInt64 timeUtcUs;
@property (nonatomic, assign) BOOL isSuccess;
@property (nonatomic, assign) NSInteger index;
@property (nonatomic, strong) NSString *fileUrl;


@end




@interface MAVSDKCameraVideoStreamSettings : NSObject


@property (nonatomic, assign) float frameRateHz;
@property (nonatomic, assign) UInt32 horizontalResolutionPix;
@property (nonatomic, assign) UInt32 verticalResolutionPix;
@property (nonatomic, assign) UInt32 bitRateBS;
@property (nonatomic, assign) UInt32 rotationDeg;
@property (nonatomic, strong) NSString *uri;


@end





@interface MAVSDKCameraVideoStreamInfo : NSObject



typedef NS_ENUM(NSInteger, MAVSDKCameraVideoStreamStatus)  {
    MAVSDKCameraVideoStreamStatusNotRunning,
    MAVSDKCameraVideoStreamStatusInProgress,
};

@property (nonatomic, strong) MAVSDKCameraVideoStreamSettings *settings;
@property (nonatomic, assign) MAVSDKCameraVideoStreamStatus status;


@end





@interface MAVSDKCameraStatus : NSObject



typedef NS_ENUM(NSInteger, MAVSDKCameraStorageStatus)  {
    MAVSDKCameraStorageStatusNotAvailable,
    MAVSDKCameraStorageStatusUnformatted,
    MAVSDKCameraStorageStatusFormatted,
};

@property (nonatomic, assign) BOOL videoOn;
@property (nonatomic, assign) BOOL photoIntervalOn;
@property (nonatomic, assign) float usedStorageMib;
@property (nonatomic, assign) float availableStorageMib;
@property (nonatomic, assign) float totalStorageMib;
@property (nonatomic, assign) float recordingTimeS;
@property (nonatomic, strong) NSString *mediaFolderName;
@property (nonatomic, assign) MAVSDKCameraStorageStatus storageStatus;


@end




@interface MAVSDKCameraOption : NSObject


@property (nonatomic, strong) NSString *optionId;
@property (nonatomic, strong) NSString *optionDescription;


@end




@interface MAVSDKCameraSetting : NSObject


@property (nonatomic, strong) NSString *settingId;
@property (nonatomic, strong) NSString *settingDescription;
@property (nonatomic, strong) MAVSDKCameraOption *option;
@property (nonatomic, assign) BOOL isRange;


@end




@interface MAVSDKCameraSettingOptions : NSObject


@property (nonatomic, strong) NSString *settingId;
@property (nonatomic, strong) NSString *settingDescription;
@property (nonatomic, strong) NSMutableArray *options; 
@property (nonatomic, assign) BOOL isRange;


@end




@interface MAVSDKCameraInformation : NSObject


@property (nonatomic, strong) NSString *vendorName;
@property (nonatomic, strong) NSString *modelName;


@end




@interface MAVSDKCamera : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKCameraResult)takePhoto ;
- (MAVSDKCameraResult)startPhotoInterval :( float) intervalS;
- (MAVSDKCameraResult)stopPhotoInterval ;
- (MAVSDKCameraResult)startVideo ;
- (MAVSDKCameraResult)stopVideo ;
- (MAVSDKCameraResult)startVideoStreaming ;
- (MAVSDKCameraResult)stopVideoStreaming ;
- (MAVSDKCameraResult)setMode :( MAVSDKCameraMode) mode;








- (MAVSDKCameraResult)setSetting :( MAVSDKCameraSetting*) setting;

- (MAVSDKCameraResult)formatStorage ;


@end

#endif // plugin_camera_h