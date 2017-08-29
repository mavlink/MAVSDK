#pragma once

#include <functional>

namespace dronecore {

class TelemetryImpl;

/**
 * The Telemetry class allows to get all kinds of telemetry infos from a device.
 */
class Telemetry
{
public:
    /**
     * Constructor for Telemetry called internally.
     */
    explicit Telemetry(TelemetryImpl *impl);

    /**
     * Destructor for Telemetry called internally.
     */
    ~Telemetry();

    /**
     * Position type.
     */
    struct Position {
        double latitude_deg; /**< latitude from -90 to +90 in degrees */
        double longitude_deg; /**< longitude from -180 to 180 in degrees */
        float absolute_altitude_m; /**< altitude AMSL (above mean sea level in meters */
        float relative_altitude_m; /**< altitude relative to takeoff altitude in meters */
    };

    /**
     * Quaternion type
     *
     * All rotations and axis systems follow the right-hand rule.
     * The Hamilton quaternion product definition is used.
     * A zero-rotation quaternion is represented by (1,0,0,0).
     * The quaternion could also be written as w + xi + yj + zk.
     */
    struct Quaternion {
        float w; /**< quaternion entry 0 also denoted as a */
        float x; /**< quaternion entry 1 also denoted as b */
        float y; /**< quaternion entry 2 also denoted as c */
        float z; /**< quaternion entry 3 also denoted as d */
    };

    /**
     * Euler angle type
     *
     * All rotations and axis systems follow the right-hand rule.
     * The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.
     */
    struct EulerAngle {
        float roll_deg; /**< roll angle in degrees, positive is banking to the right */
        float pitch_deg; /**< pitch angle in degrees, positive is pitching nose up. */
        float yaw_deg; /**< yaw angle in degrees, positive is clock-wise seen from above. */
    };

    /**
     * Ground speed type
     *
     * The ground speed is represented in the NED (North East Down) frame and in meters/second.
     */
    struct GroundSpeedNED {
        float velocity_north_m_s; /**< velocity in North direction in meters/second. */
        float velocity_east_m_s; /**< velocity in East direction in meters/second. */
        float velocity_down_m_s; /**< velocity in Down direction in meters/second. */
    };

    /**
     * GPS information type
     */
    struct GPSInfo {
        int num_satellites; /**< Number of visible satellites used for solution */
        int fix_type; /**< Fix type (0: no GPS, 1: no fix, 2: 2D fix, 3: 3D fix, 4: DGPS fix,
                                     5: RTK float, 6: RTK fixed) */
    };

    /**
     * Battery type
     */
    struct Battery {
        float voltage_v; /**< Voltage in volts */
        float remaining_percent; /**< Estimated battery percentage remaining from 0.0 to 1.0 */
    };

    /**
     * Flight modes
     */
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

    /**
     * Returns a human readable English string for a flight mode.
     */
    static const char *flight_mode_str(FlightMode flight_mode);

    /**
     * Various health flags
     */
    struct Health {
        bool gyrometer_calibration_ok; /**< true if the gyrometer is calibrated */
        bool accelerometer_calibration_ok; /**< true if the accelerometer is calibrated */
        bool magnetometer_calibration_ok; /**< true if the magnetometer is calibrated */
        bool level_calibration_ok; /**< true if the vehicle has a valid level calibration */
        bool local_position_ok; /**< true if the local position estimate is good enough */
        bool global_position_ok; /**< true if the global position estimate is good enough */
        bool home_position_ok; /**< true if the home position has been initialized properly */
    };

    /**
     * Remove control status type
     */
    struct RCStatus {
        bool available_once; /**< true if an RC signal has been available once */
        bool lost; /**< true if the RC signal is lost */
        float signal_strength_percent; /**< signal strength in percent from 0 to 100 */
    };

    /**
     * Results for telemetry requests
     */
    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        UNKNOWN
    };

    /**
     * Returns human-readable English string for Telemetry::Result.
     *
     * @param result result enum
     */
    static const char *result_str(Result result);

