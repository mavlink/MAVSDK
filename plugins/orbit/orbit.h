#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <cmath>
#include <functional>
#include "plugin_base.h"

namespace dronecore {

class OrbitImpl;
class System;

/**
 * @brief Orbit functionality for multicopter.
 */
class Orbit : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto orbit = std::make_shared<Orbit>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Orbit(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Orbit();

    /**
     * @brief Results of Orbit operations.
     * @sa result_str()
     */
    enum class Result {
        SUCCESS = 0, /**< @brief Request succeeded. */
        NO_SYSTEM, /**< @brief No system connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief Vehicle busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief Request timeout. */
        NOT_ACTIVE, /**< @brief Orbit is not activated. */
        SET_CONFIG_FAILED, /**< @brief Failed to set Orbit configuration. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief Returns English string for Orbit error codes
     *
     * @param[in] result #Result code.
     * @return Returns std::string describing error code.
     */
    static std::string result_str(Result result);

    /**
     * @brief Starts Orbit mode.
     * @return Orbit::Result::SUCCESS if succeeded, error otherwise.
     */
    Orbit::Result start() const;

    /**
     * @brief Stops Orbit mode.
     * @return Orbit::Result::SUCCESS if succeeded, error otherwise. See Orbit::Result for
     * error codes.
     */
    Orbit::Result stop() const;

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Orbit(const Orbit &) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Orbit &operator=(const Orbit &) = delete;

private:
    // Underlying implementation, set at instantiation
    std::unique_ptr<OrbitImpl> _impl;
};

} // namespace dronecore
