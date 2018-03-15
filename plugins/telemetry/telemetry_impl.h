#pragma once

#include <atomic>
#include <mutex>
#include "telemetry.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "mavlink_system.h"
#include "mavlink_include.h"

// Since not all vehicles support/require level calibration, this
// is disabled for now.
//#define LEVEL_CALIBRATION

namespace dronecore {

class System;

class TelemetryImpl : public PluginImplBase
{
public:
    TelemetryImpl(System &system);
    ~TelemetryImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Telemetry::Result set_rate_position(double rate_hz);
    Telemetry::Result set_rate_home_position(double rate_hz);
    Telemetry::Result set_rate_in_air(double rate_hz);
    Telemetry::Result set_rate_attitude(double rate_hz);
    Telemetry::Result set_rate_camera_attitude(double rate_hz);
    Telemetry::Result set_rate_ground_speed_ned(double rate_hz);
    Telemetry::Result set_rate_gps_info(double rate_hz);
    Telemetry::Result set_rate_battery(double rate_hz);
    Telemetry::Result set_rate_rc_status(double rate_hz);

    void set_rate_position_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_home_position_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_in_air_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_attitude_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_camera_attitude_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_ground_speed_ned_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_gps_info_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_battery_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_rc_status_async(double rate_hz, Telemetry::result_callback_t callback);

    Telemetry::Position get_position() const;
    Telemetry::Position get_home_position() const;
    bool in_air() const;
    bool armed() const;
    Telemetry::EulerAngle get_attitude_euler_angle() const;
    Telemetry::Quaternion get_attitude_quaternion() const;
    Telemetry::EulerAngle get_camera_attitude_euler_angle() const;
    Telemetry::Quaternion get_camera_attitude_quaternion() const;
    Telemetry::GroundSpeedNED get_ground_speed_ned() const;
    Telemetry::GPSInfo get_gps_info() const;
    Telemetry::Battery get_battery() const;
    Telemetry::FlightMode get_flight_mode() const;
    Telemetry::Health get_health() const;
    bool get_health_all_ok() const;
    Telemetry::RCStatus get_rc_status() const;

    void position_async(Telemetry::position_callback_t &callback);
    void home_position_async(Telemetry::position_callback_t &callback);
    void in_air_async(Telemetry::in_air_callback_t &callback);
    void armed_async(Telemetry::armed_callback_t &callback);
    void attitude_quaternion_async(Telemetry::attitude_quaternion_callback_t &callback);
    void attitude_euler_angle_async(Telemetry::attitude_euler_angle_callback_t &callback);
    void camera_attitude_quaternion_async(Telemetry::attitude_quaternion_callback_t &callback);
    void camera_attitude_euler_angle_async(Telemetry::attitude_euler_angle_callback_t &callback);
    void ground_speed_ned_async(Telemetry::ground_speed_ned_callback_t &callback);
    void gps_info_async(Telemetry::gps_info_callback_t &callback);
    void battery_async(Telemetry::battery_callback_t &callback);
    void flight_mode_async(Telemetry::flight_mode_callback_t &callback);
    void health_async(Telemetry::health_callback_t &callback);
    void health_all_ok_async(Telemetry::health_all_ok_callback_t &callback);
    void rc_status_async(Telemetry::rc_status_callback_t &callback);

private:
    void set_position(Telemetry::Position position);
    void set_home_position(Telemetry::Position home_position);
    void set_in_air(bool in_air);
    void set_armed(bool armed);
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);
    void set_camera_attitude_euler_angle(Telemetry::EulerAngle euler_angle);
    void set_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned);
    void set_gps_info(Telemetry::GPSInfo gps_info);
    void set_battery(Telemetry::Battery battery);
    void set_flight_mode(Telemetry::FlightMode flight_mode);
    void set_health_local_position(bool ok);
    void set_health_global_position(bool ok);
    void set_health_home_position(bool ok);
    void set_health_gyrometer_calibration(bool ok);
    void set_health_accelerometer_calibration(bool ok);
    void set_health_magnetometer_calibration(bool ok);
    void set_health_level_calibration(bool ok);
    void set_rc_status(bool available, float signal_strength_percent);

    void process_global_position_int(const mavlink_message_t &message);
    void process_home_position(const mavlink_message_t &message);
    void process_attitude_quaternion(const mavlink_message_t &message);
    void process_mount_orientation(const mavlink_message_t &message);
    void process_gps_raw_int(const mavlink_message_t &message);
    void process_extended_sys_state(const mavlink_message_t &message);
    void process_sys_status(const mavlink_message_t &message);
    void process_heartbeat(const mavlink_message_t &message);
    void process_rc_channels(const mavlink_message_t &message);

    void receive_param_cal_gyro(bool success, int value);
    void receive_param_cal_accel(bool success, int value);
    void receive_param_cal_mag(bool success, int value);
