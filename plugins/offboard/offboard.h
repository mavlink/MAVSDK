#pragma once

#include <functional>

namespace dronecore {

class OffboardImpl;

/**
 * The Offboard class allows to control a drone by velocity commands.
 *
 * The module is called offboard because the velocity commands can be sent from external sources
 * as opposed to onboard control right inside the autopilot "board".
 *
 * Attention: this is work in progress, use with caution!
 */
class Offboard
{
public:
    /**
     * Constructor for Offboard called internally.
     */
    explicit Offboard(OffboardImpl *impl);

    /**
     * Destructor for Offboard called internally.
     */
    ~Offboard();

    /**
     * Results for offboard requests
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
     * Returns human-readable English string for Offboard::Result.
     *
     * @param result result enum
     */
    static const char *result_str(Result result);

    /**
     * Callback type for offboard requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * Type for Velocity commands in NED (North East Down) coordinates and yaw.
     */
    struct VelocityNEDYaw {
        float north_m_s; /**< Velocity North in meters/second */
        float east_m_s; /**< Velocity East in meters/second */
        float down_m_s; /**< Velocity Down in meters/second */
        float yaw_deg; /**< Yaw in degrees (0 North, positive is clock-wise looking from above. */
    };

    /**
     * Type for velocity commands in body coordinates (forward, right, down and yaw angular rate).
     */
    struct VelocityBodyYawspeed {
        float forward_m_s; /**< Velocity forward in meters/second */
        float right_m_s; /**< Velocity right in meters/second */
        float down_m_s; /**< Velocity down in meters/second */
        float yawspeed_deg_s; /**< Yaw angular rate in degrees/second (positive for clock-wise
                                   looking from above */
    };

    /**
     * Starts offboard control (synchronous).
     *
     * Attention: this is work in progress, use with caution!
     *
     * @return result of request
     */
    Offboard::Result start() const;

    /**
     * Stops offboard control (synchronous).
     *
     * @return result of request
     */
    Offboard::Result stop() const;

    /**
     * Starts offboard control (asynchronous).
     *
     * Attention: this is work in progress, use with caution!
     *
     * @param callback callback to receive request result
     */
    void start_async(result_callback_t callback);

    /**
     * Stops offboard control (asynchronous).
     *
     * @param callback callback to receive request result
     */
    void stop_async(result_callback_t callback);

    /**
     * Sets the velocity in NED coordinates and yaw.
     *
     * @param velocity_ned_yaw velocity and yaw struct
     */
    void set_velocity_ned(VelocityNEDYaw velocity_ned_yaw);

    /**
     * Sets the velocity body coordinates coordinates and yaw angular rate.
     *
     * @param velocity_body_yawspeed velocity and yaw angular rate struct
     */
    void set_velocity_body(VelocityBodyYawspeed velocity_body_yawspeed);

    // Non-copyable
    Offboard(const Offboard &) = delete;
    const Offboard &operator=(const Offboard &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    OffboardImpl *_impl;
};

} // namespace dronecore
