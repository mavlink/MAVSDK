#import "plugin-gimbal.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/gimbal/gimbal.h>

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


        
    obj.control_mode = (mavsdk::Gimbal::ControlStatus::ControlMode)(controlStatus.controlMode);
        
    
        
    obj.sysid_primary_control = controlStatus.sysidPrimaryControl;
        
    
        
    obj.compid_primary_control = controlStatus.compidPrimaryControl;
        
    
        
    obj.sysid_secondary_control = controlStatus.sysidSecondaryControl;
        
    
        
    obj.compid_secondary_control = controlStatus.compidSecondaryControl;
        
    
    return obj;
}






@end