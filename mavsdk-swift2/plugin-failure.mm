#import "plugin-failure.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/failure/failure.h>


MAVSDKFailureFailureUnit translateFromCppFailureUnit(mavsdk::Failure::FailureUnit failure_unit)
{
    switch (failure_unit) {
        default:
            NSLog(@"Unknown failure_unit enum value: %d", static_cast<int>(failure_unit));
        // FALLTHROUGH
        case mavsdk::Failure::FailureUnit::SensorGyro:
            return MAVSDKFailureFailureUnitSensorGyro;
        case mavsdk::Failure::FailureUnit::SensorAccel:
            return MAVSDKFailureFailureUnitSensorAccel;
        case mavsdk::Failure::FailureUnit::SensorMag:
            return MAVSDKFailureFailureUnitSensorMag;
        case mavsdk::Failure::FailureUnit::SensorBaro:
            return MAVSDKFailureFailureUnitSensorBaro;
        case mavsdk::Failure::FailureUnit::SensorGps:
            return MAVSDKFailureFailureUnitSensorGps;
        case mavsdk::Failure::FailureUnit::SensorOpticalFlow:
            return MAVSDKFailureFailureUnitSensorOpticalFlow;
        case mavsdk::Failure::FailureUnit::SensorVio:
            return MAVSDKFailureFailureUnitSensorVio;
        case mavsdk::Failure::FailureUnit::SensorDistanceSensor:
            return MAVSDKFailureFailureUnitSensorDistanceSensor;
        case mavsdk::Failure::FailureUnit::SensorAirspeed:
            return MAVSDKFailureFailureUnitSensorAirspeed;
        case mavsdk::Failure::FailureUnit::SystemBattery:
            return MAVSDKFailureFailureUnitSystemBattery;
        case mavsdk::Failure::FailureUnit::SystemMotor:
            return MAVSDKFailureFailureUnitSystemMotor;
        case mavsdk::Failure::FailureUnit::SystemServo:
            return MAVSDKFailureFailureUnitSystemServo;
        case mavsdk::Failure::FailureUnit::SystemAvoidance:
            return MAVSDKFailureFailureUnitSystemAvoidance;
        case mavsdk::Failure::FailureUnit::SystemRcSignal:
            return MAVSDKFailureFailureUnitSystemRcSignal;
        case mavsdk::Failure::FailureUnit::SystemMavlinkSignal:
            return MAVSDKFailureFailureUnitSystemMavlinkSignal;
    }
}

mavsdk::Failure::FailureUnit translateToCppFailureUnit(MAVSDKFailureFailureUnit failureUnit)
{
    switch (failureUnit) {
        default:
            NSLog(@"Unknown FailureUnit enum value: %d", static_cast<int>(failureUnit));
        // FALLTHROUGH
        case MAVSDKFailureFailureUnitSensorGyro:
            return mavsdk::Failure::FailureUnit::SensorGyro;
        case MAVSDKFailureFailureUnitSensorAccel:
            return mavsdk::Failure::FailureUnit::SensorAccel;
        case MAVSDKFailureFailureUnitSensorMag:
            return mavsdk::Failure::FailureUnit::SensorMag;
        case MAVSDKFailureFailureUnitSensorBaro:
            return mavsdk::Failure::FailureUnit::SensorBaro;
        case MAVSDKFailureFailureUnitSensorGps:
            return mavsdk::Failure::FailureUnit::SensorGps;
        case MAVSDKFailureFailureUnitSensorOpticalFlow:
            return mavsdk::Failure::FailureUnit::SensorOpticalFlow;
        case MAVSDKFailureFailureUnitSensorVio:
            return mavsdk::Failure::FailureUnit::SensorVio;
        case MAVSDKFailureFailureUnitSensorDistanceSensor:
            return mavsdk::Failure::FailureUnit::SensorDistanceSensor;
        case MAVSDKFailureFailureUnitSensorAirspeed:
            return mavsdk::Failure::FailureUnit::SensorAirspeed;
        case MAVSDKFailureFailureUnitSystemBattery:
            return mavsdk::Failure::FailureUnit::SystemBattery;
        case MAVSDKFailureFailureUnitSystemMotor:
            return mavsdk::Failure::FailureUnit::SystemMotor;
        case MAVSDKFailureFailureUnitSystemServo:
            return mavsdk::Failure::FailureUnit::SystemServo;
        case MAVSDKFailureFailureUnitSystemAvoidance:
            return mavsdk::Failure::FailureUnit::SystemAvoidance;
        case MAVSDKFailureFailureUnitSystemRcSignal:
            return mavsdk::Failure::FailureUnit::SystemRcSignal;
        case MAVSDKFailureFailureUnitSystemMavlinkSignal:
            return mavsdk::Failure::FailureUnit::SystemMavlinkSignal;
    }
}

MAVSDKFailureFailureType translateFromCppFailureType(mavsdk::Failure::FailureType failure_type)
{
    switch (failure_type) {
        default:
            NSLog(@"Unknown failure_type enum value: %d", static_cast<int>(failure_type));
        // FALLTHROUGH
        case mavsdk::Failure::FailureType::Ok:
            return MAVSDKFailureFailureTypeOk;
        case mavsdk::Failure::FailureType::Off:
            return MAVSDKFailureFailureTypeOff;
        case mavsdk::Failure::FailureType::Stuck:
            return MAVSDKFailureFailureTypeStuck;
        case mavsdk::Failure::FailureType::Garbage:
            return MAVSDKFailureFailureTypeGarbage;
        case mavsdk::Failure::FailureType::Wrong:
            return MAVSDKFailureFailureTypeWrong;
        case mavsdk::Failure::FailureType::Slow:
            return MAVSDKFailureFailureTypeSlow;
        case mavsdk::Failure::FailureType::Delayed:
            return MAVSDKFailureFailureTypeDelayed;
        case mavsdk::Failure::FailureType::Intermittent:
            return MAVSDKFailureFailureTypeIntermittent;
    }
}

mavsdk::Failure::FailureType translateToCppFailureType(MAVSDKFailureFailureType failureType)
{
    switch (failureType) {
        default:
            NSLog(@"Unknown FailureType enum value: %d", static_cast<int>(failureType));
        // FALLTHROUGH
        case MAVSDKFailureFailureTypeOk:
            return mavsdk::Failure::FailureType::Ok;
        case MAVSDKFailureFailureTypeOff:
            return mavsdk::Failure::FailureType::Off;
        case MAVSDKFailureFailureTypeStuck:
            return mavsdk::Failure::FailureType::Stuck;
        case MAVSDKFailureFailureTypeGarbage:
            return mavsdk::Failure::FailureType::Garbage;
        case MAVSDKFailureFailureTypeWrong:
            return mavsdk::Failure::FailureType::Wrong;
        case MAVSDKFailureFailureTypeSlow:
            return mavsdk::Failure::FailureType::Slow;
        case MAVSDKFailureFailureTypeDelayed:
            return mavsdk::Failure::FailureType::Delayed;
        case MAVSDKFailureFailureTypeIntermittent:
            return mavsdk::Failure::FailureType::Intermittent;
    }
}








@implementation MAVSDKFailure

mavsdk::Failure *failure;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    failure = new mavsdk::Failure(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKFailureResult)inject:(MAVSDKFailureFailureUnit)failureUnit :(MAVSDKFailureFailureType)failureType :(SInt32)instance {
    return (MAVSDKFailureResult)failure->inject((mavsdk::Failure::FailureUnit)failureUnit, (mavsdk::Failure::FailureType)failureType, instance);
}


@end

