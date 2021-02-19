#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/gimbal/gimbal.h>



typedef NS_ENUM(NSInteger, MVSGimbalMode)  {
    MVSGimbalModeYawFollow,
    MVSGimbalModeYawLock,
};


typedef NS_ENUM(NSInteger, MVSControlMode)  {
    MVSControlModeNone,
    MVSControlModePrimary,
    MVSControlModeSecondary,
};




@interface MVSControlStatus : NSObject



@property (nonatomic, assign) MVSControlMode controlMode;


@property (nonatomic, assign) NSInteger sysidPrimaryControl;


@property (nonatomic, assign) NSInteger compidPrimaryControl;


@property (nonatomic, assign) NSInteger sysidSecondaryControl;


@property (nonatomic, assign) NSInteger compidSecondaryControl;



@end






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultError,
    MVSResultTimeout,
    MVSResultUnsupported,
};



@interface MVSGimbal : NSObject

- (MVSResult)setPitchAndYaw
:(
float)
pitchDeg :(
float)
yawDeg;
- (MVSResult)setPitchRateAndYawRate
:(
float)
pitchRateDegS :(
float)
yawRateDegS;
- (MVSResult)setMode
:(
MVSGimbalMode*)
gimbalMode;
- (MVSResult)setRoiLocation
:(
double)
latitudeDeg :(
double)
longitudeDeg :(
float)
altitudeM;
- (MVSResult)takeControl
:(
MVSControlMode*)
controlMode;
- (MVSResult)releaseControl
;



@end