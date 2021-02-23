#ifndef plugin_gimbal_h
#define plugin_gimbal_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKGimbalGimbalMode)  {
    MAVSDKGimbalGimbalModeYawFollow,
    MAVSDKGimbalGimbalModeYawLock,
};


typedef NS_ENUM(NSInteger, MAVSDKGimbalControlMode)  {
    MAVSDKGimbalControlModeNone,
    MAVSDKGimbalControlModePrimary,
    MAVSDKGimbalControlModeSecondary,
};




@interface MAVSDKGimbalControlStatus : NSObject


@property (nonatomic, assign) MAVSDKGimbalControlMode controlMode;
@property (nonatomic, assign) NSInteger sysidPrimaryControl;
@property (nonatomic, assign) NSInteger compidPrimaryControl;
@property (nonatomic, assign) NSInteger sysidSecondaryControl;
@property (nonatomic, assign) NSInteger compidSecondaryControl;


@end






typedef NS_ENUM(NSInteger, MAVSDKGimbalResult)  {
    MAVSDKGimbalResultUnknown,
    MAVSDKGimbalResultSuccess,
    MAVSDKGimbalResultError,
    MAVSDKGimbalResultTimeout,
    MAVSDKGimbalResultUnsupported,
};



@interface MAVSDKGimbal : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKGimbalResult)setPitchAndYaw :( float) pitchDeg :( float) yawDeg;
- (MAVSDKGimbalResult)setPitchRateAndYawRate :( float) pitchRateDegS :( float) yawRateDegS;
- (MAVSDKGimbalResult)setMode :( MAVSDKGimbalGimbalMode) gimbalMode;
- (MAVSDKGimbalResult)setRoiLocation :( double) latitudeDeg :( double) longitudeDeg :( float) altitudeM;
- (MAVSDKGimbalResult)takeControl :( MAVSDKGimbalControlMode) controlMode;
- (MAVSDKGimbalResult)releaseControl ;



@end

#endif // plugin_gimbal_h