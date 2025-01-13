# mavsdk::Telemetry Class Reference
`#include: telemetry.h`

----


Allow users to get vehicle telemetry and state information (e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates. 


## Data Structures


struct [AccelerationFrd](structmavsdk_1_1_telemetry_1_1_acceleration_frd.md)

struct [ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md)

struct [ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md)

struct [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)

struct [AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md)

struct [AngularVelocityFrd](structmavsdk_1_1_telemetry_1_1_angular_velocity_frd.md)

struct [Battery](structmavsdk_1_1_telemetry_1_1_battery.md)

struct [Covariance](structmavsdk_1_1_telemetry_1_1_covariance.md)

struct [DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md)

struct [EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md)

struct [FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md)

struct [GpsGlobalOrigin](structmavsdk_1_1_telemetry_1_1_gps_global_origin.md)

struct [GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md)

struct [GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md)

struct [Heading](structmavsdk_1_1_telemetry_1_1_heading.md)

struct [Health](structmavsdk_1_1_telemetry_1_1_health.md)

struct [Imu](structmavsdk_1_1_telemetry_1_1_imu.md)

struct [MagneticFieldFrd](structmavsdk_1_1_telemetry_1_1_magnetic_field_frd.md)

struct [Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md)

struct [Position](structmavsdk_1_1_telemetry_1_1_position.md)

struct [PositionBody](structmavsdk_1_1_telemetry_1_1_position_body.md)

struct [PositionNed](structmavsdk_1_1_telemetry_1_1_position_ned.md)

struct [PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md)

struct [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md)

struct [RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md)

struct [RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md)

struct [ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md)

struct [StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md)

struct [VelocityBody](structmavsdk_1_1_telemetry_1_1_velocity_body.md)

struct [VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md)

## Public Types


Type | Description
--- | ---
enum [FixType](#classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639de) | GPS fix type.
enum [FlightMode](#classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fe) | Flight modes.
enum [StatusTextType](#classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8) | Status types.
enum [LandedState](#classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046) | Landed State enumeration.
enum [VtolState](#classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166) | VTOL State enumeration.
enum [Result](#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | Possible results returned for telemetry requests.
std::function< void([Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75))> [ResultCallback](#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) | Callback type for asynchronous [Telemetry](classmavsdk_1_1_telemetry.md) calls.
std::function< void([Position](structmavsdk_1_1_telemetry_1_1_position.md))> [PositionCallback](#classmavsdk_1_1_telemetry_1a978b371d636226e198995462afa63552) | Callback type for subscribe_position.
[Handle](classmavsdk_1_1_handle.md)< [Position](structmavsdk_1_1_telemetry_1_1_position.md) > [PositionHandle](#classmavsdk_1_1_telemetry_1a7d82a98ea53c2aa254624bc943ec22f1) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_position.
std::function< void([Position](structmavsdk_1_1_telemetry_1_1_position.md))> [HomeCallback](#classmavsdk_1_1_telemetry_1aaac029969c37a001d43e2788a6abf634) | Callback type for subscribe_home.
[Handle](classmavsdk_1_1_handle.md)< [Position](structmavsdk_1_1_telemetry_1_1_position.md) > [HomeHandle](#classmavsdk_1_1_telemetry_1a7a2b3a8b50d66e911870700ea3fe007d) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_home.
std::function< void(bool)> [InAirCallback](#classmavsdk_1_1_telemetry_1af96cca452305dd8f51b42d4663f15a26) | Callback type for subscribe_in_air.
[Handle](classmavsdk_1_1_handle.md)< bool > [InAirHandle](#classmavsdk_1_1_telemetry_1a17582f68fabac027ab354073e0eca8e5) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_in_air.
std::function< void([LandedState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046))> [LandedStateCallback](#classmavsdk_1_1_telemetry_1a0cd8ef17abdd7c3d6a9ee761ccc6ae5e) | Callback type for subscribe_landed_state.
[Handle](classmavsdk_1_1_handle.md)< [LandedState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046) > [LandedStateHandle](#classmavsdk_1_1_telemetry_1a125f2ecfe3f5ce588e6d1799392a5fd7) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_landed_state.
std::function< void(bool)> [ArmedCallback](#classmavsdk_1_1_telemetry_1a9d23a4092d94e50694390e9f41b8c419) | Callback type for subscribe_armed.
[Handle](classmavsdk_1_1_handle.md)< bool > [ArmedHandle](#classmavsdk_1_1_telemetry_1a3cfe4b90e156fb055066f8164985311c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_armed.
std::function< void([VtolState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166))> [VtolStateCallback](#classmavsdk_1_1_telemetry_1abf52126ce4d4efb99560aa8e8e58f20c) | Callback type for subscribe_vtol_state.
[Handle](classmavsdk_1_1_handle.md)< [VtolState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166) > [VtolStateHandle](#classmavsdk_1_1_telemetry_1a824bab94514afa56334acf41b6f7d244) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_vtol_state.
std::function< void([Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md))> [AttitudeQuaternionCallback](#classmavsdk_1_1_telemetry_1ad16e61245511a99e930d6fdcbd761a30) | Callback type for subscribe_attitude_quaternion.
[Handle](classmavsdk_1_1_handle.md)< [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) > [AttitudeQuaternionHandle](#classmavsdk_1_1_telemetry_1a0ce62986aedcd06d845406a2d99257d3) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude_quaternion.
std::function< void([EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md))> [AttitudeEulerCallback](#classmavsdk_1_1_telemetry_1a321c7607922369926fbd5f2821986cba) | Callback type for subscribe_attitude_euler.
[Handle](classmavsdk_1_1_handle.md)< [EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) > [AttitudeEulerHandle](#classmavsdk_1_1_telemetry_1ae73f3d3ac224438a6cd07344fda9543b) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude_euler.
std::function< void([AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md))> [AttitudeAngularVelocityBodyCallback](#classmavsdk_1_1_telemetry_1a35ff8def3048faeab7f732153d51085f) | Callback type for subscribe_attitude_angular_velocity_body.
[Handle](classmavsdk_1_1_handle.md)< [AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md) > [AttitudeAngularVelocityBodyHandle](#classmavsdk_1_1_telemetry_1a2328e39c1a96ce9a090cb19283d3ffc1) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude_angular_velocity_body.
std::function< void([Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md))> [CameraAttitudeQuaternionCallback](#classmavsdk_1_1_telemetry_1aa83dafa14e9b5179573a574f6fbdd973) | Callback type for subscribe_camera_attitude_quaternion.
[Handle](classmavsdk_1_1_handle.md)< [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) > [CameraAttitudeQuaternionHandle](#classmavsdk_1_1_telemetry_1aeb2a99828961bf2a4d5c24753e020358) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_camera_attitude_quaternion.
std::function< void([EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md))> [CameraAttitudeEulerCallback](#classmavsdk_1_1_telemetry_1aa29f9bb0767ba8c384bfe1df69f2fdd9) | Callback type for subscribe_camera_attitude_euler.
[Handle](classmavsdk_1_1_handle.md)< [EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) > [CameraAttitudeEulerHandle](#classmavsdk_1_1_telemetry_1a76471c91115d6e03e6165a3e1315808b) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_camera_attitude_euler.
std::function< void([VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md))> [VelocityNedCallback](#classmavsdk_1_1_telemetry_1ab5859d2f6a9c9bd81282166b3de92342) | Callback type for subscribe_velocity_ned.
[Handle](classmavsdk_1_1_handle.md)< [VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md) > [VelocityNedHandle](#classmavsdk_1_1_telemetry_1a2c3898f33bfa1bffe86681aaca33343e) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_velocity_ned.
std::function< void([GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md))> [GpsInfoCallback](#classmavsdk_1_1_telemetry_1ad8fa90886b2283eace09b4b46708048b) | Callback type for subscribe_gps_info.
[Handle](classmavsdk_1_1_handle.md)< [GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md) > [GpsInfoHandle](#classmavsdk_1_1_telemetry_1a87598b504f6ddf3ba0620b0458f2d39c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_gps_info.
std::function< void([RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md))> [RawGpsCallback](#classmavsdk_1_1_telemetry_1a915868d562ed445fa30beaa9140ea97c) | Callback type for subscribe_raw_gps.
[Handle](classmavsdk_1_1_handle.md)< [RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md) > [RawGpsHandle](#classmavsdk_1_1_telemetry_1a75a62eb3624bf77ea6860d5d965636fd) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_raw_gps.
std::function< void([Battery](structmavsdk_1_1_telemetry_1_1_battery.md))> [BatteryCallback](#classmavsdk_1_1_telemetry_1af4b121c576ef2ae567b1d571b12dff9d) | Callback type for subscribe_battery.
[Handle](classmavsdk_1_1_handle.md)< [Battery](structmavsdk_1_1_telemetry_1_1_battery.md) > [BatteryHandle](#classmavsdk_1_1_telemetry_1ad9c79c3a89de6f28aabc771a2b5cc92f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_battery.
std::function< void([FlightMode](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fe))> [FlightModeCallback](#classmavsdk_1_1_telemetry_1a2d7318d0823771b7a586c40199bdb482) | Callback type for subscribe_flight_mode.
[Handle](classmavsdk_1_1_handle.md)< [FlightMode](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fe) > [FlightModeHandle](#classmavsdk_1_1_telemetry_1add61cde1555ed1cec93a34bbdadeef5c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_flight_mode.
std::function< void([Health](structmavsdk_1_1_telemetry_1_1_health.md))> [HealthCallback](#classmavsdk_1_1_telemetry_1a7a120dd053091c644e0e2e47fdcbeb75) | Callback type for subscribe_health.
[Handle](classmavsdk_1_1_handle.md)< [Health](structmavsdk_1_1_telemetry_1_1_health.md) > [HealthHandle](#classmavsdk_1_1_telemetry_1a52869d9dd1a240385b0d213a7057f3df) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_health.
std::function< void([RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md))> [RcStatusCallback](#classmavsdk_1_1_telemetry_1aafcd706b805898301b574ffa2b909b85) | Callback type for subscribe_rc_status.
[Handle](classmavsdk_1_1_handle.md)< [RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md) > [RcStatusHandle](#classmavsdk_1_1_telemetry_1ae3bd26bbc4c12eddc7f90f42482078de) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_rc_status.
std::function< void([StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md))> [StatusTextCallback](#classmavsdk_1_1_telemetry_1a46e51ff90fe779990ed09a593c1c7898) | Callback type for subscribe_status_text.
[Handle](classmavsdk_1_1_handle.md)< [StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md) > [StatusTextHandle](#classmavsdk_1_1_telemetry_1a5c2b31499961747b784664b87b851e04) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_status_text.
std::function< void([ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md))> [ActuatorControlTargetCallback](#classmavsdk_1_1_telemetry_1ada6af3de1b60b93a709345c3a8ede551) | Callback type for subscribe_actuator_control_target.
[Handle](classmavsdk_1_1_handle.md)< [ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md) > [ActuatorControlTargetHandle](#classmavsdk_1_1_telemetry_1a01438bbc123454e745b8874bc91bf874) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_actuator_control_target.
std::function< void([ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md))> [ActuatorOutputStatusCallback](#classmavsdk_1_1_telemetry_1a2b1e800ce1ba6fb776351416340ac8b9) | Callback type for subscribe_actuator_output_status.
[Handle](classmavsdk_1_1_handle.md)< [ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md) > [ActuatorOutputStatusHandle](#classmavsdk_1_1_telemetry_1ac0bbbb060195848f41de50600489713f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_actuator_output_status.
std::function< void([Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md))> [OdometryCallback](#classmavsdk_1_1_telemetry_1a8cd23f7364f8f5cb22869155da67c65d) | Callback type for subscribe_odometry.
[Handle](classmavsdk_1_1_handle.md)< [Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) > [OdometryHandle](#classmavsdk_1_1_telemetry_1a23b33f337ed73ec584e8ac8868c72f86) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_odometry.
std::function< void([PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md))> [PositionVelocityNedCallback](#classmavsdk_1_1_telemetry_1a5a38deb284622ff6926703e1e5c96a74) | Callback type for subscribe_position_velocity_ned.
[Handle](classmavsdk_1_1_handle.md)< [PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md) > [PositionVelocityNedHandle](#classmavsdk_1_1_telemetry_1aef1424caca3967e67336140af395e59c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_position_velocity_ned.
std::function< void([GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md))> [GroundTruthCallback](#classmavsdk_1_1_telemetry_1a222aae53852a2c535f6d69ed57221f13) | Callback type for subscribe_ground_truth.
[Handle](classmavsdk_1_1_handle.md)< [GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md) > [GroundTruthHandle](#classmavsdk_1_1_telemetry_1a373568d10564c5a3c3d325e5975a3a23) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_ground_truth.
std::function< void([FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md))> [FixedwingMetricsCallback](#classmavsdk_1_1_telemetry_1a5b42dbef0ef6d8c1768d503d0437f1e3) | Callback type for subscribe_fixedwing_metrics.
[Handle](classmavsdk_1_1_handle.md)< [FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md) > [FixedwingMetricsHandle](#classmavsdk_1_1_telemetry_1ab180a383ee3e17f20bb1b026b667db04) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_fixedwing_metrics.
std::function< void([Imu](structmavsdk_1_1_telemetry_1_1_imu.md))> [ImuCallback](#classmavsdk_1_1_telemetry_1a4fbc2ad274fd5a8af077004d2d7bd984) | Callback type for subscribe_imu.
[Handle](classmavsdk_1_1_handle.md)< [Imu](structmavsdk_1_1_telemetry_1_1_imu.md) > [ImuHandle](#classmavsdk_1_1_telemetry_1a5b45a4c22abb67617d37ae56efdf8360) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_imu.
std::function< void([Imu](structmavsdk_1_1_telemetry_1_1_imu.md))> [ScaledImuCallback](#classmavsdk_1_1_telemetry_1a26159a775adcfbc42302234b7108d94f) | Callback type for subscribe_scaled_imu.
[Handle](classmavsdk_1_1_handle.md)< [Imu](structmavsdk_1_1_telemetry_1_1_imu.md) > [ScaledImuHandle](#classmavsdk_1_1_telemetry_1a4968d9d418d48a4368f2915023ca0014) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_scaled_imu.
std::function< void([Imu](structmavsdk_1_1_telemetry_1_1_imu.md))> [RawImuCallback](#classmavsdk_1_1_telemetry_1a92711da85d343cb58b73561e6b730c76) | Callback type for subscribe_raw_imu.
[Handle](classmavsdk_1_1_handle.md)< [Imu](structmavsdk_1_1_telemetry_1_1_imu.md) > [RawImuHandle](#classmavsdk_1_1_telemetry_1a416f5b4dc6c51d78d05572d5cc18f3fb) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_raw_imu.
std::function< void(bool)> [HealthAllOkCallback](#classmavsdk_1_1_telemetry_1a71cdcadfaa988dc14029e0b9fdbe742d) | Callback type for subscribe_health_all_ok.
[Handle](classmavsdk_1_1_handle.md)< bool > [HealthAllOkHandle](#classmavsdk_1_1_telemetry_1aa882d4938eb491cce5b7ca6aead2384c) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_health_all_ok.
std::function< void(uint64_t)> [UnixEpochTimeCallback](#classmavsdk_1_1_telemetry_1a321c7d809ae8f56bb8a361d5e5ce6391) | Callback type for subscribe_unix_epoch_time.
[Handle](classmavsdk_1_1_handle.md)< uint64_t > [UnixEpochTimeHandle](#classmavsdk_1_1_telemetry_1a4c4b00adfab9b7f04530133aafbafbd5) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_unix_epoch_time.
std::function< void([DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md))> [DistanceSensorCallback](#classmavsdk_1_1_telemetry_1aacfdb5e2cce7f3f77c68b36f020ed1f2) | Callback type for subscribe_distance_sensor.
[Handle](classmavsdk_1_1_handle.md)< [DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md) > [DistanceSensorHandle](#classmavsdk_1_1_telemetry_1aaa6bd0660aeb7d3f1d12240ce0710878) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_distance_sensor.
std::function< void([ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md))> [ScaledPressureCallback](#classmavsdk_1_1_telemetry_1ac123edc254bb1874edc08a0f531f82b1) | Callback type for subscribe_scaled_pressure.
[Handle](classmavsdk_1_1_handle.md)< [ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md) > [ScaledPressureHandle](#classmavsdk_1_1_telemetry_1ad19871b63833f64bed071db6017e4a22) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_scaled_pressure.
std::function< void([Heading](structmavsdk_1_1_telemetry_1_1_heading.md))> [HeadingCallback](#classmavsdk_1_1_telemetry_1aa3bca0adab525a4c733c1e7f5c5dd8b3) | Callback type for subscribe_heading.
[Handle](classmavsdk_1_1_handle.md)< [Heading](structmavsdk_1_1_telemetry_1_1_heading.md) > [HeadingHandle](#classmavsdk_1_1_telemetry_1a11cc43135c8162fd8478ba76a5b3166d) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_heading.
std::function< void([Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md))> [AltitudeCallback](#classmavsdk_1_1_telemetry_1a868986de0ab3990d4fe1ec842bce6adf) | Callback type for subscribe_altitude.
[Handle](classmavsdk_1_1_handle.md)< [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) > [AltitudeHandle](#classmavsdk_1_1_telemetry_1ae0714c14e07a08b4748c96ea20f1d12f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_altitude.
std::function< void([Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75), [GpsGlobalOrigin](structmavsdk_1_1_telemetry_1_1_gps_global_origin.md))> [GetGpsGlobalOriginCallback](#classmavsdk_1_1_telemetry_1a350ee89a7e30a691e130e29ace8917ef) | Callback type for get_gps_global_origin_async.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Telemetry](#classmavsdk_1_1_telemetry_1a6c8c8ed8759fc8c6e9fd4e7644c63cbe) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Telemetry](#classmavsdk_1_1_telemetry_1af78f18fbb117c82d5ffe21e015535067) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Telemetry](#classmavsdk_1_1_telemetry_1a6ffca3dba4c6102ae6602822a140c8fc) () override | Destructor (internal use only).
&nbsp; | [Telemetry](#classmavsdk_1_1_telemetry_1ad734f199b82a9928c63230676c9789e9) (const [Telemetry](classmavsdk_1_1_telemetry.md) & other) | Copy constructor.
[PositionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7d82a98ea53c2aa254624bc943ec22f1) | [subscribe_position](#classmavsdk_1_1_telemetry_1a647f3d61b4f3301fd8c2aca1afcafb31) (const [PositionCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a978b371d636226e198995462afa63552) & callback) | Subscribe to 'position' updates.
void | [unsubscribe_position](#classmavsdk_1_1_telemetry_1abf10cb55e2c477bbb875c0944938b76c) ([PositionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7d82a98ea53c2aa254624bc943ec22f1) handle) | Unsubscribe from subscribe_position.
[Position](structmavsdk_1_1_telemetry_1_1_position.md) | [position](#classmavsdk_1_1_telemetry_1a2299da1bc63313c429f07ab0fdbe5335) () const | Poll for '[Position](structmavsdk_1_1_telemetry_1_1_position.md)' (blocking).
[HomeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7a2b3a8b50d66e911870700ea3fe007d) | [subscribe_home](#classmavsdk_1_1_telemetry_1ae57e4c8741ba7ae05961cadfabc82875) (const [HomeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aaac029969c37a001d43e2788a6abf634) & callback) | Subscribe to 'home position' updates.
void | [unsubscribe_home](#classmavsdk_1_1_telemetry_1a83feb66aa64946b954ca305b6cc2f7ea) ([HomeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7a2b3a8b50d66e911870700ea3fe007d) handle) | Unsubscribe from subscribe_home.
[Position](structmavsdk_1_1_telemetry_1_1_position.md) | [home](#classmavsdk_1_1_telemetry_1ad5c239b93aa1923edd1b97494a3fbfe7) () const | Poll for '[Position](structmavsdk_1_1_telemetry_1_1_position.md)' (blocking).
[InAirHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a17582f68fabac027ab354073e0eca8e5) | [subscribe_in_air](#classmavsdk_1_1_telemetry_1a0fc586a0c90141e125a305fd8211e8d8) (const [InAirCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1af96cca452305dd8f51b42d4663f15a26) & callback) | Subscribe to in-air updates.
void | [unsubscribe_in_air](#classmavsdk_1_1_telemetry_1a2e23bf258ef69c267ab5b8dee203bd95) ([InAirHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a17582f68fabac027ab354073e0eca8e5) handle) | Unsubscribe from subscribe_in_air.
bool | [in_air](#classmavsdk_1_1_telemetry_1a909738ff2fbe104c6eb4524cc9bf2dd5) () const | Poll for 'bool' (blocking).
[LandedStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a125f2ecfe3f5ce588e6d1799392a5fd7) | [subscribe_landed_state](#classmavsdk_1_1_telemetry_1ae618a7ba0ed197e3d5c3427a63c3cfa0) (const [LandedStateCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0cd8ef17abdd7c3d6a9ee761ccc6ae5e) & callback) | Subscribe to landed state updates.
void | [unsubscribe_landed_state](#classmavsdk_1_1_telemetry_1a17df3b1e3917f788725f39cf879145f6) ([LandedStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a125f2ecfe3f5ce588e6d1799392a5fd7) handle) | Unsubscribe from subscribe_landed_state.
[LandedState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046) | [landed_state](#classmavsdk_1_1_telemetry_1af7d7c385852db38d6320516508ce7465) () const | Poll for 'LandedState' (blocking).
[ArmedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a3cfe4b90e156fb055066f8164985311c) | [subscribe_armed](#classmavsdk_1_1_telemetry_1a2ea3978439ed7e21973a41e1f2c2b24f) (const [ArmedCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a9d23a4092d94e50694390e9f41b8c419) & callback) | Subscribe to armed updates.
void | [unsubscribe_armed](#classmavsdk_1_1_telemetry_1a8c6080d520f6d2fdac9cb97279ffcbb9) ([ArmedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a3cfe4b90e156fb055066f8164985311c) handle) | Unsubscribe from subscribe_armed.
bool | [armed](#classmavsdk_1_1_telemetry_1a6620142adc47f069262e5bf69dbb3876) () const | Poll for 'bool' (blocking).
[VtolStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a824bab94514afa56334acf41b6f7d244) | [subscribe_vtol_state](#classmavsdk_1_1_telemetry_1a956eb63dc90a2cf905590f6f309edb36) (const [VtolStateCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1abf52126ce4d4efb99560aa8e8e58f20c) & callback) | subscribe to vtol state Updates
void | [unsubscribe_vtol_state](#classmavsdk_1_1_telemetry_1a932eb8b43e90e64bd131b3257f747963) ([VtolStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a824bab94514afa56334acf41b6f7d244) handle) | Unsubscribe from subscribe_vtol_state.
[VtolState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166) | [vtol_state](#classmavsdk_1_1_telemetry_1a6d3fd68f3e639a2bf9940e9f38f220c3) () const | Poll for 'VtolState' (blocking).
[AttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0ce62986aedcd06d845406a2d99257d3) | [subscribe_attitude_quaternion](#classmavsdk_1_1_telemetry_1a9248befe3416252c7d8fb63b19842421) (const [AttitudeQuaternionCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad16e61245511a99e930d6fdcbd761a30) & callback) | Subscribe to 'attitude' updates (quaternion).
void | [unsubscribe_attitude_quaternion](#classmavsdk_1_1_telemetry_1a06034401ac16901d50b850ded0b064b5) ([AttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0ce62986aedcd06d845406a2d99257d3) handle) | Unsubscribe from subscribe_attitude_quaternion.
[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) | [attitude_quaternion](#classmavsdk_1_1_telemetry_1aae76890957b33727be72a39807448c88) () const | Poll for '[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md)' (blocking).
[AttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae73f3d3ac224438a6cd07344fda9543b) | [subscribe_attitude_euler](#classmavsdk_1_1_telemetry_1aaf90ca9cc8adcc9cb1963293bdfcefd6) (const [AttitudeEulerCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a321c7607922369926fbd5f2821986cba) & callback) | Subscribe to 'attitude' updates (Euler).
void | [unsubscribe_attitude_euler](#classmavsdk_1_1_telemetry_1aa770088117b037446efa8cbfe058e54d) ([AttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae73f3d3ac224438a6cd07344fda9543b) handle) | Unsubscribe from subscribe_attitude_euler.
[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) | [attitude_euler](#classmavsdk_1_1_telemetry_1a03035bb72324e843372eb69cf7899ce5) () const | Poll for '[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md)' (blocking).
[AttitudeAngularVelocityBodyHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2328e39c1a96ce9a090cb19283d3ffc1) | [subscribe_attitude_angular_velocity_body](#classmavsdk_1_1_telemetry_1a3bbefcdb4e9cd9af9692626984504cb7) (const [AttitudeAngularVelocityBodyCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a35ff8def3048faeab7f732153d51085f) & callback) | Subscribe to 'attitude' updates (angular velocity)
void | [unsubscribe_attitude_angular_velocity_body](#classmavsdk_1_1_telemetry_1a2319d78b4e214e5b2f3ef55a1c32e2c5) ([AttitudeAngularVelocityBodyHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2328e39c1a96ce9a090cb19283d3ffc1) handle) | Unsubscribe from subscribe_attitude_angular_velocity_body.
[AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md) | [attitude_angular_velocity_body](#classmavsdk_1_1_telemetry_1a8d9e2489b79c2cdbabaef8b6bb8e2952) () const | Poll for '[AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md)' (blocking).
[CameraAttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aeb2a99828961bf2a4d5c24753e020358) | [subscribe_camera_attitude_quaternion](#classmavsdk_1_1_telemetry_1a9b7517e15c841765fbb8b1230e73bf82) (const [CameraAttitudeQuaternionCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa83dafa14e9b5179573a574f6fbdd973) & callback) | Subscribe to 'camera attitude' updates (quaternion).
void | [unsubscribe_camera_attitude_quaternion](#classmavsdk_1_1_telemetry_1aa92270b8a30b22271f65d92e9a7ed9a1) ([CameraAttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aeb2a99828961bf2a4d5c24753e020358) handle) | Unsubscribe from subscribe_camera_attitude_quaternion.
[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) | [camera_attitude_quaternion](#classmavsdk_1_1_telemetry_1a3c07447351d3b6195d5e2526e7b128b3) () const | Poll for '[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md)' (blocking).
[CameraAttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a76471c91115d6e03e6165a3e1315808b) | [subscribe_camera_attitude_euler](#classmavsdk_1_1_telemetry_1aa50015ba9b1decb825d45e459191c342) (const [CameraAttitudeEulerCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa29f9bb0767ba8c384bfe1df69f2fdd9) & callback) | Subscribe to 'camera attitude' updates (Euler).
void | [unsubscribe_camera_attitude_euler](#classmavsdk_1_1_telemetry_1a90b8dfe6b83afc908e4c236bbbc32930) ([CameraAttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a76471c91115d6e03e6165a3e1315808b) handle) | Unsubscribe from subscribe_camera_attitude_euler.
[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) | [camera_attitude_euler](#classmavsdk_1_1_telemetry_1a635643d955f0cd9a805914501f819796) () const | Poll for '[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md)' (blocking).
[VelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2c3898f33bfa1bffe86681aaca33343e) | [subscribe_velocity_ned](#classmavsdk_1_1_telemetry_1a9b5e6bd8fb05324fd7a99d0260933c9d) (const [VelocityNedCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ab5859d2f6a9c9bd81282166b3de92342) & callback) | Subscribe to 'ground speed' updates (NED).
void | [unsubscribe_velocity_ned](#classmavsdk_1_1_telemetry_1ae14a663f3b4820e891a4d83c2f8aa2c3) ([VelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2c3898f33bfa1bffe86681aaca33343e) handle) | Unsubscribe from subscribe_velocity_ned.
[VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md) | [velocity_ned](#classmavsdk_1_1_telemetry_1a40a86062c0322d6be7c86d8e15a52f28) () const | Poll for '[VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md)' (blocking).
[GpsInfoHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a87598b504f6ddf3ba0620b0458f2d39c) | [subscribe_gps_info](#classmavsdk_1_1_telemetry_1a1933fa5b008856b5d621489d785c68e7) (const [GpsInfoCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad8fa90886b2283eace09b4b46708048b) & callback) | Subscribe to 'GPS info' updates.
void | [unsubscribe_gps_info](#classmavsdk_1_1_telemetry_1a96ba022d27983893a814ad4a787bfa51) ([GpsInfoHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a87598b504f6ddf3ba0620b0458f2d39c) handle) | Unsubscribe from subscribe_gps_info.
[GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md) | [gps_info](#classmavsdk_1_1_telemetry_1a983dabc1aed50745b326072662c419e8) () const | Poll for '[GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md)' (blocking).
[RawGpsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a75a62eb3624bf77ea6860d5d965636fd) | [subscribe_raw_gps](#classmavsdk_1_1_telemetry_1a39c324fdd723a2b5c5559ddadad597f0) (const [RawGpsCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a915868d562ed445fa30beaa9140ea97c) & callback) | Subscribe to 'Raw GPS' updates.
void | [unsubscribe_raw_gps](#classmavsdk_1_1_telemetry_1acc6c03a6d27b268f609c72b66aa48a09) ([RawGpsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a75a62eb3624bf77ea6860d5d965636fd) handle) | Unsubscribe from subscribe_raw_gps.
[RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md) | [raw_gps](#classmavsdk_1_1_telemetry_1ac43c29e435b6c1a6594854adc6a1bf6c) () const | Poll for '[RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md)' (blocking).
[BatteryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad9c79c3a89de6f28aabc771a2b5cc92f) | [subscribe_battery](#classmavsdk_1_1_telemetry_1ad7e7ceb85181aaac7bf5553af7f45e2a) (const [BatteryCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1af4b121c576ef2ae567b1d571b12dff9d) & callback) | Subscribe to 'battery' updates.
void | [unsubscribe_battery](#classmavsdk_1_1_telemetry_1a72ddd509a9480ba59f073fa8f1ee5dbc) ([BatteryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad9c79c3a89de6f28aabc771a2b5cc92f) handle) | Unsubscribe from subscribe_battery.
[Battery](structmavsdk_1_1_telemetry_1_1_battery.md) | [battery](#classmavsdk_1_1_telemetry_1afb3bad3c7a36c14ae97492df3f6bbd54) () const | Poll for '[Battery](structmavsdk_1_1_telemetry_1_1_battery.md)' (blocking).
[FlightModeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1add61cde1555ed1cec93a34bbdadeef5c) | [subscribe_flight_mode](#classmavsdk_1_1_telemetry_1accd831cbb9134c845744a5c70da1b418) (const [FlightModeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2d7318d0823771b7a586c40199bdb482) & callback) | Subscribe to 'flight mode' updates.
void | [unsubscribe_flight_mode](#classmavsdk_1_1_telemetry_1a64dc8d45d06b23332ba6353d50c9cc17) ([FlightModeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1add61cde1555ed1cec93a34bbdadeef5c) handle) | Unsubscribe from subscribe_flight_mode.
[FlightMode](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fe) | [flight_mode](#classmavsdk_1_1_telemetry_1a4972a3968e379d565e7700f2f51158dd) () const | Poll for 'FlightMode' (blocking).
[HealthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a52869d9dd1a240385b0d213a7057f3df) | [subscribe_health](#classmavsdk_1_1_telemetry_1a093a702abd1370ff97f4d9ebce449942) (const [HealthCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7a120dd053091c644e0e2e47fdcbeb75) & callback) | Subscribe to 'health' updates.
void | [unsubscribe_health](#classmavsdk_1_1_telemetry_1af7830aff90366a0255e71117c07f5484) ([HealthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a52869d9dd1a240385b0d213a7057f3df) handle) | Unsubscribe from subscribe_health.
[Health](structmavsdk_1_1_telemetry_1_1_health.md) | [health](#classmavsdk_1_1_telemetry_1aae4824c9eeb72603b197c864b5cc5df5) () const | Poll for '[Health](structmavsdk_1_1_telemetry_1_1_health.md)' (blocking).
[RcStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae3bd26bbc4c12eddc7f90f42482078de) | [subscribe_rc_status](#classmavsdk_1_1_telemetry_1a0252d9434190edab26fc5b3874bdcc1c) (const [RcStatusCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aafcd706b805898301b574ffa2b909b85) & callback) | Subscribe to 'RC status' updates.
void | [unsubscribe_rc_status](#classmavsdk_1_1_telemetry_1a975a70f6345f73a37894aa7a49495dc7) ([RcStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae3bd26bbc4c12eddc7f90f42482078de) handle) | Unsubscribe from subscribe_rc_status.
[RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md) | [rc_status](#classmavsdk_1_1_telemetry_1a59cd497c69f1d32be29a940a2d34a474) () const | Poll for '[RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md)' (blocking).
[StatusTextHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5c2b31499961747b784664b87b851e04) | [subscribe_status_text](#classmavsdk_1_1_telemetry_1a56e54b5dc245cb07da6088a3033d45f8) (const [StatusTextCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a46e51ff90fe779990ed09a593c1c7898) & callback) | Subscribe to 'status text' updates.
void | [unsubscribe_status_text](#classmavsdk_1_1_telemetry_1a972adcc6c0c88ddb6aefdd372bc06418) ([StatusTextHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5c2b31499961747b784664b87b851e04) handle) | Unsubscribe from subscribe_status_text.
[StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md) | [status_text](#classmavsdk_1_1_telemetry_1a2f31c0668ed1ac1bfdfa4b2e9a2023a9) () const | Poll for '[StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md)' (blocking).
[ActuatorControlTargetHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a01438bbc123454e745b8874bc91bf874) | [subscribe_actuator_control_target](#classmavsdk_1_1_telemetry_1ae9754c6f90e3fd95a9cab766ca588e0d) (const [ActuatorControlTargetCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ada6af3de1b60b93a709345c3a8ede551) & callback) | Subscribe to 'actuator control target' updates.
void | [unsubscribe_actuator_control_target](#classmavsdk_1_1_telemetry_1a13672a6c91e4bb74bf92fe9c779eeb40) ([ActuatorControlTargetHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a01438bbc123454e745b8874bc91bf874) handle) | Unsubscribe from subscribe_actuator_control_target.
[ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md) | [actuator_control_target](#classmavsdk_1_1_telemetry_1af4ffa70ff58c46b50be93a0fbf960f95) () const | Poll for '[ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md)' (blocking).
[ActuatorOutputStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac0bbbb060195848f41de50600489713f) | [subscribe_actuator_output_status](#classmavsdk_1_1_telemetry_1a65bbf4a157aa8898beeb57440808bbe6) (const [ActuatorOutputStatusCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2b1e800ce1ba6fb776351416340ac8b9) & callback) | Subscribe to 'actuator output status' updates.
void | [unsubscribe_actuator_output_status](#classmavsdk_1_1_telemetry_1aa40e1ff8e1eb2c9d0b6a8f36db6823b6) ([ActuatorOutputStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac0bbbb060195848f41de50600489713f) handle) | Unsubscribe from subscribe_actuator_output_status.
[ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md) | [actuator_output_status](#classmavsdk_1_1_telemetry_1a68fa1619dfad0a7cfcc2725025669252) () const | Poll for '[ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md)' (blocking).
[OdometryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a23b33f337ed73ec584e8ac8868c72f86) | [subscribe_odometry](#classmavsdk_1_1_telemetry_1a6def40a24f3eb85ca0cb076c5a576677) (const [OdometryCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a8cd23f7364f8f5cb22869155da67c65d) & callback) | Subscribe to 'odometry' updates.
void | [unsubscribe_odometry](#classmavsdk_1_1_telemetry_1a235d029cb79a071d137c7cfb03afb4b6) ([OdometryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a23b33f337ed73ec584e8ac8868c72f86) handle) | Unsubscribe from subscribe_odometry.
[Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) | [odometry](#classmavsdk_1_1_telemetry_1a715b6e8ba1206059706f08844a0b96d2) () const | Poll for '[Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md)' (blocking).
[PositionVelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aef1424caca3967e67336140af395e59c) | [subscribe_position_velocity_ned](#classmavsdk_1_1_telemetry_1a70f1331b7df5e5bda2711b1141d86d68) (const [PositionVelocityNedCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5a38deb284622ff6926703e1e5c96a74) & callback) | Subscribe to 'position velocity' updates.
void | [unsubscribe_position_velocity_ned](#classmavsdk_1_1_telemetry_1ab561338c465b3b3e1d060fdd8c078306) ([PositionVelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aef1424caca3967e67336140af395e59c) handle) | Unsubscribe from subscribe_position_velocity_ned.
[PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md) | [position_velocity_ned](#classmavsdk_1_1_telemetry_1af9b06944ca73ad09caadacd9f4fae950) () const | Poll for '[PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md)' (blocking).
[GroundTruthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a373568d10564c5a3c3d325e5975a3a23) | [subscribe_ground_truth](#classmavsdk_1_1_telemetry_1a3853c0b62fe26202e13896f94af4f72c) (const [GroundTruthCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a222aae53852a2c535f6d69ed57221f13) & callback) | Subscribe to 'ground truth' updates.
void | [unsubscribe_ground_truth](#classmavsdk_1_1_telemetry_1a69dedf60328c385dedae208e7ca3e2b5) ([GroundTruthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a373568d10564c5a3c3d325e5975a3a23) handle) | Unsubscribe from subscribe_ground_truth.
[GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md) | [ground_truth](#classmavsdk_1_1_telemetry_1a1b5f387edc39e33b86954f2048133f71) () const | Poll for '[GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md)' (blocking).
[FixedwingMetricsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ab180a383ee3e17f20bb1b026b667db04) | [subscribe_fixedwing_metrics](#classmavsdk_1_1_telemetry_1ac5117bf112c932f47ffe283df7aac7f1) (const [FixedwingMetricsCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5b42dbef0ef6d8c1768d503d0437f1e3) & callback) | Subscribe to 'fixedwing metrics' updates.
void | [unsubscribe_fixedwing_metrics](#classmavsdk_1_1_telemetry_1a40a910811684cffe050f8dea0d682565) ([FixedwingMetricsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ab180a383ee3e17f20bb1b026b667db04) handle) | Unsubscribe from subscribe_fixedwing_metrics.
[FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md) | [fixedwing_metrics](#classmavsdk_1_1_telemetry_1a2ab8d2a8d017d46e77d49c4f899c7cbf) () const | Poll for '[FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md)' (blocking).
[ImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5b45a4c22abb67617d37ae56efdf8360) | [subscribe_imu](#classmavsdk_1_1_telemetry_1a24d0904fd8529724a76f8a4fda5bdb70) (const [ImuCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4fbc2ad274fd5a8af077004d2d7bd984) & callback) | Subscribe to 'IMU' updates (in SI units in NED body frame).
void | [unsubscribe_imu](#classmavsdk_1_1_telemetry_1a2af03957c69efd9b5cbb7537def56155) ([ImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5b45a4c22abb67617d37ae56efdf8360) handle) | Unsubscribe from subscribe_imu.
[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) | [imu](#classmavsdk_1_1_telemetry_1a1a4e43b7bdcd988442955d2a5465b977) () const | Poll for '[Imu](structmavsdk_1_1_telemetry_1_1_imu.md)' (blocking).
[ScaledImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4968d9d418d48a4368f2915023ca0014) | [subscribe_scaled_imu](#classmavsdk_1_1_telemetry_1a61bd540f505a3a6acd858ca169e868b3) (const [ScaledImuCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a26159a775adcfbc42302234b7108d94f) & callback) | Subscribe to 'Scaled IMU' updates.
void | [unsubscribe_scaled_imu](#classmavsdk_1_1_telemetry_1a69acf5d201425f0a318a36ad6230fb46) ([ScaledImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4968d9d418d48a4368f2915023ca0014) handle) | Unsubscribe from subscribe_scaled_imu.
[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) | [scaled_imu](#classmavsdk_1_1_telemetry_1ab6a515ba85a67bc80e6e1c9a05d1f94d) () const | Poll for '[Imu](structmavsdk_1_1_telemetry_1_1_imu.md)' (blocking).
[RawImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a416f5b4dc6c51d78d05572d5cc18f3fb) | [subscribe_raw_imu](#classmavsdk_1_1_telemetry_1a98db826585b84957478a6195d46f0491) (const [RawImuCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a92711da85d343cb58b73561e6b730c76) & callback) | Subscribe to 'Raw IMU' updates.
void | [unsubscribe_raw_imu](#classmavsdk_1_1_telemetry_1aec5b5fbbb37654bb6dae9607451929d7) ([RawImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a416f5b4dc6c51d78d05572d5cc18f3fb) handle) | Unsubscribe from subscribe_raw_imu.
[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) | [raw_imu](#classmavsdk_1_1_telemetry_1a691464f001ddf8d02b97bcf137f5cf8a) () const | Poll for '[Imu](structmavsdk_1_1_telemetry_1_1_imu.md)' (blocking).
[HealthAllOkHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa882d4938eb491cce5b7ca6aead2384c) | [subscribe_health_all_ok](#classmavsdk_1_1_telemetry_1acc64edfa8230926024cdefe93ab10c7f) (const [HealthAllOkCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a71cdcadfaa988dc14029e0b9fdbe742d) & callback) | Subscribe to 'HealthAllOk' updates.
void | [unsubscribe_health_all_ok](#classmavsdk_1_1_telemetry_1ab0dd814f6ca883ded70839bf7ec0010c) ([HealthAllOkHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa882d4938eb491cce5b7ca6aead2384c) handle) | Unsubscribe from subscribe_health_all_ok.
bool | [health_all_ok](#classmavsdk_1_1_telemetry_1ad6d833741b5576f07204d268c5cd4d06) () const | Poll for 'bool' (blocking).
[UnixEpochTimeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4c4b00adfab9b7f04530133aafbafbd5) | [subscribe_unix_epoch_time](#classmavsdk_1_1_telemetry_1a172eb7793c61744a8195e38c0612ec1f) (const [UnixEpochTimeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a321c7d809ae8f56bb8a361d5e5ce6391) & callback) | Subscribe to 'unix epoch time' updates.
void | [unsubscribe_unix_epoch_time](#classmavsdk_1_1_telemetry_1acba0439f1175aa29c072d85efa0c68c6) ([UnixEpochTimeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4c4b00adfab9b7f04530133aafbafbd5) handle) | Unsubscribe from subscribe_unix_epoch_time.
uint64_t | [unix_epoch_time](#classmavsdk_1_1_telemetry_1ab5ea5f6bb35b5670e34d5697d8c880f4) () const | Poll for 'uint64_t' (blocking).
[DistanceSensorHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aaa6bd0660aeb7d3f1d12240ce0710878) | [subscribe_distance_sensor](#classmavsdk_1_1_telemetry_1a07cb00743cab5df75f0a656eb2ebaed1) (const [DistanceSensorCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aacfdb5e2cce7f3f77c68b36f020ed1f2) & callback) | Subscribe to 'Distance Sensor' updates.
void | [unsubscribe_distance_sensor](#classmavsdk_1_1_telemetry_1a726d49ea6b63128e7d16f781e016c192) ([DistanceSensorHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aaa6bd0660aeb7d3f1d12240ce0710878) handle) | Unsubscribe from subscribe_distance_sensor.
[DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md) | [distance_sensor](#classmavsdk_1_1_telemetry_1aa01828c0ffcb4727b884ffeae8fef59a) () const | Poll for '[DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md)' (blocking).
[ScaledPressureHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad19871b63833f64bed071db6017e4a22) | [subscribe_scaled_pressure](#classmavsdk_1_1_telemetry_1a934fe2b50a79219e3c6d404fc7d6825d) (const [ScaledPressureCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac123edc254bb1874edc08a0f531f82b1) & callback) | Subscribe to 'Scaled Pressure' updates.
void | [unsubscribe_scaled_pressure](#classmavsdk_1_1_telemetry_1a725b8f4f2bdda5eca799a0829f287848) ([ScaledPressureHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad19871b63833f64bed071db6017e4a22) handle) | Unsubscribe from subscribe_scaled_pressure.
[ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md) | [scaled_pressure](#classmavsdk_1_1_telemetry_1a825ecb6af46663034f982c3c3d6da022) () const | Poll for '[ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md)' (blocking).
[HeadingHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a11cc43135c8162fd8478ba76a5b3166d) | [subscribe_heading](#classmavsdk_1_1_telemetry_1a0ebfabba364867869314e389b118584b) (const [HeadingCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa3bca0adab525a4c733c1e7f5c5dd8b3) & callback) | Subscribe to '[Heading](structmavsdk_1_1_telemetry_1_1_heading.md)' updates.
void | [unsubscribe_heading](#classmavsdk_1_1_telemetry_1afe400d67b33f60a43d3bd823f598c0e9) ([HeadingHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a11cc43135c8162fd8478ba76a5b3166d) handle) | Unsubscribe from subscribe_heading.
[Heading](structmavsdk_1_1_telemetry_1_1_heading.md) | [heading](#classmavsdk_1_1_telemetry_1a2aec80b167a3076903be4fe52847a4d3) () const | Poll for '[Heading](structmavsdk_1_1_telemetry_1_1_heading.md)' (blocking).
[AltitudeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae0714c14e07a08b4748c96ea20f1d12f) | [subscribe_altitude](#classmavsdk_1_1_telemetry_1ae50ada2726b9a074157c73ddf0894b23) (const [AltitudeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a868986de0ab3990d4fe1ec842bce6adf) & callback) | Subscribe to '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' updates.
void | [unsubscribe_altitude](#classmavsdk_1_1_telemetry_1aee258bae5149771cf37a6e5b5d63415c) ([AltitudeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae0714c14e07a08b4748c96ea20f1d12f) handle) | Unsubscribe from subscribe_altitude.
[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) | [altitude](#classmavsdk_1_1_telemetry_1a53f3c06d9b4e0f737f69c060c2be621d) () const | Poll for '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' (blocking).
void | [set_rate_position_async](#classmavsdk_1_1_telemetry_1ad7e5b576edb9398c8f5f2f14626b984a) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'position' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_position](#classmavsdk_1_1_telemetry_1a665439f3d5f8c58b3ef3dd427cf4782b) (double rate_hz)const | Set rate to 'position' updates.
void | [set_rate_home_async](#classmavsdk_1_1_telemetry_1a098f4c4f50fc3ac2c153ef152208fbbe) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'home position' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_home](#classmavsdk_1_1_telemetry_1af90e28ad8a8f05401176c98e427eecfc) (double rate_hz)const | Set rate to 'home position' updates.
void | [set_rate_in_air_async](#classmavsdk_1_1_telemetry_1a9ea77b7ef64acd1e25b05e593e638c70) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to in-air updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_in_air](#classmavsdk_1_1_telemetry_1a8f179e8397b395e61a48529ceeba2b14) (double rate_hz)const | Set rate to in-air updates.
void | [set_rate_landed_state_async](#classmavsdk_1_1_telemetry_1a180fff93b120a67c16ad5993f0b38847) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to landed state updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_landed_state](#classmavsdk_1_1_telemetry_1a53a3428c602c1f91cfcffdba188a4e51) (double rate_hz)const | Set rate to landed state updates.
void | [set_rate_vtol_state_async](#classmavsdk_1_1_telemetry_1a18f47beba583e6814061f95e68a3851d) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to VTOL state updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_vtol_state](#classmavsdk_1_1_telemetry_1a943c2e32a12098a117c4bd4eed7cdc22) (double rate_hz)const | Set rate to VTOL state updates.
void | [set_rate_attitude_quaternion_async](#classmavsdk_1_1_telemetry_1a1eb6bc9b25d1043405ac30e13172a272) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'attitude euler angle' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_attitude_quaternion](#classmavsdk_1_1_telemetry_1adfc8e1a3bfa0f459350640630283716d) (double rate_hz)const | Set rate to 'attitude euler angle' updates.
void | [set_rate_attitude_euler_async](#classmavsdk_1_1_telemetry_1aabf20f904d9c65582cdf167f7b0275a9) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'attitude quaternion' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_attitude_euler](#classmavsdk_1_1_telemetry_1adc7a43d7143261df5f97fdc8a882fdf3) (double rate_hz)const | Set rate to 'attitude quaternion' updates.
void | [set_rate_camera_attitude_async](#classmavsdk_1_1_telemetry_1a520f15e42f5f1b3987ca2a9cd94a3d9a) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate of camera attitude updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_camera_attitude](#classmavsdk_1_1_telemetry_1a427da223d16ce07a61b07d4e5af1ab04) (double rate_hz)const | Set rate of camera attitude updates.
void | [set_rate_velocity_ned_async](#classmavsdk_1_1_telemetry_1a9429ffa784fa56adee69c5017abedee4) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'ground speed' updates (NED).
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_velocity_ned](#classmavsdk_1_1_telemetry_1ab5cb79fd53f27f245808a6bb9ed3225d) (double rate_hz)const | Set rate to 'ground speed' updates (NED).
void | [set_rate_gps_info_async](#classmavsdk_1_1_telemetry_1ae6ada3cd6d4e9835dd4d1d712f1195e4) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'GPS info' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_gps_info](#classmavsdk_1_1_telemetry_1a14510bcb6fe3c31d91653d32d354613f) (double rate_hz)const | Set rate to 'GPS info' updates.
void | [set_rate_battery_async](#classmavsdk_1_1_telemetry_1a5615e21f616997dfca1318c96a7e550e) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'battery' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_battery](#classmavsdk_1_1_telemetry_1ae781d2e950a535a465f2bc1575e9f893) (double rate_hz)const | Set rate to 'battery' updates.
void | [set_rate_rc_status_async](#classmavsdk_1_1_telemetry_1a8cf84eaca875626bc53ed03e98d6eb7e) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'RC status' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_rc_status](#classmavsdk_1_1_telemetry_1acbfc54792f79c5fd2a9855278981f8ca) (double rate_hz)const | Set rate to 'RC status' updates.
void | [set_rate_actuator_control_target_async](#classmavsdk_1_1_telemetry_1aa44e3a76c482f273a2f1bc1a09bec27c) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'actuator control target' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_actuator_control_target](#classmavsdk_1_1_telemetry_1aa43efb510038f1bb95241953ae09c998) (double rate_hz)const | Set rate to 'actuator control target' updates.
void | [set_rate_actuator_output_status_async](#classmavsdk_1_1_telemetry_1a2ad19c1101962ed7cfeec89b7fae0f9c) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'actuator output status' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_actuator_output_status](#classmavsdk_1_1_telemetry_1a48b3e3a288ba6a8d38914c4827124006) (double rate_hz)const | Set rate to 'actuator output status' updates.
void | [set_rate_odometry_async](#classmavsdk_1_1_telemetry_1a23e507e1d53c6603479701f5e2af49ce) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'odometry' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_odometry](#classmavsdk_1_1_telemetry_1a4368bf825cec3bc9369d57546a45391e) (double rate_hz)const | Set rate to 'odometry' updates.
void | [set_rate_position_velocity_ned_async](#classmavsdk_1_1_telemetry_1a9a4c3b6affa497dd22e464f515ca278c) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'position velocity' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_position_velocity_ned](#classmavsdk_1_1_telemetry_1a64fe3457589cd208a9f7bd5dea763da1) (double rate_hz)const | Set rate to 'position velocity' updates.
void | [set_rate_ground_truth_async](#classmavsdk_1_1_telemetry_1a16b28ebdc6d211a5b182bd8d0abb4d2e) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'ground truth' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_ground_truth](#classmavsdk_1_1_telemetry_1a23b2962e5b7681ece3fcbc72220d6b48) (double rate_hz)const | Set rate to 'ground truth' updates.
void | [set_rate_fixedwing_metrics_async](#classmavsdk_1_1_telemetry_1a1484ccdcf4ba20a151e380e7bd7b9869) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'fixedwing metrics' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_fixedwing_metrics](#classmavsdk_1_1_telemetry_1ab345a5925d132c27e0a5e1ab65a1e2c1) (double rate_hz)const | Set rate to 'fixedwing metrics' updates.
void | [set_rate_imu_async](#classmavsdk_1_1_telemetry_1a7dca435daa0de2db2d2e9d588c6bed99) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'IMU' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_imu](#classmavsdk_1_1_telemetry_1a4e0d1dc2350e06f68f472d85dc69d175) (double rate_hz)const | Set rate to 'IMU' updates.
void | [set_rate_scaled_imu_async](#classmavsdk_1_1_telemetry_1aebbf2eb2e5d117d8b40f21075845467c) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'Scaled IMU' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_scaled_imu](#classmavsdk_1_1_telemetry_1af8dc4f38bf7cc89f700c985a04e03237) (double rate_hz)const | Set rate to 'Scaled IMU' updates.
void | [set_rate_raw_imu_async](#classmavsdk_1_1_telemetry_1a36d19058a0f71d711de3e50ba718704e) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'Raw IMU' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_raw_imu](#classmavsdk_1_1_telemetry_1a020cb8760e6f00b759c8ef564d8801ad) (double rate_hz)const | Set rate to 'Raw IMU' updates.
void | [set_rate_unix_epoch_time_async](#classmavsdk_1_1_telemetry_1a74b18cd8a5faed4d46b244db0a6e3c50) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'unix epoch time' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_unix_epoch_time](#classmavsdk_1_1_telemetry_1a340ac34547672ee07131bca34cbbb820) (double rate_hz)const | Set rate to 'unix epoch time' updates.
void | [set_rate_distance_sensor_async](#classmavsdk_1_1_telemetry_1a0371c470866b539b3aa1e254c974aa43) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to 'Distance Sensor' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_distance_sensor](#classmavsdk_1_1_telemetry_1a7f536359536478691d7db980ffe49e49) (double rate_hz)const | Set rate to 'Distance Sensor' updates.
void | [set_rate_altitude_async](#classmavsdk_1_1_telemetry_1a15461dd3f64aef2b921c9f06ee144bc1) (double rate_hz, const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) callback) | Set rate to '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' updates.
[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) | [set_rate_altitude](#classmavsdk_1_1_telemetry_1a100fc786b86637385c6188ea53121b98) (double rate_hz)const | Set rate to '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' updates.
void | [get_gps_global_origin_async](#classmavsdk_1_1_telemetry_1a60cca43e2f87e3fd3a9e170ff2b64e0a) (const [GetGpsGlobalOriginCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a350ee89a7e30a691e130e29ace8917ef) callback) | Get the GPS location of where the estimator has been initialized.
std::pair< [Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75), [Telemetry::GpsGlobalOrigin](structmavsdk_1_1_telemetry_1_1_gps_global_origin.md) > | [get_gps_global_origin](#classmavsdk_1_1_telemetry_1a77747e7cea5a4d644bd6bec9441c7bfb) () const | Get the GPS location of where the estimator has been initialized.
const [Telemetry](classmavsdk_1_1_telemetry.md) & | [operator=](#classmavsdk_1_1_telemetry_1a703ac978c925be8806921925cf16aca9) (const [Telemetry](classmavsdk_1_1_telemetry.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Telemetry() {#classmavsdk_1_1_telemetry_1a6c8c8ed8759fc8c6e9fd4e7644c63cbe}
```cpp
mavsdk::Telemetry::Telemetry(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto telemetry = Telemetry(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Telemetry() {#classmavsdk_1_1_telemetry_1af78f18fbb117c82d5ffe21e015535067}
```cpp
mavsdk::Telemetry::Telemetry(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto telemetry = Telemetry(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Telemetry() {#classmavsdk_1_1_telemetry_1a6ffca3dba4c6102ae6602822a140c8fc}
```cpp
mavsdk::Telemetry::~Telemetry() override
```


Destructor (internal use only).


### Telemetry() {#classmavsdk_1_1_telemetry_1ad734f199b82a9928c63230676c9789e9}
```cpp
mavsdk::Telemetry::Telemetry(const Telemetry &other)
```


Copy constructor.


**Parameters**

* const [Telemetry](classmavsdk_1_1_telemetry.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b}

```cpp
using mavsdk::Telemetry::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Telemetry](classmavsdk_1_1_telemetry.md) calls.


### typedef PositionCallback {#classmavsdk_1_1_telemetry_1a978b371d636226e198995462afa63552}

```cpp
using mavsdk::Telemetry::PositionCallback =  std::function<void(Position)>
```


Callback type for subscribe_position.


### typedef PositionHandle {#classmavsdk_1_1_telemetry_1a7d82a98ea53c2aa254624bc943ec22f1}

```cpp
using mavsdk::Telemetry::PositionHandle =  Handle<Position>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_position.


### typedef HomeCallback {#classmavsdk_1_1_telemetry_1aaac029969c37a001d43e2788a6abf634}

```cpp
using mavsdk::Telemetry::HomeCallback =  std::function<void(Position)>
```


Callback type for subscribe_home.


### typedef HomeHandle {#classmavsdk_1_1_telemetry_1a7a2b3a8b50d66e911870700ea3fe007d}

```cpp
using mavsdk::Telemetry::HomeHandle =  Handle<Position>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_home.


### typedef InAirCallback {#classmavsdk_1_1_telemetry_1af96cca452305dd8f51b42d4663f15a26}

```cpp
using mavsdk::Telemetry::InAirCallback =  std::function<void(bool)>
```


Callback type for subscribe_in_air.


### typedef InAirHandle {#classmavsdk_1_1_telemetry_1a17582f68fabac027ab354073e0eca8e5}

```cpp
using mavsdk::Telemetry::InAirHandle =  Handle<bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_in_air.


### typedef LandedStateCallback {#classmavsdk_1_1_telemetry_1a0cd8ef17abdd7c3d6a9ee761ccc6ae5e}

```cpp
using mavsdk::Telemetry::LandedStateCallback =  std::function<void(LandedState)>
```


Callback type for subscribe_landed_state.


### typedef LandedStateHandle {#classmavsdk_1_1_telemetry_1a125f2ecfe3f5ce588e6d1799392a5fd7}

```cpp
using mavsdk::Telemetry::LandedStateHandle =  Handle<LandedState>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_landed_state.


### typedef ArmedCallback {#classmavsdk_1_1_telemetry_1a9d23a4092d94e50694390e9f41b8c419}

```cpp
using mavsdk::Telemetry::ArmedCallback =  std::function<void(bool)>
```


Callback type for subscribe_armed.


### typedef ArmedHandle {#classmavsdk_1_1_telemetry_1a3cfe4b90e156fb055066f8164985311c}

```cpp
using mavsdk::Telemetry::ArmedHandle =  Handle<bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_armed.


### typedef VtolStateCallback {#classmavsdk_1_1_telemetry_1abf52126ce4d4efb99560aa8e8e58f20c}

```cpp
using mavsdk::Telemetry::VtolStateCallback =  std::function<void(VtolState)>
```


Callback type for subscribe_vtol_state.


### typedef VtolStateHandle {#classmavsdk_1_1_telemetry_1a824bab94514afa56334acf41b6f7d244}

```cpp
using mavsdk::Telemetry::VtolStateHandle =  Handle<VtolState>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_vtol_state.


### typedef AttitudeQuaternionCallback {#classmavsdk_1_1_telemetry_1ad16e61245511a99e930d6fdcbd761a30}

```cpp
using mavsdk::Telemetry::AttitudeQuaternionCallback =  std::function<void(Quaternion)>
```


Callback type for subscribe_attitude_quaternion.


### typedef AttitudeQuaternionHandle {#classmavsdk_1_1_telemetry_1a0ce62986aedcd06d845406a2d99257d3}

```cpp
using mavsdk::Telemetry::AttitudeQuaternionHandle =  Handle<Quaternion>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude_quaternion.


### typedef AttitudeEulerCallback {#classmavsdk_1_1_telemetry_1a321c7607922369926fbd5f2821986cba}

```cpp
using mavsdk::Telemetry::AttitudeEulerCallback =  std::function<void(EulerAngle)>
```


Callback type for subscribe_attitude_euler.


### typedef AttitudeEulerHandle {#classmavsdk_1_1_telemetry_1ae73f3d3ac224438a6cd07344fda9543b}

```cpp
using mavsdk::Telemetry::AttitudeEulerHandle =  Handle<EulerAngle>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude_euler.


### typedef AttitudeAngularVelocityBodyCallback {#classmavsdk_1_1_telemetry_1a35ff8def3048faeab7f732153d51085f}

```cpp
using mavsdk::Telemetry::AttitudeAngularVelocityBodyCallback =  std::function<void(AngularVelocityBody)>
```


Callback type for subscribe_attitude_angular_velocity_body.


### typedef AttitudeAngularVelocityBodyHandle {#classmavsdk_1_1_telemetry_1a2328e39c1a96ce9a090cb19283d3ffc1}

```cpp
using mavsdk::Telemetry::AttitudeAngularVelocityBodyHandle =  Handle<AngularVelocityBody>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_attitude_angular_velocity_body.


### typedef CameraAttitudeQuaternionCallback {#classmavsdk_1_1_telemetry_1aa83dafa14e9b5179573a574f6fbdd973}

```cpp
using mavsdk::Telemetry::CameraAttitudeQuaternionCallback =  std::function<void(Quaternion)>
```


Callback type for subscribe_camera_attitude_quaternion.


### typedef CameraAttitudeQuaternionHandle {#classmavsdk_1_1_telemetry_1aeb2a99828961bf2a4d5c24753e020358}

```cpp
using mavsdk::Telemetry::CameraAttitudeQuaternionHandle =  Handle<Quaternion>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_camera_attitude_quaternion.


### typedef CameraAttitudeEulerCallback {#classmavsdk_1_1_telemetry_1aa29f9bb0767ba8c384bfe1df69f2fdd9}

```cpp
using mavsdk::Telemetry::CameraAttitudeEulerCallback =  std::function<void(EulerAngle)>
```


Callback type for subscribe_camera_attitude_euler.


### typedef CameraAttitudeEulerHandle {#classmavsdk_1_1_telemetry_1a76471c91115d6e03e6165a3e1315808b}

```cpp
using mavsdk::Telemetry::CameraAttitudeEulerHandle =  Handle<EulerAngle>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_camera_attitude_euler.


### typedef VelocityNedCallback {#classmavsdk_1_1_telemetry_1ab5859d2f6a9c9bd81282166b3de92342}

```cpp
using mavsdk::Telemetry::VelocityNedCallback =  std::function<void(VelocityNed)>
```


Callback type for subscribe_velocity_ned.


### typedef VelocityNedHandle {#classmavsdk_1_1_telemetry_1a2c3898f33bfa1bffe86681aaca33343e}

```cpp
using mavsdk::Telemetry::VelocityNedHandle =  Handle<VelocityNed>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_velocity_ned.


### typedef GpsInfoCallback {#classmavsdk_1_1_telemetry_1ad8fa90886b2283eace09b4b46708048b}

```cpp
using mavsdk::Telemetry::GpsInfoCallback =  std::function<void(GpsInfo)>
```


Callback type for subscribe_gps_info.


### typedef GpsInfoHandle {#classmavsdk_1_1_telemetry_1a87598b504f6ddf3ba0620b0458f2d39c}

```cpp
using mavsdk::Telemetry::GpsInfoHandle =  Handle<GpsInfo>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_gps_info.


### typedef RawGpsCallback {#classmavsdk_1_1_telemetry_1a915868d562ed445fa30beaa9140ea97c}

```cpp
using mavsdk::Telemetry::RawGpsCallback =  std::function<void(RawGps)>
```


Callback type for subscribe_raw_gps.


### typedef RawGpsHandle {#classmavsdk_1_1_telemetry_1a75a62eb3624bf77ea6860d5d965636fd}

```cpp
using mavsdk::Telemetry::RawGpsHandle =  Handle<RawGps>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_raw_gps.


### typedef BatteryCallback {#classmavsdk_1_1_telemetry_1af4b121c576ef2ae567b1d571b12dff9d}

```cpp
using mavsdk::Telemetry::BatteryCallback =  std::function<void(Battery)>
```


Callback type for subscribe_battery.


### typedef BatteryHandle {#classmavsdk_1_1_telemetry_1ad9c79c3a89de6f28aabc771a2b5cc92f}

```cpp
using mavsdk::Telemetry::BatteryHandle =  Handle<Battery>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_battery.


### typedef FlightModeCallback {#classmavsdk_1_1_telemetry_1a2d7318d0823771b7a586c40199bdb482}

```cpp
using mavsdk::Telemetry::FlightModeCallback =  std::function<void(FlightMode)>
```


Callback type for subscribe_flight_mode.


### typedef FlightModeHandle {#classmavsdk_1_1_telemetry_1add61cde1555ed1cec93a34bbdadeef5c}

```cpp
using mavsdk::Telemetry::FlightModeHandle =  Handle<FlightMode>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_flight_mode.


### typedef HealthCallback {#classmavsdk_1_1_telemetry_1a7a120dd053091c644e0e2e47fdcbeb75}

```cpp
using mavsdk::Telemetry::HealthCallback =  std::function<void(Health)>
```


Callback type for subscribe_health.


### typedef HealthHandle {#classmavsdk_1_1_telemetry_1a52869d9dd1a240385b0d213a7057f3df}

```cpp
using mavsdk::Telemetry::HealthHandle =  Handle<Health>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_health.


### typedef RcStatusCallback {#classmavsdk_1_1_telemetry_1aafcd706b805898301b574ffa2b909b85}

```cpp
using mavsdk::Telemetry::RcStatusCallback =  std::function<void(RcStatus)>
```


Callback type for subscribe_rc_status.


### typedef RcStatusHandle {#classmavsdk_1_1_telemetry_1ae3bd26bbc4c12eddc7f90f42482078de}

```cpp
using mavsdk::Telemetry::RcStatusHandle =  Handle<RcStatus>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_rc_status.


### typedef StatusTextCallback {#classmavsdk_1_1_telemetry_1a46e51ff90fe779990ed09a593c1c7898}

```cpp
using mavsdk::Telemetry::StatusTextCallback =  std::function<void(StatusText)>
```


Callback type for subscribe_status_text.


### typedef StatusTextHandle {#classmavsdk_1_1_telemetry_1a5c2b31499961747b784664b87b851e04}

```cpp
using mavsdk::Telemetry::StatusTextHandle =  Handle<StatusText>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_status_text.


### typedef ActuatorControlTargetCallback {#classmavsdk_1_1_telemetry_1ada6af3de1b60b93a709345c3a8ede551}

```cpp
using mavsdk::Telemetry::ActuatorControlTargetCallback =  std::function<void(ActuatorControlTarget)>
```


Callback type for subscribe_actuator_control_target.


### typedef ActuatorControlTargetHandle {#classmavsdk_1_1_telemetry_1a01438bbc123454e745b8874bc91bf874}

```cpp
using mavsdk::Telemetry::ActuatorControlTargetHandle =  Handle<ActuatorControlTarget>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_actuator_control_target.


### typedef ActuatorOutputStatusCallback {#classmavsdk_1_1_telemetry_1a2b1e800ce1ba6fb776351416340ac8b9}

```cpp
using mavsdk::Telemetry::ActuatorOutputStatusCallback =  std::function<void(ActuatorOutputStatus)>
```


Callback type for subscribe_actuator_output_status.


### typedef ActuatorOutputStatusHandle {#classmavsdk_1_1_telemetry_1ac0bbbb060195848f41de50600489713f}

```cpp
using mavsdk::Telemetry::ActuatorOutputStatusHandle =  Handle<ActuatorOutputStatus>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_actuator_output_status.


### typedef OdometryCallback {#classmavsdk_1_1_telemetry_1a8cd23f7364f8f5cb22869155da67c65d}

```cpp
using mavsdk::Telemetry::OdometryCallback =  std::function<void(Odometry)>
```


Callback type for subscribe_odometry.


### typedef OdometryHandle {#classmavsdk_1_1_telemetry_1a23b33f337ed73ec584e8ac8868c72f86}

```cpp
using mavsdk::Telemetry::OdometryHandle =  Handle<Odometry>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_odometry.


### typedef PositionVelocityNedCallback {#classmavsdk_1_1_telemetry_1a5a38deb284622ff6926703e1e5c96a74}

```cpp
using mavsdk::Telemetry::PositionVelocityNedCallback =  std::function<void(PositionVelocityNed)>
```


Callback type for subscribe_position_velocity_ned.


### typedef PositionVelocityNedHandle {#classmavsdk_1_1_telemetry_1aef1424caca3967e67336140af395e59c}

```cpp
using mavsdk::Telemetry::PositionVelocityNedHandle =  Handle<PositionVelocityNed>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_position_velocity_ned.


### typedef GroundTruthCallback {#classmavsdk_1_1_telemetry_1a222aae53852a2c535f6d69ed57221f13}

```cpp
using mavsdk::Telemetry::GroundTruthCallback =  std::function<void(GroundTruth)>
```


Callback type for subscribe_ground_truth.


### typedef GroundTruthHandle {#classmavsdk_1_1_telemetry_1a373568d10564c5a3c3d325e5975a3a23}

```cpp
using mavsdk::Telemetry::GroundTruthHandle =  Handle<GroundTruth>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_ground_truth.


### typedef FixedwingMetricsCallback {#classmavsdk_1_1_telemetry_1a5b42dbef0ef6d8c1768d503d0437f1e3}

```cpp
using mavsdk::Telemetry::FixedwingMetricsCallback =  std::function<void(FixedwingMetrics)>
```


Callback type for subscribe_fixedwing_metrics.


### typedef FixedwingMetricsHandle {#classmavsdk_1_1_telemetry_1ab180a383ee3e17f20bb1b026b667db04}

```cpp
using mavsdk::Telemetry::FixedwingMetricsHandle =  Handle<FixedwingMetrics>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_fixedwing_metrics.


### typedef ImuCallback {#classmavsdk_1_1_telemetry_1a4fbc2ad274fd5a8af077004d2d7bd984}

```cpp
using mavsdk::Telemetry::ImuCallback =  std::function<void(Imu)>
```


Callback type for subscribe_imu.


### typedef ImuHandle {#classmavsdk_1_1_telemetry_1a5b45a4c22abb67617d37ae56efdf8360}

```cpp
using mavsdk::Telemetry::ImuHandle =  Handle<Imu>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_imu.


### typedef ScaledImuCallback {#classmavsdk_1_1_telemetry_1a26159a775adcfbc42302234b7108d94f}

```cpp
using mavsdk::Telemetry::ScaledImuCallback =  std::function<void(Imu)>
```


Callback type for subscribe_scaled_imu.


### typedef ScaledImuHandle {#classmavsdk_1_1_telemetry_1a4968d9d418d48a4368f2915023ca0014}

```cpp
using mavsdk::Telemetry::ScaledImuHandle =  Handle<Imu>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_scaled_imu.


### typedef RawImuCallback {#classmavsdk_1_1_telemetry_1a92711da85d343cb58b73561e6b730c76}

```cpp
using mavsdk::Telemetry::RawImuCallback =  std::function<void(Imu)>
```


Callback type for subscribe_raw_imu.


### typedef RawImuHandle {#classmavsdk_1_1_telemetry_1a416f5b4dc6c51d78d05572d5cc18f3fb}

```cpp
using mavsdk::Telemetry::RawImuHandle =  Handle<Imu>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_raw_imu.


### typedef HealthAllOkCallback {#classmavsdk_1_1_telemetry_1a71cdcadfaa988dc14029e0b9fdbe742d}

```cpp
using mavsdk::Telemetry::HealthAllOkCallback =  std::function<void(bool)>
```


Callback type for subscribe_health_all_ok.


### typedef HealthAllOkHandle {#classmavsdk_1_1_telemetry_1aa882d4938eb491cce5b7ca6aead2384c}

```cpp
using mavsdk::Telemetry::HealthAllOkHandle =  Handle<bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_health_all_ok.


### typedef UnixEpochTimeCallback {#classmavsdk_1_1_telemetry_1a321c7d809ae8f56bb8a361d5e5ce6391}

```cpp
using mavsdk::Telemetry::UnixEpochTimeCallback =  std::function<void(uint64_t)>
```


Callback type for subscribe_unix_epoch_time.


### typedef UnixEpochTimeHandle {#classmavsdk_1_1_telemetry_1a4c4b00adfab9b7f04530133aafbafbd5}

```cpp
using mavsdk::Telemetry::UnixEpochTimeHandle =  Handle<uint64_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_unix_epoch_time.


### typedef DistanceSensorCallback {#classmavsdk_1_1_telemetry_1aacfdb5e2cce7f3f77c68b36f020ed1f2}

```cpp
using mavsdk::Telemetry::DistanceSensorCallback =  std::function<void(DistanceSensor)>
```


Callback type for subscribe_distance_sensor.


### typedef DistanceSensorHandle {#classmavsdk_1_1_telemetry_1aaa6bd0660aeb7d3f1d12240ce0710878}

```cpp
using mavsdk::Telemetry::DistanceSensorHandle =  Handle<DistanceSensor>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_distance_sensor.


### typedef ScaledPressureCallback {#classmavsdk_1_1_telemetry_1ac123edc254bb1874edc08a0f531f82b1}

```cpp
using mavsdk::Telemetry::ScaledPressureCallback =  std::function<void(ScaledPressure)>
```


Callback type for subscribe_scaled_pressure.


### typedef ScaledPressureHandle {#classmavsdk_1_1_telemetry_1ad19871b63833f64bed071db6017e4a22}

```cpp
using mavsdk::Telemetry::ScaledPressureHandle =  Handle<ScaledPressure>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_scaled_pressure.


### typedef HeadingCallback {#classmavsdk_1_1_telemetry_1aa3bca0adab525a4c733c1e7f5c5dd8b3}

```cpp
using mavsdk::Telemetry::HeadingCallback =  std::function<void(Heading)>
```


Callback type for subscribe_heading.


### typedef HeadingHandle {#classmavsdk_1_1_telemetry_1a11cc43135c8162fd8478ba76a5b3166d}

```cpp
using mavsdk::Telemetry::HeadingHandle =  Handle<Heading>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_heading.


### typedef AltitudeCallback {#classmavsdk_1_1_telemetry_1a868986de0ab3990d4fe1ec842bce6adf}

```cpp
using mavsdk::Telemetry::AltitudeCallback =  std::function<void(Altitude)>
```


Callback type for subscribe_altitude.


### typedef AltitudeHandle {#classmavsdk_1_1_telemetry_1ae0714c14e07a08b4748c96ea20f1d12f}

```cpp
using mavsdk::Telemetry::AltitudeHandle =  Handle<Altitude>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_altitude.


### typedef GetGpsGlobalOriginCallback {#classmavsdk_1_1_telemetry_1a350ee89a7e30a691e130e29ace8917ef}

```cpp
using mavsdk::Telemetry::GetGpsGlobalOriginCallback =  std::function<void(Result, GpsGlobalOrigin)>
```


Callback type for get_gps_global_origin_async.


## Member Enumeration Documentation


### enum FixType {#classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639de}


GPS fix type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639deaeb42b0f27d1fc2ca9bd645212a14c874"></span> `NoGps` | No GPS connected. 
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639dea7458f6cf09e53df9495d3ee0d11868c4"></span> `NoFix` | No position information, GPS is connected. 
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639dead6800eded02a7eaceb638929dbd9ea55"></span> `Fix2D` | 2D position. 
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639dead791aa90d108c9b0c76ec28cd4dfbb0f"></span> `Fix3D` | 3D position. 
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639dea8372860807abaec59412bd6376f51b5f"></span> `FixDgps` | DGPS/SBAS aided 3D position. 
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639dea1eacb557d24c49af2ec6832c5fc32413"></span> `RtkFloat` | RTK float, 3D position. 
<span id="classmavsdk_1_1_telemetry_1a548213e1b26615d7b6d1b0b3934639dea9effa0afed44833d540fec2c57e67426"></span> `RtkFixed` | RTK Fixed, 3D position. 

### enum FlightMode {#classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fe}


Flight modes.

For more information about flight modes, check out [https://docs.px4.io/master/en/config/flight_mode.html](https://docs.px4.io/master/en/config/flight_mode.html).

Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Mode not known. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feae7d31fc0602fb2ede144d18cdffd816b"></span> `Ready` | Armed and ready to take off. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea56373a80447c41b9a29e500e62d6884e"></span> `Takeoff` | Taking off. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feabcd8db575b47c838e5d551e3973db4ac"></span> `Hold` | Holding (hovering in place (or circling for fixed-wing vehicles). 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea70d529695c253d17e992cb9265abc57f"></span> `Mission` | In mission. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea146115c3278581584dcbaac1a77a2588"></span> `ReturnToLaunch` | Returning to launch position (then landing). 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea512ef7c688a2c8572d5e16f44e17e869"></span> `Land` | Landing. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feabb085f1e0d3843dda2a4c70437ad1410"></span> `Offboard` | In 'offboard' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea08bb3de7dafcf47f05b8c5a9dc0983c0"></span> `FollowMe` | In 'follow-me' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feae1ba155a9f2e8c3be94020eef32a0301"></span> `Manual` | In 'Manual' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feaa7a697581399b9be37a545416d4cd2d9"></span> `Altctl` | In '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) Control' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea46780e4f8c113481c868da4dd16fcd41"></span> `Posctl` | In '[Position](structmavsdk_1_1_telemetry_1_1_position.md) Control' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feae10e0f017fee32a9bb2fa9fae53afd70"></span> `Acro` | In 'Acro' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4feafda22026db89cdc5e88b262ad9424b41"></span> `Stabilized` | In 'Stabilize' mode. 
<span id="classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fea1e0c225e976316373c8c08ddc7a154fa"></span> `Rattitude` | In 'Rattitude' mode. 

### enum StatusTextType {#classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8}


Status types.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8aa603905470e2a5b8c13e96b579ef0dba"></span> `Debug` | Debug. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8a4059b0251f66a18cb56f544728796875"></span> `Info` | Information. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8a24efa7ee4511563b16144f39706d594f"></span> `Notice` | Notice. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8a0eaadb4fcb48a0a0ed7bc9868be9fbaa"></span> `Warning` | Warning. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8a278d01e5af56273bae1bb99a98b370cd"></span> `Critical` | Critical. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8ab92071d61c88498171928745ca53078b"></span> `Alert` | Alert. 
<span id="classmavsdk_1_1_telemetry_1ada3ebb336abad223a98bc2a625e0e7d8aa3fa706f20bc0b7858b7ae6932261940"></span> `Emergency` | Emergency. 

### enum LandedState {#classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046}


Landed State enumeration.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Landed state is unknown. 
<span id="classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046a2b9beed57034f5727573d7ded76cf777"></span> `OnGround` | The vehicle is on the ground. 
<span id="classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046aee4e669a07b061d70b9b79dfed9cb5e7"></span> `InAir` | The vehicle is in the air. 
<span id="classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046a85916c5f3400cfa25d988f05b6736a94"></span> `TakingOff` | The vehicle is taking off. 
<span id="classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046a41bd61e268fedccfb0d91dd571dd28b2"></span> `Landing` | The vehicle is landing. 

### enum VtolState {#classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166}


VTOL State enumeration.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166aec0fc0100c4fc1ce4eea230c3dc10360"></span> `Undefined` | MAV is not configured as VTOL. 
<span id="classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166a45f439a04f9144adc1dcf5a4fbda0026"></span> `TransitionToFw` | VTOL is in transition from multicopter to fixed-wing. 
<span id="classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166af3abadebd9602a0acb33350f64fa7661"></span> `TransitionToMc` | VTOL is in transition from fixed-wing to multicopter. 
<span id="classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166aac81adaad0b2a7d6077edd5c319a6048"></span> `Mc` | VTOL is in multicopter state. 
<span id="classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166a9b5de99abdffa3a42bd6517ed1a034e4"></span> `Fw` | VTOL is in fixed-wing state. 

### enum Result {#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75}


Possible results returned for telemetry requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Success: the telemetry command was accepted by the vehicle. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command refused by vehicle. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Request not supported. 

## Member Function Documentation


### subscribe_position() {#classmavsdk_1_1_telemetry_1a647f3d61b4f3301fd8c2aca1afcafb31}
```cpp
PositionHandle mavsdk::Telemetry::subscribe_position(const PositionCallback &callback)
```


Subscribe to 'position' updates.


**Parameters**

* const [PositionCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a978b371d636226e198995462afa63552)& **callback** - 

**Returns**

&emsp;[PositionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7d82a98ea53c2aa254624bc943ec22f1) - 

### unsubscribe_position() {#classmavsdk_1_1_telemetry_1abf10cb55e2c477bbb875c0944938b76c}
```cpp
void mavsdk::Telemetry::unsubscribe_position(PositionHandle handle)
```


Unsubscribe from subscribe_position.


**Parameters**

* [PositionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7d82a98ea53c2aa254624bc943ec22f1) **handle** - 

### position() {#classmavsdk_1_1_telemetry_1a2299da1bc63313c429f07ab0fdbe5335}
```cpp
Position mavsdk::Telemetry::position() const
```


Poll for '[Position](structmavsdk_1_1_telemetry_1_1_position.md)' (blocking).


**Returns**

&emsp;[Position](structmavsdk_1_1_telemetry_1_1_position.md) - One [Position](structmavsdk_1_1_telemetry_1_1_position.md) update.

### subscribe_home() {#classmavsdk_1_1_telemetry_1ae57e4c8741ba7ae05961cadfabc82875}
```cpp
HomeHandle mavsdk::Telemetry::subscribe_home(const HomeCallback &callback)
```


Subscribe to 'home position' updates.


**Parameters**

* const [HomeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aaac029969c37a001d43e2788a6abf634)& **callback** - 

**Returns**

&emsp;[HomeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7a2b3a8b50d66e911870700ea3fe007d) - 

### unsubscribe_home() {#classmavsdk_1_1_telemetry_1a83feb66aa64946b954ca305b6cc2f7ea}
```cpp
void mavsdk::Telemetry::unsubscribe_home(HomeHandle handle)
```


Unsubscribe from subscribe_home.


**Parameters**

* [HomeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7a2b3a8b50d66e911870700ea3fe007d) **handle** - 

### home() {#classmavsdk_1_1_telemetry_1ad5c239b93aa1923edd1b97494a3fbfe7}
```cpp
Position mavsdk::Telemetry::home() const
```


Poll for '[Position](structmavsdk_1_1_telemetry_1_1_position.md)' (blocking).


**Returns**

&emsp;[Position](structmavsdk_1_1_telemetry_1_1_position.md) - One [Position](structmavsdk_1_1_telemetry_1_1_position.md) update.

### subscribe_in_air() {#classmavsdk_1_1_telemetry_1a0fc586a0c90141e125a305fd8211e8d8}
```cpp
InAirHandle mavsdk::Telemetry::subscribe_in_air(const InAirCallback &callback)
```


Subscribe to in-air updates.


**Parameters**

* const [InAirCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1af96cca452305dd8f51b42d4663f15a26)& **callback** - 

**Returns**

&emsp;[InAirHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a17582f68fabac027ab354073e0eca8e5) - 

### unsubscribe_in_air() {#classmavsdk_1_1_telemetry_1a2e23bf258ef69c267ab5b8dee203bd95}
```cpp
void mavsdk::Telemetry::unsubscribe_in_air(InAirHandle handle)
```


Unsubscribe from subscribe_in_air.


**Parameters**

* [InAirHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a17582f68fabac027ab354073e0eca8e5) **handle** - 

### in_air() {#classmavsdk_1_1_telemetry_1a909738ff2fbe104c6eb4524cc9bf2dd5}
```cpp
bool mavsdk::Telemetry::in_air() const
```


Poll for 'bool' (blocking).


**Returns**

&emsp;bool - One bool update.

### subscribe_landed_state() {#classmavsdk_1_1_telemetry_1ae618a7ba0ed197e3d5c3427a63c3cfa0}
```cpp
LandedStateHandle mavsdk::Telemetry::subscribe_landed_state(const LandedStateCallback &callback)
```


Subscribe to landed state updates.


**Parameters**

* const [LandedStateCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0cd8ef17abdd7c3d6a9ee761ccc6ae5e)& **callback** - 

**Returns**

&emsp;[LandedStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a125f2ecfe3f5ce588e6d1799392a5fd7) - 

### unsubscribe_landed_state() {#classmavsdk_1_1_telemetry_1a17df3b1e3917f788725f39cf879145f6}
```cpp
void mavsdk::Telemetry::unsubscribe_landed_state(LandedStateHandle handle)
```


Unsubscribe from subscribe_landed_state.


**Parameters**

* [LandedStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a125f2ecfe3f5ce588e6d1799392a5fd7) **handle** - 

### landed_state() {#classmavsdk_1_1_telemetry_1af7d7c385852db38d6320516508ce7465}
```cpp
LandedState mavsdk::Telemetry::landed_state() const
```


Poll for 'LandedState' (blocking).


**Returns**

&emsp;[LandedState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac6639935bc3b35b1da553cde41e8f046) - One LandedState update.

### subscribe_armed() {#classmavsdk_1_1_telemetry_1a2ea3978439ed7e21973a41e1f2c2b24f}
```cpp
ArmedHandle mavsdk::Telemetry::subscribe_armed(const ArmedCallback &callback)
```


Subscribe to armed updates.


**Parameters**

* const [ArmedCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a9d23a4092d94e50694390e9f41b8c419)& **callback** - 

**Returns**

&emsp;[ArmedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a3cfe4b90e156fb055066f8164985311c) - 

### unsubscribe_armed() {#classmavsdk_1_1_telemetry_1a8c6080d520f6d2fdac9cb97279ffcbb9}
```cpp
void mavsdk::Telemetry::unsubscribe_armed(ArmedHandle handle)
```


Unsubscribe from subscribe_armed.


**Parameters**

* [ArmedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a3cfe4b90e156fb055066f8164985311c) **handle** - 

### armed() {#classmavsdk_1_1_telemetry_1a6620142adc47f069262e5bf69dbb3876}
```cpp
bool mavsdk::Telemetry::armed() const
```


Poll for 'bool' (blocking).


**Returns**

&emsp;bool - One bool update.

### subscribe_vtol_state() {#classmavsdk_1_1_telemetry_1a956eb63dc90a2cf905590f6f309edb36}
```cpp
VtolStateHandle mavsdk::Telemetry::subscribe_vtol_state(const VtolStateCallback &callback)
```


subscribe to vtol state Updates


**Parameters**

* const [VtolStateCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1abf52126ce4d4efb99560aa8e8e58f20c)& **callback** - 

**Returns**

&emsp;[VtolStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a824bab94514afa56334acf41b6f7d244) - 

### unsubscribe_vtol_state() {#classmavsdk_1_1_telemetry_1a932eb8b43e90e64bd131b3257f747963}
```cpp
void mavsdk::Telemetry::unsubscribe_vtol_state(VtolStateHandle handle)
```


Unsubscribe from subscribe_vtol_state.


**Parameters**

* [VtolStateHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a824bab94514afa56334acf41b6f7d244) **handle** - 

### vtol_state() {#classmavsdk_1_1_telemetry_1a6d3fd68f3e639a2bf9940e9f38f220c3}
```cpp
VtolState mavsdk::Telemetry::vtol_state() const
```


Poll for 'VtolState' (blocking).


**Returns**

&emsp;[VtolState](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a256f951d993aa120c437d989a6e94166) - One VtolState update.

### subscribe_attitude_quaternion() {#classmavsdk_1_1_telemetry_1a9248befe3416252c7d8fb63b19842421}
```cpp
AttitudeQuaternionHandle mavsdk::Telemetry::subscribe_attitude_quaternion(const AttitudeQuaternionCallback &callback)
```


Subscribe to 'attitude' updates (quaternion).


**Parameters**

* const [AttitudeQuaternionCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad16e61245511a99e930d6fdcbd761a30)& **callback** - 

**Returns**

&emsp;[AttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0ce62986aedcd06d845406a2d99257d3) - 

### unsubscribe_attitude_quaternion() {#classmavsdk_1_1_telemetry_1a06034401ac16901d50b850ded0b064b5}
```cpp
void mavsdk::Telemetry::unsubscribe_attitude_quaternion(AttitudeQuaternionHandle handle)
```


Unsubscribe from subscribe_attitude_quaternion.


**Parameters**

* [AttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a0ce62986aedcd06d845406a2d99257d3) **handle** - 

### attitude_quaternion() {#classmavsdk_1_1_telemetry_1aae76890957b33727be72a39807448c88}
```cpp
Quaternion mavsdk::Telemetry::attitude_quaternion() const
```


Poll for '[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md)' (blocking).


**Returns**

&emsp;[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) - One [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) update.

### subscribe_attitude_euler() {#classmavsdk_1_1_telemetry_1aaf90ca9cc8adcc9cb1963293bdfcefd6}
```cpp
AttitudeEulerHandle mavsdk::Telemetry::subscribe_attitude_euler(const AttitudeEulerCallback &callback)
```


Subscribe to 'attitude' updates (Euler).


**Parameters**

* const [AttitudeEulerCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a321c7607922369926fbd5f2821986cba)& **callback** - 

**Returns**

&emsp;[AttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae73f3d3ac224438a6cd07344fda9543b) - 

### unsubscribe_attitude_euler() {#classmavsdk_1_1_telemetry_1aa770088117b037446efa8cbfe058e54d}
```cpp
void mavsdk::Telemetry::unsubscribe_attitude_euler(AttitudeEulerHandle handle)
```


Unsubscribe from subscribe_attitude_euler.


**Parameters**

* [AttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae73f3d3ac224438a6cd07344fda9543b) **handle** - 

### attitude_euler() {#classmavsdk_1_1_telemetry_1a03035bb72324e843372eb69cf7899ce5}
```cpp
EulerAngle mavsdk::Telemetry::attitude_euler() const
```


Poll for '[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md)' (blocking).


**Returns**

&emsp;[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) - One [EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) update.

### subscribe_attitude_angular_velocity_body() {#classmavsdk_1_1_telemetry_1a3bbefcdb4e9cd9af9692626984504cb7}
```cpp
AttitudeAngularVelocityBodyHandle mavsdk::Telemetry::subscribe_attitude_angular_velocity_body(const AttitudeAngularVelocityBodyCallback &callback)
```


Subscribe to 'attitude' updates (angular velocity)


**Parameters**

* const [AttitudeAngularVelocityBodyCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a35ff8def3048faeab7f732153d51085f)& **callback** - 

**Returns**

&emsp;[AttitudeAngularVelocityBodyHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2328e39c1a96ce9a090cb19283d3ffc1) - 

### unsubscribe_attitude_angular_velocity_body() {#classmavsdk_1_1_telemetry_1a2319d78b4e214e5b2f3ef55a1c32e2c5}
```cpp
void mavsdk::Telemetry::unsubscribe_attitude_angular_velocity_body(AttitudeAngularVelocityBodyHandle handle)
```


Unsubscribe from subscribe_attitude_angular_velocity_body.


**Parameters**

* [AttitudeAngularVelocityBodyHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2328e39c1a96ce9a090cb19283d3ffc1) **handle** - 

### attitude_angular_velocity_body() {#classmavsdk_1_1_telemetry_1a8d9e2489b79c2cdbabaef8b6bb8e2952}
```cpp
AngularVelocityBody mavsdk::Telemetry::attitude_angular_velocity_body() const
```


Poll for '[AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md)' (blocking).


**Returns**

&emsp;[AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md) - One [AngularVelocityBody](structmavsdk_1_1_telemetry_1_1_angular_velocity_body.md) update.

### subscribe_camera_attitude_quaternion() {#classmavsdk_1_1_telemetry_1a9b7517e15c841765fbb8b1230e73bf82}
```cpp
CameraAttitudeQuaternionHandle mavsdk::Telemetry::subscribe_camera_attitude_quaternion(const CameraAttitudeQuaternionCallback &callback)
```


Subscribe to 'camera attitude' updates (quaternion).


**Parameters**

* const [CameraAttitudeQuaternionCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa83dafa14e9b5179573a574f6fbdd973)& **callback** - 

**Returns**

&emsp;[CameraAttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aeb2a99828961bf2a4d5c24753e020358) - 

### unsubscribe_camera_attitude_quaternion() {#classmavsdk_1_1_telemetry_1aa92270b8a30b22271f65d92e9a7ed9a1}
```cpp
void mavsdk::Telemetry::unsubscribe_camera_attitude_quaternion(CameraAttitudeQuaternionHandle handle)
```


Unsubscribe from subscribe_camera_attitude_quaternion.


**Parameters**

* [CameraAttitudeQuaternionHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aeb2a99828961bf2a4d5c24753e020358) **handle** - 

### camera_attitude_quaternion() {#classmavsdk_1_1_telemetry_1a3c07447351d3b6195d5e2526e7b128b3}
```cpp
Quaternion mavsdk::Telemetry::camera_attitude_quaternion() const
```


Poll for '[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md)' (blocking).


**Returns**

&emsp;[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) - One [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) update.

### subscribe_camera_attitude_euler() {#classmavsdk_1_1_telemetry_1aa50015ba9b1decb825d45e459191c342}
```cpp
CameraAttitudeEulerHandle mavsdk::Telemetry::subscribe_camera_attitude_euler(const CameraAttitudeEulerCallback &callback)
```


Subscribe to 'camera attitude' updates (Euler).


**Parameters**

* const [CameraAttitudeEulerCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa29f9bb0767ba8c384bfe1df69f2fdd9)& **callback** - 

**Returns**

&emsp;[CameraAttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a76471c91115d6e03e6165a3e1315808b) - 

### unsubscribe_camera_attitude_euler() {#classmavsdk_1_1_telemetry_1a90b8dfe6b83afc908e4c236bbbc32930}
```cpp
void mavsdk::Telemetry::unsubscribe_camera_attitude_euler(CameraAttitudeEulerHandle handle)
```


Unsubscribe from subscribe_camera_attitude_euler.


**Parameters**

* [CameraAttitudeEulerHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a76471c91115d6e03e6165a3e1315808b) **handle** - 

### camera_attitude_euler() {#classmavsdk_1_1_telemetry_1a635643d955f0cd9a805914501f819796}
```cpp
EulerAngle mavsdk::Telemetry::camera_attitude_euler() const
```


Poll for '[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md)' (blocking).


**Returns**

&emsp;[EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) - One [EulerAngle](structmavsdk_1_1_telemetry_1_1_euler_angle.md) update.

### subscribe_velocity_ned() {#classmavsdk_1_1_telemetry_1a9b5e6bd8fb05324fd7a99d0260933c9d}
```cpp
VelocityNedHandle mavsdk::Telemetry::subscribe_velocity_ned(const VelocityNedCallback &callback)
```


Subscribe to 'ground speed' updates (NED).


**Parameters**

* const [VelocityNedCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ab5859d2f6a9c9bd81282166b3de92342)& **callback** - 

**Returns**

&emsp;[VelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2c3898f33bfa1bffe86681aaca33343e) - 

### unsubscribe_velocity_ned() {#classmavsdk_1_1_telemetry_1ae14a663f3b4820e891a4d83c2f8aa2c3}
```cpp
void mavsdk::Telemetry::unsubscribe_velocity_ned(VelocityNedHandle handle)
```


Unsubscribe from subscribe_velocity_ned.


**Parameters**

* [VelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2c3898f33bfa1bffe86681aaca33343e) **handle** - 

### velocity_ned() {#classmavsdk_1_1_telemetry_1a40a86062c0322d6be7c86d8e15a52f28}
```cpp
VelocityNed mavsdk::Telemetry::velocity_ned() const
```


Poll for '[VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md)' (blocking).


**Returns**

&emsp;[VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md) - One [VelocityNed](structmavsdk_1_1_telemetry_1_1_velocity_ned.md) update.

### subscribe_gps_info() {#classmavsdk_1_1_telemetry_1a1933fa5b008856b5d621489d785c68e7}
```cpp
GpsInfoHandle mavsdk::Telemetry::subscribe_gps_info(const GpsInfoCallback &callback)
```


Subscribe to 'GPS info' updates.


**Parameters**

* const [GpsInfoCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad8fa90886b2283eace09b4b46708048b)& **callback** - 

**Returns**

&emsp;[GpsInfoHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a87598b504f6ddf3ba0620b0458f2d39c) - 

### unsubscribe_gps_info() {#classmavsdk_1_1_telemetry_1a96ba022d27983893a814ad4a787bfa51}
```cpp
void mavsdk::Telemetry::unsubscribe_gps_info(GpsInfoHandle handle)
```


Unsubscribe from subscribe_gps_info.


**Parameters**

* [GpsInfoHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a87598b504f6ddf3ba0620b0458f2d39c) **handle** - 

### gps_info() {#classmavsdk_1_1_telemetry_1a983dabc1aed50745b326072662c419e8}
```cpp
GpsInfo mavsdk::Telemetry::gps_info() const
```


Poll for '[GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md)' (blocking).


**Returns**

&emsp;[GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md) - One [GpsInfo](structmavsdk_1_1_telemetry_1_1_gps_info.md) update.

### subscribe_raw_gps() {#classmavsdk_1_1_telemetry_1a39c324fdd723a2b5c5559ddadad597f0}
```cpp
RawGpsHandle mavsdk::Telemetry::subscribe_raw_gps(const RawGpsCallback &callback)
```


Subscribe to 'Raw GPS' updates.


**Parameters**

* const [RawGpsCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a915868d562ed445fa30beaa9140ea97c)& **callback** - 

**Returns**

&emsp;[RawGpsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a75a62eb3624bf77ea6860d5d965636fd) - 

### unsubscribe_raw_gps() {#classmavsdk_1_1_telemetry_1acc6c03a6d27b268f609c72b66aa48a09}
```cpp
void mavsdk::Telemetry::unsubscribe_raw_gps(RawGpsHandle handle)
```


Unsubscribe from subscribe_raw_gps.


**Parameters**

* [RawGpsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a75a62eb3624bf77ea6860d5d965636fd) **handle** - 

### raw_gps() {#classmavsdk_1_1_telemetry_1ac43c29e435b6c1a6594854adc6a1bf6c}
```cpp
RawGps mavsdk::Telemetry::raw_gps() const
```


Poll for '[RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md)' (blocking).


**Returns**

&emsp;[RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md) - One [RawGps](structmavsdk_1_1_telemetry_1_1_raw_gps.md) update.

### subscribe_battery() {#classmavsdk_1_1_telemetry_1ad7e7ceb85181aaac7bf5553af7f45e2a}
```cpp
BatteryHandle mavsdk::Telemetry::subscribe_battery(const BatteryCallback &callback)
```


Subscribe to 'battery' updates.


**Parameters**

* const [BatteryCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1af4b121c576ef2ae567b1d571b12dff9d)& **callback** - 

**Returns**

&emsp;[BatteryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad9c79c3a89de6f28aabc771a2b5cc92f) - 

### unsubscribe_battery() {#classmavsdk_1_1_telemetry_1a72ddd509a9480ba59f073fa8f1ee5dbc}
```cpp
void mavsdk::Telemetry::unsubscribe_battery(BatteryHandle handle)
```


Unsubscribe from subscribe_battery.


**Parameters**

* [BatteryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad9c79c3a89de6f28aabc771a2b5cc92f) **handle** - 

### battery() {#classmavsdk_1_1_telemetry_1afb3bad3c7a36c14ae97492df3f6bbd54}
```cpp
Battery mavsdk::Telemetry::battery() const
```


Poll for '[Battery](structmavsdk_1_1_telemetry_1_1_battery.md)' (blocking).


**Returns**

&emsp;[Battery](structmavsdk_1_1_telemetry_1_1_battery.md) - One [Battery](structmavsdk_1_1_telemetry_1_1_battery.md) update.

### subscribe_flight_mode() {#classmavsdk_1_1_telemetry_1accd831cbb9134c845744a5c70da1b418}
```cpp
FlightModeHandle mavsdk::Telemetry::subscribe_flight_mode(const FlightModeCallback &callback)
```


Subscribe to 'flight mode' updates.


**Parameters**

* const [FlightModeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2d7318d0823771b7a586c40199bdb482)& **callback** - 

**Returns**

&emsp;[FlightModeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1add61cde1555ed1cec93a34bbdadeef5c) - 

### unsubscribe_flight_mode() {#classmavsdk_1_1_telemetry_1a64dc8d45d06b23332ba6353d50c9cc17}
```cpp
void mavsdk::Telemetry::unsubscribe_flight_mode(FlightModeHandle handle)
```


Unsubscribe from subscribe_flight_mode.


**Parameters**

* [FlightModeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1add61cde1555ed1cec93a34bbdadeef5c) **handle** - 

### flight_mode() {#classmavsdk_1_1_telemetry_1a4972a3968e379d565e7700f2f51158dd}
```cpp
FlightMode mavsdk::Telemetry::flight_mode() const
```


Poll for 'FlightMode' (blocking).


**Returns**

&emsp;[FlightMode](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a8317d953a82a23654db6f14509acb4fe) - One FlightMode update.

### subscribe_health() {#classmavsdk_1_1_telemetry_1a093a702abd1370ff97f4d9ebce449942}
```cpp
HealthHandle mavsdk::Telemetry::subscribe_health(const HealthCallback &callback)
```


Subscribe to 'health' updates.


**Parameters**

* const [HealthCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a7a120dd053091c644e0e2e47fdcbeb75)& **callback** - 

**Returns**

&emsp;[HealthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a52869d9dd1a240385b0d213a7057f3df) - 

### unsubscribe_health() {#classmavsdk_1_1_telemetry_1af7830aff90366a0255e71117c07f5484}
```cpp
void mavsdk::Telemetry::unsubscribe_health(HealthHandle handle)
```


Unsubscribe from subscribe_health.


**Parameters**

* [HealthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a52869d9dd1a240385b0d213a7057f3df) **handle** - 

### health() {#classmavsdk_1_1_telemetry_1aae4824c9eeb72603b197c864b5cc5df5}
```cpp
Health mavsdk::Telemetry::health() const
```


Poll for '[Health](structmavsdk_1_1_telemetry_1_1_health.md)' (blocking).


**Returns**

&emsp;[Health](structmavsdk_1_1_telemetry_1_1_health.md) - One [Health](structmavsdk_1_1_telemetry_1_1_health.md) update.

### subscribe_rc_status() {#classmavsdk_1_1_telemetry_1a0252d9434190edab26fc5b3874bdcc1c}
```cpp
RcStatusHandle mavsdk::Telemetry::subscribe_rc_status(const RcStatusCallback &callback)
```


Subscribe to 'RC status' updates.


**Parameters**

* const [RcStatusCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aafcd706b805898301b574ffa2b909b85)& **callback** - 

**Returns**

&emsp;[RcStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae3bd26bbc4c12eddc7f90f42482078de) - 

### unsubscribe_rc_status() {#classmavsdk_1_1_telemetry_1a975a70f6345f73a37894aa7a49495dc7}
```cpp
void mavsdk::Telemetry::unsubscribe_rc_status(RcStatusHandle handle)
```


Unsubscribe from subscribe_rc_status.


**Parameters**

* [RcStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae3bd26bbc4c12eddc7f90f42482078de) **handle** - 

### rc_status() {#classmavsdk_1_1_telemetry_1a59cd497c69f1d32be29a940a2d34a474}
```cpp
RcStatus mavsdk::Telemetry::rc_status() const
```


Poll for '[RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md)' (blocking).


**Returns**

&emsp;[RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md) - One [RcStatus](structmavsdk_1_1_telemetry_1_1_rc_status.md) update.

### subscribe_status_text() {#classmavsdk_1_1_telemetry_1a56e54b5dc245cb07da6088a3033d45f8}
```cpp
StatusTextHandle mavsdk::Telemetry::subscribe_status_text(const StatusTextCallback &callback)
```


Subscribe to 'status text' updates.


**Parameters**

* const [StatusTextCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a46e51ff90fe779990ed09a593c1c7898)& **callback** - 

**Returns**

&emsp;[StatusTextHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5c2b31499961747b784664b87b851e04) - 

### unsubscribe_status_text() {#classmavsdk_1_1_telemetry_1a972adcc6c0c88ddb6aefdd372bc06418}
```cpp
void mavsdk::Telemetry::unsubscribe_status_text(StatusTextHandle handle)
```


Unsubscribe from subscribe_status_text.


**Parameters**

* [StatusTextHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5c2b31499961747b784664b87b851e04) **handle** - 

### status_text() {#classmavsdk_1_1_telemetry_1a2f31c0668ed1ac1bfdfa4b2e9a2023a9}
```cpp
StatusText mavsdk::Telemetry::status_text() const
```


Poll for '[StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md)' (blocking).


**Returns**

&emsp;[StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md) - One [StatusText](structmavsdk_1_1_telemetry_1_1_status_text.md) update.

### subscribe_actuator_control_target() {#classmavsdk_1_1_telemetry_1ae9754c6f90e3fd95a9cab766ca588e0d}
```cpp
ActuatorControlTargetHandle mavsdk::Telemetry::subscribe_actuator_control_target(const ActuatorControlTargetCallback &callback)
```


Subscribe to 'actuator control target' updates.


**Parameters**

* const [ActuatorControlTargetCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ada6af3de1b60b93a709345c3a8ede551)& **callback** - 

**Returns**

&emsp;[ActuatorControlTargetHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a01438bbc123454e745b8874bc91bf874) - 

### unsubscribe_actuator_control_target() {#classmavsdk_1_1_telemetry_1a13672a6c91e4bb74bf92fe9c779eeb40}
```cpp
void mavsdk::Telemetry::unsubscribe_actuator_control_target(ActuatorControlTargetHandle handle)
```


Unsubscribe from subscribe_actuator_control_target.


**Parameters**

* [ActuatorControlTargetHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a01438bbc123454e745b8874bc91bf874) **handle** - 

### actuator_control_target() {#classmavsdk_1_1_telemetry_1af4ffa70ff58c46b50be93a0fbf960f95}
```cpp
ActuatorControlTarget mavsdk::Telemetry::actuator_control_target() const
```


Poll for '[ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md)' (blocking).


**Returns**

&emsp;[ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md) - One [ActuatorControlTarget](structmavsdk_1_1_telemetry_1_1_actuator_control_target.md) update.

### subscribe_actuator_output_status() {#classmavsdk_1_1_telemetry_1a65bbf4a157aa8898beeb57440808bbe6}
```cpp
ActuatorOutputStatusHandle mavsdk::Telemetry::subscribe_actuator_output_status(const ActuatorOutputStatusCallback &callback)
```


Subscribe to 'actuator output status' updates.


**Parameters**

* const [ActuatorOutputStatusCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a2b1e800ce1ba6fb776351416340ac8b9)& **callback** - 

**Returns**

&emsp;[ActuatorOutputStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac0bbbb060195848f41de50600489713f) - 

### unsubscribe_actuator_output_status() {#classmavsdk_1_1_telemetry_1aa40e1ff8e1eb2c9d0b6a8f36db6823b6}
```cpp
void mavsdk::Telemetry::unsubscribe_actuator_output_status(ActuatorOutputStatusHandle handle)
```


Unsubscribe from subscribe_actuator_output_status.


**Parameters**

* [ActuatorOutputStatusHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac0bbbb060195848f41de50600489713f) **handle** - 

### actuator_output_status() {#classmavsdk_1_1_telemetry_1a68fa1619dfad0a7cfcc2725025669252}
```cpp
ActuatorOutputStatus mavsdk::Telemetry::actuator_output_status() const
```


Poll for '[ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md)' (blocking).


**Returns**

&emsp;[ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md) - One [ActuatorOutputStatus](structmavsdk_1_1_telemetry_1_1_actuator_output_status.md) update.

### subscribe_odometry() {#classmavsdk_1_1_telemetry_1a6def40a24f3eb85ca0cb076c5a576677}
```cpp
OdometryHandle mavsdk::Telemetry::subscribe_odometry(const OdometryCallback &callback)
```


Subscribe to 'odometry' updates.


**Parameters**

* const [OdometryCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a8cd23f7364f8f5cb22869155da67c65d)& **callback** - 

**Returns**

&emsp;[OdometryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a23b33f337ed73ec584e8ac8868c72f86) - 

### unsubscribe_odometry() {#classmavsdk_1_1_telemetry_1a235d029cb79a071d137c7cfb03afb4b6}
```cpp
void mavsdk::Telemetry::unsubscribe_odometry(OdometryHandle handle)
```


Unsubscribe from subscribe_odometry.


**Parameters**

* [OdometryHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a23b33f337ed73ec584e8ac8868c72f86) **handle** - 

### odometry() {#classmavsdk_1_1_telemetry_1a715b6e8ba1206059706f08844a0b96d2}
```cpp
Odometry mavsdk::Telemetry::odometry() const
```


Poll for '[Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md)' (blocking).


**Returns**

&emsp;[Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) - One [Odometry](structmavsdk_1_1_telemetry_1_1_odometry.md) update.

### subscribe_position_velocity_ned() {#classmavsdk_1_1_telemetry_1a70f1331b7df5e5bda2711b1141d86d68}
```cpp
PositionVelocityNedHandle mavsdk::Telemetry::subscribe_position_velocity_ned(const PositionVelocityNedCallback &callback)
```


Subscribe to 'position velocity' updates.


**Parameters**

* const [PositionVelocityNedCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5a38deb284622ff6926703e1e5c96a74)& **callback** - 

**Returns**

&emsp;[PositionVelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aef1424caca3967e67336140af395e59c) - 

### unsubscribe_position_velocity_ned() {#classmavsdk_1_1_telemetry_1ab561338c465b3b3e1d060fdd8c078306}
```cpp
void mavsdk::Telemetry::unsubscribe_position_velocity_ned(PositionVelocityNedHandle handle)
```


Unsubscribe from subscribe_position_velocity_ned.


**Parameters**

* [PositionVelocityNedHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aef1424caca3967e67336140af395e59c) **handle** - 

### position_velocity_ned() {#classmavsdk_1_1_telemetry_1af9b06944ca73ad09caadacd9f4fae950}
```cpp
PositionVelocityNed mavsdk::Telemetry::position_velocity_ned() const
```


Poll for '[PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md)' (blocking).


**Returns**

&emsp;[PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md) - One [PositionVelocityNed](structmavsdk_1_1_telemetry_1_1_position_velocity_ned.md) update.

### subscribe_ground_truth() {#classmavsdk_1_1_telemetry_1a3853c0b62fe26202e13896f94af4f72c}
```cpp
GroundTruthHandle mavsdk::Telemetry::subscribe_ground_truth(const GroundTruthCallback &callback)
```


Subscribe to 'ground truth' updates.


**Parameters**

* const [GroundTruthCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a222aae53852a2c535f6d69ed57221f13)& **callback** - 

**Returns**

&emsp;[GroundTruthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a373568d10564c5a3c3d325e5975a3a23) - 

### unsubscribe_ground_truth() {#classmavsdk_1_1_telemetry_1a69dedf60328c385dedae208e7ca3e2b5}
```cpp
void mavsdk::Telemetry::unsubscribe_ground_truth(GroundTruthHandle handle)
```


Unsubscribe from subscribe_ground_truth.


**Parameters**

* [GroundTruthHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a373568d10564c5a3c3d325e5975a3a23) **handle** - 

### ground_truth() {#classmavsdk_1_1_telemetry_1a1b5f387edc39e33b86954f2048133f71}
```cpp
GroundTruth mavsdk::Telemetry::ground_truth() const
```


Poll for '[GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md)' (blocking).


**Returns**

&emsp;[GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md) - One [GroundTruth](structmavsdk_1_1_telemetry_1_1_ground_truth.md) update.

### subscribe_fixedwing_metrics() {#classmavsdk_1_1_telemetry_1ac5117bf112c932f47ffe283df7aac7f1}
```cpp
FixedwingMetricsHandle mavsdk::Telemetry::subscribe_fixedwing_metrics(const FixedwingMetricsCallback &callback)
```


Subscribe to 'fixedwing metrics' updates.


**Parameters**

* const [FixedwingMetricsCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5b42dbef0ef6d8c1768d503d0437f1e3)& **callback** - 

**Returns**

&emsp;[FixedwingMetricsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ab180a383ee3e17f20bb1b026b667db04) - 

### unsubscribe_fixedwing_metrics() {#classmavsdk_1_1_telemetry_1a40a910811684cffe050f8dea0d682565}
```cpp
void mavsdk::Telemetry::unsubscribe_fixedwing_metrics(FixedwingMetricsHandle handle)
```


Unsubscribe from subscribe_fixedwing_metrics.


**Parameters**

* [FixedwingMetricsHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ab180a383ee3e17f20bb1b026b667db04) **handle** - 

### fixedwing_metrics() {#classmavsdk_1_1_telemetry_1a2ab8d2a8d017d46e77d49c4f899c7cbf}
```cpp
FixedwingMetrics mavsdk::Telemetry::fixedwing_metrics() const
```


Poll for '[FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md)' (blocking).


**Returns**

&emsp;[FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md) - One [FixedwingMetrics](structmavsdk_1_1_telemetry_1_1_fixedwing_metrics.md) update.

### subscribe_imu() {#classmavsdk_1_1_telemetry_1a24d0904fd8529724a76f8a4fda5bdb70}
```cpp
ImuHandle mavsdk::Telemetry::subscribe_imu(const ImuCallback &callback)
```


Subscribe to 'IMU' updates (in SI units in NED body frame).


**Parameters**

* const [ImuCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4fbc2ad274fd5a8af077004d2d7bd984)& **callback** - 

**Returns**

&emsp;[ImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5b45a4c22abb67617d37ae56efdf8360) - 

### unsubscribe_imu() {#classmavsdk_1_1_telemetry_1a2af03957c69efd9b5cbb7537def56155}
```cpp
void mavsdk::Telemetry::unsubscribe_imu(ImuHandle handle)
```


Unsubscribe from subscribe_imu.


**Parameters**

* [ImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a5b45a4c22abb67617d37ae56efdf8360) **handle** - 

### imu() {#classmavsdk_1_1_telemetry_1a1a4e43b7bdcd988442955d2a5465b977}
```cpp
Imu mavsdk::Telemetry::imu() const
```


Poll for '[Imu](structmavsdk_1_1_telemetry_1_1_imu.md)' (blocking).


**Returns**

&emsp;[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) - One [Imu](structmavsdk_1_1_telemetry_1_1_imu.md) update.

### subscribe_scaled_imu() {#classmavsdk_1_1_telemetry_1a61bd540f505a3a6acd858ca169e868b3}
```cpp
ScaledImuHandle mavsdk::Telemetry::subscribe_scaled_imu(const ScaledImuCallback &callback)
```


Subscribe to 'Scaled IMU' updates.


**Parameters**

* const [ScaledImuCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a26159a775adcfbc42302234b7108d94f)& **callback** - 

**Returns**

&emsp;[ScaledImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4968d9d418d48a4368f2915023ca0014) - 

### unsubscribe_scaled_imu() {#classmavsdk_1_1_telemetry_1a69acf5d201425f0a318a36ad6230fb46}
```cpp
void mavsdk::Telemetry::unsubscribe_scaled_imu(ScaledImuHandle handle)
```


Unsubscribe from subscribe_scaled_imu.


**Parameters**

* [ScaledImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4968d9d418d48a4368f2915023ca0014) **handle** - 

### scaled_imu() {#classmavsdk_1_1_telemetry_1ab6a515ba85a67bc80e6e1c9a05d1f94d}
```cpp
Imu mavsdk::Telemetry::scaled_imu() const
```


Poll for '[Imu](structmavsdk_1_1_telemetry_1_1_imu.md)' (blocking).


**Returns**

&emsp;[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) - One [Imu](structmavsdk_1_1_telemetry_1_1_imu.md) update.

### subscribe_raw_imu() {#classmavsdk_1_1_telemetry_1a98db826585b84957478a6195d46f0491}
```cpp
RawImuHandle mavsdk::Telemetry::subscribe_raw_imu(const RawImuCallback &callback)
```


Subscribe to 'Raw IMU' updates.


**Parameters**

* const [RawImuCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a92711da85d343cb58b73561e6b730c76)& **callback** - 

**Returns**

&emsp;[RawImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a416f5b4dc6c51d78d05572d5cc18f3fb) - 

### unsubscribe_raw_imu() {#classmavsdk_1_1_telemetry_1aec5b5fbbb37654bb6dae9607451929d7}
```cpp
void mavsdk::Telemetry::unsubscribe_raw_imu(RawImuHandle handle)
```


Unsubscribe from subscribe_raw_imu.


**Parameters**

* [RawImuHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a416f5b4dc6c51d78d05572d5cc18f3fb) **handle** - 

### raw_imu() {#classmavsdk_1_1_telemetry_1a691464f001ddf8d02b97bcf137f5cf8a}
```cpp
Imu mavsdk::Telemetry::raw_imu() const
```


Poll for '[Imu](structmavsdk_1_1_telemetry_1_1_imu.md)' (blocking).


**Returns**

&emsp;[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) - One [Imu](structmavsdk_1_1_telemetry_1_1_imu.md) update.

### subscribe_health_all_ok() {#classmavsdk_1_1_telemetry_1acc64edfa8230926024cdefe93ab10c7f}
```cpp
HealthAllOkHandle mavsdk::Telemetry::subscribe_health_all_ok(const HealthAllOkCallback &callback)
```


Subscribe to 'HealthAllOk' updates.


**Parameters**

* const [HealthAllOkCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a71cdcadfaa988dc14029e0b9fdbe742d)& **callback** - 

**Returns**

&emsp;[HealthAllOkHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa882d4938eb491cce5b7ca6aead2384c) - 

### unsubscribe_health_all_ok() {#classmavsdk_1_1_telemetry_1ab0dd814f6ca883ded70839bf7ec0010c}
```cpp
void mavsdk::Telemetry::unsubscribe_health_all_ok(HealthAllOkHandle handle)
```


Unsubscribe from subscribe_health_all_ok.


**Parameters**

* [HealthAllOkHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa882d4938eb491cce5b7ca6aead2384c) **handle** - 

### health_all_ok() {#classmavsdk_1_1_telemetry_1ad6d833741b5576f07204d268c5cd4d06}
```cpp
bool mavsdk::Telemetry::health_all_ok() const
```


Poll for 'bool' (blocking).


**Returns**

&emsp;bool - One bool update.

### subscribe_unix_epoch_time() {#classmavsdk_1_1_telemetry_1a172eb7793c61744a8195e38c0612ec1f}
```cpp
UnixEpochTimeHandle mavsdk::Telemetry::subscribe_unix_epoch_time(const UnixEpochTimeCallback &callback)
```


Subscribe to 'unix epoch time' updates.


**Parameters**

* const [UnixEpochTimeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a321c7d809ae8f56bb8a361d5e5ce6391)& **callback** - 

**Returns**

&emsp;[UnixEpochTimeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4c4b00adfab9b7f04530133aafbafbd5) - 

### unsubscribe_unix_epoch_time() {#classmavsdk_1_1_telemetry_1acba0439f1175aa29c072d85efa0c68c6}
```cpp
void mavsdk::Telemetry::unsubscribe_unix_epoch_time(UnixEpochTimeHandle handle)
```


Unsubscribe from subscribe_unix_epoch_time.


**Parameters**

* [UnixEpochTimeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a4c4b00adfab9b7f04530133aafbafbd5) **handle** - 

### unix_epoch_time() {#classmavsdk_1_1_telemetry_1ab5ea5f6bb35b5670e34d5697d8c880f4}
```cpp
uint64_t mavsdk::Telemetry::unix_epoch_time() const
```


Poll for 'uint64_t' (blocking).


**Returns**

&emsp;uint64_t - One uint64_t update.

### subscribe_distance_sensor() {#classmavsdk_1_1_telemetry_1a07cb00743cab5df75f0a656eb2ebaed1}
```cpp
DistanceSensorHandle mavsdk::Telemetry::subscribe_distance_sensor(const DistanceSensorCallback &callback)
```


Subscribe to 'Distance Sensor' updates.


**Parameters**

* const [DistanceSensorCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aacfdb5e2cce7f3f77c68b36f020ed1f2)& **callback** - 

**Returns**

&emsp;[DistanceSensorHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aaa6bd0660aeb7d3f1d12240ce0710878) - 

### unsubscribe_distance_sensor() {#classmavsdk_1_1_telemetry_1a726d49ea6b63128e7d16f781e016c192}
```cpp
void mavsdk::Telemetry::unsubscribe_distance_sensor(DistanceSensorHandle handle)
```


Unsubscribe from subscribe_distance_sensor.


**Parameters**

* [DistanceSensorHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aaa6bd0660aeb7d3f1d12240ce0710878) **handle** - 

### distance_sensor() {#classmavsdk_1_1_telemetry_1aa01828c0ffcb4727b884ffeae8fef59a}
```cpp
DistanceSensor mavsdk::Telemetry::distance_sensor() const
```


Poll for '[DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md)' (blocking).


**Returns**

&emsp;[DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md) - One [DistanceSensor](structmavsdk_1_1_telemetry_1_1_distance_sensor.md) update.

### subscribe_scaled_pressure() {#classmavsdk_1_1_telemetry_1a934fe2b50a79219e3c6d404fc7d6825d}
```cpp
ScaledPressureHandle mavsdk::Telemetry::subscribe_scaled_pressure(const ScaledPressureCallback &callback)
```


Subscribe to 'Scaled Pressure' updates.


**Parameters**

* const [ScaledPressureCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ac123edc254bb1874edc08a0f531f82b1)& **callback** - 

**Returns**

&emsp;[ScaledPressureHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad19871b63833f64bed071db6017e4a22) - 

### unsubscribe_scaled_pressure() {#classmavsdk_1_1_telemetry_1a725b8f4f2bdda5eca799a0829f287848}
```cpp
void mavsdk::Telemetry::unsubscribe_scaled_pressure(ScaledPressureHandle handle)
```


Unsubscribe from subscribe_scaled_pressure.


**Parameters**

* [ScaledPressureHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ad19871b63833f64bed071db6017e4a22) **handle** - 

### scaled_pressure() {#classmavsdk_1_1_telemetry_1a825ecb6af46663034f982c3c3d6da022}
```cpp
ScaledPressure mavsdk::Telemetry::scaled_pressure() const
```


Poll for '[ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md)' (blocking).


**Returns**

&emsp;[ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md) - One [ScaledPressure](structmavsdk_1_1_telemetry_1_1_scaled_pressure.md) update.

### subscribe_heading() {#classmavsdk_1_1_telemetry_1a0ebfabba364867869314e389b118584b}
```cpp
HeadingHandle mavsdk::Telemetry::subscribe_heading(const HeadingCallback &callback)
```


Subscribe to '[Heading](structmavsdk_1_1_telemetry_1_1_heading.md)' updates.


**Parameters**

* const [HeadingCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1aa3bca0adab525a4c733c1e7f5c5dd8b3)& **callback** - 

**Returns**

&emsp;[HeadingHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a11cc43135c8162fd8478ba76a5b3166d) - 

### unsubscribe_heading() {#classmavsdk_1_1_telemetry_1afe400d67b33f60a43d3bd823f598c0e9}
```cpp
void mavsdk::Telemetry::unsubscribe_heading(HeadingHandle handle)
```


Unsubscribe from subscribe_heading.


**Parameters**

* [HeadingHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a11cc43135c8162fd8478ba76a5b3166d) **handle** - 

### heading() {#classmavsdk_1_1_telemetry_1a2aec80b167a3076903be4fe52847a4d3}
```cpp
Heading mavsdk::Telemetry::heading() const
```


Poll for '[Heading](structmavsdk_1_1_telemetry_1_1_heading.md)' (blocking).


**Returns**

&emsp;[Heading](structmavsdk_1_1_telemetry_1_1_heading.md) - One [Heading](structmavsdk_1_1_telemetry_1_1_heading.md) update.

### subscribe_altitude() {#classmavsdk_1_1_telemetry_1ae50ada2726b9a074157c73ddf0894b23}
```cpp
AltitudeHandle mavsdk::Telemetry::subscribe_altitude(const AltitudeCallback &callback)
```


Subscribe to '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' updates.


**Parameters**

* const [AltitudeCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a868986de0ab3990d4fe1ec842bce6adf)& **callback** - 

**Returns**

&emsp;[AltitudeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae0714c14e07a08b4748c96ea20f1d12f) - 

### unsubscribe_altitude() {#classmavsdk_1_1_telemetry_1aee258bae5149771cf37a6e5b5d63415c}
```cpp
void mavsdk::Telemetry::unsubscribe_altitude(AltitudeHandle handle)
```


Unsubscribe from subscribe_altitude.


**Parameters**

* [AltitudeHandle](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1ae0714c14e07a08b4748c96ea20f1d12f) **handle** - 

### altitude() {#classmavsdk_1_1_telemetry_1a53f3c06d9b4e0f737f69c060c2be621d}
```cpp
Altitude mavsdk::Telemetry::altitude() const
```


Poll for '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' (blocking).


**Returns**

&emsp;[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) - One [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) update.

### set_rate_position_async() {#classmavsdk_1_1_telemetry_1ad7e5b576edb9398c8f5f2f14626b984a}
```cpp
void mavsdk::Telemetry::set_rate_position_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'position' updates.

This function is non-blocking. See 'set_rate_position' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_position() {#classmavsdk_1_1_telemetry_1a665439f3d5f8c58b3ef3dd427cf4782b}
```cpp
Result mavsdk::Telemetry::set_rate_position(double rate_hz) const
```


Set rate to 'position' updates.

This function is blocking. See 'set_rate_position_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_home_async() {#classmavsdk_1_1_telemetry_1a098f4c4f50fc3ac2c153ef152208fbbe}
```cpp
void mavsdk::Telemetry::set_rate_home_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'home position' updates.

This function is non-blocking. See 'set_rate_home' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_home() {#classmavsdk_1_1_telemetry_1af90e28ad8a8f05401176c98e427eecfc}
```cpp
Result mavsdk::Telemetry::set_rate_home(double rate_hz) const
```


Set rate to 'home position' updates.

This function is blocking. See 'set_rate_home_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_in_air_async() {#classmavsdk_1_1_telemetry_1a9ea77b7ef64acd1e25b05e593e638c70}
```cpp
void mavsdk::Telemetry::set_rate_in_air_async(double rate_hz, const ResultCallback callback)
```


Set rate to in-air updates.

This function is non-blocking. See 'set_rate_in_air' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_in_air() {#classmavsdk_1_1_telemetry_1a8f179e8397b395e61a48529ceeba2b14}
```cpp
Result mavsdk::Telemetry::set_rate_in_air(double rate_hz) const
```


Set rate to in-air updates.

This function is blocking. See 'set_rate_in_air_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_landed_state_async() {#classmavsdk_1_1_telemetry_1a180fff93b120a67c16ad5993f0b38847}
```cpp
void mavsdk::Telemetry::set_rate_landed_state_async(double rate_hz, const ResultCallback callback)
```


Set rate to landed state updates.

This function is non-blocking. See 'set_rate_landed_state' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_landed_state() {#classmavsdk_1_1_telemetry_1a53a3428c602c1f91cfcffdba188a4e51}
```cpp
Result mavsdk::Telemetry::set_rate_landed_state(double rate_hz) const
```


Set rate to landed state updates.

This function is blocking. See 'set_rate_landed_state_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_vtol_state_async() {#classmavsdk_1_1_telemetry_1a18f47beba583e6814061f95e68a3851d}
```cpp
void mavsdk::Telemetry::set_rate_vtol_state_async(double rate_hz, const ResultCallback callback)
```


Set rate to VTOL state updates.

This function is non-blocking. See 'set_rate_vtol_state' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_vtol_state() {#classmavsdk_1_1_telemetry_1a943c2e32a12098a117c4bd4eed7cdc22}
```cpp
Result mavsdk::Telemetry::set_rate_vtol_state(double rate_hz) const
```


Set rate to VTOL state updates.

This function is blocking. See 'set_rate_vtol_state_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_attitude_quaternion_async() {#classmavsdk_1_1_telemetry_1a1eb6bc9b25d1043405ac30e13172a272}
```cpp
void mavsdk::Telemetry::set_rate_attitude_quaternion_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'attitude euler angle' updates.

This function is non-blocking. See 'set_rate_attitude_quaternion' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_attitude_quaternion() {#classmavsdk_1_1_telemetry_1adfc8e1a3bfa0f459350640630283716d}
```cpp
Result mavsdk::Telemetry::set_rate_attitude_quaternion(double rate_hz) const
```


Set rate to 'attitude euler angle' updates.

This function is blocking. See 'set_rate_attitude_quaternion_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_attitude_euler_async() {#classmavsdk_1_1_telemetry_1aabf20f904d9c65582cdf167f7b0275a9}
```cpp
void mavsdk::Telemetry::set_rate_attitude_euler_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'attitude quaternion' updates.

This function is non-blocking. See 'set_rate_attitude_euler' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_attitude_euler() {#classmavsdk_1_1_telemetry_1adc7a43d7143261df5f97fdc8a882fdf3}
```cpp
Result mavsdk::Telemetry::set_rate_attitude_euler(double rate_hz) const
```


Set rate to 'attitude quaternion' updates.

This function is blocking. See 'set_rate_attitude_euler_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_camera_attitude_async() {#classmavsdk_1_1_telemetry_1a520f15e42f5f1b3987ca2a9cd94a3d9a}
```cpp
void mavsdk::Telemetry::set_rate_camera_attitude_async(double rate_hz, const ResultCallback callback)
```


Set rate of camera attitude updates.

This function is non-blocking. See 'set_rate_camera_attitude' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_camera_attitude() {#classmavsdk_1_1_telemetry_1a427da223d16ce07a61b07d4e5af1ab04}
```cpp
Result mavsdk::Telemetry::set_rate_camera_attitude(double rate_hz) const
```


Set rate of camera attitude updates.

This function is blocking. See 'set_rate_camera_attitude_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_velocity_ned_async() {#classmavsdk_1_1_telemetry_1a9429ffa784fa56adee69c5017abedee4}
```cpp
void mavsdk::Telemetry::set_rate_velocity_ned_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'ground speed' updates (NED).

This function is non-blocking. See 'set_rate_velocity_ned' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_velocity_ned() {#classmavsdk_1_1_telemetry_1ab5cb79fd53f27f245808a6bb9ed3225d}
```cpp
Result mavsdk::Telemetry::set_rate_velocity_ned(double rate_hz) const
```


Set rate to 'ground speed' updates (NED).

This function is blocking. See 'set_rate_velocity_ned_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_gps_info_async() {#classmavsdk_1_1_telemetry_1ae6ada3cd6d4e9835dd4d1d712f1195e4}
```cpp
void mavsdk::Telemetry::set_rate_gps_info_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'GPS info' updates.

This function is non-blocking. See 'set_rate_gps_info' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_gps_info() {#classmavsdk_1_1_telemetry_1a14510bcb6fe3c31d91653d32d354613f}
```cpp
Result mavsdk::Telemetry::set_rate_gps_info(double rate_hz) const
```


Set rate to 'GPS info' updates.

This function is blocking. See 'set_rate_gps_info_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_battery_async() {#classmavsdk_1_1_telemetry_1a5615e21f616997dfca1318c96a7e550e}
```cpp
void mavsdk::Telemetry::set_rate_battery_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'battery' updates.

This function is non-blocking. See 'set_rate_battery' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_battery() {#classmavsdk_1_1_telemetry_1ae781d2e950a535a465f2bc1575e9f893}
```cpp
Result mavsdk::Telemetry::set_rate_battery(double rate_hz) const
```


Set rate to 'battery' updates.

This function is blocking. See 'set_rate_battery_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_rc_status_async() {#classmavsdk_1_1_telemetry_1a8cf84eaca875626bc53ed03e98d6eb7e}
```cpp
void mavsdk::Telemetry::set_rate_rc_status_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'RC status' updates.

This function is non-blocking. See 'set_rate_rc_status' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_rc_status() {#classmavsdk_1_1_telemetry_1acbfc54792f79c5fd2a9855278981f8ca}
```cpp
Result mavsdk::Telemetry::set_rate_rc_status(double rate_hz) const
```


Set rate to 'RC status' updates.

This function is blocking. See 'set_rate_rc_status_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_actuator_control_target_async() {#classmavsdk_1_1_telemetry_1aa44e3a76c482f273a2f1bc1a09bec27c}
```cpp
void mavsdk::Telemetry::set_rate_actuator_control_target_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'actuator control target' updates.

This function is non-blocking. See 'set_rate_actuator_control_target' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_actuator_control_target() {#classmavsdk_1_1_telemetry_1aa43efb510038f1bb95241953ae09c998}
```cpp
Result mavsdk::Telemetry::set_rate_actuator_control_target(double rate_hz) const
```


Set rate to 'actuator control target' updates.

This function is blocking. See 'set_rate_actuator_control_target_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_actuator_output_status_async() {#classmavsdk_1_1_telemetry_1a2ad19c1101962ed7cfeec89b7fae0f9c}
```cpp
void mavsdk::Telemetry::set_rate_actuator_output_status_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'actuator output status' updates.

This function is non-blocking. See 'set_rate_actuator_output_status' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_actuator_output_status() {#classmavsdk_1_1_telemetry_1a48b3e3a288ba6a8d38914c4827124006}
```cpp
Result mavsdk::Telemetry::set_rate_actuator_output_status(double rate_hz) const
```


Set rate to 'actuator output status' updates.

This function is blocking. See 'set_rate_actuator_output_status_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_odometry_async() {#classmavsdk_1_1_telemetry_1a23e507e1d53c6603479701f5e2af49ce}
```cpp
void mavsdk::Telemetry::set_rate_odometry_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'odometry' updates.

This function is non-blocking. See 'set_rate_odometry' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_odometry() {#classmavsdk_1_1_telemetry_1a4368bf825cec3bc9369d57546a45391e}
```cpp
Result mavsdk::Telemetry::set_rate_odometry(double rate_hz) const
```


Set rate to 'odometry' updates.

This function is blocking. See 'set_rate_odometry_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_position_velocity_ned_async() {#classmavsdk_1_1_telemetry_1a9a4c3b6affa497dd22e464f515ca278c}
```cpp
void mavsdk::Telemetry::set_rate_position_velocity_ned_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'position velocity' updates.

This function is non-blocking. See 'set_rate_position_velocity_ned' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_position_velocity_ned() {#classmavsdk_1_1_telemetry_1a64fe3457589cd208a9f7bd5dea763da1}
```cpp
Result mavsdk::Telemetry::set_rate_position_velocity_ned(double rate_hz) const
```


Set rate to 'position velocity' updates.

This function is blocking. See 'set_rate_position_velocity_ned_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_ground_truth_async() {#classmavsdk_1_1_telemetry_1a16b28ebdc6d211a5b182bd8d0abb4d2e}
```cpp
void mavsdk::Telemetry::set_rate_ground_truth_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'ground truth' updates.

This function is non-blocking. See 'set_rate_ground_truth' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_ground_truth() {#classmavsdk_1_1_telemetry_1a23b2962e5b7681ece3fcbc72220d6b48}
```cpp
Result mavsdk::Telemetry::set_rate_ground_truth(double rate_hz) const
```


Set rate to 'ground truth' updates.

This function is blocking. See 'set_rate_ground_truth_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_fixedwing_metrics_async() {#classmavsdk_1_1_telemetry_1a1484ccdcf4ba20a151e380e7bd7b9869}
```cpp
void mavsdk::Telemetry::set_rate_fixedwing_metrics_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'fixedwing metrics' updates.

This function is non-blocking. See 'set_rate_fixedwing_metrics' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_fixedwing_metrics() {#classmavsdk_1_1_telemetry_1ab345a5925d132c27e0a5e1ab65a1e2c1}
```cpp
Result mavsdk::Telemetry::set_rate_fixedwing_metrics(double rate_hz) const
```


Set rate to 'fixedwing metrics' updates.

This function is blocking. See 'set_rate_fixedwing_metrics_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_imu_async() {#classmavsdk_1_1_telemetry_1a7dca435daa0de2db2d2e9d588c6bed99}
```cpp
void mavsdk::Telemetry::set_rate_imu_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'IMU' updates.

This function is non-blocking. See 'set_rate_imu' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_imu() {#classmavsdk_1_1_telemetry_1a4e0d1dc2350e06f68f472d85dc69d175}
```cpp
Result mavsdk::Telemetry::set_rate_imu(double rate_hz) const
```


Set rate to 'IMU' updates.

This function is blocking. See 'set_rate_imu_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_scaled_imu_async() {#classmavsdk_1_1_telemetry_1aebbf2eb2e5d117d8b40f21075845467c}
```cpp
void mavsdk::Telemetry::set_rate_scaled_imu_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'Scaled IMU' updates.

This function is non-blocking. See 'set_rate_scaled_imu' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_scaled_imu() {#classmavsdk_1_1_telemetry_1af8dc4f38bf7cc89f700c985a04e03237}
```cpp
Result mavsdk::Telemetry::set_rate_scaled_imu(double rate_hz) const
```


Set rate to 'Scaled IMU' updates.

This function is blocking. See 'set_rate_scaled_imu_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_raw_imu_async() {#classmavsdk_1_1_telemetry_1a36d19058a0f71d711de3e50ba718704e}
```cpp
void mavsdk::Telemetry::set_rate_raw_imu_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'Raw IMU' updates.

This function is non-blocking. See 'set_rate_raw_imu' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_raw_imu() {#classmavsdk_1_1_telemetry_1a020cb8760e6f00b759c8ef564d8801ad}
```cpp
Result mavsdk::Telemetry::set_rate_raw_imu(double rate_hz) const
```


Set rate to 'Raw IMU' updates.

This function is blocking. See 'set_rate_raw_imu_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_unix_epoch_time_async() {#classmavsdk_1_1_telemetry_1a74b18cd8a5faed4d46b244db0a6e3c50}
```cpp
void mavsdk::Telemetry::set_rate_unix_epoch_time_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'unix epoch time' updates.

This function is non-blocking. See 'set_rate_unix_epoch_time' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_unix_epoch_time() {#classmavsdk_1_1_telemetry_1a340ac34547672ee07131bca34cbbb820}
```cpp
Result mavsdk::Telemetry::set_rate_unix_epoch_time(double rate_hz) const
```


Set rate to 'unix epoch time' updates.

This function is blocking. See 'set_rate_unix_epoch_time_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_distance_sensor_async() {#classmavsdk_1_1_telemetry_1a0371c470866b539b3aa1e254c974aa43}
```cpp
void mavsdk::Telemetry::set_rate_distance_sensor_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'Distance Sensor' updates.

This function is non-blocking. See 'set_rate_distance_sensor' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_distance_sensor() {#classmavsdk_1_1_telemetry_1a7f536359536478691d7db980ffe49e49}
```cpp
Result mavsdk::Telemetry::set_rate_distance_sensor(double rate_hz) const
```


Set rate to 'Distance Sensor' updates.

This function is blocking. See 'set_rate_distance_sensor_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### set_rate_altitude_async() {#classmavsdk_1_1_telemetry_1a15461dd3f64aef2b921c9f06ee144bc1}
```cpp
void mavsdk::Telemetry::set_rate_altitude_async(double rate_hz, const ResultCallback callback)
```


Set rate to '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' updates.

This function is non-blocking. See 'set_rate_altitude' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a166e81c6573532978e5940eafdfcec0b) **callback** - 

### set_rate_altitude() {#classmavsdk_1_1_telemetry_1a100fc786b86637385c6188ea53121b98}
```cpp
Result mavsdk::Telemetry::set_rate_altitude(double rate_hz) const
```


Set rate to '[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md)' updates.

This function is blocking. See 'set_rate_altitude_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75) - Result of request.

### get_gps_global_origin_async() {#classmavsdk_1_1_telemetry_1a60cca43e2f87e3fd3a9e170ff2b64e0a}
```cpp
void mavsdk::Telemetry::get_gps_global_origin_async(const GetGpsGlobalOriginCallback callback)
```


Get the GPS location of where the estimator has been initialized.

This function is non-blocking. See 'get_gps_global_origin' for the blocking counterpart.

**Parameters**

* const [GetGpsGlobalOriginCallback](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a350ee89a7e30a691e130e29ace8917ef) **callback** - 

### get_gps_global_origin() {#classmavsdk_1_1_telemetry_1a77747e7cea5a4d644bd6bec9441c7bfb}
```cpp
std::pair<Result, Telemetry::GpsGlobalOrigin> mavsdk::Telemetry::get_gps_global_origin() const
```


Get the GPS location of where the estimator has been initialized.

This function is blocking. See 'get_gps_global_origin_async' for the non-blocking counterpart.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_telemetry.md#classmavsdk_1_1_telemetry_1a241427df9a06234df2d3020fb524db75), [Telemetry::GpsGlobalOrigin](structmavsdk_1_1_telemetry_1_1_gps_global_origin.md) > - Result of request.

### operator=() {#classmavsdk_1_1_telemetry_1a703ac978c925be8806921925cf16aca9}
```cpp
const Telemetry& mavsdk::Telemetry::operator=(const Telemetry &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Telemetry](classmavsdk_1_1_telemetry.md)&  - 

**Returns**

&emsp;const [Telemetry](classmavsdk_1_1_telemetry.md) & - 