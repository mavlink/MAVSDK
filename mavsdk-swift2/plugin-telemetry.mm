#import "plugin-telemetry.h"
#import "mavsdk-swift2-impl.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

@implementation MAVSDKTelemetry

mavsdk::Telemetry *telemetry;


+(id)alloc{
    return [super alloc];
}

- (id)initWithMavsdkSwift2Impl:(MavsdkSwift2Impl*)mavsdkSwift2Impl {
    telemetry = new mavsdk::Telemetry(*[mavsdkSwift2Impl mavsdkSystem]);
    return [super init];
}




























- (MAVSDKTelemetryResult)setRatePosition:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_position(rateHz);
}
- (MAVSDKTelemetryResult)setRateHome:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_home(rateHz);
}
- (MAVSDKTelemetryResult)setRateInAir:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_in_air(rateHz);
}
- (MAVSDKTelemetryResult)setRateLandedState:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_landed_state(rateHz);
}
- (MAVSDKTelemetryResult)setRateAttitude:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_attitude(rateHz);
}
- (MAVSDKTelemetryResult)setRateCameraAttitude:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_camera_attitude(rateHz);
}
- (MAVSDKTelemetryResult)setRateVelocityNed:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_velocity_ned(rateHz);
}
- (MAVSDKTelemetryResult)setRateGpsInfo:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_gps_info(rateHz);
}
- (MAVSDKTelemetryResult)setRateBattery:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_battery(rateHz);
}
- (MAVSDKTelemetryResult)setRateRcStatus:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_rc_status(rateHz);
}
- (MAVSDKTelemetryResult)setRateActuatorControlTarget:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_actuator_control_target(rateHz);
}
- (MAVSDKTelemetryResult)setRateActuatorOutputStatus:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_actuator_output_status(rateHz);
}
- (MAVSDKTelemetryResult)setRateOdometry:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_odometry(rateHz);
}
- (MAVSDKTelemetryResult)setRatePositionVelocityNed:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_position_velocity_ned(rateHz);
}
- (MAVSDKTelemetryResult)setRateGroundTruth:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_ground_truth(rateHz);
}
- (MAVSDKTelemetryResult)setRateFixedwingMetrics:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_fixedwing_metrics(rateHz);
}
- (MAVSDKTelemetryResult)setRateImu:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_imu(rateHz);
}
- (MAVSDKTelemetryResult)setRateUnixEpochTime:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_unix_epoch_time(rateHz);
}
- (MAVSDKTelemetryResult)setRateDistanceSensor:(double)rateHz {
    return (MAVSDKTelemetryResult)telemetry->set_rate_distance_sensor(rateHz);
}














MAVSDKTelemetryPosition* translateFromCppPosition(mavsdk::Telemetry::Position position)
{
    MAVSDKTelemetryPosition *obj = [[MAVSDKTelemetryPosition alloc] init];


        
    obj.latitudeDeg = position.latitude_deg;
        
    
        
    obj.longitudeDeg = position.longitude_deg;
        
    
        
    obj.absoluteAltitudeM = position.absolute_altitude_m;
        
    
        
    obj.relativeAltitudeM = position.relative_altitude_m;
        
    
    return obj;
}

mavsdk::Telemetry::Position translateToCppPosition(MAVSDKTelemetryPosition* position)
{
    mavsdk::Telemetry::Position obj;


        
    obj.latitude_deg = position.latitudeDeg;
        
    
        
    obj.longitude_deg = position.longitudeDeg;
        
    
        
    obj.absolute_altitude_m = position.absoluteAltitudeM;
        
    
        
    obj.relative_altitude_m = position.relativeAltitudeM;
        
    
    return obj;
}



MAVSDKTelemetryQuaternion* translateFromCppQuaternion(mavsdk::Telemetry::Quaternion quaternion)
{
    MAVSDKTelemetryQuaternion *obj = [[MAVSDKTelemetryQuaternion alloc] init];


        
    obj.w = quaternion.w;
        
    
        
    obj.x = quaternion.x;
        
    
        
    obj.y = quaternion.y;
        
    
        
    obj.z = quaternion.z;
        
    
        
    obj.timestampUs = quaternion.timestamp_us;
        
    
    return obj;
}

