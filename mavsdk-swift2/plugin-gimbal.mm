#import "plugin-gimbal.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/gimbal/gimbal.h>


MAVSDKGimbalGimbalMode translateFromCppGimbalMode(mavsdk::Gimbal::GimbalMode gimbal_mode)
{
    switch (gimbal_mode) {
        default:
            NSLog(@"Unknown gimbal_mode enum value: %d", static_cast<int>(gimbal_mode));
        // FALLTHROUGH
        case mavsdk::Gimbal::GimbalMode::YawFollow:
            return MAVSDKGimbalGimbalModeYawFollow;
        case mavsdk::Gimbal::GimbalMode::YawLock:
            return MAVSDKGimbalGimbalModeYawLock;
    }
}

mavsdk::Gimbal::GimbalMode translateToCppGimbalMode(MAVSDKGimbalGimbalMode gimbalMode)
{
    switch (gimbalMode) {
        default:
            NSLog(@"Unknown GimbalMode enum value: %d", static_cast<int>(gimbalMode));
        // FALLTHROUGH
        case MAVSDKGimbalGimbalModeYawFollow:
            return mavsdk::Gimbal::GimbalMode::YawFollow;
        case MAVSDKGimbalGimbalModeYawLock:
            return mavsdk::Gimbal::GimbalMode::YawLock;
    }
}

MAVSDKGimbalControlMode translateFromCppControlMode(mavsdk::Gimbal::ControlMode control_mode)
{
    switch (control_mode) {
        default:
            NSLog(@"Unknown control_mode enum value: %d", static_cast<int>(control_mode));
        // FALLTHROUGH
        case mavsdk::Gimbal::ControlMode::None:
            return MAVSDKGimbalControlModeNone;
        case mavsdk::Gimbal::ControlMode::Primary:
            return MAVSDKGimbalControlModePrimary;
        case mavsdk::Gimbal::ControlMode::Secondary:
            return MAVSDKGimbalControlModeSecondary;
    }
}

mavsdk::Gimbal::ControlMode translateToCppControlMode(MAVSDKGimbalControlMode controlMode)
{
    switch (controlMode) {
        default:
            NSLog(@"Unknown ControlMode enum value: %d", static_cast<int>(controlMode));
        // FALLTHROUGH
        case MAVSDKGimbalControlModeNone:
            return mavsdk::Gimbal::ControlMode::None;
        case MAVSDKGimbalControlModePrimary:
            return mavsdk::Gimbal::ControlMode::Primary;
        case MAVSDKGimbalControlModeSecondary:
            return mavsdk::Gimbal::ControlMode::Secondary;
    }
}



MAVSDKGimbalControlStatus* translateFromCppControlStatus(mavsdk::Gimbal::ControlStatus control_status)
{
    MAVSDKGimbalControlStatus *obj = [[MAVSDKGimbalControlStatus alloc] init];


        
    obj.controlMode = (MAVSDKGimbalControlMode)control_status.control_mode;
        
    
        
    obj.sysidPrimaryControl = control_status.sysid_primary_control;
        
    
        
    obj.compidPrimaryControl = control_status.compid_primary_control;
        
    
        
    obj.sysidSecondaryControl = control_status.sysid_secondary_control;
        
    
        
    obj.compidSecondaryControl = control_status.compid_secondary_control;
        
    
    return obj;
}

mavsdk::Gimbal::ControlStatus translateToCppControlStatus(MAVSDKGimbalControlStatus* controlStatus)
{
    mavsdk::Gimbal::ControlStatus obj;


        
    obj.control_mode = translateToCppControlMode(controlStatus.controlMode);
        
    
        
    obj.sysid_primary_control = controlStatus.sysidPrimaryControl;
        
    
        
    obj.compid_primary_control = controlStatus.compidPrimaryControl;
        
    
        
    obj.sysid_secondary_control = controlStatus.sysidSecondaryControl;
        
    
        
    obj.compid_secondary_control = controlStatus.compidSecondaryControl;
        
    
    return obj;
}



@implementation MAVSDKGimbalControlStatus
@end








@implementation MAVSDKGimbal

mavsdk::Gimbal *gimbal;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    gimbal = new mavsdk::Gimbal(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKGimbalResult)setPitchAndYaw:(float)pitchDeg :(float)yawDeg {
    return (MAVSDKGimbalResult)gimbal->set_pitch_and_yaw(pitchDeg, yawDeg);
}
- (MAVSDKGimbalResult)setPitchRateAndYawRate:(float)pitchRateDegS :(float)yawRateDegS {
    return (MAVSDKGimbalResult)gimbal->set_pitch_rate_and_yaw_rate(pitchRateDegS, yawRateDegS);
}
- (MAVSDKGimbalResult)setMode:(MAVSDKGimbalGimbalMode)gimbalMode {
    return (MAVSDKGimbalResult)gimbal->set_mode((mavsdk::Gimbal::GimbalMode)gimbalMode);
}
- (MAVSDKGimbalResult)setRoiLocation:(double)latitudeDeg :(double)longitudeDeg :(float)altitudeM {
    return (MAVSDKGimbalResult)gimbal->set_roi_location(latitudeDeg, longitudeDeg, altitudeM);
}
- (MAVSDKGimbalResult)takeControl:(MAVSDKGimbalControlMode)controlMode {
    return (MAVSDKGimbalResult)gimbal->take_control((mavsdk::Gimbal::ControlMode)controlMode);
}
- (MAVSDKGimbalResult)releaseControl {
    return (MAVSDKGimbalResult)gimbal->release_control();
}



@end

