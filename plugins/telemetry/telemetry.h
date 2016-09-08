#pragma once

#include "plugin_base.h"

namespace dronelink {

class TelemetryImpl;

class Telemetry : public PluginBase
{
public:
    explicit Telemetry(TelemetryImpl *impl);
    ~Telemetry();

    struct Position {
        double latitude_deg;
        double longitude_deg;
        float absolute_altitude_m;
        float relative_altitude_m;
    };

    struct Quaternion {
        float vec[4];
    };

    struct EulerAngle {
        float roll_deg;
        float pitch_deg;
        float yaw_deg;
    };

    struct GroundSpeedNED {
        float velocity_north_m_s;
        float velocity_east_m_s;
        float velocity_down_m_s;
    };

    struct GPSInfo {
        int num_satellites;
        int fix_type;
    };

    struct Battery {
        float voltage_v;
        float remaining;
    };

    Position position() const;
    Position home_position() const;

    bool in_air() const;

    Quaternion attitude_quaternion() const;
    EulerAngle attitude_euler_angle() const;

    GroundSpeedNED ground_speed_ned() const;

    GPSInfo gps_info() const;

    Battery battery() const;

    // Non-copyable
    Telemetry(const Telemetry &) = delete;
    const Telemetry &operator=(const Telemetry &) = delete;

private:
    // Underlying implementation, set at instantiation
    TelemetryImpl *_impl;
};

} // namespace dronelink
