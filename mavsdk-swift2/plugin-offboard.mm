#import "plugin-offboard.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/offboard/offboard.h>

@implementation MAVSDKOffboard

mavsdk::Offboard *offboard;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    offboard = new mavsdk::Offboard(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKOffboardResult)start {
    return (MAVSDKOffboardResult)offboard->start();
}
- (MAVSDKOffboardResult)stop {
    return (MAVSDKOffboardResult)offboard->stop();
}

- (MAVSDKOffboardResult)setAttitude:(MAVSDKOffboardAttitude*)attitude {
    return (MAVSDKOffboardResult)offboard->set_attitude(translateToCppAttitude(attitude));
}
- (MAVSDKOffboardResult)setActuatorControl:(MAVSDKOffboardActuatorControl*)actuatorControl {
    return (MAVSDKOffboardResult)offboard->set_actuator_control(translateToCppActuatorControl(actuatorControl));
}
- (MAVSDKOffboardResult)setAttitudeRate:(MAVSDKOffboardAttitudeRate*)attitudeRate {
    return (MAVSDKOffboardResult)offboard->set_attitude_rate(translateToCppAttitudeRate(attitudeRate));
}
- (MAVSDKOffboardResult)setPositionNed:(MAVSDKOffboardPositionNedYaw*)positionNedYaw {
    return (MAVSDKOffboardResult)offboard->set_position_ned(translateToCppPositionNedYaw(positionNedYaw));
}
- (MAVSDKOffboardResult)setVelocityBody:(MAVSDKOffboardVelocityBodyYawspeed*)velocityBodyYawspeed {
    return (MAVSDKOffboardResult)offboard->set_velocity_body(translateToCppVelocityBodyYawspeed(velocityBodyYawspeed));
}
- (MAVSDKOffboardResult)setVelocityNed:(MAVSDKOffboardVelocityNedYaw*)velocityNedYaw {
    return (MAVSDKOffboardResult)offboard->set_velocity_ned(translateToCppVelocityNedYaw(velocityNedYaw));
}
- (MAVSDKOffboardResult)setPositionVelocityNed:(MAVSDKOffboardPositionNedYaw*)positionNedYaw :(MAVSDKOffboardVelocityNedYaw*)velocityNedYaw {
    return (MAVSDKOffboardResult)offboard->set_position_velocity_ned(translateToCppPositionNedYaw(positionNedYaw), translateToCppVelocityNedYaw(velocityNedYaw));
}





MAVSDKOffboardAttitude* translateFromCppAttitude(mavsdk::Offboard::Attitude attitude)
{
    MAVSDKOffboardAttitude *obj = [[MAVSDKOffboardAttitude alloc] init];


        
    obj.rollDeg = attitude.roll_deg;
        
    
        
    obj.pitchDeg = attitude.pitch_deg;
        
    
        
    obj.yawDeg = attitude.yaw_deg;
        
    
        
    obj.thrustValue = attitude.thrust_value;
        
    
    return obj;
}

mavsdk::Offboard::Attitude translateToCppAttitude(MAVSDKOffboardAttitude* attitude)
{
    mavsdk::Offboard::Attitude obj;


        
    obj.roll_deg = attitude.rollDeg;
        
    
        
    obj.pitch_deg = attitude.pitchDeg;
        
    
        
    obj.yaw_deg = attitude.yawDeg;
        
    
        
    obj.thrust_value = attitude.thrustValue;
        
    
    return obj;
}



MAVSDKOffboardActuatorControlGroup* translateFromCppActuatorControlGroup(mavsdk::Offboard::ActuatorControlGroup actuator_control_group)
{
    MAVSDKOffboardActuatorControlGroup *obj = [[MAVSDKOffboardActuatorControlGroup alloc] init];


        
            //for (const auto& elem : actuatorControlGroup.controls()) {
            //    [obj.controls addObject:elem];
            //}
        
    
    return obj;
}

mavsdk::Offboard::ActuatorControlGroup translateToCppActuatorControlGroup(MAVSDKOffboardActuatorControlGroup* actuatorControlGroup)
{
    mavsdk::Offboard::ActuatorControlGroup obj;


        
            //for (MAVSDKOffboardfloat *elem in actuatorControlGroup.controls) {
            //    [obj.controls push_back(elem)];
            //}
        
    
    return obj;
}



MAVSDKOffboardActuatorControl* translateFromCppActuatorControl(mavsdk::Offboard::ActuatorControl actuator_control)
{
    MAVSDKOffboardActuatorControl *obj = [[MAVSDKOffboardActuatorControl alloc] init];


        
            for (const auto& elem : actuator_control.groups) {
                [obj.groups addObject:translateFromCppActuatorControlGroup(elem)];
            }
        
    
    return obj;
}

mavsdk::Offboard::ActuatorControl translateToCppActuatorControl(MAVSDKOffboardActuatorControl* actuatorControl)
{
    mavsdk::Offboard::ActuatorControl obj;


        
            for (MAVSDKOffboardActuatorControlGroup *elem in actuatorControl.groups) {
                obj.groups.push_back(translateToCppActuatorControlGroup(elem));
            }
        
    
    return obj;
}