mavsdk::Telemetry::Quaternion translateToCppQuaternion(MAVSDKTelemetryQuaternion* quaternion)
{
    mavsdk::Telemetry::Quaternion obj;


        
    obj.w = quaternion.w;
        
    
        
    obj.x = quaternion.x;
        
    
        
    obj.y = quaternion.y;
        
    
        
    obj.z = quaternion.z;
        
    
        
    obj.timestamp_us = quaternion.timestampUs;
        
    
    return obj;
}



MAVSDKTelemetryEulerAngle* translateFromCppEulerAngle(mavsdk::Telemetry::EulerAngle euler_angle)
{
    MAVSDKTelemetryEulerAngle *obj = [[MAVSDKTelemetryEulerAngle alloc] init];


        
    obj.rollDeg = euler_angle.roll_deg;
        
    
        
    obj.pitchDeg = euler_angle.pitch_deg;
        
    
        
    obj.yawDeg = euler_angle.yaw_deg;
        
    
        
    obj.timestampUs = euler_angle.timestamp_us;
        
    
    return obj;
}

mavsdk::Telemetry::EulerAngle translateToCppEulerAngle(MAVSDKTelemetryEulerAngle* eulerAngle)
{
    mavsdk::Telemetry::EulerAngle obj;


        
    obj.roll_deg = eulerAngle.rollDeg;
        
    
        
    obj.pitch_deg = eulerAngle.pitchDeg;
        
    
        
    obj.yaw_deg = eulerAngle.yawDeg;
        
    
        
    obj.timestamp_us = eulerAngle.timestampUs;
        
    
    return obj;
}



MAVSDKTelemetryAngularVelocityBody* translateFromCppAngularVelocityBody(mavsdk::Telemetry::AngularVelocityBody angular_velocity_body)
{
    MAVSDKTelemetryAngularVelocityBody *obj = [[MAVSDKTelemetryAngularVelocityBody alloc] init];


        
    obj.rollRadS = angular_velocity_body.roll_rad_s;
        
    
        
    obj.pitchRadS = angular_velocity_body.pitch_rad_s;
        
    
        
    obj.yawRadS = angular_velocity_body.yaw_rad_s;
        
    
    return obj;
}

mavsdk::Telemetry::AngularVelocityBody translateToCppAngularVelocityBody(MAVSDKTelemetryAngularVelocityBody* angularVelocityBody)
{
    mavsdk::Telemetry::AngularVelocityBody obj;


        
    obj.roll_rad_s = angularVelocityBody.rollRadS;
        
    
        
    obj.pitch_rad_s = angularVelocityBody.pitchRadS;
        
    
        
    obj.yaw_rad_s = angularVelocityBody.yawRadS;
        
    
    return obj;
}



MAVSDKTelemetryGpsInfo* translateFromCppGpsInfo(mavsdk::Telemetry::GpsInfo gps_info)
{
    MAVSDKTelemetryGpsInfo *obj = [[MAVSDKTelemetryGpsInfo alloc] init];


        
    obj.numSatellites = gps_info.num_satellites;
        
    
        
    obj.fixType = (MAVSDKTelemetryFixType)gps_info.fix_type;
        
    
    return obj;
}

mavsdk::Telemetry::GpsInfo translateToCppGpsInfo(MAVSDKTelemetryGpsInfo* gpsInfo)
{
    mavsdk::Telemetry::GpsInfo obj;


        
    obj.num_satellites = gpsInfo.numSatellites;
        
    
        
    obj.fix_type = (mavsdk::Telemetry::GpsInfo::FixType)(gpsInfo.fixType);
        
    
    return obj;
}



MAVSDKTelemetryBattery* translateFromCppBattery(mavsdk::Telemetry::Battery battery)
{
    MAVSDKTelemetryBattery *obj = [[MAVSDKTelemetryBattery alloc] init];


        
    obj.voltageV = battery.voltage_v;
        
    
        
    obj.remainingPercent = battery.remaining_percent;
        
    
    return obj;
}

mavsdk::Telemetry::Battery translateToCppBattery(MAVSDKTelemetryBattery* battery)
{
    mavsdk::Telemetry::Battery obj;


        
    obj.voltage_v = battery.voltageV;
        
    
        
    obj.remaining_percent = battery.remainingPercent;
        
    
    return obj;
}



