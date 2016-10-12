#pragma once

#include "telemetry.h"
#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include <atomic>
#include <mutex>

namespace dronelink {

class DeviceImpl;

class TelemetryImpl : public PluginImplBase
{
public:
    TelemetryImpl();
    ~TelemetryImpl();

    void init() override;
    void deinit() override;

    Telemetry::Position get_position() const;
    Telemetry::Position get_home_position() const;
    bool in_air() const;
    bool armed() const;
    Telemetry::EulerAngle get_attitude_euler_angle() const;
    Telemetry::Quaternion get_attitude_quaternion() const;
    Telemetry::GroundSpeedNED get_ground_speed_ned() const;
    Telemetry::GPSInfo get_gps_info() const;
    Telemetry::Battery get_battery() const;
    Telemetry::FlightMode get_flight_mode() const;
    Telemetry::Health get_health() const;

    void position_async(double rate_hz, Telemetry::position_callback_t &callback);
    void home_position_async(double rate_hz, Telemetry::position_callback_t &callback);
    void in_air_async(double rate_hz, Telemetry::in_air_callback_t &callback);
    void armed_async(Telemetry::armed_callback_t &callback);
    void attitude_quaternion_async(double rate_hz,
                                   Telemetry::attitude_quaternion_callback_t &callback);
    void attitude_euler_angle_async(double rate_hz,
                                    Telemetry::attitude_euler_angle_callback_t &callback);
    void ground_speed_ned_async(double rate_hz, Telemetry::ground_speed_ned_callback_t &callback);
    void gps_info_async(double rate_hz, Telemetry::gps_info_callback_t &callback);
    void battery_async(double rate_hz, Telemetry::battery_callback_t &callback);
    void flight_mode_async(Telemetry::flight_mode_callback_t &callback);
    void health_async(Telemetry::health_callback_t &callback);

private:
    void set_position(Telemetry::Position position);
    void set_home_position(Telemetry::Position home_position);
    void set_in_air(bool in_air);
    void set_armed(bool armed);
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);
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

    void process_global_position_int(const mavlink_message_t &message);
    void process_home_position(const mavlink_message_t &message);
    void process_attitude_quaternion(const mavlink_message_t &message);
    void process_gps_raw_int(const mavlink_message_t &message);
    void process_extended_sys_state(const mavlink_message_t &message);
    void process_sys_status(const mavlink_message_t &message);
    void process_heartbeat(const mavlink_message_t &message);

    void receive_param_cal_gyro(bool success, int value);
    void receive_param_cal_accel(bool success, int value);
    void receive_param_cal_mag(bool success, int value);
    void receive_param_cal_level(bool success, float value);

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

    Telemetry::position_callback_t _position_subscription;
    Telemetry::position_callback_t _home_position_subscription;
    Telemetry::in_air_callback_t _in_air_subscription;
    Telemetry::armed_callback_t _armed_subscription;
    Telemetry::attitude_quaternion_callback_t _attitude_quaternion_subscription;
    Telemetry::attitude_euler_angle_callback_t _attitude_euler_angle_subscription;
    Telemetry::ground_speed_ned_callback_t _ground_speed_ned_subscription;
    Telemetry::gps_info_callback_t _gps_info_subscription;
    Telemetry::battery_callback_t _battery_subscription;
    Telemetry::flight_mode_callback_t _flight_mode_subscription;
    Telemetry::health_callback_t _health_subscription;
};

} // namespace dronelink
