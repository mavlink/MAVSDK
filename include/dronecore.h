#pragma once

#include <string>
#include <vector>
#include <functional>

#include "device.h"

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
        SOCKET_CONNECTION_ERROR,
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
     * @brief Adds a UDP connection to the default port.
     *
     * This will listen on UDP port 14540.
     *
     * @return The result of adding the connection.
     */
    ConnectionResult add_udp_connection();

    /**
     * @brief Adds a UDP connection to the specified port number.
     *
     * @param local_port_number The local UDP port to listen to.
     * @return The result of adding the connection.
     */
    ConnectionResult add_udp_connection(int local_port_number);

    /**
     * @brief Adds a TCP connection to the default IP address/port.
     *
     * This will connect to local TCP port 5760.
     *
     * @return The result of adding the connection.
     */
    ConnectionResult add_tcp_connection();

    /**
     * @brief Adds a TCP connection with a specific IP address and port number.
     *
     * @param remote_ip Remote IP address to connect to.
     * @param remote_port The TCP port to connect to.
     * @return The result of adding the connection.
     */
    ConnectionResult add_tcp_connection(std::string remote_ip, int remote_port);

    /**
     * @brief Adds a serial connection with the default arguments.
     *
     * This will connect to serial port ttyS1 (COM0).
     *
     * @return The result of adding the connection.
     */
    ConnectionResult add_serial_connection();

    /**
     * @brief Adds a serial connection with a specific port (COM or UART dev node) and baudrate as specified.
     *
     * @param dev_path COM or UART dev node name/path.
     * @param baudrate Baudrate of the serial port.
     * @return The result of adding the connection.
     */
    ConnectionResult add_serial_connection(std::string dev_path, int baudrate);

    /**
     * @brief Get vector of device UUIDs.
     *
     * This returns a vector of the UUIDs of all devices that have been discovered.
     * If a device doesn't have a UUID then DroneCore will instead use its MAVLink system ID (range: 0..255).
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
    DeviceImpl &device() const;

    /**
     * @brief Get the device with the specified UUID.
     *
     * This returns a device for a given UUID if such a device has been discovered and a null
     * device otherwise.
     *
     * @param uuid UUID of device to get.
     * @return A reference to the specified device.
     */
    DeviceImpl &device(uint64_t uuid) const;

    /**
     * @brief Callback type for discover and timeout notifications.
     *
     * @param uuid UUID of device (or MAVLink system ID for devices that don't have a UUID).
     */
    typedef std::function<void(uint64_t uuid)> event_callback_t;

    /**
     * @brief Returns `true` if exactly one device is currently connected.
     *
     * Connected means we are receiving heartbeats from this device.
     * It means the same as "discovered" and "not timed out".
     *
     * If multiple devices have connected, this will return `false`.
     *
     * @return `true` if exactly one device is connected.
     */
    bool is_connected() const;

    /**
     * @brief Returns `true` if a device is currently connected.
     *
     * Connected means we are receiving heartbeats from this device.
     * It means the same as "discovered" and "not timed out".
     *
     * @param uuid UUID of device to check.
     * @return `true` if device is connected.
     */
    bool is_connected(uint64_t uuid) const;

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
    /* @private. */
    DroneCoreImpl *_impl;

    // Non-copyable
    DroneCore(const DroneCore &) = delete;
    const DroneCore &operator=(const DroneCore &) = delete;
};

} // namespace dronecore