MAVSDKTelemetryHealth* translateFromCppHealth(mavsdk::Telemetry::Health health)
{
    MAVSDKTelemetryHealth *obj = [[MAVSDKTelemetryHealth alloc] init];


        
    obj.isGyrometerCalibrationOk = health.is_gyrometer_calibration_ok;
        
    
        
    obj.isAccelerometerCalibrationOk = health.is_accelerometer_calibration_ok;
        
    
        
    obj.isMagnetometerCalibrationOk = health.is_magnetometer_calibration_ok;
        
    
        
    obj.isLevelCalibrationOk = health.is_level_calibration_ok;
        
    
        
    obj.isLocalPositionOk = health.is_local_position_ok;
        
    
        
    obj.isGlobalPositionOk = health.is_global_position_ok;
        
    
        
    obj.isHomePositionOk = health.is_home_position_ok;
        
    
    return obj;
}

mavsdk::Telemetry::Health translateToCppHealth(MAVSDKTelemetryHealth* health)
{
    mavsdk::Telemetry::Health obj;


        
    obj.is_gyrometer_calibration_ok = health.isGyrometerCalibrationOk;
        
    
        
    obj.is_accelerometer_calibration_ok = health.isAccelerometerCalibrationOk;
        
    
        
    obj.is_magnetometer_calibration_ok = health.isMagnetometerCalibrationOk;
        
    
        
    obj.is_level_calibration_ok = health.isLevelCalibrationOk;
        
    
        
    obj.is_local_position_ok = health.isLocalPositionOk;
        
    
        
    obj.is_global_position_ok = health.isGlobalPositionOk;
        
    
        
    obj.is_home_position_ok = health.isHomePositionOk;
        
    
    return obj;
}



MAVSDKTelemetryRcStatus* translateFromCppRcStatus(mavsdk::Telemetry::RcStatus rc_status)
{
    MAVSDKTelemetryRcStatus *obj = [[MAVSDKTelemetryRcStatus alloc] init];


        
    obj.wasAvailableOnce = rc_status.was_available_once;
        
    
        
    obj.isAvailable = rc_status.is_available;
        
    
        
    obj.signalStrengthPercent = rc_status.signal_strength_percent;
        
    
    return obj;
}

mavsdk::Telemetry::RcStatus translateToCppRcStatus(MAVSDKTelemetryRcStatus* rcStatus)
{
    mavsdk::Telemetry::RcStatus obj;


        
    obj.was_available_once = rcStatus.wasAvailableOnce;
        
    
        
    obj.is_available = rcStatus.isAvailable;
        
    
        
    obj.signal_strength_percent = rcStatus.signalStrengthPercent;
        
    
    return obj;
}



MAVSDKTelemetryStatusText* translateFromCppStatusText(mavsdk::Telemetry::StatusText status_text)
{
    MAVSDKTelemetryStatusText *obj = [[MAVSDKTelemetryStatusText alloc] init];


        
    obj.type = (MAVSDKTelemetryStatusTextType)status_text.type;
        
    
        
    [NSString stringWithCString:status_text.text.c_str()
                       encoding:[NSString defaultCStringEncoding]];
        
    
    return obj;
}

mavsdk::Telemetry::StatusText translateToCppStatusText(MAVSDKTelemetryStatusText* statusText)
{
    mavsdk::Telemetry::StatusText obj;


        
    obj.type = (mavsdk::Telemetry::StatusText::StatusTextType)(statusText.type);
        
    
        
    obj.text = [statusText.text UTF8String];
        
    
    return obj;
}



MAVSDKTelemetryActuatorControlTarget* translateFromCppActuatorControlTarget(mavsdk::Telemetry::ActuatorControlTarget actuator_control_target)
{
    MAVSDKTelemetryActuatorControlTarget *obj = [[MAVSDKTelemetryActuatorControlTarget alloc] init];


        
    obj.group = actuator_control_target.group;
        
    
        
            for (const auto& elem : actuatorControlTarget.controls()) {
                [obj.controls addObject:elem];
            }
        
    
    return obj;
}

mavsdk::Telemetry::ActuatorControlTarget translateToCppActuatorControlTarget(MAVSDKTelemetryActuatorControlTarget* actuatorControlTarget)
{
    mavsdk::Telemetry::ActuatorControlTarget obj;


        
    obj.group = actuatorControlTarget.group;
        
    
        
            for (MAVSDKTelemetryfloat *elem in actuatorControlTarget.controls) {
                [obj.controls push_back(elem)];
            }
        
    
    return obj;
}