MAVSDKOffboardAttitudeRate* translateFromCppAttitudeRate(mavsdk::Offboard::AttitudeRate attitude_rate)
{
    MAVSDKOffboardAttitudeRate *obj = [[MAVSDKOffboardAttitudeRate alloc] init];


        
    obj.rollDegS = attitude_rate.roll_deg_s;
        
    
        
    obj.pitchDegS = attitude_rate.pitch_deg_s;
        
    
        
    obj.yawDegS = attitude_rate.yaw_deg_s;
        
    
        
    obj.thrustValue = attitude_rate.thrust_value;
        
    
    return obj;
}

mavsdk::Offboard::AttitudeRate translateToCppAttitudeRate(MAVSDKOffboardAttitudeRate* attitudeRate)
{
    mavsdk::Offboard::AttitudeRate obj;


        
    obj.roll_deg_s = attitudeRate.rollDegS;
        
    
        
    obj.pitch_deg_s = attitudeRate.pitchDegS;
        
    
        
    obj.yaw_deg_s = attitudeRate.yawDegS;
        
    
        
    obj.thrust_value = attitudeRate.thrustValue;
        
    
    return obj;
}



MAVSDKOffboardPositionNedYaw* translateFromCppPositionNedYaw(mavsdk::Offboard::PositionNedYaw position_ned_yaw)
{
    MAVSDKOffboardPositionNedYaw *obj = [[MAVSDKOffboardPositionNedYaw alloc] init];


        
    obj.northM = position_ned_yaw.north_m;
        
    
        
    obj.eastM = position_ned_yaw.east_m;
        
    
        
    obj.downM = position_ned_yaw.down_m;
        
    
        
    obj.yawDeg = position_ned_yaw.yaw_deg;
        
    
    return obj;
}

mavsdk::Offboard::PositionNedYaw translateToCppPositionNedYaw(MAVSDKOffboardPositionNedYaw* positionNedYaw)
{
    mavsdk::Offboard::PositionNedYaw obj;


        
    obj.north_m = positionNedYaw.northM;
        
    
        
    obj.east_m = positionNedYaw.eastM;
        
    
        
    obj.down_m = positionNedYaw.downM;
        
    
        
    obj.yaw_deg = positionNedYaw.yawDeg;
        
    
    return obj;
}



MAVSDKOffboardVelocityBodyYawspeed* translateFromCppVelocityBodyYawspeed(mavsdk::Offboard::VelocityBodyYawspeed velocity_body_yawspeed)
{
    MAVSDKOffboardVelocityBodyYawspeed *obj = [[MAVSDKOffboardVelocityBodyYawspeed alloc] init];


        
    obj.forwardMS = velocity_body_yawspeed.forward_m_s;
        
    
        
    obj.rightMS = velocity_body_yawspeed.right_m_s;
        
    
        
    obj.downMS = velocity_body_yawspeed.down_m_s;
        
    
        
    obj.yawspeedDegS = velocity_body_yawspeed.yawspeed_deg_s;
        
    
    return obj;
}

mavsdk::Offboard::VelocityBodyYawspeed translateToCppVelocityBodyYawspeed(MAVSDKOffboardVelocityBodyYawspeed* velocityBodyYawspeed)
{
    mavsdk::Offboard::VelocityBodyYawspeed obj;


        
    obj.forward_m_s = velocityBodyYawspeed.forwardMS;
        
    
        
    obj.right_m_s = velocityBodyYawspeed.rightMS;
        
    
        
    obj.down_m_s = velocityBodyYawspeed.downMS;
        
    
        
    obj.yawspeed_deg_s = velocityBodyYawspeed.yawspeedDegS;
        
    
    return obj;
}



MAVSDKOffboardVelocityNedYaw* translateFromCppVelocityNedYaw(mavsdk::Offboard::VelocityNedYaw velocity_ned_yaw)
{
    MAVSDKOffboardVelocityNedYaw *obj = [[MAVSDKOffboardVelocityNedYaw alloc] init];


        
    obj.northMS = velocity_ned_yaw.north_m_s;
        
    
        
    obj.eastMS = velocity_ned_yaw.east_m_s;
        
    
        
    obj.downMS = velocity_ned_yaw.down_m_s;
        
    
        
    obj.yawDeg = velocity_ned_yaw.yaw_deg;
        
    
    return obj;
}

mavsdk::Offboard::VelocityNedYaw translateToCppVelocityNedYaw(MAVSDKOffboardVelocityNedYaw* velocityNedYaw)
{
    mavsdk::Offboard::VelocityNedYaw obj;


        
    obj.north_m_s = velocityNedYaw.northMS;
        
    
        
    obj.east_m_s = velocityNedYaw.eastMS;
        
    
        
    obj.down_m_s = velocityNedYaw.downMS;
        
    
        
    obj.yaw_deg = velocityNedYaw.yawDeg;
        
    
    return obj;
}






@end
