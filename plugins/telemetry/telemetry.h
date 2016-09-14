#pragma once

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

    Position position() const;
    Position home_position() const;

    bool in_air() const;

    Quaternion attitude_quaternion() const;
    EulerAngle attitude_euler_angle() const;

    GroundSpeedNED ground_speed_ned() const;

    GPSInfo gps_info() const;

    Battery battery() const;

    typedef void (*position_callback_t)(Position position, void *user);
    struct PositionCallbackData {
         position_callback_t callback;
         void *user;
    };
    void position_async(double rate_hz, PositionCallbackData callback_data);
    void home_position_async(double rate_hz, PositionCallbackData callback_data);

    typedef void (*in_air_callback_t)(bool in_air, void *user);
    struct InAirCallbackData {
         in_air_callback_t callback;
         void *user;
    };
    void in_air_async(double rate_hz, InAirCallbackData callback_data);

    typedef void (*attitude_quaternion_callback_t)(Quaternion quaternion, void *user);
    struct AttitudeQuaternionCallbackData {
         attitude_quaternion_callback_t callback;
         void *user;
    };
    void attitude_quaternion_async(double rate_hz, AttitudeQuaternionCallbackData callback_data);

    typedef void (*attitude_euler_angle_callback_t)(EulerAngle euler_angle, void *user);
    struct AttitudeEulerAngleCallbackData {
         attitude_euler_angle_callback_t callback;
         void *user;
    };
    void attitude_euler_angle_async(double rate_hz, AttitudeEulerAngleCallbackData callback_data);

    typedef void (*ground_speed_ned_callback_t)(GroundSpeedNED ground_speed_ned, void *user);
    struct GroundSpeedNEDCallbackData {
         ground_speed_ned_callback_t callback;
         void *user;
    };
    void ground_speed_ned_async(double rate_hz, GroundSpeedNEDCallbackData callback_data);

    typedef void (*gps_info_callback_t)(GPSInfo gps_info, void *user);
    struct GPSInfoCallbackData {
         gps_info_callback_t callback;
         void *user;
    };
    void gps_info_async(double rate_hz, GPSInfoCallbackData callback_data);

    typedef void (*battery_callback_t)(Battery battery, void *user);
    struct BatteryCallbackData {
         battery_callback_t callback;
         void *user;
    };
    void battery_async(double rate_hz, BatteryCallbackData callback_data);

    // Non-copyable
    Telemetry(const Telemetry &) = delete;
    const Telemetry &operator=(const Telemetry &) = delete;

private:
    // Underlying implementation, set at instantiation
    TelemetryImpl *_impl;
};

} // namespace dronelink