#ifdef LEVEL_CALIBRATION
    void receive_param_cal_level(bool success, float value);
#endif

    void receive_rc_channels_timeout();


    static Telemetry::Result telemetry_result_from_command_result(
        MAVLinkCommands::Result command_result);

    static void command_result_callback(MAVLinkCommands::Result command_result,
                                        const Telemetry::result_callback_t &callback);

    static Telemetry::FlightMode to_flight_mode_from_custom_mode(uint32_t custom_mode);

    // Make all fields thread-safe using mutexs
    // The mutexs are mutable so that the lock can get aqcuired in
    // methods marked const.
    mutable std::mutex _position_mutex;
    Telemetry::Position _position;

    mutable std::mutex _home_position_mutex;
    Telemetry::Position _home_position;

    // If possible, just use atomic instead of a mutex.
    std::atomic_bool _in_air;
    std::atomic_bool _armed;

    mutable std::mutex _attitude_quaternion_mutex;
    Telemetry::Quaternion _attitude_quaternion;

    mutable std::mutex _camera_attitude_euler_angle_mutex;
    Telemetry::EulerAngle _camera_attitude_euler_angle;

    mutable std::mutex _ground_speed_ned_mutex;
    Telemetry::GroundSpeedNED _ground_speed_ned;

    mutable std::mutex _gps_info_mutex;
    Telemetry::GPSInfo _gps_info;

    mutable std::mutex _battery_mutex;
    Telemetry::Battery _battery;

    mutable std::mutex _flight_mode_mutex;
    Telemetry::FlightMode _flight_mode;

    mutable std::mutex _health_mutex;
    Telemetry::Health _health;

    mutable std::mutex _rc_status_mutex;
    Telemetry::RCStatus _rc_status;

    Telemetry::position_callback_t _position_subscription;
    Telemetry::position_callback_t _home_position_subscription;
    Telemetry::in_air_callback_t _in_air_subscription;
    Telemetry::armed_callback_t _armed_subscription;
    Telemetry::attitude_quaternion_callback_t _attitude_quaternion_subscription;
    Telemetry::attitude_euler_angle_callback_t _attitude_euler_angle_subscription;
    Telemetry::attitude_quaternion_callback_t _camera_attitude_quaternion_subscription;
    Telemetry::attitude_euler_angle_callback_t _camera_attitude_euler_angle_subscription;
    Telemetry::ground_speed_ned_callback_t _ground_speed_ned_subscription;
    Telemetry::gps_info_callback_t _gps_info_subscription;
    Telemetry::battery_callback_t _battery_subscription;
    Telemetry::flight_mode_callback_t _flight_mode_subscription;
    Telemetry::health_callback_t _health_subscription;
    Telemetry::health_all_ok_callback_t _health_all_ok_subscription;
    Telemetry::rc_status_callback_t _rc_status_subscription;

    // The ground speed and position are coupled to the same message, therefore, we just use
    // the faster between the two.
    double _ground_speed_ned_rate_hz;
    double _position_rate_hz;

    void *_timeout_cookie = nullptr;
};

} // namespace dronecore
