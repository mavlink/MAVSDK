#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <functional>

#include "deprecated.h"
#include "handle.h"
#include "system.h"
#include "server_component.h"
#include "connection_result.h"
#include "mavlink_include.h"

namespace mavsdk {

class ServerPluginImplBase;

/**
 * @brief ForwardingOption for Connection, used to set message forwarding option.
 */
enum class ForwardingOption {
    ForwardingOff = 0,
    ForwardingOn = 1,
};

class MavsdkImpl;

/**
 * @brief This is the main class of MAVSDK (a MAVLink API Library).

 * It is used to discover vehicles and manage active connections.
 *
 * An instance of this class must be created (first) in order to use the library.
 * The instance must be destroyed after use in order to break connections and release all resources.
 */
class Mavsdk {
public:
    /** @brief Default UDP bind IP (accepts any incoming connections). */
    static constexpr auto DEFAULT_UDP_BIND_IP = "0.0.0.0";
    /** @brief Default UDP bind port (same port as used by MAVROS). */
    static constexpr int DEFAULT_UDP_PORT = 14540;
    /** @brief Default TCP remote IP (localhost). */
    static constexpr auto DEFAULT_TCP_REMOTE_IP = "127.0.0.1";
    /** @brief Default TCP remote port. */
    static constexpr int DEFAULT_TCP_REMOTE_PORT = 5760;
    /** @brief Default serial baudrate. */
    static constexpr int DEFAULT_SERIAL_BAUDRATE = 57600;

    /** @brief Default internal timeout in seconds. */
    static constexpr double DEFAULT_TIMEOUT_S = 0.5;

    /**
     * @brief Constructor.
     */
    Mavsdk();

    /**
     * @brief Destructor.
     *
     * Disconnects all connected vehicles and releases all resources.
     */
    ~Mavsdk();

    /**
     * @brief Returns the version of MAVSDK.
     *
     * Note, you're not supposed to request the version too many times.
     *
     * @return A string containing the version.
     */
    std::string version() const;