MAVSDKTelemetryActuatorOutputStatus* translateFromCppActuatorOutputStatus(mavsdk::Telemetry::ActuatorOutputStatus actuator_output_status)
{
    MAVSDKTelemetryActuatorOutputStatus *obj = [[MAVSDKTelemetryActuatorOutputStatus alloc] init];


        
    obj.active = actuator_output_status.active;
        
    
        
            for (const auto& elem : actuatorOutputStatus.actuator()) {
                [obj.actuator addObject:elem];
            }
        
    
    return obj;
}

mavsdk::Telemetry::ActuatorOutputStatus translateToCppActuatorOutputStatus(MAVSDKTelemetryActuatorOutputStatus* actuatorOutputStatus)
{
    mavsdk::Telemetry::ActuatorOutputStatus obj;


        
    obj.active = actuatorOutputStatus.active;
        
    
        
            for (MAVSDKTelemetryfloat *elem in actuatorOutputStatus.actuator) {
                [obj.actuator push_back(elem)];
            }
        
    
    return obj;
}



MAVSDKTelemetryCovariance* translateFromCppCovariance(mavsdk::Telemetry::Covariance covariance)
{
    MAVSDKTelemetryCovariance *obj = [[MAVSDKTelemetryCovariance alloc] init];


        
            for (const auto& elem : covariance.covariance_matrix()) {
                [obj.covarianceMatrix addObject:elem];
            }
        
    
    return obj;
}

mavsdk::Telemetry::Covariance translateToCppCovariance(MAVSDKTelemetryCovariance* covariance)
{
    mavsdk::Telemetry::Covariance obj;


        
            for (MAVSDKTelemetryfloat *elem in covariance.covarianceMatrix) {
                [obj.covariance_matrix push_back(elem)];
            }
        
    
    return obj;
}



MAVSDKTelemetryVelocityBody* translateFromCppVelocityBody(mavsdk::Telemetry::VelocityBody velocity_body)
{
    MAVSDKTelemetryVelocityBody *obj = [[MAVSDKTelemetryVelocityBody alloc] init];


        
    obj.xMS = velocity_body.x_m_s;
        
    
        
    obj.yMS = velocity_body.y_m_s;
        
    
        
    obj.zMS = velocity_body.z_m_s;
        
    
    return obj;
}

mavsdk::Telemetry::VelocityBody translateToCppVelocityBody(MAVSDKTelemetryVelocityBody* velocityBody)
{
    mavsdk::Telemetry::VelocityBody obj;


        
    obj.x_m_s = velocityBody.xMS;
        
    
        
    obj.y_m_s = velocityBody.yMS;
        
    
        
    obj.z_m_s = velocityBody.zMS;
        
    
    return obj;
}



MAVSDKTelemetryPositionBody* translateFromCppPositionBody(mavsdk::Telemetry::PositionBody position_body)
{
    MAVSDKTelemetryPositionBody *obj = [[MAVSDKTelemetryPositionBody alloc] init];


        
    obj.xM = position_body.x_m;
        
    
        
    obj.yM = position_body.y_m;
        
    
        
    obj.zM = position_body.z_m;
        
    
    return obj;
}

mavsdk::Telemetry::PositionBody translateToCppPositionBody(MAVSDKTelemetryPositionBody* positionBody)
{
    mavsdk::Telemetry::PositionBody obj;


        
    obj.x_m = positionBody.xM;
        
    
        
    obj.y_m = positionBody.yM;
        
    
        
    obj.z_m = positionBody.zM;
        
    
    return obj;
}



MAVSDKTelemetryOdometry* translateFromCppOdometry(mavsdk::Telemetry::Odometry odometry)
{
    MAVSDKTelemetryOdometry *obj = [[MAVSDKTelemetryOdometry alloc] init];


        
    obj.timeUsec = odometry.time_usec;
        
    
        
    obj.frameId = (MAVSDKTelemetryMavFrame)odometry.frame_id;
        
    
        
    obj.childFrameId = (MAVSDKTelemetryMavFrame)odometry.child_frame_id;
        
    
        
    obj.positionBody = translateFromCppPositionBody(odometry.position_body);
        
    
        
    obj.q = translateFromCppQuaternion(odometry.q);
        
    
        
    obj.velocityBody = translateFromCppVelocityBody(odometry.velocity_body);
        
    
        
    obj.angularVelocityBody = translateFromCppAngularVelocityBody(odometry.angular_velocity_body);
        
    
        
    obj.poseCovariance = translateFromCppCovariance(odometry.pose_covariance);
        
    
        
    obj.velocityCovariance = translateFromCppCovariance(odometry.velocity_covariance);
        
    
    return obj;
}

