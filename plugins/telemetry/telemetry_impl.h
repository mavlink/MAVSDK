#pragma once

#include "telemetry.h"
#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include <atomic>
#include <mutex>

namespace dronelink {

class DeviceImpl;

class TelemetryImpl : public PluginImplBase {
public:
    TelemetryImpl();
    ~TelemetryImpl();

    void init() override;
    void deinit() override;

    Telemetry::Position get_position() const;
    Telemetry::Position get_home_position() const;
    bool in_air() const;
    Telemetry::EulerAngle get_attitude_euler_angle() const;
    Telemetry::Quaternion get_attitude_quaternion() const;
    Telemetry::GroundSpeedNED get_ground_speed_ned() const;
    Telemetry::GPSInfo get_gps_info() const;
    Telemetry::Battery get_battery() const;

private:
    void set_position(Telemetry::Position position);
    void set_home_position(Telemetry::Position home_position);
    void set_in_air(bool in_air);
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);
    void set_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned);
    void set_gps_info(Telemetry::GPSInfo gps_info);
    void set_battery(Telemetry::Battery battery);

    void process_global_position_int(const mavlink_message_t &message);
    void process_home_position(const mavlink_message_t &message);
    void process_attitude_quaternion(const mavlink_message_t &message);
    void process_gps_raw_int(const mavlink_message_t &message);
    void process_extended_sys_state(const mavlink_message_t &message);
    void process_sys_status(const mavlink_message_t &message);

    // Make all fields thread-safe using mutexs
    // The mutexs are mutable so that the lock can get aqcuired in
    // methods marked const.
    mutable std::mutex _position_mutex;
    Telemetry::Position _position;

    mutable std::mutex _home_position_mutex;
    Telemetry::Position _home_position;

    // If possible, just use atomic instead of a mutex.
    std::atomic_bool _in_air;

    mutable std::mutex _attitude_quaternion_mutex;
    Telemetry::Quaternion _attitude_quaternion;

    mutable std::mutex _ground_speed_ned_mutex;
    Telemetry::GroundSpeedNED _ground_speed_ned;

    mutable std::mutex _gps_info_mutex;
    Telemetry::GPSInfo _gps_info;

    mutable std::mutex _battery_mutex;
    Telemetry::Battery _battery;
};

} // namespace dronelink
