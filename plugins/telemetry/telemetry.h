#pragma once

#include <functional>

namespace dronelink {

class TelemetryImpl;

class Telemetry
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

    enum class FlightMode {
        READY,
        TAKEOFF,
        HOLD,
        MISSION,
        RETURN_TO_LAND,
        LAND,
        POSITION_CONTROL,
        UNKNOWN
    };

    struct Health {
        bool gyrometer_calibration_ok;
        bool accelerometer_calibration_ok;
        bool magnetometer_calibration_ok;
        bool level_calibration_ok;
        bool local_position_ok;
        bool global_position_ok;
        bool home_position_ok;
    };

    Position position() const;
    Position home_position() const;

    bool in_air() const;

    bool armed() const;

    Quaternion attitude_quaternion() const;
    EulerAngle attitude_euler_angle() const;

    GroundSpeedNED ground_speed_ned() const;

    GPSInfo gps_info() const;

    Battery battery() const;

    FlightMode flight_mode() const;

    Health health() const;

    typedef std::function<void(Position position)> position_callback_t;
    void position_async(double rate_hz, position_callback_t callback);
    void home_position_async(double rate_hz, position_callback_t callback);

    typedef std::function<void(bool in_air)> in_air_callback_t;
    void in_air_async(double rate_hz, in_air_callback_t callback);

    typedef std::function<void(bool armed)> armed_callback_t;
    // Note: this function is limited to 1Hz.
    void armed_async(armed_callback_t callback);

    typedef std::function<void(Quaternion quaternion)> attitude_quaternion_callback_t;
    void attitude_quaternion_async(double rate_hz, attitude_quaternion_callback_t callback);

    typedef std::function<void(EulerAngle euler_angle)> attitude_euler_angle_callback_t;
    void attitude_euler_angle_async(double rate_hz, attitude_euler_angle_callback_t callback);

    typedef std::function<void(GroundSpeedNED ground_speed_ned)> ground_speed_ned_callback_t;
    void ground_speed_ned_async(double rate_hz, ground_speed_ned_callback_t callback);

    typedef std::function<void(GPSInfo gps_info)> gps_info_callback_t;
    void gps_info_async(double rate_hz, gps_info_callback_t callback);

    typedef std::function<void(Battery battery)> battery_callback_t;
    void battery_async(double rate_hz, battery_callback_t callback);

    typedef std::function<void(FlightMode flight_mode)> flight_mode_callback_t;
    // Note: this function is limited to 1Hz.
    void flight_mode_async(flight_mode_callback_t callback);

    typedef std::function<void(Health health)> health_callback_t;
    // Note: this function is limited to 1Hz.
    void health_async(health_callback_t callback);

    // Non-copyable
    Telemetry(const Telemetry &) = delete;
    const Telemetry &operator=(const Telemetry &) = delete;

private:
    // Underlying implementation, set at instantiation
    TelemetryImpl *_impl;
};

} // namespace dronelink
