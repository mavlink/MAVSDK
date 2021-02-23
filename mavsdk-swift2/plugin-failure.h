#ifndef plugin_failure_h
#define plugin_failure_h

#import <Foundation/Foundation.h>

@class MavsdkSwift2Impl;



typedef NS_ENUM(NSInteger, MAVSDKFailureFailureUnit)  {
    MAVSDKFailureFailureUnitSensorGyro,
    MAVSDKFailureFailureUnitSensorAccel,
    MAVSDKFailureFailureUnitSensorMag,
    MAVSDKFailureFailureUnitSensorBaro,
    MAVSDKFailureFailureUnitSensorGps,
    MAVSDKFailureFailureUnitSensorOpticalFlow,
    MAVSDKFailureFailureUnitSensorVio,
    MAVSDKFailureFailureUnitSensorDistanceSensor,
    MAVSDKFailureFailureUnitSensorAirspeed,
    MAVSDKFailureFailureUnitSystemBattery,
    MAVSDKFailureFailureUnitSystemMotor,
    MAVSDKFailureFailureUnitSystemServo,
    MAVSDKFailureFailureUnitSystemAvoidance,
    MAVSDKFailureFailureUnitSystemRcSignal,
    MAVSDKFailureFailureUnitSystemMavlinkSignal,
};


typedef NS_ENUM(NSInteger, MAVSDKFailureFailureType)  {
    MAVSDKFailureFailureTypeOk,
    MAVSDKFailureFailureTypeOff,
    MAVSDKFailureFailureTypeStuck,
    MAVSDKFailureFailureTypeGarbage,
    MAVSDKFailureFailureTypeWrong,
    MAVSDKFailureFailureTypeSlow,
    MAVSDKFailureFailureTypeDelayed,
    MAVSDKFailureFailureTypeIntermittent,
};






typedef NS_ENUM(NSInteger, MAVSDKFailureResult)  {
    MAVSDKFailureResultUnknown,
    MAVSDKFailureResultSuccess,
    MAVSDKFailureResultNoSystem,
    MAVSDKFailureResultConnectionError,
    MAVSDKFailureResultUnsupported,
    MAVSDKFailureResultDenied,
    MAVSDKFailureResultDisabled,
    MAVSDKFailureResultTimeout,
};



@interface MAVSDKFailure : NSObject

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl;
- (MAVSDKFailureResult)inject :( MAVSDKFailureFailureUnit) failureUnit :( MAVSDKFailureFailureType) failureType :( NSInteger) instance;


@end

#endif // plugin_failure_h