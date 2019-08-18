#pragma once

#include <functional>
#include <memory>
#include <string>

#include "plugin_base.h"

namespace mavsdk {

class TelemetryImpl;
class System;

/**
 * @brief This class allows users to get vehicle telemetry and state information
 * (e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.
 */
class Telemetry : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto telemetry = std::make_shared<Telemetry>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Telemetry(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Telemetry();

    /**
     * @brief Position type in global coordinates.
     */
    struct Position {
        double latitude_deg; /**< @brief Latitude in degrees (range: -90 to +90) */
        double longitude_deg; /**< @brief Longitude in degrees (range: -180 to 180) */
        float absolute_altitude_m; /**< @brief Altitude AMSL (above mean sea level) in metres */
        float relative_altitude_m; /**< @brief Altitude relative to takeoff altitude in metres */
    };

    /**
     * @brief Position type in local coordinates.
     *
     * Position is represented in the NED (North East Down) frame in local coordinate system.
     */
    struct PositionNED {
        float north_m; /**< @brief Position along north-direction in meters. */
        float east_m; /**< @brief Position along east-direction  in meters. */
        float down_m; /**< @brief Position along down-direction  in meters. */
    };

    /**
     * @brief Velocity type in local coordinates.
     *
     * Velocity is represented in NED (North East Down) frame in local coordinate system.
     */
    struct VelocityNED {
        float north_m_s; /**< @brief Velocity along north-direction in meters per seconds. */
        float east_m_s; /**< @brief Velocity along east-direction in meters per seconds. */
        float down_m_s; /**< @brief Velocity along down-direction in meters per seconds. */
    };

    /**
     * @brief Kinematic type in local coordinates.
     *
     * Position and Velocity are represented in NED (North East Down) frame in local coordinate
     * system.
     */
    struct PositionVelocityNED {
        PositionNED position; /**< @see PositionNED */
        VelocityNED velocity; /**< @see VelocityNED */
    };

    /**
     * @brief Quaternion type.
     *
     * All rotations and axis systems follow the right-hand rule.
     * The Hamilton quaternion product definition is used.
     * A zero-rotation quaternion is represented by (1,0,0,0).
     * The quaternion could also be written as w + xi + yj + zk.
     *
     * For more info see: https://en.wikipedia.org/wiki/Quaternion
     */
    struct Quaternion {
        float w; /**< @brief Quaternion entry 0 also denoted as a. */
        float x; /**< @brief Quaternion entry 1 also denoted as b. */
        float y; /**< @brief Quaternion entry 2 also denoted as c. */
        float z; /**< @brief Quaternion entry 3 also denoted as d. */
    };

    /**
     * @brief Euler angle type.
     *
     * All rotations and axis systems follow the right-hand rule.
     * The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.
     *
     * For more info see https://en.wikipedia.org/wiki/Euler_angles
     */
    struct EulerAngle {
        float roll_deg; /**< @brief Roll angle in degrees, positive is banking to the right. */
        float pitch_deg; /**< @brief Pitch angle in degrees, positive is pitching nose up. */
        float yaw_deg; /**< @brief Yaw angle in degrees, positive is clock-wise seen from above. */
    };

    /**
     * @brief Ground speed type.
     *
     * The ground speed is represented in the NED (North East Down) frame and in metres/second.
     */
    struct GroundSpeedNED {
        float velocity_north_m_s; /**< @brief Velocity in North direction in metres/second. */
        float velocity_east_m_s; /**< @brief Velocity in East direction in metres/second. */
        float velocity_down_m_s; /**< @brief Velocity in Down direction in metres/second. */
    };

    /**
     * @brief Acceleration type in local coordinates.
     *
     * The acceleration is represented in the NED (North East Down) frame and in metres/second^2.
     */
    struct AccelerationNED {
        float north_m_s2; /**< @brief Acceleration in North direction in metres/second^2 */
        float east_m_s2; /**< @brief Acceleration in East direction in metres/second^2 */
        float down_m_s2; /**< @brief Acceleration in Down direction in metres/second^2 */
    };

    /**
     * @brief Angular velocity type in local coordinates.
     *
     * The angular velocity is represented in NED (North East Down) frame and in radians/second.
     */
    struct AngularVelocityNED {
        float north_rad_s; /**< @brief Angular velocity in North direction in radians/second */
        float east_rad_s; /**< @brief Angular velocity in East direction in radians/second */
        float down_rad_s; /**< @brief Angular velocity in Down direction in radians/second */
    };

    /**
     * @brief Magnetic field type in local coordinates.
     *
     * The magnetic field is represented in NED (North East Down) frame and is measured in Gauss.
     */
    struct MagneticFieldNED {
        float north_gauss; /**< @brief Magnetic field in North direction measured in Gauss. */
        float east_gauss; /**< @brief Magnetic field in East direction measured in Gauss. */
        float down_gauss; /**< @brief Magnetic field in Down direction measured in Gauss. */
    };

    /**
     * @brief Inertial measurement unit type in local coordinates.
     *
     * Acceleration, angular velocity and magnetic field are represented in NED (North East Down)
     * frame in local coordinate system. Temperature is measured in degrees Celsius.
     */
    struct IMUReadingNED {
        AccelerationNED acceleration; /**< @see AccelerationNED */
        AngularVelocityNED angular_velocity; /**< @see AngularVelocityNED */
        MagneticFieldNED magnetic_field; /**< @see MagneticFieldNED */
        float temperature_degC; /**< @brief Temperature measured in degrees Celsius. */
    };

    /**
     * @brief GPS information type.
     */
    struct GPSInfo {
        int num_satellites; /**< @brief Number of visible satellites used for solution. */
        int fix_type; /**< @brief Fix type (0: no GPS, 1: no fix, 2: 2D fix, 3: 3D fix, 4: DGPS fix,
                                     5: RTK float, 6: RTK fixed). */
    };

    /**
     * @brief Status Text information type.
     */
    struct StatusText {
        /**
         * @brief Status Types.
         *
         * @note PX4 only supports these 3 status types.
         * If other status types are returned for some reason,
         * they will be marked as INFO type and logged as a warning.
         */
        enum class StatusType {
            INFO, /**< @brief Message type is an information or other. */
            WARNING, /**< @brief Message type is a warning. */
            CRITICAL /**< @brief Message type is critical. */
        } type; /**< @brief Message type. */
        std::string text; /**< @brief Mavlink status message. */
    };

    /**
     * @brief Battery type.
     */
    struct Battery {
        float voltage_v; /**< @brief Voltage in volts. */
        float remaining_percent; /**< @brief Estimated battery percentage remaining (range: 0.0
                                    to 1.0). */
    };

    /**
     * @brief Flight modes.
     *
     * For more information about flight modes, check out
     * https://docs.px4.io/en/config/flight_mode.html.
     */
    enum class FlightMode {
        READY, /**< @brief Armed and ready to take off. */
        TAKEOFF, /**< @brief Taking off. */
        HOLD, /**< @brief Hold mode (hovering in place (or circling for fixed-wing vehicles). */
        MISSION, /**< @brief Mission mode. */
        RETURN_TO_LAUNCH, /**< @brief Returning to launch position (then landing). */
        LAND, /**< @brief Landing. */
        OFFBOARD, /**< @brief Offboard mode. */
        FOLLOW_ME, /**< @brief FollowMe mode. */
        UNKNOWN /**< @brief Mode not known. */
    };

    /**
     *@brief LandedState.
     * Enumeration of landed detector states
     * For more information, check out MavLink messages in
     * https://mavlink.io/en/messages/common.html
     */
    enum class LandeState {
        UNDEFINED,
        ON_GROUND,
        IN_AIR,
        TAKEOFF,
        LANDING
    }

    /**
     * @brief Get a human readable English string for a flight mode.
     */
    static std::string
    flight_mode_str(FlightMode flight_mode);

    /**
     * @brief Various health flags.
     */
    struct Health {
        bool gyrometer_calibration_ok; /**< @brief true if the gyrometer is calibrated. */
        bool accelerometer_calibration_ok; /**< @brief true if the accelerometer is calibrated. */
        bool magnetometer_calibration_ok; /**< @brief true if the magnetometer is calibrated. */
        bool level_calibration_ok; /**< @brief true if the vehicle has a valid level calibration. */
        bool local_position_ok; /**< @brief true if the local position estimate is good enough to
                                   fly in a position control mode. */
        bool global_position_ok; /**< @brief true if the global position estimate is good enough to
                                    fly in a position controlled mode. */
        bool home_position_ok; /**< @brief true if the home position has been initialized properly.
                                */
    };

    /**
     * @brief Remote control status type.
     */
    struct RCStatus {
        bool available_once; /**< @brief true if an RC signal has been available once. */
        bool available; /**< @brief true if the RC signal is available now. */
        float signal_strength_percent; /**< @brief Signal strength as a percentage (range: 0 to
                                          100). */
    };

    /**
     * @brief Results enum for telemetry requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        NO_SYSTEM, /**< @brief No system connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief System busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief %Request timeout. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief Get human-readable English string for Telemetry::Result.
     *
     * @param result The enum value for which string is needed.
     * @return Human readable string for the Telemetry::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for telemetry requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Set rate of kinematic (position and velocity) updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @see PositionVelocityNED
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_position_velocity_ned(double rate_hz);

    /**
     * @brief Set rate of position updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_position(double rate_hz);

    /**
     * @brief Set rate of home position updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_home_position(double rate_hz);

    /**
     * @brief Set rate of in-air status updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_in_air(double rate_hz);

    /**
     * @brief Set rate of attitude updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_attitude(double rate_hz);

    /**
     * @brief Set rate of camera attitude updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_camera_attitude(double rate_hz);

    /**
     * @brief Set rate of ground speed (NED) updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_ground_speed_ned(double rate_hz);

    /**
     * @brief Set rate of IMU reading (NED) updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_imu_reading_ned(double rate_hz);

    /**
     * @brief Set rate of GPS information updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_gps_info(double rate_hz);

    /**
     * @brief Set rate of battery status updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_battery(double rate_hz);

    /**
     * @brief Set rate of RC status updates (synchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_rc_status(double rate_hz);

    /**
     * @brief Set rate of kinematic (position and velocity) updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @see PositionVelocityNED
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_position_velocity_ned_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of position updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_position_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of home position updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_home_position_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of in-air status updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_in_air_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of attitude updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_attitude_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of camera attitude updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_camera_attitude_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of ground speed (NED) updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_ground_speed_ned_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of IMU reading (NED) updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Cabllback to receive request result.
     */
    void set_rate_imu_reading_ned_async(double rate_hz, result_callback_t callback);
    /**
     * @brief Set rate of GPS information updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_gps_info_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of battery status updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_battery_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of RC status updates (asynchronous).
     *
     * @note To stop sending it completely, use a rate_hz of -1, for default rate use 0.
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_rc_status_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of Unix Epoch Time update (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_unix_epoch_time_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Get the current kinematic (position and velocity) in NED frame (synchronous).
     *
     * @return PositionVelocityNED.
     */
    PositionVelocityNED position_velocity_ned() const;

    /**
     * @brief Get the current position (synchronous).
     *
     * @return Position.
     */
    Position position() const;

    /**
     * @brief Get the home position (synchronous).
     *
     * @return Home position.
     */
    Position home_position() const;

    /**
     * @brief Get status text (synchronous).
     *
     * @return Status text.
     */
    StatusText status_text() const;

    /**
     * @brief Get the in-air status (synchronous).
     *
     * @return true if in-air (flying) and not on-ground (landed).
     */
    bool in_air() const;

    /**
     * @brief Get the landed state status (synchronous).
     *
     * @return Landed state.
     */
    LandeState landed_state() const;

    /**
     * @brief Get the arming status (synchronous).
     *
     * @return true if armed (propellers spinning).
     */
    bool armed() const;

    /**
     * @brief Get the current attitude in quaternions (synchronous).
     *
     * @return Attitude as quaternion.
     */
    Quaternion attitude_quaternion() const;

    /**
     * @brief Get the current attitude in Euler angles (synchronous).
     *
     * @return Attitude as Euler angle.
     */
    EulerAngle attitude_euler_angle() const;

    /**
     * @brief Get the camera's attitude in quaternions (synchronous).
     *
     * Note that the yaw component of attitude is relative to North (absolute frame).
     *
     * @return Camera's attitude as quaternion.
     */
    Quaternion camera_attitude_quaternion() const;

    /**
     * @brief Get the camera's attitude in Euler angles (synchronous).
     *
     * Note that the yaw component of attitude is relative to North (absolute frame).
     *
     * @return Camera's attitude as Euler angle.
     */
    EulerAngle camera_attitude_euler_angle() const;

    /**
     * @brief Get the current ground speed (NED) (synchronous).
     *
     * @return Ground speed in NED.
     */
    GroundSpeedNED ground_speed_ned() const;

    /**
     * @brief Get the current IMU reading (NED) (synchronous).
     *
     * @return IMU reading in NED.
     */
    IMUReadingNED imu_reading_ned() const;

    /**
     * @brief Get the current GPS information (synchronous).
     *
     * @return GPS information.
     */
    GPSInfo gps_info() const;

    /**
     * @brief Get the current battery status (synchronous).
     */
    Battery battery() const;

    /**
     * @brief Get the current flight mode (synchronous).
     *
     * @return Flight mode.
     */
    FlightMode flight_mode() const;

    /**
     * @brief Get the current health status (synchronous).
     *
     * @return Health status.
     */
    Health health() const;

    /**
     * @brief Returns true if the overall health is ok (synchronous).
     *
     * @return True if all health flags are OK.
     */
    bool health_all_ok() const;

    /**
     * @brief Get the RC status (synchronous).
     *
     * @return RC status.
     */
    RCStatus rc_status() const;

    /**
     * @brief Callback type for kinematic (position and velocity) updates.
     */
    typedef std::function<void(PositionVelocityNED)> position_velocity_ned_callback_t;

    /**
     * @brief Subscribe to kinematic (position and velocity) updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void position_velocity_ned_async(position_velocity_ned_callback_t callback);

    /**
     * @brief Callback type for position updates.
     */
    typedef std::function<void(Position)> position_callback_t;

    /**
     * @brief Subscribe to position updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void position_async(position_callback_t callback);

    /**
     * @brief Subscribe to home position updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void home_position_async(position_callback_t callback);

    /**
     * @brief Callback type for in-air updates.
     *
     * @param in_air true if in-air (flying) and not on-ground (landed).
     */
    typedef std::function<void(bool in_air)> in_air_callback_t;

    /**
     * @brief Callback for mavlink status text updates.
     *
     * @param status text with message type and text.
     */
    typedef std::function<void(StatusText status_text)> status_text_callback_t;

    /**
     * @brief Subscribe to in-air updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void in_air_async(in_air_callback_t callback);

    /**
     * @brief Subscribe to status text updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void status_text_async(status_text_callback_t callback);

    /**
     * @brief Callback type for armed updates (asynchronous).
     *
     * @param armed true if armed (motors spinning).
     */
    typedef std::function<void(bool armed)> armed_callback_t;

    /**
     * @brief Subscribe to armed updates (asynchronous).
     *
     * Note that armed updates are limited to 1Hz.
     *
     * @param callback Function to call with updates.
     */
    void armed_async(armed_callback_t callback);

    /**
     * @brief Callback type for attitude updates in quaternion.
     *
     * @param quaternion Attitude quaternion.
     */
    typedef std::function<void(Quaternion quaternion)> attitude_quaternion_callback_t;

    /**
     * @brief Subscribe to attitude updates in quaternion (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void attitude_quaternion_async(attitude_quaternion_callback_t callback);

    /**
     * @brief Callback type for attitude updates in Euler angles.
     *
     * @param euler_angle Attitude Euler angle.
     */
    typedef std::function<void(EulerAngle euler_angle)> attitude_euler_angle_callback_t;

    /**
     * @brief Subscribe to attitude updates in Euler angles (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void attitude_euler_angle_async(attitude_euler_angle_callback_t callback);

    /**
     * @brief Subscribe to camera attitude updates in quaternion (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void camera_attitude_quaternion_async(attitude_quaternion_callback_t callback);

    /**
     * @brief Subscribe to camera attitude updates in Euler angles (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void camera_attitude_euler_angle_async(attitude_euler_angle_callback_t callback);

    /**
     * @brief Callback type for ground speed (NED) updates.
     *
     * @param ground_speed_ned Ground speed (NED).
     */
    typedef std::function<void(GroundSpeedNED ground_speed_ned)> ground_speed_ned_callback_t;

    /**
     * @brief Subscribe to ground speed (NED) updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void ground_speed_ned_async(ground_speed_ned_callback_t callback);

    /**
     * @brief Callback type for IMU (NED) updates.
     *
     * @param imu_reading_ned IMU reading (NED).
     */
    typedef std::function<void(IMUReadingNED imu_reading_ned)> imu_reading_ned_callback_t;

    /**
     * @brief Subscribe to IMU reading (NED) updates (asynchronous).
     *
     * @param callback function to call with updates.
     */
    void imu_reading_ned_async(imu_reading_ned_callback_t callback);

    /**
     * @brief Callback type for GPS information updates.
     *
     * @param gps_info GPS information.
     */
    typedef std::function<void(GPSInfo gps_info)> gps_info_callback_t;

    /**
     * @brief Subscribe to GPS information updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void gps_info_async(gps_info_callback_t callback);

    /**
     * @brief Callback type for battery status updates.
     *
     * @param battery Battery status.
     */
    typedef std::function<void(Battery battery)> battery_callback_t;

    /**
     * @brief Subscribe to battery status updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void battery_async(battery_callback_t callback);

    /**
     * @brief Callback type for flight mode updates.
     *
     * @param flight_mode Flight mode.
     */
    typedef std::function<void(FlightMode flight_mode)> flight_mode_callback_t;

    /**
     * @brief Subscribe to flight mode updates (asynchronous).
     *
     * Note that flight mode updates are limited to 1Hz.
     *
     * @param callback Function to call with updates.
     */
    void flight_mode_async(flight_mode_callback_t callback);

    /**
     * @brief Callback type for health status updates.
     *
     * @param health health flags.
     */
    typedef std::function<void(Health health)> health_callback_t;

    /**
     * @brief Subscribe to health status updates (asynchronous).
     *
     * Note that health status updates are limited to 1Hz.
     *
     * @param callback Function to call with updates.
     */
    void health_async(health_callback_t callback);

    /**
     * @brief Callback type for health status updates.
     *
     * @param health_all_ok If all health flags are ok.
     */
    typedef std::function<void(bool health_all_ok)> health_all_ok_callback_t;

    /**
     * @brief Subscribe to overall health status updates (asynchronous).
     *
     * Note that overall health status updates are limited to 1Hz.
     *
     * @param callback Function to call with updates.
     */
    void health_all_ok_async(health_all_ok_callback_t callback);

    /**
     * @brief Callback type for landed state updates.
     *
     * @param LandedState enumeration.
     */

    typedef std::function<void(LandedState landed_state)> landed_state_callback_t;

    /**
     * @brief Callback type for Landed state updates.
     *
     * @param callback Function to call with updates.
     */
    void landed_state(landed_state_callback_t callback);

    /**
     * @brief Callback type for RC status updates.
     *
     * @param rc_status RC status.
     */
    typedef std::function<void(RCStatus rc_status)> rc_status_callback_t;

    /**
     * @brief Callback type for Unix Epoch Time updates.
     *
     * @param uint64_t Epoch time [us].
     */
    typedef std::function<void(uint64_t time_us)> unix_epoch_time_callback_t;

    /**
     * @brief Subscribe to RC status updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void rc_status_async(rc_status_callback_t callback);

    /**
     * @brief Subscribe to Unix Epoch Time updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void unix_epoch_time_async(unix_epoch_time_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Telemetry(const Telemetry&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Telemetry& operator=(const Telemetry&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<TelemetryImpl> _impl;
};

/**
 * @brief Equal operator to compare two `Telemetry::PositionVelocityNED` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(
    const Telemetry::PositionVelocityNED& lhs, const Telemetry::PositionVelocityNED& rhs);

/**
 * @brief Equal operator to compare two `Telemetry::Position` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::Position& lhs, const Telemetry::Position& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::Position`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::Position const& position);

/**
 * @brief Stream operator to print information about a `Telemetry::PositionNED`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::PositionNED const& position_ned);

/**
 * @brief Stream operator to print information about a `Telemetry::VelocityNED`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::VelocityNED const& velocity_ned);

/**
 * @brief Stream operator to print information about a `Telemetry::PositionVelocityNED`.
 *
 * @return A reference to the stream.
 */
std::ostream&
operator<<(std::ostream& str, Telemetry::PositionVelocityNED const& position_velocity_ned);

/**
 * @brief Equal operator to compare two `Telemetry::Health` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::Health& lhs, const Telemetry::Health& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::Health`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::Health const& health);

/**
 * @brief Equal operator to compare two `Telemetry::IMUReadingNED` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::IMUReadingNED& lhs, const Telemetry::IMUReadingNED& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::AccelerationNED`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::AccelerationNED const& acceleration_ned);

/**
 * @brief Stream operator to print information about a `Telemetry::AngularVelocityNED`.
 *
 * @return A reference to the stream.
 */
std::ostream&
operator<<(std::ostream& str, Telemetry::AngularVelocityNED const& angular_velocity_ned);

/**
 * @brief Stream operator to print information about a `Telemetry::MagneticFieldNED`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::MagneticFieldNED const& magnetic_field);

/**
 * @brief Stream operator to print information about a `Telemetry::IMUReadingNED`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::IMUReadingNED const& imu_reading_ned);

/**
 * @brief Equal operator to compare two `Telemetry::GPSInfo` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::GPSInfo& lhs, const Telemetry::GPSInfo& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::GPSInfo`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::GPSInfo const& gps_info);

/**
 * @brief Equal operator to compare two `Telemetry::Battery` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::Battery& lhs, const Telemetry::Battery& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::Battery`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::Battery const& battery);

/**
 * @brief Equal operator to compare two `Telemetry::Quaternion` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::Quaternion& lhs, const Telemetry::Quaternion& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::Quaternion`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::Quaternion const& quaternion);

/**
 * @brief Equal operator to compare two `Telemetry::EulerAngle` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::EulerAngle& lhs, const Telemetry::EulerAngle& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::EulerAngle`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::EulerAngle const& euler_angle);

/**
 * @brief Equal operator to compare two `Telemetry::GroundSpeedNED` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::GroundSpeedNED& lhs, const Telemetry::GroundSpeedNED& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::GroundSpeedNED`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::GroundSpeedNED const& ground_speed);

/**
 * @brief Equal operator to compare two `Telemetry::RCStatus` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Telemetry::RCStatus& lhs, const Telemetry::RCStatus& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::RCStatus`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::RCStatus const& rc_status);

/**
 * @brief Stream operator to print information about a `Telemetry::StatusText`.
 *
 * @returns A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Telemetry::StatusText const& status_text);

} // namespace mavsdk
