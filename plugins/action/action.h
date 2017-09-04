#pragma once

#include <functional>

namespace dronecore {

class ActionImpl;

/**
 * @brief The Action class enables simple actions for a drone
 * such as arming, taking off, and landing.
 */
class Action
{
public:
    /**
     * @brief Constructor (internal use only).
     *
     * @param impl Private internal implementation.
     */
    explicit Action(ActionImpl *impl);

    /**
     * @brief Destructor (internal use only).
     */
    ~Action();

    /**
     * @brief Possible results returned for commanded actions.
     */
    enum class Result {
        /** @brief %Action succeeded. */
        SUCCESS = 0,
        /** @brief No device is connected. */
        NO_DEVICE,
        /** @brief %Connection error. */
        CONNECTION_ERROR,
        /** @brief Vehicle busy error. */
        BUSY,
        /** @brief Command refused. */
        COMMAND_DENIED,
        /** @brief Command refused because landed state is unknown. */
        COMMAND_DENIED_LANDED_STATE_UNKNOWN,
        /** @brief Command refused because vehicle not landed. */
        COMMAND_DENIED_NOT_LANDED,
        /** @brief Timeout waiting for %Action to complete. */
        TIMEOUT,
        /** @brief Unspecified error. */
        UNKNOWN
    };

    /**
     * @brief Returns a human-readable English string for an Action::Result.
     */
    static const char *result_str(Result);

    /**
     * @brief Arm the drone (synchronous).
     *
     * **Note** Arming a drone normally causes motors to spin at idle.
     * Before arming take all safety precautions and stand clear of the drone!

     * @param Result The Action::Result value for which a human readable string is required.
     * @return Result of request.
     */
    Result arm() const;

    /**
     * @brief Disarm the drone (synchronous).
     *
     * This will disarm a drone that considers itself landed. If flying, the drone should
     * reject the disarm command. Disarming means that all motors will stop.
     *
     * @return Result of request.
     */
    Result disarm() const;

    /**
     * @brief Kill the drone (synchronous).
     *
     * This will disarm a drone irrespective of whether it is landed or flying.
     * Note that the drone will fall out of the sky if you use this command while flying.
     *
     * @return Result of request.
     */
    Result kill() const;

    /**
     * @brief Take off and hover (synchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * the takeoff altitude set using `set_takeoff_altitude()`.
     *
     * Note that the vehicle must be armed before it can take off.
     *
     * @return Result of request.
     */
    Result takeoff() const;

    /**
     * @brief Land at the current position (synchronous).
     *
     * @return Result of request.
     */
    Result land() const;

    /**
     * @brief Return to the launch (takeoff) position and land (asynchronous).
     *
     * This switches the drone into RTL mode which generally means it will rise up to a certain
     * altitude to clear any obstacles before heading back to the launch (takeoff) position and
     * land there.
     *
     * @return Result of request.
     */
    Result return_to_launch() const;

    /**
     * @brief Callback type for async Action calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Arm the drone (asynchronous).
     *
     * Note that arming a drone normally means that the motors will spin at idle.
     * Therefore, before arming take all safety precautions and stand clear of the drone.
     *
     * @param callback Function to call with result of request.
     */
    void arm_async(result_callback_t callback);

    /**
     * @brief Disarm the drone (asynchronous).
     *
     * This will disarm a drone that considers itself landed. If flying, the drone should
     * reject the disarm command. Disarming means that all motors will stop.
     *
     * @param callback Function to call with result of request.
     */
    void disarm_async(result_callback_t callback);

    /**
     * @brief Kill the drone (asynchronous).
     *
     * This will disarm a drone irrespective of whether it is landed or flying.
     * Note that the drone will fall out of the sky if you use this command while flying.
     *
     * @param callback Function to call with result of request.
     */
    void kill_async(result_callback_t callback);

    /**
     * @brief Take off and hover (asynchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * the takeoff altitude set using set_takeoff_altitude().
     *
     * Note that the vehicle must be armed before it can take off.
     *
     * @param callback Function to call with result of request
     */
    void takeoff_async(result_callback_t callback);

    /**
     * @brief Land at the current position (asynchronous).
     *
     * @param callback Function to call with result of request.
     */
    void land_async(result_callback_t callback);

    /**
     * @brief Return to the launch (takeoff) position and land (asynchronous).
     *
     * This switches the drone into RTL mode which generally means it will rise up to a certain
     * altitude to clear any obstacles before heading back to the launch (takeoff) position and
     * land there.
     *
     * @param callback Function to call with result of request.
     */
    void return_to_launch_async(result_callback_t callback);

    /**
     * @brief Set takeoff altitude above ground.
     *
     * @param relative_altitude_m Takeoff altitude relative to takeoff location in meters
     */
    void set_takeoff_altitude(float relative_altitude_m);

    /**
     * @brief Get the takeoff altitude.
     *
     * @return takeoff Altitude relative to takeoff location in meters.
     */
    float get_takeoff_altitude_m() const;

    /**
     * @brief Set vehicle maximum speed.
     *
     * @param speed_m_s Maximum speed in metres/second.
     */
    void set_max_speed(float speed_m_s);

    /**
     * @brief Get the vehicle maximum speed.
     *
     * @return Maximum speed in metres/second.
     */
    float get_max_speed_m_s() const;

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Action(const Action &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Action &operator=(const Action &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    ActionImpl *_impl;
};

} // namespace dronecore
