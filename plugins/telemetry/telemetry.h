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
        float w;
        float x;
        float y;
        float z;
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
        float remaining_percent;
    };

    enum class FlightMode {
        READY,
        TAKEOFF,
        HOLD,
        MISSION,
        RETURN_TO_LAUNCH,
        LAND,
        OFFBOARD,
        UNKNOWN
    };

    static const char *flight_mode_str(FlightMode flight_mode);

    struct Health {
        bool gyrometer_calibration_ok;
        bool accelerometer_calibration_ok;
        bool magnetometer_calibration_ok;
        bool level_calibration_ok;
        bool local_position_ok;
        bool global_position_ok;
        bool home_position_ok;
    };

    struct RCStatus {
        bool available_once;
        bool lost;
        float signal_strength_percent;
    };

    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        UNKNOWN
    };

    static const char *result_str(Result result);

    typedef std::function<void(Result)> result_callback_t;

    Result set_rate_position(double rate_hz);
    Result set_rate_home_position(double rate_hz);
    Result set_rate_in_air(double rate_hz);
    Result set_rate_attitude(double rate_hz);
    Result set_rate_camera_attitude(double rate_hz);
    Result set_rate_ground_speed_ned(double rate_hz);
    Result set_rate_gps_info(double rate_hz);
    Result set_rate_battery(double rate_hz);
    Result set_rate_rc_status(double rate_hz);

    void set_rate_position_async(double rate_hz, result_callback_t callback);
    void set_rate_home_position_async(double rate_hz, result_callback_t callback);
    void set_rate_in_air_async(double rate_hz, result_callback_t callback);
    void set_rate_attitude_async(double rate_hz, result_callback_t callback);
    void set_rate_camera_attitude_async(double rate_hz, result_callback_t callback);
    void set_rate_ground_speed_ned_async(double rate_hz, result_callback_t callback);
    void set_rate_gps_info_async(double rate_hz, result_callback_t callback);
    void set_rate_battery_async(double rate_hz, result_callback_t callback);
    void set_rate_rc_status_async(double rate_hz, result_callback_t callback);

    Position position() const;
    Position home_position() const;

    bool in_air() const;

    bool armed() const;

    Quaternion attitude_quaternion() const;
    EulerAngle attitude_euler_angle() const;

    Quaternion camera_attitude_quaternion() const;
    EulerAngle camera_attitude_euler_angle() const;

    GroundSpeedNED ground_speed_ned() const;

    GPSInfo gps_info() const;

    Battery battery() const;

    FlightMode flight_mode() const;

    Health health() const;

    bool health_all_ok() const;

    RCStatus rc_status() const;

    typedef std::function<void(Position position)> position_callback_t;
    void position_async(position_callback_t callback);
    void home_position_async(position_callback_t callback);

    typedef std::function<void(bool in_air)> in_air_callback_t;
    void in_air_async(in_air_callback_t callback);

    typedef std::function<void(bool armed)> armed_callback_t;
    // Note: this function is limited to 1Hz.
    void armed_async(armed_callback_t callback);

    typedef std::function<void(Quaternion quaternion)> attitude_quaternion_callback_t;
    void attitude_quaternion_async(attitude_quaternion_callback_t callback);

    typedef std::function<void(EulerAngle euler_angle)> attitude_euler_angle_callback_t;
    void attitude_euler_angle_async(attitude_euler_angle_callback_t callback);

    void camera_attitude_quaternion_async(attitude_quaternion_callback_t callback);

    void camera_attitude_euler_angle_async(attitude_euler_angle_callback_t callback);

    typedef std::function<void(GroundSpeedNED ground_speed_ned)> ground_speed_ned_callback_t;
    void ground_speed_ned_async(ground_speed_ned_callback_t callback);

    typedef std::function<void(GPSInfo gps_info)> gps_info_callback_t;
    void gps_info_async(gps_info_callback_t callback);

    typedef std::function<void(Battery battery)> battery_callback_t;
    void battery_async(battery_callback_t callback);

    typedef std::function<void(FlightMode flight_mode)> flight_mode_callback_t;
    // Note: this function is limited to 1Hz.
    void flight_mode_async(flight_mode_callback_t callback);

    typedef std::function<void(Health health)> health_callback_t;
    // Note: this function is limited to 1Hz.
    void health_async(health_callback_t callback);

    typedef std::function<void(bool health_all_ok)> health_all_ok_callback_t;
    // Note: this function is limited to 1Hz.
    void health_all_ok_async(health_all_ok_callback_t callback);

    typedef std::function<void(RCStatus rc_status)> rc_status_callback_t;
    void rc_status_async(rc_status_callback_t callback);

    // Non-copyable
    Telemetry(const Telemetry &) = delete;
    const Telemetry &operator=(const Telemetry &) = delete;

private:
    // Underlying implementation, set at instantiation
    TelemetryImpl *_impl;
};

} // namespace dronelink
