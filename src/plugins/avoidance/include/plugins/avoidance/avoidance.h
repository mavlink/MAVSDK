#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "plugin_base.h"

namespace mavsdk {

class AvoidanceImpl;
class System;

/**
 * @brief The Avoidance class creates an interface for automatic-mode obstacle avoidance
 */
class Avoidance : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto avoidance = std::make_shared<Avoidance>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Avoidance(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Avoidance();

    /**
     * @brief Possible results returned for avoidance requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
    };

    /**
     * @brief Gets a human-readable English string for an Avoidance::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the Avoidance::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for trajectory (waypoints format) updates.
     */
    typedef std::function<void(WaypointsTrajectory)>
        trajectory_representation_waypoints_callback_t;

    /**
     * @brief Subscribe to trajectory (waypoints format) updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void receive_trajectory_representation_waypoints_async(
        trajectory_representation_waypoints_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Avoidance(const Avoidance&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Avoidance& operator=(const Avoidance&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<AvoidanceImpl> _impl;
};

} // namespace mavsdk
