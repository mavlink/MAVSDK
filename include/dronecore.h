#pragma once

#include "device.h"

#include <string>
#include <vector>
#include <functional>

namespace dronecore {

class DroneCoreImpl;

/**
 * DroneCore is an API and library for the PX4 flight stack using Mavlink.
 *
 * It is written in C++ and aiming to be:
 *
 * - Easy to use with a simple API.
 * - Fast and lightweight.
 * - Cross-platform (Linux, Mac, Windows, iOS, Android).
 * - Extensible (using compile-time plugins).
 *
 */
class DroneCore
{
public:

    /**
     * Constructor for the main DroneCore class
     *
     * In order to use DroneCore, an instance of this class needs to be created first.
     */
    DroneCore();

    /**
     * Destructor of the main DroneCore class
     *
     * In order to disconnect everything and release all resources, a DroneCore instance should be
     * destroyed.
     */
    ~DroneCore();

    /**
     * Result returned when adding a connection.
     *
     * DroneCore does not throw exceptions, instead a result will be returned.
     */
    enum class ConnectionResult {
        SUCCESS = 0,
        TIMEOUT,
        SOCKET_ERROR,
        BIND_ERROR,
        CONNECTION_ERROR,
        NOT_IMPLEMENTED,
        DEVICE_NOT_CONNECTED,
        DEVICE_BUSY,
        COMMAND_DENIED,
        DESTINATION_IP_UNKNOWN,
        CONNECTIONS_EXHAUSTED
    };

    /**
     * Translates the ConnectionResult enum to a human-readable English string.
     */
    static const char *connection_result_str(ConnectionResult);

    /**
     * Adds a UDP connection with the default arguments.
     *
     * This will listen on UDP port 14540 on localhost.
     *
     * @return the result of adding the connection.
     */
    ConnectionResult add_udp_connection();

    /**
     * Adds a UDP connection with a specific port number.
     *
     * @param local_port_number The local UDP port to listen to
     * @return the result of adding the connection.
     */
    ConnectionResult add_udp_connection(int local_port_number);

    /**
     * Return vector of device UUIDs.
     *
     * This returns a vector of the UUIS of all devices that have been discovered so far and can be
     * accessed.
     *
     * @return a reference to the vector containing the UUIDs
     */
    const std::vector<uint64_t> &device_uuids() const;

    /**
     * Return the only discovered device.
     *
     * This returns the only discovered device or a null device if no device has been discovered
     * yet. If there is more than one device discovered, it will just return the device which was
     * discovered first.
     *
     * @return a reference to a device
     */
    Device &device() const;

    /**
     * Return a device given a UUID.
     *
     * This returns a device for a given UUID if such a device has been discovered and a null
     * device otherwise.
     */
    Device &device(uint64_t uuid) const;

    /**
     * Callback type for discover and timeout notifications.
     *
     * @param uuid UUID of device
     */
    typedef std::function<void(uint64_t uuid)> event_callback_t;

    /**
     * Register callback for device discovery.
     *
     * This sets a callback that will be notified if a new device is discovered.
     *
     * Note that only one callback can be registered at a time. If this function is called several
     * times, previous callbacks will be overwritten.
     */
    void register_on_discover(event_callback_t callback);

    /**
     * Register callback for device timeout.
     *
     * This sets a callback that will be notified if  device has timed out which means that no
     * more messages have been received in 3 seconds.
     *
     * Note that only one callback can be registered at a time. If this function is called several
     * times, previous callbacks will be overwritten.
     */
    void register_on_timeout(event_callback_t callback);

private:
    DroneCoreImpl *_impl;

    // Non-copyable
    DroneCore(const DroneCore &) = delete;
    const DroneCore &operator=(const DroneCore &) = delete;
};

} // namespace dronecore
