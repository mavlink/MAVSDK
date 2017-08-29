#pragma once

#include <functional>

namespace dronecore {

class ActionImpl;

/* The Action class enables simple actions for a drone
 * such as arming, taking off, and landing.
 */
class Action
{
public:
    /**
     * Constructor for Action called internally.
     */
    explicit Action(ActionImpl *impl);
    /**
     * Destructor for Action called internally.
     */
    ~Action();

    /**
     * Possible results returned for commanded actions.
     */
    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        COMMAND_DENIED_LANDED_STATE_UNKNOWN,
        COMMAND_DENIED_NOT_LANDED,
        TIMEOUT,
        UNKNOWN
    };

    /**
     * Returns a human-readable English string for an Action::Result.
     */
    static const char *result_str(Result);

    /**
     * Arms the drone (synchronous).
     *
     * Note that arming a drone normally means that the motors will spin at idle.
     * Therefore, before arming take all safety precautions and stand clear of the drone.
     *
     * @return result of request
     */
    Result arm() const;

    /**
     * Disarms the drone (synchronous).
     *
     * This will disarm a drone if it considers itself landed. If flying in-air, the drone should
     * reject the disarm command. Disarming means that all motors will stop.
     *
     * @return result of request
     */
    Result disarm() const;

    /**
     * Kills the drone (synchronous).
     *
     * This will disarm a drone no matter if still landed or still in-air.
     * Note that the drone will fall out of the sky if you use this command while flying.
     *
     * @return result of request
     */
    Result kill() const;

    /**
     * Commands the drone to take off and hover at a set takeoff altitude (synchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * set takeoff altitude.
     *
     * @return result of request
     */
    Result takeoff() const;

    /**
     * Commands the drone to land at the current position (synchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * set takeoff altitude.
     *
     * @return result of request
     */
    Result land() const;

    /**
     * Commands the drone to return to the launch (takeoff) position and land there (synchronous).
     *
     * This switches the drone into RTL mode which generally means it will rise up to a certain
     * altitude to clear any obstacles before heading back to the launch (takeoff) position and
     * land there.
     *
     * @return result of request
     */
    Result return_to_launch() const;

    /**
     * Callback type for async action calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * Arms the drone (asynchronous).
     *
     * Note that arming a drone normally means that the motors will spin at idle.
     * Therefore, before arming take all safety precautions and stand clear of the drone.
     *
     * @param callback function to call with result of request
     */
    void arm_async(result_callback_t callback);

    /**
     * Disarms the drone (asynchronous).
     *
     * This will disarm a drone if it considers itself landed. If flying in-air, the drone should
     * reject the disarm command. Disarming means that all motors will stop.
     *
     * @param callback function to call with result of request
     */
    void disarm_async(result_callback_t callback);

    /**
     * Kills the drone (asynchronous).
     *
     * This will disarm a drone no matter if still landed or still in-air.
     * Note that the drone will fall out of the sky if you use this command while flying.
     *
     * @param callback function to call with result of request
     */
    void kill_async(result_callback_t callback);

    /**
     * Commands the drone to take off and hover at a set takeoff altitude (asynchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * set takeoff altitude.
     *
     * @param callback function to call with result of request
     */
    void takeoff_async(result_callback_t callback);

    /**
     * Commands the drone to land at the current position (asynchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * set takeoff altitude.
     *
     * @param callback function to call with result of request
     */
    void land_async(result_callback_t callback);

    /**
     * Commands the drone to return to the launch (takeoff) position and land there (asynchronous).
     *
     * This switches the drone into RTL mode which generally means it will rise up to a certain
     * altitude to clear any obstacles before heading back to the launch (takeoff) position and
     * land there.
     *
     * @param callback function to call with result of request
     */
    void return_to_launch_async(result_callback_t callback);

    /**
     * Sets takeoff altitude above ground.
     *
     * @param relative_altitude_m takeoff altitude relative to takeoff location in meters
     */
    void set_takeoff_altitude(float relative_altitude_m);

    /**
     * Gets the takeoff altitude above ground.
     *
     * @return takeoff altitude relative to takeoff location in meters
     */
    float get_takeoff_altitude_m() const;

    /**
     * Sets maximum speed that drone is set to fly at.
     *
     * @param speed_m_s maximum speed in meters/second
     */
    void set_max_speed(float speed_m_s);

    /**
     * Gets maximum speed that drone is set to fly at.
     *
     * @return maximum speed in meters/second
     */
    float get_max_speed_m_s() const;

    // Non-copyable
    Action(const Action &) = delete;
    const Action &operator=(const Action &) = delete;

private:
    // Underlying implementation, set at instantiation
    ActionImpl *_impl;
};

} // namespace dronecore