    /**
     * @brief Adds Connection via URL
     *
     * Supports connection: Serial, TCP or UDP.
     * Connection URL format should be:
     * - UDP:    udp://[host][:bind_port]
     * - TCP:    tcp://[host][:remote_port]
     * - Serial: serial://dev_node[:baudrate]
     *
     * For UDP, the host can be set to either:
     *   - zero IP: 0.0.0.0 -> behave like a server and listen for heartbeats.
     *   - some IP: 192.168.1.12 -> behave like a client, initiate connection
     *     and start sending heartbeats.
     *
     * @param connection_url connection URL string.
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return The result of adding the connection.
     */
    ConnectionResult add_any_connection(
        const std::string& connection_url,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * @brief Handle type to remove a connection.
     */
    using ConnectionHandle = Handle<>;

    /**
     * @brief Adds Connection via URL Additionally returns a handle to remove
     *        the connection later.
     *
     * Supports connection: Serial, TCP or UDP.
     * Connection URL format should be:
     * - UDP:    udp://[host][:bind_port]
     * - TCP:    tcp://[host][:remote_port]
     * - Serial: serial://dev_node[:baudrate]
     *
     * For UDP, the host can be set to either:
     *   - zero IP: 0.0.0.0 -> behave like a server and listen for heartbeats.
     *   - some IP: 192.168.1.12 -> behave like a client, initiate connection
     *     and start sending heartbeats.
     *
     * @param connection_url connection URL string.
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return A pair containing the result of adding the connection as well
     *         as a handle to remove it later.
     */
    std::pair<ConnectionResult, ConnectionHandle> add_any_connection_with_handle(
        const std::string& connection_url,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * @brief Adds a UDP connection to the specified port number.
     *
     * Any incoming connections are accepted (0.0.0.0).
     *
     * @param local_port The local UDP port to listen to (defaults to 14540, the same as MAVROS).
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return The result of adding the connection.
     */
    ConnectionResult add_udp_connection(
        int local_port = DEFAULT_UDP_PORT,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * @brief Adds a UDP connection to the specified port number and local interface.
     *
     * To accept only local connections of the machine, use 127.0.0.1.
     * For any incoming connections, use 0.0.0.0.
     *
     * @param local_ip The local UDP IP address to listen to.
     * @param local_port The local UDP port to listen to (defaults to 14540, the same as MAVROS).
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return The result of adding the connection.
     */
    ConnectionResult add_udp_connection(
        const std::string& local_ip,
        int local_port = DEFAULT_UDP_PORT,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * @brief Sets up instance to send heartbeats to the specified remote interface and port number.
     *
     * @param remote_ip The remote UDP IP address to report to.
     * @param remote_port The local UDP port to report to.
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return The result of operation.
     */
    ConnectionResult setup_udp_remote(
        const std::string& remote_ip,
        int remote_port,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * @brief Adds a TCP connection with a specific IP address and port number.
     *
     * @param remote_ip Remote IP address to connect to.
     * @param remote_port The TCP port to connect to (defaults to 5760).
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return The result of adding the connection.
     */
    ConnectionResult add_tcp_connection(
        const std::string& remote_ip,
        int remote_port = DEFAULT_TCP_REMOTE_PORT,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * @brief Adds a serial connection with a specific port (COM or UART dev node) and baudrate as
     * specified.
     *
     *
     * @param dev_path COM or UART dev node name/path (e.g. "/dev/ttyS0", or "COM3" on Windows).
     * @param baudrate Baudrate of the serial port (defaults to 57600).
     * @param flow_control enable/disable flow control.
     * @param forwarding_option message forwarding option (when multiple interfaces are used).
     * @return The result of adding the connection.
     */
    ConnectionResult add_serial_connection(
        const std::string& dev_path,
        int baudrate = DEFAULT_SERIAL_BAUDRATE,
        bool flow_control = false,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);

    /**
     * Remove connection again.
     *
     * @param handle Handle returned when connection was added.
     */
    void remove_connection(ConnectionHandle handle);

    /**
     * @brief Get a vector of systems which have been discovered or set-up.
     *
     * @return The vector of systems which are available.
     */
    std::vector<std::shared_ptr<System>> systems() const;

    /**
     * @brief Get the first autopilot that has been discovered.
     *
     * @note This requires a MAVLink component with component ID 1 sending
     *       heartbeats.
     *
     * @param timeout_s A timeout in seconds.
     *                  A timeout of 0 will not wait and return immediately.
     *                  A negative timeout will wait forever.
     *
     * @return A system or nothing if nothing was discovered within the timeout.
     */
    std::optional<std::shared_ptr<System>> first_autopilot(double timeout_s) const;

    /**
     * @brief Possible configurations.
     */
    class Configuration {
    public:
        /**
         * @brief UsageTypes of configurations, used for automatic ID setting
         */
        enum class UsageType {
            Autopilot, /**< @brief SDK is used as an autopilot. */
            GroundStation, /**< @brief SDK is used as a ground station. */
            CompanionComputer, /**< @brief SDK is used as a companion computer on board the MAV. */
            Camera, /** < @brief SDK is used as a camera. */
            Custom /**< @brief the SDK is used in a custom configuration, no automatic ID will be
                      provided */
        };

        /**
         * @brief Create new Configuration via manually configured
         * system and component ID.
         * @param system_id the system id to store in this configuration
         * @param component_id the component id to store in this configuration
         * @param always_send_heartbeats send heartbeats by default even without a system connected
         */
        explicit Configuration(
            uint8_t system_id, uint8_t component_id, bool always_send_heartbeats);
        /**
         * @brief Create new Configuration using a usage type.
         * In this mode, the system and component ID will be automatically chosen.
         * @param usage_type the usage type, used for automatically choosing ids.
         */
        explicit Configuration(UsageType usage_type);

        Configuration() = delete;
        ~Configuration() = default;

        /**
         * @brief Get the system id of this configuration
         * @return `uint8_t` the system id stored in this configuration, from 1-255
         */
        uint8_t get_system_id() const;

        /**
         * @brief Set the system id of this configuration.
         */
        void set_system_id(uint8_t system_id);

        /**
         * @brief Get the component id of this configuration
         * @return `uint8_t` the component id stored in this configuration,from 1-255
         */
        uint8_t get_component_id() const;

        /**
         * @brief Set the component id of this configuration.
         */
        void set_component_id(uint8_t component_id);

        /**
         * @brief Get whether to send heartbeats by default.
         * @return whether to always send heartbeats
         */
        bool get_always_send_heartbeats() const;

        /**
         * @brief Set whether to send heartbeats by default.
         */
        void set_always_send_heartbeats(bool always_send_heartbeats);

        /** @brief Usage type of this configuration, used for automatic ID set */
        UsageType get_usage_type() const;

        /**
         * @brief Set the usage type of this configuration.
         */
        void set_usage_type(UsageType usage_type);

    private:
        uint8_t _system_id;
        uint8_t _component_id;
        bool _always_send_heartbeats;
        UsageType _usage_type;

        static Mavsdk::Configuration::UsageType usage_type_for_component(uint8_t component_id);
    };

    /**
     * @brief Set `Configuration` of SDK.
     *
     * The default configuration is `Configuration::GroundStation`
     * The configuration is used in order to set the MAVLink system ID, the
     * component ID, as well as the MAV_TYPE accordingly.
     *
     * @param configuration Configuration chosen.
     */
    void set_configuration(Configuration configuration);

    /**
     * @brief Set timeout of MAVLink transfers.
     *
     * The default timeout used is generally DEFAULT_SERIAL_BAUDRATE (0.5 seconds) seconds.
     * If MAVSDK is used on the same host this timeout can be reduced, while
     * if MAVSDK has to communicate over links with high latency it might
     * need to be increased to prevent timeouts.
     */
    void set_timeout_s(double timeout_s);

    /**
     * @brief Set system status of this MAVLink entity.
     *
     * The default system status is MAV_STATE_UNINIT.
     *
     * @param system_status system status.
     */
    void set_system_status(uint8_t system_status);

    /**
     * @brief Callback type discover and timeout notifications.
     */
    using NewSystemCallback = std::function<void()>;

    /**
     * @brief Handle type to unsubscribe from subscribe_on_new_system.
     */
    using NewSystemHandle = Handle<>;

    /**
     * @brief Get notification about a change in systems.
     *
     * This gets called whenever a system is added.
     *
     * @param callback Callback to subscribe.
     *
     * @return A handle to unsubscribe again.
     */
    NewSystemHandle subscribe_on_new_system(const NewSystemCallback& callback);

    /**
     * @brief unsubscribe from subscribe_on_new_system.
     *
     * @param handle Handle received on subscription.
     */
    void unsubscribe_on_new_system(NewSystemHandle handle);

    /**
     * @brief High level type of a server component.
     */
    enum class ServerComponentType {
        Autopilot, /**< @brief The component identifies as an autopilot. */
        GroundStation, /**< @brief The component identifies as a ground station. */
        CompanionComputer, /**< @brief The component identifies as a companion computer on board the
                              system. */
        Camera, /** < @brief The component identifies as a camera. */
    };

    /**
     * @brief Get server component by a high level type.
     *
     * This represents a server component of the MAVSDK instance.
     *
     * @param server_component_type The high level type of the component.
     * @param instance The instance of the component if there are multiple, starting at 0.
     *
     * @return A valid shared pointer to a server component if it was successful, an empty pointer
     * otherwise.
     */
    std::shared_ptr<ServerComponent>
    server_component_by_type(ServerComponentType server_component_type, unsigned instance = 0);

    /**
     * @brief Get server component by the low MAVLink component ID.
     *
     * This represents a server component of the MAVSDK instance.
     *
     * @param component_id MAVLink component ID to use
     *
     * @return A valid shared pointer to a server component if it was successful, an empty pointer
     * otherwise.
     */
    std::shared_ptr<ServerComponent> server_component_by_id(uint8_t component_id);

    /**
     * @brief Intercept incoming messages.
     *
     * This is a hook which allows to change or drop MAVLink messages as they
     * are received before they get forwarded any subscribers.
     *
     * @note This functionality is provided primarily for testing in order to
     * simulate packet drops or actors not adhering to the MAVLink protocols.
     *
     * @param callback Callback to be called for each incoming message.
     *        To drop a message, return 'false' from the callback.
     */
    void intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback);

    /**
     * @brief Intercept outgoing messages.
     *
     * This is a hook which allows to change or drop MAVLink messages before
     * they are sent.
     *
     * @note This functionality is provided primarily for testing in order to
     * simulate packet drops or actors not adhering to the MAVLink protocols.
     *
     * @param callback Callback to be called for each outgoing message.
     *        To drop a message, return 'false' from the callback.
     */
    void intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback);

private:
    /* @private. */
    std::shared_ptr<MavsdkImpl> _impl{};

    friend ServerPluginImplBase;

    // Non-copyable
    Mavsdk(const Mavsdk&) = delete;
    const Mavsdk& operator=(const Mavsdk&) = delete;
};

} // namespace mavsdk