    /**
     * Callback type for telemetry requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * Set rate of position updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_position(double rate_hz);

    /**
     * Set rate of home position updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_home_position(double rate_hz);

    /**
     * Set rate of in-air status updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_in_air(double rate_hz);

    /**
     * Set rate of attitude updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_attitude(double rate_hz);

    /**
     * Set rate of camera attitude updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_camera_attitude(double rate_hz);

    /**
     * Set rate of ground speed (NED) updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_ground_speed_ned(double rate_hz);

    /**
     * Set rate of GPS information updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_gps_info(double rate_hz);

    /**
     * Set rate of battery status updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_battery(double rate_hz);

    /**
     * Set rate of RC status updates (synchronous).
     *
     * @param rate_hz rate in Hz
     * @return result of request
     */
    Result set_rate_rc_status(double rate_hz);

    /**
     * Set rate of position updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_position_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of home position updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_home_position_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of in-air status updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_in_air_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of attitude updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_attitude_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of camera attitude updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_camera_attitude_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of ground speed (NED) updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_ground_speed_ned_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of GPS information updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_gps_info_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of battery status updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_battery_async(double rate_hz, result_callback_t callback);

    /**
     * Set rate of RC status updates (asynchronous).
     *
     * @param rate_hz rate in Hz
     * @param callback callback to receive request result
     */
    void set_rate_rc_status_async(double rate_hz, result_callback_t callback);

    /**
     * Returns the current position (synchronous).
     *
     * @return position
     */
    Position position() const;

    /**
     * Returns the home position (synchronous).
     *
     * @return home position
     */
    Position home_position() const;

    /**
     * Returns the in-air status (synchronous).
     *
     * @return true if in-air (flying) and not on-ground (landed)
     */
    bool in_air() const;

    /**
     * Returns the arming status (synchronous).
     *
     * @return true if armed (propellers spinning)
     */
    bool armed() const;

    /**
     * Returns the current attitude in quaternions (synchronous).
     *
     * @return attitude as quaternion
     */
    Quaternion attitude_quaternion() const;

    /**
     * Returns the current attitude in Euler angles (synchronous).
     *
     * @return attitude as Euler angle
     */
    EulerAngle attitude_euler_angle() const;

    /**
     * Returns the camera's attitude in quaternions (synchronous).
     *
     * @return camera's attitude as quaternion
     */
    Quaternion camera_attitude_quaternion() const;

    /**
     * Returns the camera's attitude in Euler angles (synchronous).
     *
     * @return camera's attitude as Euler angle
     */
    EulerAngle camera_attitude_euler_angle() const;

    /**
     * Returns the current ground speed (NED) (synchronous).
     *
     * @return ground speed in NED
     */
    GroundSpeedNED ground_speed_ned() const;

    /**
     * Returns the current GPS information (synchronous).
     *
     * @return GPS information
     */
    GPSInfo gps_info() const;

    /**
     * Returns the current battery status (synchronous).
     */
    Battery battery() const;

    /**
     * Returns the current flight mode (synchronous).
     *
     * @return flight mode
     */
    FlightMode flight_mode() const;

    /**
     * Returns the current health status (synchronous).
     *
     * @return the health status
     */
    Health health() const;

    /**
     * Returns true if the overall health is ok (synchronous).
     *
     * @return true if all individual health flags are true.
     */
    bool health_all_ok() const;

    /**
     * Returns the RC status (synchronous).
     *
     * @return the RC status
     */
    RCStatus rc_status() const;

    /**
     * Callback type for position updates.
     */
    typedef std::function<void(Position)> position_callback_t;

    /**
     * Subscribes to position updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void position_async(position_callback_t callback);

    /**
     * Subscribes to home position updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void home_position_async(position_callback_t callback);

    /**
     * Callback type for in-air updates.
     *
     * @param in_air true if in-air (flying) and not on-ground (landed)
     */
    typedef std::function<void(bool in_air)> in_air_callback_t;

    /**
     * Subscribes to in-air updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void in_air_async(in_air_callback_t callback);

    /**
     * Callback type for armed updates (asynchronous).
     *
     * @param armed true if armed (motors spinning)
     */
    typedef std::function<void(bool armed)> armed_callback_t;

