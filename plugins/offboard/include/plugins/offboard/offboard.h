#pragma once

#include <functional>
#include <memory>

#include <dronecode_sdk/plugin_base.h>

namespace dronecode_sdk {

class OffboardImpl;
class System;

/**
 * @brief This class is used to control a drone with velocity commands.
 *
 * The module is called offboard because the velocity commands can be sent from external sources
 * as opposed to onboard control right inside the autopilot "board".
 *
 * Client code must specify a setpoint before starting offboard mode.
 * DronecodeSDK automatically resends setpoints at 20Hz (PX4 Offboard mode requires that setpoints
 * are minimally resent at 2Hz). If more precise control is required, clients can call the setpoint
 * methods at whatever rate is required.
 *
 * **Attention:** this is work in progress, use with caution!
 */

class Offboard : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto offboard = std::make_shared<Offboard>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Offboard(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Offboard();

    /**
     * @brief Results for offboard requests.
     */
    enum class Result {
        UNKNOWN, /**< @brief Unknown error. */
        SUCCESS, /**< @brief %Request succeeded. */
        NO_SYSTEM, /**< @brief No system connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief Vehicle busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief %Request timeout. */
        NO_SETPOINT_SET /**< Can't start without setpoint set. */
    };

    /**
     * @brief Get human-readable English string for Offboard::Result.
     *
     * @param result The enum value for which a string is required.
     * @returns Human-readable string for enum value.
     */
    static const char *result_str(Result result);

    /**
     * @brief Callback type for offboard requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Type for Velocity commands in NED (North East Down) coordinates and yaw.
     */
    struct VelocityNEDYaw {
        float north_m_s; /**< @brief Velocity North in metres/second. */
        float east_m_s; /**< @brief Velocity East in metres/second. */
        float down_m_s; /**< @brief Velocity Down in metres/second. */
        float yaw_deg; /**< @brief Yaw in degrees (0 North, positive is clock-wise looking from
                          above). */
    };

    /**
     * @brief Type for Position commands in NED (North East Down) coordinates and yaw.
     */
    struct PositionNEDYaw {
        float north_m; /**< @brief Position North in metres. */
        float east_m; /**< @brief Position East in metres. */
        float down_m; /**< @brief Position Down in metres. */
        float yaw_deg; /**< @brief Yaw in degrees (0 North, positive is clock-wise looking from
                          above). */
    };

    /**
     * @brief Type for velocity commands in body coordinates (forward, right, down and yaw angular
     * rate).
     */
    struct VelocityBodyYawspeed {
        float forward_m_s; /**< @brief Velocity forward in metres/second. */
        float right_m_s; /**< @brief Velocity right in metres/secon.d */
        float down_m_s; /**< @brief Velocity down in metres/second. */
        float yawspeed_deg_s; /**< @brief Yaw angular rate in degrees/second (positive for
                                 clock-wise looking from above). */
    };

    /**
     * @brief Type for attitude body angles in NED reference frame (roll, pitch, yaw and thrust)
     */
    struct Attitude {
        float roll_deg; /**< @brief Roll angle in degrees (positive is right side down). */
        float pitch_deg; /**< @brief Pitch angle in degrees (positive is nose up). */
        float yaw_deg; /**< @brief Yaw in degrees (positive is move nose to the right). */
        float thrust_value; /**< @brief Thrust in percentage ranging from 0 to 1 ( 0 to 100
                               percent). */
    };

    /**
     * @brief Type for Attitude rate commands in body coordinates (roll, pitch, yaw  angular
     * rate and Thrust).
     */
    struct AttitudeRate {
        float roll_deg_s; /**< @brief Roll Angular Rate in degrees/second (positive for clock-wise
                             looking from front). */
        float pitch_deg_s; /**< @brief Pitch Angular Rate in degrees/second (positive for head/front
                              moving up). */
        float yaw_deg_s; /**< @brief Yaw Angular Rate in degrees/second (positive for clock-wise
                            looking from above). */
        float thrust_value; /**< @brief Thrust in percentage ranging from 0 to 1 ( 0 to 100
                               percent). */
    };

    /**
     * @brief Start offboard control (synchronous).
     *
     * **Attention:** this is work in progress, use with caution!
     *
     * @return Result of request.
     */
    Offboard::Result start();

    /**
     * @brief Stop offboard control (synchronous).
     *
     * The vehicle will be put into Hold mode: https://docs.px4.io/en/flight_modes/hold.html
     *
     * @return Result of request.
     */
    Offboard::Result stop();

    /**
     * @brief Start offboard control (asynchronous).
     *
     * **Attention:** This is work in progress, use with caution!
     *
     * @param callback Callback to receive request result.
     */
    void start_async(result_callback_t callback);

    /**
     * @brief Stop offboard control (asynchronous).
     *
     * The vehicle will be put into Hold mode: https://docs.px4.io/en/flight_modes/hold.html
     *
     * @param callback Callback to receive request result.
     */
    void stop_async(result_callback_t callback);

    /**
     * @brief Check if offboard control is active.
     *
     * `true` means that the vehicle is in offboard mode and we are actively sending
     * setpoints.
     *
     * @return `true` if active
     */
    bool is_active() const;

    /**
     * @brief Set the position in NED coordinates and yaw.
     *
     * @param position_ned_yaw Position and yaw `struct`.
     */
    void set_position_ned(PositionNEDYaw position_ned_yaw);

    /**
     * @brief Set the velocity in NED coordinates and yaw.
     *
     * @param velocity_ned_yaw Velocity and yaw `struct`.
     */
    void set_velocity_ned(VelocityNEDYaw velocity_ned_yaw);

    /**
     * @brief Set the velocity body coordinates and yaw angular rate.
     *
     * @param velocity_body_yawspeed Velocity and yaw angular rate `struct`.
     */
    void set_velocity_body(VelocityBodyYawspeed velocity_body_yawspeed);

    /**
     * @brief Set the attitude in terms of roll, pitch and yaw in degrees with thrust
     * in percentage.
     *
     * @param attitude roll, pitch and yaw in degrees along with thrust in percentage.
     */
    void set_attitude(Attitude attitude);

    /**
     * @brief Set the attitude rate in terms of pitch, roll and yaw angular rate along with thrust
     * in percentage.
     *
     * @param attitude_rate roll, pitch and yaw angular rate along with thrust in percentage.
     */
    void set_attitude_rate(AttitudeRate attitude_rate);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Offboard(const Offboard &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Offboard &operator=(const Offboard &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<OffboardImpl> _impl;
};

/**
 * @brief Equal operator to compare two `Offboard::Attitude` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Offboard::Attitude &lhs, const Offboard::Attitude &rhs);

/**
 * @brief Equal operator to compare two `Offboard::AttitudeRate` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Offboard::AttitudeRate &lhs, const Offboard::AttitudeRate &rhs);

/**
 * @brief Stream operator to print information about a `Offboard::Attitude`.
 *
 * @return A reference to the stream.
 */
std::ostream &operator<<(std::ostream &str, Offboard::Attitude const &attitude);

/**
 * @brief Stream operator to print information about a `Offboard::AttitudeRate`.
 *
 * @return A reference to the stream.
 */
std::ostream &operator<<(std::ostream &str, Offboard::AttitudeRate const &attitude_rate);

/**
 * @brief Equal operator to compare two `Offboard::PositionNEDYaw` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Offboard::PositionNEDYaw &lhs, const Offboard::PositionNEDYaw &rhs);

/**
 * @brief Stream operator to print information about a `Offboard::PositionNEDYaw`.
 *
 * @return A reference to the stream.
 */
std::ostream &operator<<(std::ostream &str, Offboard::PositionNEDYaw const &position_ned_yaw);

/**
 * @brief Equal operator to compare two `Offboard::VelocityBodyYawspeed` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Offboard::VelocityBodyYawspeed &lhs,
                const Offboard::VelocityBodyYawspeed &rhs);

/**
 * @brief Stream operator to print information about a `Offboard::VelocityBodyYawspeed`.
 *
 * @return A reference to the stream.
 */
std::ostream &operator<<(std::ostream &str,
                         Offboard::VelocityBodyYawspeed const &velocity_body_yawspeed);

/**
 * @brief Equal operator to compare two `Offboard::VelocityNEDYaw` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Offboard::VelocityNEDYaw &lhs, const Offboard::VelocityNEDYaw &rhs);

/**
 * @brief Stream operator to print information about a `Offboard::VelocityNEDYaw`.
 *
 * @return A reference to the stream.
 */
std::ostream &operator<<(std::ostream &str, Offboard::VelocityNEDYaw const &velocity_ned_yaw);

} // namespace dronecode_sdk
