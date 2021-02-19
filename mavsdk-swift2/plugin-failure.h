#import <Foundation/Foundation.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/failure/failure.h>



typedef NS_ENUM(NSInteger, MVSFailureUnit)  {
    MVSFailureUnitSensorGyro,
    MVSFailureUnitSensorAccel,
    MVSFailureUnitSensorMag,
    MVSFailureUnitSensorBaro,
    MVSFailureUnitSensorGps,
    MVSFailureUnitSensorOpticalFlow,
    MVSFailureUnitSensorVio,
    MVSFailureUnitSensorDistanceSensor,
    MVSFailureUnitSensorAirspeed,
    MVSFailureUnitSystemBattery,
    MVSFailureUnitSystemMotor,
    MVSFailureUnitSystemServo,
    MVSFailureUnitSystemAvoidance,
    MVSFailureUnitSystemRcSignal,
    MVSFailureUnitSystemMavlinkSignal,
};


typedef NS_ENUM(NSInteger, MVSFailureType)  {
    MVSFailureTypeOk,
    MVSFailureTypeOff,
    MVSFailureTypeStuck,
    MVSFailureTypeGarbage,
    MVSFailureTypeWrong,
    MVSFailureTypeSlow,
    MVSFailureTypeDelayed,
    MVSFailureTypeIntermittent,
};






typedef NS_ENUM(NSInteger, MVSResult)  {
    MVSResultUnknown,
    MVSResultSuccess,
    MVSResultNoSystem,
    MVSResultConnectionError,
    MVSResultUnsupported,
    MVSResultDenied,
    MVSResultDisabled,
    MVSResultTimeout,
};



@interface MVSFailure : NSObject

- (MVSResult)inject
:(
MVSFailureUnit*)
failureUnit :(
MVSFailureType*)
failureType :(
NSInteger)
instance;


@end