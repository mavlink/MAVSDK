#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "system.h"

namespace dronecode_sdk {

class DronecodeSDKImpl;
class System;

/**
 * @brief This is the main class of Dronecode SDK (a MAVLink API Library for the Dronecode
 Platform).

 * It is used to discover vehicles and manage active connections.
 *
 * An instance of this class must be created (first) in order to use the library.
 * The instance must be destroyed after use in order to break connections and release all resources.
 */
class DronecodeSDK {
public:
 
    /**
     * @brief Constructor.
     */
    DronecodeSDK();

    /**
     * @brief Destructor.
     *
     * Disconnects all connected vehicles and releases all resources.
     */
    ~DronecodeSDK();
    /**
     * @brief Get vector of system UUIDs.
     *
     * This returns a vector of the UUIDs of all systems that have been discovered.
     * If a system doesn't have a UUID then DronecodeSDK will instead use its MAVLink system ID
     * (range: 0..255).
     *
     * @return A vector containing the UUIDs.
     */
    std::vector<uint64_t> system_uuids() const;

    /**
     * @brief Get the first discovered system.
     *
     * This returns the first discovered system or a null system if no system has yet been found.
     *
     * @return A reference to a system.
     */
    System &system() const;

    /**
     * @brief Get the system with the specified UUID.
     *
     * This returns a system for a given UUID if such a system has been discovered and a null
     * system otherwise.
     *
     * @param uuid UUID of system to get.
     * @return A reference to the specified system.
     */
    System &system(uint64_t uuid) const;

    /**
     * @brief Callback type for discover and timeout notifications.
     *
     * @param uuid UUID of system (or MAVLink system ID for systems that don't have a UUID).
     */
    typedef std::function<void(uint64_t uuid)> event_callback_t;

    /**
     * @brief Returns `true` if exactly one system is currently connected.
     *
     * Connected means we are receiving heartbeats from this system.
     * It means the same as "discovered" and "not timed out".
     *
     * If multiple systems have connected, this will return `false`.
     *
     * @return `true` if exactly one system is connected.
     */
    bool is_connected() const;

    /**
     * @brief Returns `true` if a system is currently connected.
     *
     * Connected means we are receiving heartbeats from this system.
     * It means the same as "discovered" and "not timed out".
     *
     * @param uuid UUID of system to check.
     * @return `true` if system is connected.
     */
    bool is_connected(uint64_t uuid) const;

    /**
     * @brief Register callback for system discovery.
     *
     * This sets a callback that will be notified if a new system is discovered.
     *
     * If systems have been discovered before this callback is registered, they will be notified
     * at the time this callback is registered.
     *
     * @note Only one callback can be registered at a time. If this function is called several
     * times, previous callbacks will be overwritten.
     *
     * @param callback Callback to register.
     *
     */
    void register_on_discover(event_callback_t callback);

    /**
     * @brief Register callback for system timeout.
     *
     * This sets a callback that will be notified if no heartbeat of the system has been received
     * in 3 seconds.
     *
     * @note Only one callback can be registered at a time. If this function is called several
     * times, previous callbacks will be overwritten.
     *
     * @param callback Callback to register.
     */
    void register_on_timeout(event_callback_t callback);

private:
    /* @private. */
    std::unique_ptr<DronecodeSDKImpl> _impl;

    // Non-copyable
    DronecodeSDK(const DronecodeSDK &) = delete;
    const DronecodeSDK &operator=(const DronecodeSDK &) = delete;
};

} // namespace dronecode_sdk
