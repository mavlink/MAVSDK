#pragma once

#include "device.h"

#include <string>
#include <vector>
#include <functional>

namespace dronecore {

class DroneCoreImpl;

/**
 * @brief This is the main class of **%DroneCore MAVLink API Library** (for the Dronecode Platform).

 * It is used to discover vehicles and manage active connections.
 *
 * An instance of this class must be created (first) in order to use the library.
 * The instance must be destroyed after use in order to break connections and release all resources.
 */
class DroneCore
{
public:

    /**
     * @brief Constructor.
     */
    DroneCore();

    /**
     * @brief Destructor.
     *
     * Disconnects all connected vehicles and releases all resources.
     */
    ~DroneCore();

    /**
     * @brief Result type returned when adding a connection.
     *
     * **Note**: DroneCore does not throw exceptions. Instead a result of this type will be
     * returned when you add a connection: add_udp_connection().
     */
    enum class ConnectionResult {
        SUCCESS = 0, /**< @brief %Connection succeeded. */
        TIMEOUT, /**< @brief %Connection timed out. */
        SOCKET_ERROR, /**< @brief Socket error. */
        BIND_ERROR, /**< @brief Bind error. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        NOT_IMPLEMENTED, /**< @brief %Connection type not implemented. */
        DEVICE_NOT_CONNECTED, /**< @brief No device is connected. */
        DEVICE_BUSY, /**< @brief %Device is busy. */
        COMMAND_DENIED, /**< @brief Command is denied. */
        DESTINATION_IP_UNKNOWN, /**< @brief %Connection IP is unknown. */
        CONNECTIONS_EXHAUSTED /**< @brief %Connections exhausted. */
    };

    /**
     * @brief Translates the DroneCore::ConnectionResult enum to a human-readable English string.
     */
    static const char *connection_result_str(ConnectionResult);

    /**
     * @brief Adds a UDP connection with the default arguments.
     *
     * This will listen on UDP port 14540.
     *
     * @return The result of adding the connection.
     */
    ConnectionResult add_udp_connection();

    /**
     * @brief Adds a UDP connection with a specific port number.
     *
     * @param local_port_number The local UDP port to listen to.
     * @return The result of adding the connection.
     */
    ConnectionResult add_udp_connection(int local_port_number);

    /**
     * @brief Get vector of device UUIDs.
     *
     * This returns a vector of the UUIDs of all devices that have been discovered.
     *
     * @return A reference to the vector containing the UUIDs.
     */
    const std::vector<uint64_t> &device_uuids() const;

    /**
     * @brief Get the first discovered device.
     *
     * This returns the first discovered device or a null device if no device has yet been found.
     *
     * @return A reference to a device.
     */
    Device &device() const;

    /**
     * @brief Get the device with the specified UUID.
     *
     * This returns a device for a given UUID if such a device has been discovered and a null
     * device otherwise.
     *
     * @param uuid UUID of device to get.
     * @return A reference to the specified device.
     */
    Device &device(uint64_t uuid) const;

    /**
     * @brief Callback type for discover and timeout notifications.
     *
     * @param uuid UUID of device.
     */
    typedef std::function<void(uint64_t uuid)> event_callback_t;

    /**
     * @brief Register callback for device discovery.
     *
     * This sets a callback that will be notified if a new device is discovered.
     *
     * **Note** Only one callback can be registered at a time. If this function is called several
     * times, previous callbacks will be overwritten.
     *
     * @param callback Callback to register.
     *
     */
    void register_on_discover(event_callback_t callback);

    /**
     * @brief Register callback for device timeout.
     *
     * This sets a callback that will be notified if no heartbeat of the device has been received
     * in 3 seconds.
     *
     * **Note** Only one callback can be registered at a time. If this function is called several
     * times, previous callbacks will be overwritten.
     *
     * @param callback Callback to register.
     */
    void register_on_timeout(event_callback_t callback);

private:
    /** @private. */
    DroneCoreImpl *_impl;

    // Non-copyable
    DroneCore(const DroneCore &) = delete;
    const DroneCore &operator=(const DroneCore &) = delete;
};

} // namespace dronecore
