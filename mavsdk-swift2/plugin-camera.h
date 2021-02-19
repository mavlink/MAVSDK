#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>



typedef NS_ENUM(NSInteger, MVSMode)  {
    MVSModeUnknown,
    MVSModePhoto,
    MVSModeVideo,
};






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultInProgress,
    MVSResultBusy,
    MVSResultDenied,
    MVSResultError,
    MVSResultTimeout,
    MVSResultWrongArgument,
};



@interface MVSPosition : NSObject



@property (nonatomic, assign) double latitudeDeg;


@property (nonatomic, assign) double longitudeDeg;


@property (nonatomic, assign) float absoluteAltitudeM;


@property (nonatomic, assign) float relativeAltitudeM;



@end




@interface MVSQuaternion : NSObject



@property (nonatomic, assign) float w;


@property (nonatomic, assign) float x;


@property (nonatomic, assign) float y;


@property (nonatomic, assign) float z;



@end




@interface MVSEulerAngle : NSObject



@property (nonatomic, assign) float rollDeg;


@property (nonatomic, assign) float pitchDeg;


@property (nonatomic, assign) float yawDeg;



@end




@interface MVSCaptureInfo : NSObject



@property (nonatomic, strong) MVSPosition *position;


@property (nonatomic, strong) MVSQuaternion *attitudeQuaternion;


@property (nonatomic, strong) MVSEulerAngle *attitudeEulerAngle;


@property (nonatomic, assign) UInt64 timeUtcUs;


@property (nonatomic, assign) BOOL isSuccess;


@property (nonatomic, assign) NSInteger index;


@property (nonatomic, strong) NSString *fileUrl;



@end




@interface MVSVideoStreamSettings : NSObject



@property (nonatomic, assign) float frameRateHz;


@property (nonatomic, assign) UInt32 horizontalResolutionPix;


@property (nonatomic, assign) UInt32 verticalResolutionPix;


@property (nonatomic, assign) UInt32 bitRateBS;


@property (nonatomic, assign) UInt32 rotationDeg;


@property (nonatomic, strong) NSString *uri;



@end





@interface MVSVideoStreamInfo : NSObject



typedef NS_ENUM(NSInteger, MVSStatus)  {
    MVSStatusNotRunning,
    MVSStatusInProgress,
};


@property (nonatomic, strong) MVSVideoStreamSettings *settings;

    
@property (nonatomic, assign) MVSStatus status;



@end





@interface MVSStatus : NSObject



typedef NS_ENUM(NSInteger, MVSStorageStatus)  {
    MVSStorageStatusNotAvailable,
    MVSStorageStatusUnformatted,
    MVSStorageStatusFormatted,
};


@property (nonatomic, assign) BOOL videoOn;


@property (nonatomic, assign) BOOL photoIntervalOn;


@property (nonatomic, assign) float usedStorageMib;


@property (nonatomic, assign) float availableStorageMib;


@property (nonatomic, assign) float totalStorageMib;


@property (nonatomic, assign) float recordingTimeS;


@property (nonatomic, strong) NSString *mediaFolderName;


@property (nonatomic, assign) MVSStorageStatus storageStatus;



@end




@interface MVSOption : NSObject



@property (nonatomic, strong) NSString *optionId;


@property (nonatomic, strong) NSString *optionDescription;



@end




@interface MVSSetting : NSObject



@property (nonatomic, strong) NSString *settingId;


@property (nonatomic, strong) NSString *settingDescription;


@property (nonatomic, strong) MVSOption *option;


@property (nonatomic, assign) BOOL isRange;



@end




@interface MVSSettingOptions : NSObject



@property (nonatomic, strong) NSString *settingId;


@property (nonatomic, strong) NSString *settingDescription;


@property (nonatomic, strong) NSMutableArray *options;


@property (nonatomic, assign) BOOL isRange;



@end




@interface MVSInformation : NSObject



@property (nonatomic, strong) NSString *vendorName;


@property (nonatomic, strong) NSString *modelName;



@end




@interface MVSCamera : NSObject

- (MVSResult)takePhoto
;
- (MVSResult)startPhotoInterval
:(
float)
intervalS;
- (MVSResult)stopPhotoInterval
;
- (MVSResult)startVideo
;
- (MVSResult)stopVideo
;
- (MVSResult)startVideoStreaming
;
- (MVSResult)stopVideoStreaming
;
- (MVSResult)setMode
:(
MVSMode*)
mode;







- (MVSResult)setSetting
:(
MVSSetting*)
setting;

- (MVSResult)formatStorage
;


@end