mavsdk::Telemetry::Odometry translateToCppOdometry(MAVSDKTelemetryOdometry* odometry)
{
    mavsdk::Telemetry::Odometry obj;


        
    obj.time_usec = odometry.timeUsec;
        
    
        
    obj.frame_id = (mavsdk::Telemetry::Odometry::MavFrame)(odometry.frameId);
        
    
        
    obj.child_frame_id = (mavsdk::Telemetry::Odometry::MavFrame)(odometry.childFrameId);
        
    
        
    obj.position_body = translateToCppPositionBody(odometry.positionBody);
        
    
        
    obj.q = translateToCppQuaternion(odometry.q);
        
    
        
    obj.velocity_body = translateToCppVelocityBody(odometry.velocityBody);
        
    
        
    obj.angular_velocity_body = translateToCppAngularVelocityBody(odometry.angularVelocityBody);
        
    
        
    obj.pose_covariance = translateToCppCovariance(odometry.poseCovariance);
        
    
        
    obj.velocity_covariance = translateToCppCovariance(odometry.velocityCovariance);
        
    
    return obj;
}



MAVSDKTelemetryDistanceSensor* translateFromCppDistanceSensor(mavsdk::Telemetry::DistanceSensor distance_sensor)
{
    MAVSDKTelemetryDistanceSensor *obj = [[MAVSDKTelemetryDistanceSensor alloc] init];


        
    obj.minimumDistanceM = distance_sensor.minimum_distance_m;
        
    
        
    obj.maximumDistanceM = distance_sensor.maximum_distance_m;
        
    
        
    obj.currentDistanceM = distance_sensor.current_distance_m;
        
    
    return obj;
}

mavsdk::Telemetry::DistanceSensor translateToCppDistanceSensor(MAVSDKTelemetryDistanceSensor* distanceSensor)
{
    mavsdk::Telemetry::DistanceSensor obj;


        
    obj.minimum_distance_m = distanceSensor.minimumDistanceM;
        
    
        
    obj.maximum_distance_m = distanceSensor.maximumDistanceM;
        
    
        
    obj.current_distance_m = distanceSensor.currentDistanceM;
        
    
    return obj;
}



MAVSDKTelemetryPositionNed* translateFromCppPositionNed(mavsdk::Telemetry::PositionNed position_ned)
{
    MAVSDKTelemetryPositionNed *obj = [[MAVSDKTelemetryPositionNed alloc] init];


        
    obj.northM = position_ned.north_m;
        
    
        
    obj.eastM = position_ned.east_m;
        
    
        
    obj.downM = position_ned.down_m;
        
    
    return obj;
}

mavsdk::Telemetry::PositionNed translateToCppPositionNed(MAVSDKTelemetryPositionNed* positionNed)
{
    mavsdk::Telemetry::PositionNed obj;


        
    obj.north_m = positionNed.northM;
        
    
        
    obj.east_m = positionNed.eastM;
        
    
        
    obj.down_m = positionNed.downM;
        
    
    return obj;
}



MAVSDKTelemetryVelocityNed* translateFromCppVelocityNed(mavsdk::Telemetry::VelocityNed velocity_ned)
{
    MAVSDKTelemetryVelocityNed *obj = [[MAVSDKTelemetryVelocityNed alloc] init];


        
    obj.northMS = velocity_ned.north_m_s;
        
    
        
    obj.eastMS = velocity_ned.east_m_s;
        
    
        
    obj.downMS = velocity_ned.down_m_s;
        
    
    return obj;
}

mavsdk::Telemetry::VelocityNed translateToCppVelocityNed(MAVSDKTelemetryVelocityNed* velocityNed)
{
    mavsdk::Telemetry::VelocityNed obj;


        
    obj.north_m_s = velocityNed.northMS;
        
    
        
    obj.east_m_s = velocityNed.eastMS;
        
    
        
    obj.down_m_s = velocityNed.downMS;
        
    
    return obj;
}



