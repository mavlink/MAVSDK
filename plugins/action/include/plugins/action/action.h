#pragma once

#include <functional>
#include <memory>

#include "action_result.h"
#include "plugin_base.h"

namespace dronecode_sdk {

class System;
class ActionImpl;

/**
 * @brief The Action class enables simple actions for a drone
 * such as arming, taking off, and landing.
 *
 * Synchronous and asynchronous variants of the action methods are supplied.
 *
 * The action methods send their associated MAVLink commands to the vehicle and complete
 * (return synchronously or callback asynchronously) with an ActionResult value
 * indicating whether the vehicle has accepted or rejected the command, or that there has been some
 * error.
 * If the command is accepted, the vehicle will then start to perform the associated action.
 */
class Action : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto action = std::make_shared<Action>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Action(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Action();

    /**
     * @brief Send command to *arm* the drone (synchronous).
     *
     * @note Arming a drone normally causes motors to spin at idle.
     * Before arming take all safety precautions and stand clear of the drone!
     *
     * @return ActionResult of request.
     */
    ActionResult arm() const;

    /**
     * @brief Send command to *disarm* the drone (synchronous).
     *
     * This will disarm a drone that considers itself landed. If flying, the drone should
     * reject the disarm command. Disarming means that all motors will stop.
     *
     * @return ActionResult of request.
     */
    ActionResult disarm() const;

    /**
     * @brief Send command to *kill* the drone (synchronous).
     *
     * This will disarm a drone irrespective of whether it is landed or flying.
     * Note that the drone will fall out of the sky if this command is used while flying.
     *
     * @return ActionResult of request.
     */
    ActionResult kill() const;

    /**
     * @brief Send command to *take off and hover* (synchronous).
     *
     * This switches the drone into position control mode and commands it to take off and hover at
     * the takeoff altitude (set using set_takeoff_altitude()).
     *
     * Note that the vehicle must be armed before it can take off.
     *
     * @return ActionResult of request.
     */
    ActionResult takeoff() const;

    /**
     * @brief Send command to *land* at the current position (synchronous).
     *
     * This switches the drone to
     * [Land mode](https://docs.px4.io/en/flight_modes/land.html).
     *
     * @return ActionResult of request.
     */
    ActionResult land() const;

    /**
     * @brief Send command to *return to the launch* (takeoff) position and *land* (asynchronous).
     *
     * This switches the drone into [RTL mode](https://docs.px4.io/en/flight_modes/rtl.html) which
     * generally means it will rise up to a certain altitude to clear any obstacles before heading
     * back to the launch (takeoff) position and land there.
     *
     * @return ActionResult of request.
     */
    ActionResult return_to_launch() const;

    /**
     * @brief Send command to transition the drone to fixedwing.
     *
     * The associated action will only be executed for VTOL vehicles (on other vehicle types the
     * command will fail with an ActionResult). The command will succeed if called when the vehicle
     * is already in fixedwing mode.
     *
     * @return ActionResult of request.
     */
    ActionResult transition_to_fixedwing() const;

    /**
     * @brief Send command to transition the drone to multicopter.
     *
     * The associated action will only be executed for VTOL vehicles (on other vehicle types the
     * command will fail with an ActionResult). The command will succeed if called when the vehicle
     * is already in multicopter mode.
     *
     * @return ActionResult of request.
     */
    ActionResult transition_to_multicopter() const;

    /**
     * @brief Callback type for asynchronous Action calls.
     */
    typedef std::function<void(ActionResult)> result_callback_t;

    /**
     * @brief Send command to *arm* the drone (asynchronous).
     *
     * Note that arming a drone normally means that the motors will spin at idle.
     * Therefore, before arming take all safety precautions and stand clear of the drone.
     *
     * @param callback Function to call with result of request.
     */
    void arm_async(result_callback_t callback);

    /**
     * @brief Send command to *disarm* the drone (asynchronous).
     *
     * This will disarm a drone that considers itself landed. If flying, the drone should
     * reject the disarm command. Disarming means that all motors will stop.
     *
     * @param callback Function to call with result of request.
     */
    void disarm_async(result_callback_t callback);

    /**
     * @brief Send command to *kill* the drone (asynchronous).
     *
     * This will disarm a drone irrespective of whether it is landed or flying.
     * Note that the drone will fall out of the sky if you use this command while flying.
     *
     * @param callback Function to call with result of request.
     */
    void kill_async(result_callback_t callback);

    /**
     * @brief Send command to *take off and hover* (asynchronous).
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
     * @brief Send command to *land* at the current position (asynchronous).
     *
     * This switches the drone to
     * [Land mode](https://docs.px4.io/en/flight_modes/land.html).
     *
     * @param callback Function to call with result of request.
     */
    void land_async(result_callback_t callback);

    /**
     * @brief Send command to *return to the launch* (takeoff) position and *land*  (asynchronous).
     *
     * This switches the drone into [RTL mode](https://docs.px4.io/en/flight_modes/rtl.html) which
     * generally means it will rise up to a certain altitude to clear any obstacles before heading
     * back to the launch (takeoff) position and land there.
     *
     * @param callback Function to call with result of request.
     */
    void return_to_launch_async(result_callback_t callback);

    /**
     * @brief Send command to transition the drone to fixedwing (asynchronous).
     *
     * The associated action will only be executed for VTOL vehicles (on other vehicle types the
     * command will fail with an ActionResult). The command will succeed if called when the vehicle
     * is already in fixedwing mode.
     *
     * @param callback Function to call with result of request.
     */
    void transition_to_fixedwing_async(result_callback_t callback);

    /**
     * @brief Send command to transition the drone to multicopter (asynchronous).
     *
     * The associated action will only be executed for VTOL vehicles (on other vehicle types the
     * command will fail with an ActionResult). The command will succeed if called when the vehicle
     * is already in multicopter mode.
     *
     * @param callback Function to call with result of request.
     */
    void transition_to_multicopter_async(result_callback_t callback);

    /**
     * @brief Set takeoff altitude above ground.
     *
     * @param relative_altitude_m Takeoff altitude relative to takeoff location, in meters.
     * @return Result of request.
     */
    ActionResult set_takeoff_altitude(float relative_altitude_m);

    /**
     * @brief Get the takeoff altitude.
     *
     * @return A pair containing the result of request and if successful, the
     * takeoff altitude relative to ground/takeoff location, in meters.
     */
    std::pair<ActionResult, float> get_takeoff_altitude() const;

    /**
     * @brief Set vehicle maximum speed.
     *
     * @param speed_m_s Maximum speed in metres/second.
     * @return Result of request.
     */
    ActionResult set_max_speed(float speed_m_s);

    /**
     * @brief Get the vehicle maximum speed.
     *
     * @return A pair containing the result of the request and if successful, the
     * maximum speed in metres/second.
     */
    std::pair<ActionResult, float> get_max_speed() const;

    /**
     * @brief Set the return to launch minimum return altitude.
     *
     * When return to launch is initiated, the vehicle climbs to the return altitude if it is lower
     * and stays at the current altitude if higher than the return altitude. Then it returns to the
     * home location and lands there.
     *
     * @param relative_altitude_m Return altitude relative to takeoff location, in meters.
     * @return Result of request.
     */
    ActionResult set_return_to_launch_return_altitude(float relative_altitude_m);

    /**
     * @brief Get the return to launch minimum return altitude.
     *
     * @sa `set_return_to_launch_return_altitude`.
     *
     * @return A pair containing the result of the request and if successful, the
     * return altitude relative to takeoff location, in meters.
     */
    std::pair<ActionResult, float> get_return_to_launch_return_altitude() const;

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
    std::unique_ptr<ActionImpl> _impl;
};

} // namespace dronecode_sdk
