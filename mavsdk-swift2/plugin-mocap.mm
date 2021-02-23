#import "plugin-mocap.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mocap/mocap.h>

@implementation MAVSDKMocap

mavsdk::Mocap *mocap;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    mocap = new mavsdk::Mocap(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}

- (MAVSDKMocapResult)setVisionPositionEstimate:(MAVSDKMocapVisionPositionEstimate*)visionPositionEstimate {
    return (MAVSDKMocapResult)mocap->set_vision_position_estimate(translateToCppVisionPositionEstimate(visionPositionEstimate));
}
- (MAVSDKMocapResult)setAttitudePositionMocap:(MAVSDKMocapAttitudePositionMocap*)attitudePositionMocap {
    return (MAVSDKMocapResult)mocap->set_attitude_position_mocap(translateToCppAttitudePositionMocap(attitudePositionMocap));
}
- (MAVSDKMocapResult)setOdometry:(MAVSDKMocapOdometry*)odometry {
    return (MAVSDKMocapResult)mocap->set_odometry(translateToCppOdometry(odometry));
}





MAVSDKMocapPositionBody* translateFromCppPositionBody(mavsdk::Mocap::PositionBody position_body)
{
    MAVSDKMocapPositionBody *obj = [[MAVSDKMocapPositionBody alloc] init];


        
    obj.xM = position_body.x_m;
        
    
        
    obj.yM = position_body.y_m;
        
    
        
    obj.zM = position_body.z_m;
        
    
    return obj;
}

mavsdk::Mocap::PositionBody translateToCppPositionBody(MAVSDKMocapPositionBody* positionBody)
{
    mavsdk::Mocap::PositionBody obj;


        
    obj.x_m = positionBody.xM;
        
    
        
    obj.y_m = positionBody.yM;
        
    
        
    obj.z_m = positionBody.zM;
        
    
    return obj;
}



MAVSDKMocapAngleBody* translateFromCppAngleBody(mavsdk::Mocap::AngleBody angle_body)
{
    MAVSDKMocapAngleBody *obj = [[MAVSDKMocapAngleBody alloc] init];


        
    obj.rollRad = angle_body.roll_rad;
        
    
        
    obj.pitchRad = angle_body.pitch_rad;
        
    
        
    obj.yawRad = angle_body.yaw_rad;
        
    
    return obj;
}

mavsdk::Mocap::AngleBody translateToCppAngleBody(MAVSDKMocapAngleBody* angleBody)
{
    mavsdk::Mocap::AngleBody obj;


        
    obj.roll_rad = angleBody.rollRad;
        
    
        
    obj.pitch_rad = angleBody.pitchRad;
        
    
        
    obj.yaw_rad = angleBody.yawRad;
        
    
    return obj;
}



MAVSDKMocapSpeedBody* translateFromCppSpeedBody(mavsdk::Mocap::SpeedBody speed_body)
{
    MAVSDKMocapSpeedBody *obj = [[MAVSDKMocapSpeedBody alloc] init];


        
    obj.xMS = speed_body.x_m_s;
        
    
        
    obj.yMS = speed_body.y_m_s;
        
    
        
    obj.zMS = speed_body.z_m_s;
        
    
    return obj;
}

mavsdk::Mocap::SpeedBody translateToCppSpeedBody(MAVSDKMocapSpeedBody* speedBody)
{
    mavsdk::Mocap::SpeedBody obj;


        
    obj.x_m_s = speedBody.xMS;
        
    
        
    obj.y_m_s = speedBody.yMS;
        
    
        
    obj.z_m_s = speedBody.zMS;
        
    
    return obj;
}



MAVSDKMocapAngularVelocityBody* translateFromCppAngularVelocityBody(mavsdk::Mocap::AngularVelocityBody angular_velocity_body)
{
    MAVSDKMocapAngularVelocityBody *obj = [[MAVSDKMocapAngularVelocityBody alloc] init];


        
    obj.rollRadS = angular_velocity_body.roll_rad_s;
        
    
        
    obj.pitchRadS = angular_velocity_body.pitch_rad_s;
        
    
        
    obj.yawRadS = angular_velocity_body.yaw_rad_s;
        
    
    return obj;
}

mavsdk::Mocap::AngularVelocityBody translateToCppAngularVelocityBody(MAVSDKMocapAngularVelocityBody* angularVelocityBody)
{
    mavsdk::Mocap::AngularVelocityBody obj;


        
    obj.roll_rad_s = angularVelocityBody.rollRadS;
        
    
        
    obj.pitch_rad_s = angularVelocityBody.pitchRadS;
        
    
        
    obj.yaw_rad_s = angularVelocityBody.yawRadS;
        
    
    return obj;
}



MAVSDKMocapCovariance* translateFromCppCovariance(mavsdk::Mocap::Covariance covariance)
{
    MAVSDKMocapCovariance *obj = [[MAVSDKMocapCovariance alloc] init];


        
            for (const auto& elem : covariance.covariance_matrix()) {
                [obj.covarianceMatrix addObject:elem];
            }
        
    
    return obj;
}

mavsdk::Mocap::Covariance translateToCppCovariance(MAVSDKMocapCovariance* covariance)
{
    mavsdk::Mocap::Covariance obj;


        
            for (MAVSDKMocapfloat *elem in covariance.covarianceMatrix) {
                [obj.covariance_matrix push_back(elem)];
            }
        
    
    return obj;
}