MAVSDKTelemetryPositionVelocityNed* translateFromCppPositionVelocityNed(mavsdk::Telemetry::PositionVelocityNed position_velocity_ned)
{
    MAVSDKTelemetryPositionVelocityNed *obj = [[MAVSDKTelemetryPositionVelocityNed alloc] init];


        
    obj.position = translateFromCppPositionNed(position_velocity_ned.position);
        
    
        
    obj.velocity = translateFromCppVelocityNed(position_velocity_ned.velocity);
        
    
    return obj;
}

mavsdk::Telemetry::PositionVelocityNed translateToCppPositionVelocityNed(MAVSDKTelemetryPositionVelocityNed* positionVelocityNed)
{
    mavsdk::Telemetry::PositionVelocityNed obj;


        
    obj.position = translateToCppPositionNed(positionVelocityNed.position);
        
    
        
    obj.velocity = translateToCppVelocityNed(positionVelocityNed.velocity);
        
    
    return obj;
}



MAVSDKTelemetryGroundTruth* translateFromCppGroundTruth(mavsdk::Telemetry::GroundTruth ground_truth)
{
    MAVSDKTelemetryGroundTruth *obj = [[MAVSDKTelemetryGroundTruth alloc] init];


        
    obj.latitudeDeg = ground_truth.latitude_deg;
        
    
        
    obj.longitudeDeg = ground_truth.longitude_deg;
        
    
        
    obj.absoluteAltitudeM = ground_truth.absolute_altitude_m;
        
    
    return obj;
}

mavsdk::Telemetry::GroundTruth translateToCppGroundTruth(MAVSDKTelemetryGroundTruth* groundTruth)
{
    mavsdk::Telemetry::GroundTruth obj;


        
    obj.latitude_deg = groundTruth.latitudeDeg;
        
    
        
    obj.longitude_deg = groundTruth.longitudeDeg;
        
    
        
    obj.absolute_altitude_m = groundTruth.absoluteAltitudeM;
        
    
    return obj;
}



MAVSDKTelemetryFixedwingMetrics* translateFromCppFixedwingMetrics(mavsdk::Telemetry::FixedwingMetrics fixedwing_metrics)
{
    MAVSDKTelemetryFixedwingMetrics *obj = [[MAVSDKTelemetryFixedwingMetrics alloc] init];


        
    obj.airspeedMS = fixedwing_metrics.airspeed_m_s;
        
    
        
    obj.throttlePercentage = fixedwing_metrics.throttle_percentage;
        
    
        
    obj.climbRateMS = fixedwing_metrics.climb_rate_m_s;
        
    
    return obj;
}

mavsdk::Telemetry::FixedwingMetrics translateToCppFixedwingMetrics(MAVSDKTelemetryFixedwingMetrics* fixedwingMetrics)
{
    mavsdk::Telemetry::FixedwingMetrics obj;


        
    obj.airspeed_m_s = fixedwingMetrics.airspeedMS;
        
    
        
    obj.throttle_percentage = fixedwingMetrics.throttlePercentage;
        
    
        
    obj.climb_rate_m_s = fixedwingMetrics.climbRateMS;
        
    
    return obj;
}



MAVSDKTelemetryAccelerationFrd* translateFromCppAccelerationFrd(mavsdk::Telemetry::AccelerationFrd acceleration_frd)
{
    MAVSDKTelemetryAccelerationFrd *obj = [[MAVSDKTelemetryAccelerationFrd alloc] init];


        
    obj.forwardMS2 = acceleration_frd.forward_m_s2;
        
    
        
    obj.rightMS2 = acceleration_frd.right_m_s2;
        
    
        
    obj.downMS2 = acceleration_frd.down_m_s2;
        
    
    return obj;
}

mavsdk::Telemetry::AccelerationFrd translateToCppAccelerationFrd(MAVSDKTelemetryAccelerationFrd* accelerationFrd)
{
    mavsdk::Telemetry::AccelerationFrd obj;


        
    obj.forward_m_s2 = accelerationFrd.forwardMS2;
        
    
        
    obj.right_m_s2 = accelerationFrd.rightMS2;
        
    
        
    obj.down_m_s2 = accelerationFrd.downMS2;
        
    
    return obj;
}



MAVSDKTelemetryAngularVelocityFrd* translateFromCppAngularVelocityFrd(mavsdk::Telemetry::AngularVelocityFrd angular_velocity_frd)
{
    MAVSDKTelemetryAngularVelocityFrd *obj = [[MAVSDKTelemetryAngularVelocityFrd alloc] init];


        
    obj.forwardRadS = angular_velocity_frd.forward_rad_s;
        
    
        
    obj.rightRadS = angular_velocity_frd.right_rad_s;
        
    
        
    obj.downRadS = angular_velocity_frd.down_rad_s;
        
    
    return obj;
}

