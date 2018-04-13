#pragma once

#include <functional>
#include <memory>

#include "plugin_base.h"
#include "telemetry_structs.h"

namespace dronecore {

class TelemetryImpl;
class System;

/**
 * @brief This class allows users to get vehicle telemetry and state information
 * (e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.
 */
class Telemetry : public PluginBase
{
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
    explicit Telemetry(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Telemetry();

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
    static const char *result_str(Result result);

    /**
     * @brief Callback type for telemetry requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Set rate of position updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_position(double rate_hz);

    /**
     * @brief Set rate of home position updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_home_position(double rate_hz);

    /**
     * @brief Set rate of in-air status updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_in_air(double rate_hz);

    /**
     * @brief Set rate of attitude updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_attitude(double rate_hz);

    /**
     * @brief Set rate of camera attitude updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_camera_attitude(double rate_hz);

    /**
     * @brief Set rate of ground speed (NED) updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_ground_speed_ned(double rate_hz);

    /**
     * @brief Set rate of GPS information updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_gps_info(double rate_hz);

    /**
     * @brief Set rate of battery status updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_battery(double rate_hz);

    /**
     * @brief Set rate of RC status updates (synchronous).
     *
     * @param rate_hz Rate in Hz.
     * @return Result of request.
     */
    Result set_rate_rc_status(double rate_hz);

    /**
     * @brief Set rate of position updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_position_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of home position updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_home_position_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of in-air status updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_in_air_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of attitude updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_attitude_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of camera attitude updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_camera_attitude_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of ground speed (NED) updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_ground_speed_ned_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of GPS information updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_gps_info_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of battery status updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_battery_async(double rate_hz, result_callback_t callback);

    /**
     * @brief Set rate of RC status updates (asynchronous).
     *
     * @param rate_hz Rate in Hz.
     * @param callback Callback to receive request result.
     */
    void set_rate_rc_status_async(double rate_hz, result_callback_t callback);

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
     * @brief Get the in-air status (synchronous).
     *
     * @return true if in-air (flying) and not on-ground (landed).
     */
    bool in_air() const;

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
     * @brief Subscribe to in-air updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void in_air_async(in_air_callback_t callback);

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
     * @brief Callback type for RC status updates.
     *
     * @param rc_status RC status.
     */
    typedef std::function<void(RCStatus rc_status)> rc_status_callback_t;

    /**
     * @brief Subscribe to RC status updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void rc_status_async(rc_status_callback_t callback);

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Telemetry(const Telemetry &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Telemetry &operator=(const Telemetry &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<TelemetryImpl> _impl;
};

} // namespace dronecore
