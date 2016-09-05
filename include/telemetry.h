#pragma once

namespace dronelink {


class TelemetryImpl;

class Telemetry
{
public:
    Telemetry(TelemetryImpl *impl);
    ~Telemetry();

    struct Coordinates {
        double latitude_deg;
        double longitude_deg;
    };

    struct Quaternion {
        float vec[4];
    };

    struct EulerAngle {
        float roll_deg;
        float pitch_deg;
        float yaw_deg;
    };

    float absolute_altitude_m() const;
    float relative_altitude_m() const;

    Coordinates coordinates() const;

    bool in_air() const;

    Quaternion attitude_quaternion() const;
    EulerAngle attitude_euler_angle() const;

private:
    // Underlying implementation, set at instantiation
    TelemetryImpl *_impl;

    // Non-copyable
    Telemetry(const Telemetry &) = delete;
    const Telemetry &operator=(const Telemetry &) = delete;
};

} // namespace dronelink