mavsdk::Telemetry::AngularVelocityFrd translateToCppAngularVelocityFrd(MAVSDKTelemetryAngularVelocityFrd* angularVelocityFrd)
{
    mavsdk::Telemetry::AngularVelocityFrd obj;


        
    obj.forward_rad_s = angularVelocityFrd.forwardRadS;
        
    
        
    obj.right_rad_s = angularVelocityFrd.rightRadS;
        
    
        
    obj.down_rad_s = angularVelocityFrd.downRadS;
        
    
    return obj;
}



MAVSDKTelemetryMagneticFieldFrd* translateFromCppMagneticFieldFrd(mavsdk::Telemetry::MagneticFieldFrd magnetic_field_frd)
{
    MAVSDKTelemetryMagneticFieldFrd *obj = [[MAVSDKTelemetryMagneticFieldFrd alloc] init];


        
    obj.forwardGauss = magnetic_field_frd.forward_gauss;
        
    
        
    obj.rightGauss = magnetic_field_frd.right_gauss;
        
    
        
    obj.downGauss = magnetic_field_frd.down_gauss;
        
    
    return obj;
}

mavsdk::Telemetry::MagneticFieldFrd translateToCppMagneticFieldFrd(MAVSDKTelemetryMagneticFieldFrd* magneticFieldFrd)
{
    mavsdk::Telemetry::MagneticFieldFrd obj;


        
    obj.forward_gauss = magneticFieldFrd.forwardGauss;
        
    
        
    obj.right_gauss = magneticFieldFrd.rightGauss;
        
    
        
    obj.down_gauss = magneticFieldFrd.downGauss;
        
    
    return obj;
}



MAVSDKTelemetryImu* translateFromCppImu(mavsdk::Telemetry::Imu imu)
{
    MAVSDKTelemetryImu *obj = [[MAVSDKTelemetryImu alloc] init];


        
    obj.accelerationFrd = translateFromCppAccelerationFrd(imu.acceleration_frd);
        
    
        
    obj.angularVelocityFrd = translateFromCppAngularVelocityFrd(imu.angular_velocity_frd);
        
    
        
    obj.magneticFieldFrd = translateFromCppMagneticFieldFrd(imu.magnetic_field_frd);
        
    
        
    obj.temperatureDegc = imu.temperature_degc;
        
    
    return obj;
}

mavsdk::Telemetry::Imu translateToCppImu(MAVSDKTelemetryImu* imu)
{
    mavsdk::Telemetry::Imu obj;


        
    obj.acceleration_frd = translateToCppAccelerationFrd(imu.accelerationFrd);
        
    
        
    obj.angular_velocity_frd = translateToCppAngularVelocityFrd(imu.angularVelocityFrd);
        
    
        
    obj.magnetic_field_frd = translateToCppMagneticFieldFrd(imu.magneticFieldFrd);
        
    
        
    obj.temperature_degc = imu.temperatureDegc;
        
    
    return obj;
}



MAVSDKTelemetryGpsGlobalOrigin* translateFromCppGpsGlobalOrigin(mavsdk::Telemetry::GpsGlobalOrigin gps_global_origin)
{
    MAVSDKTelemetryGpsGlobalOrigin *obj = [[MAVSDKTelemetryGpsGlobalOrigin alloc] init];


        
    obj.latitudeDeg = gps_global_origin.latitude_deg;
        
    
        
    obj.longitudeDeg = gps_global_origin.longitude_deg;
        
    
        
    obj.altitudeM = gps_global_origin.altitude_m;
        
    
    return obj;
}

mavsdk::Telemetry::GpsGlobalOrigin translateToCppGpsGlobalOrigin(MAVSDKTelemetryGpsGlobalOrigin* gpsGlobalOrigin)
{
    mavsdk::Telemetry::GpsGlobalOrigin obj;


        
    obj.latitude_deg = gpsGlobalOrigin.latitudeDeg;
        
    
        
    obj.longitude_deg = gpsGlobalOrigin.longitudeDeg;
        
    
        
    obj.altitude_m = gpsGlobalOrigin.altitudeM;
        
    
    return obj;
}






@end