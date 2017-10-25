#pragma once

#include <functional>

namespace dronecore {

class OffboardImpl;

/**
 * @brief This class is used to control a drone with velocity commands.
 *
 * The module is called offboard because the velocity commands can be sent from external sources
 * as opposed to onboard control right inside the autopilot "board".
 *
 * **Attention:** this is work in progress, use with caution!
 */
class Offboard
{
public:
    /**
     * @brief Constructor (internal use only).
     */
    explicit Offboard(OffboardImpl *impl);

    /**
     * @brief Destructor (internal use only).
     */
    ~Offboard();

    /**
     * @brief Results for offboard requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        NO_DEVICE, /**< @brief No device connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief Vehicle busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief %Request timeout. */
        NO_SETPOINT_SET, /**< Can't start without setpoint set. */
        UNKNOWN /**< @brief Unknown error. */
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
        float north_m_s; /**< Velocity North in metres/second. */
        float east_m_s; /**< Velocity East in metres/second. */
        float down_m_s; /**< Velocity Down in metres/second. */
        float yaw_deg; /**< Yaw in degrees (0 North, positive is clock-wise looking from above. */
    };

    /**
     * @brief Type for velocity commands in body coordinates (forward, right, down and yaw angular rate).
     */
    struct VelocityBodyYawspeed {
        float forward_m_s; /**< Velocity forward in metres/second. */
        float right_m_s; /**< Velocity right in metres/secon.d */
        float down_m_s; /**< Velocity down in metres/second. */
        float yawspeed_deg_s; /**< Yaw angular rate in degrees/second (positive for clock-wise
                                   looking from above. */
    };

    /**
     * @brief Start offboard control (synchronous).
     *
     * **Attention:** this is work in progress, use with caution!
     *
     * @return Result of request.
     */
    Offboard::Result start() const;

    /**
     * @brief Stop offboard control (synchronous).
     *
     * @return Result of request.
     */
    Offboard::Result stop() const;

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
     * @param callback Callback to receive request result.
     */
    void stop_async(result_callback_t callback);

    /**
     * @brief Set the velocity in NED coordinates and yaw.
     *
     * @param velocity_ned_yaw Velocity and yaw `struct`.
     */
    void set_velocity_ned(VelocityNEDYaw velocity_ned_yaw);

    /**
     * @brief Set the velocity body coordinates coordinates and yaw angular rate.
     *
     * @param velocity_body_yawspeed Velocity and yaw angular rate `struct`.
     */
    void set_velocity_body(VelocityBodyYawspeed velocity_body_yawspeed);

    // Non-copyable
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
    OffboardImpl *_impl;
};

} // namespace dronecore