    /**
     * Subscribes to armed updates (asynchronous).
     *
     * Note that armed updates are limited to 1Hz.
     *
     * @param callback function to call with updates
     */
    void armed_async(armed_callback_t callback);

    /**
     * Callback type for attitude updates in quaternion.
     *
     * @param quaternion attitude quaternion
     */
    typedef std::function<void(Quaternion quaternion)> attitude_quaternion_callback_t;

    /**
     * Subscribes to attitude updates in quaternion (asynchronous).
     *
     * @param callback function to call with updates
     */
    void attitude_quaternion_async(attitude_quaternion_callback_t callback);

    /**
     * Callback type for attitude updates in Euler angles.
     *
     * @param euler_angle attitude Euler angle
     */
    typedef std::function<void(EulerAngle euler_angle)> attitude_euler_angle_callback_t;

    /**
     * Subscribes to attitude updates in Euler angles (asynchronous).
     *
     * @param callback function to call with updates
     */
    void attitude_euler_angle_async(attitude_euler_angle_callback_t callback);

    /**
     * Subscribes to camera attitude updates in quaternion (asynchronous).
     *
     * @param callback function to call with updates
     */
    void camera_attitude_quaternion_async(attitude_quaternion_callback_t callback);

    /**
     * Subscribes to camera attitude updates in Euler angles (asynchronous).
     *
     * @param callback function to call with updates
     */
    void camera_attitude_euler_angle_async(attitude_euler_angle_callback_t callback);

    /**
     * Callback type for ground speed (NED) updates.
     *
     * @param ground_speed_ned ground speed (NED)
     */
    typedef std::function<void(GroundSpeedNED ground_speed_ned)> ground_speed_ned_callback_t;

    /**
     * Subscribes to ground speed (NED) updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void ground_speed_ned_async(ground_speed_ned_callback_t callback);

    /**
     * Callback type for GPS information updates.
     *
     * @param gps_info GPS information
     */
    typedef std::function<void(GPSInfo gps_info)> gps_info_callback_t;

    /**
     * Subscribes to GPS information updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void gps_info_async(gps_info_callback_t callback);

    /**
     * Callback type for battery status updates.
     *
     * @param battery battery status
     */
    typedef std::function<void(Battery battery)> battery_callback_t;

    /**
     * Subscribes to battery status updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void battery_async(battery_callback_t callback);

    /**
     * Callback type for flight mode updates.
     *
     * @param flight_mode flight mode
     */
    typedef std::function<void(FlightMode flight_mode)> flight_mode_callback_t;

    /**
     * Subscribes to battery status updates (asynchronous).
     *
     * Note that flight mode updates are limited to 1Hz.
     *
     * @param callback function to call with updates
     */
    void flight_mode_async(flight_mode_callback_t callback);

    /**
     * Callback type for health status updates.
     *
     * @param health health flags
     */
    typedef std::function<void(Health health)> health_callback_t;

    /**
     * Subscribes to health status updates (asynchronous).
     *
     * Note that health status updates are limited to 1Hz.
     *
     * @param callback function to call with updates
     */
    void health_async(health_callback_t callback);

    /**
     * Callback type for health status updates.
     *
     * @param flight_mode flight mode
     */
    typedef std::function<void(bool health_all_ok)> health_all_ok_callback_t;

    /**
     * Subscribes to overall health status updates (asynchronous).
     *
     * Note that overall health status updates are limited to 1Hz.
     *
     * @param callback function to call with updates
     */
    void health_all_ok_async(health_all_ok_callback_t callback);

    /**
     * Callback type for RC status updates.
     *
     * @param rc_status RC status
     */
    typedef std::function<void(RCStatus rc_status)> rc_status_callback_t;

    /**
     * Subscribes to RC status updates (asynchronous).
     *
     * @param callback function to call with updates
     */
    void rc_status_async(rc_status_callback_t callback);

    // Non-copyable
    Telemetry(const Telemetry &) = delete;
    const Telemetry &operator=(const Telemetry &) = delete;

private:
    // Underlying implementation, set at instantiation
    TelemetryImpl *_impl;
};

} // namespace dronecore