MAVSDKMocapQuaternion* translateFromCppQuaternion(mavsdk::Mocap::Quaternion quaternion)
{
    MAVSDKMocapQuaternion *obj = [[MAVSDKMocapQuaternion alloc] init];


        
    obj.w = quaternion.w;
        
    
        
    obj.x = quaternion.x;
        
    
        
    obj.y = quaternion.y;
        
    
        
    obj.z = quaternion.z;
        
    
    return obj;
}

mavsdk::Mocap::Quaternion translateToCppQuaternion(MAVSDKMocapQuaternion* quaternion)
{
    mavsdk::Mocap::Quaternion obj;


        
    obj.w = quaternion.w;
        
    
        
    obj.x = quaternion.x;
        
    
        
    obj.y = quaternion.y;
        
    
        
    obj.z = quaternion.z;
        
    
    return obj;
}



MAVSDKMocapVisionPositionEstimate* translateFromCppVisionPositionEstimate(mavsdk::Mocap::VisionPositionEstimate vision_position_estimate)
{
    MAVSDKMocapVisionPositionEstimate *obj = [[MAVSDKMocapVisionPositionEstimate alloc] init];


        
    obj.timeUsec = vision_position_estimate.time_usec;
        
    
        
    obj.positionBody = translateFromCppPositionBody(vision_position_estimate.position_body);
        
    
        
    obj.angleBody = translateFromCppAngleBody(vision_position_estimate.angle_body);
        
    
        
    obj.poseCovariance = translateFromCppCovariance(vision_position_estimate.pose_covariance);
        
    
    return obj;
}

mavsdk::Mocap::VisionPositionEstimate translateToCppVisionPositionEstimate(MAVSDKMocapVisionPositionEstimate* visionPositionEstimate)
{
    mavsdk::Mocap::VisionPositionEstimate obj;


        
    obj.time_usec = visionPositionEstimate.timeUsec;
        
    
        
    obj.position_body = translateToCppPositionBody(visionPositionEstimate.positionBody);
        
    
        
    obj.angle_body = translateToCppAngleBody(visionPositionEstimate.angleBody);
        
    
        
    obj.pose_covariance = translateToCppCovariance(visionPositionEstimate.poseCovariance);
        
    
    return obj;
}



MAVSDKMocapAttitudePositionMocap* translateFromCppAttitudePositionMocap(mavsdk::Mocap::AttitudePositionMocap attitude_position_mocap)
{
    MAVSDKMocapAttitudePositionMocap *obj = [[MAVSDKMocapAttitudePositionMocap alloc] init];


        
    obj.timeUsec = attitude_position_mocap.time_usec;
        
    
        
    obj.q = translateFromCppQuaternion(attitude_position_mocap.q);
        
    
        
    obj.positionBody = translateFromCppPositionBody(attitude_position_mocap.position_body);
        
    
        
    obj.poseCovariance = translateFromCppCovariance(attitude_position_mocap.pose_covariance);
        
    
    return obj;
}

mavsdk::Mocap::AttitudePositionMocap translateToCppAttitudePositionMocap(MAVSDKMocapAttitudePositionMocap* attitudePositionMocap)
{
    mavsdk::Mocap::AttitudePositionMocap obj;


        
    obj.time_usec = attitudePositionMocap.timeUsec;
        
    
        
    obj.q = translateToCppQuaternion(attitudePositionMocap.q);
        
    
        
    obj.position_body = translateToCppPositionBody(attitudePositionMocap.positionBody);
        
    
        
    obj.pose_covariance = translateToCppCovariance(attitudePositionMocap.poseCovariance);
        
    
    return obj;
}



MAVSDKMocapOdometry* translateFromCppOdometry(mavsdk::Mocap::Odometry odometry)
{
    MAVSDKMocapOdometry *obj = [[MAVSDKMocapOdometry alloc] init];


        
    obj.timeUsec = odometry.time_usec;
        
    
        
    obj.frameId = (MAVSDKMocapMavFrame)odometry.frame_id;
        
    
        
    obj.positionBody = translateFromCppPositionBody(odometry.position_body);
        
    
        
    obj.q = translateFromCppQuaternion(odometry.q);
        
    
        
    obj.speedBody = translateFromCppSpeedBody(odometry.speed_body);
        
    
        
    obj.angularVelocityBody = translateFromCppAngularVelocityBody(odometry.angular_velocity_body);
        
    
        
    obj.poseCovariance = translateFromCppCovariance(odometry.pose_covariance);
        
    
        
    obj.velocityCovariance = translateFromCppCovariance(odometry.velocity_covariance);
        
    
    return obj;
}

mavsdk::Mocap::Odometry translateToCppOdometry(MAVSDKMocapOdometry* odometry)
{
    mavsdk::Mocap::Odometry obj;


        
    obj.time_usec = odometry.timeUsec;
        
    
        
    obj.frame_id = (mavsdk::Mocap::Odometry::MavFrame)(odometry.frameId);
        
    
        
    obj.position_body = translateToCppPositionBody(odometry.positionBody);
        
    
        
    obj.q = translateToCppQuaternion(odometry.q);
        
    
        
    obj.speed_body = translateToCppSpeedBody(odometry.speedBody);
        
    
        
    obj.angular_velocity_body = translateToCppAngularVelocityBody(odometry.angularVelocityBody);
        
    
        
    obj.pose_covariance = translateToCppCovariance(odometry.poseCovariance);
        
    
        
    obj.velocity_covariance = translateToCppCovariance(odometry.velocityCovariance);
        
    
    return obj;
}






@end