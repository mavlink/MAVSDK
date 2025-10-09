#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <functional>

#include "deprecated.h"
#include "handle.h"
#include "system.h"
#include "component_type.h"
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
 * An instance of this class must be created and kept alive in order to use the library.
 * The instance can be destroyed after use in order to break connections and release all resources.
 */
class Mavsdk {
public:
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
     *
     * - UDP in  (server): udpin://our_ip:port
     * - UDP out (client): udpout://remote_ip:port
     *
     * - TCP in  (server):  tcpin://our_ip:port
     * - TCP out (client): tcpout://remote_ip:port
     *
     * - Serial: serial://dev_node:baudrate
     * - Serial with flow control: serial_flowcontrol://dev_node:baudrate
     *
     * For UDP in and TCP in (as server), our IP can be set to:
     *   - 0.0.0.0: listen on all interfaces
     *   - 127.0.0.1: listen on loopback (local) interface only
     *   - Our IP: (e.g. 192.168.1.12): listen only on the network interface
     *             with this IP.
     *
     * For UDP out and TCP out, the IP needs to be set to the remote IP,
     * where the MAVLink messages are to be sent to.
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
     *
     * - UDP in  (server): udpin://our_ip:port
     * - UDP out (client): udpout://remote_ip:port
     *
     * - TCP in  (server):  tcpin://our_ip:port
     * - TCP out (client): tcpout://remote_ip:port
     *
     * - Serial: serial://dev_node:baudrate
     * - Serial with flow control: serial_flowcontrol://dev_node:baudrate
     *
     * For UDP in and TCP in (as server), our IP can be set to:
     *   - 0.0.0.0: listen on all interfaces
     *   - 127.0.0.1: listen on loopback (local) interface only
     *   - Our IP: (e.g. 192.168.1.12): listen only on the network interface
     *             with this IP.
     *
     * For UDP out and TCP out, the IP needs to be set to the remote IP,
     * where the MAVLink messages are to be sent to.
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
     * Remove connection again.
     *
     * @param handle Handle returned when connection was added.
     */
    void remove_connection(ConnectionHandle handle);

    /**
     * ConnectionError type
     */
    struct ConnectionError {
        std::string error_description; ///< The error description
        ConnectionHandle connection_handle; ///< The connection handle
    };

    /**
     * Connection Error callback type
     */
    using ConnectionErrorCallback = std::function<void(ConnectionError)>;

    /**
     * @brief Handle type to remove a connection error subscription.
     */
    using ConnectionErrorHandle = Handle<ConnectionError>;

    /**
     * Subscribe to connection errors.
     *
     * This will trigger when messages fail to be sent which can help
     * diagnosing network interfaces or serial devices disappearing.
     *
     * Usually, an error will require to remove a connection and add it fresh.
     *
     * @param callback Callback to subscribe.
     * @return Handle to unsubscribe again.
     */
    ConnectionErrorHandle subscribe_connection_errors(ConnectionErrorCallback callback);

    /**
     * Unsubscribe from connection errors.
     *
     * @param handle Handle to unsubscribe.
     */
    void unsubscribe_connection_errors(ConnectionErrorHandle handle);

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
         * @brief Create new Configuration via manually configured
         * system and component ID.
         * @param system_id the system id to store in this configuration
         * @param component_id the component id to store in this configuration
         * @param always_send_heartbeats send heartbeats by default even without a system connected
         */
        explicit Configuration(
            uint8_t system_id, uint8_t component_id, bool always_send_heartbeats);
        /**
         * @brief Create new Configuration using a component type.
         * In this mode, the system and component ID will be automatically chosen.
         * @param component_type the component type, used for automatically choosing ids.
         */
        explicit Configuration(ComponentType component_type);

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

        /** @brief Component type of this configuration, used for automatic ID set */
        ComponentType get_component_type() const;

        /**
         * @brief Set the component type of this configuration.
         */
        void set_component_type(ComponentType component_type);

        /**
         * @brief Get the mav type (vehicle type) of this configuration
         * @return `uint8_t` the mav type stored in this configuration
         */
        uint8_t get_mav_type() const;

        /**
         * @brief Set the mav type (vehicle type) of this configuration.
         */
        void set_mav_type(uint8_t mav_type);

    private:
        uint8_t _system_id;
        uint8_t _component_id;
        bool _always_send_heartbeats;
        ComponentType _component_type;
        MAV_TYPE _mav_type;

        static ComponentType component_type_for_component_id(uint8_t component_id);
        static MAV_TYPE mav_type_for_component_type(ComponentType component_type);
    };

    /**
     * @brief Default constructor without configuration, no longer recommended.
     *
     * @note This has been removed because MAVSDK used to identify itself as a
     *       ground station by default which isn't always the safest choice.
     *       For instance, when MAVSDK is used on a companion computer (set as
     *       a ground station) it means that the appropriate failsafe doesn't
     *       trigger.
     */
    Mavsdk() = delete;

    /**
     * @brief Constructor with configuration.
     *
     * @param configuration Configuration to use in MAVSDK instance.
     */
    Mavsdk(Configuration configuration);

    /**
     * @brief Destructor.
     *
     * Disconnects all connected vehicles and releases all resources.
     */
    ~Mavsdk();

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
     * @brief Get server component with default type of Mavsdk instance.
     *
     * @return A valid shared pointer to a server component if it was successful, an empty pointer
     * otherwise.
     */
    std::shared_ptr<ServerComponent> server_component(unsigned instance = 0);

    /**
     * @brief Get server component by a high level type.
     *
     * This represents a server component of the MAVSDK instance.
     *
     * @param component_type The high level type of the component.
     * @param instance The instance of the component if there are multiple, starting at 0.
     *
     * @return A valid shared pointer to a server component if it was successful, an empty pointer
     * otherwise.
     */
    std::shared_ptr<ServerComponent>
    server_component_by_type(ComponentType component_type, unsigned instance = 0);

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
     * @brief A complete MAVLink message with all header information and fields
     */
    struct MavlinkMessage {
        std::string message_name{}; /**< @brief MAVLink message name (e.g., "HEARTBEAT",
                                       "GLOBAL_POSITION_INT") */
        uint32_t system_id{}; /**< @brief System ID of the sender (for received messages) */
        uint32_t component_id{}; /**< @brief Component ID of the sender (for received messages) */
        uint32_t target_system_id{}; /**< @brief Target system ID (for sending, 0 for broadcast) */
        uint32_t
            target_component_id{}; /**< @brief Target component ID (for sending, 0 for broadcast) */
        std::string fields_json{}; /**< @brief All message fields as single JSON object */
    };

    /**
     * @brief Handle for intercepting messages.
     */
    using InterceptJsonHandle = Handle<bool(MavlinkMessage)>;

    /**
     * @brief Callback type for intercepting messages.
     */
    using InterceptJsonCallback = std::function<bool(MavlinkMessage)>;

    /**
     * @brief Intercept incoming messages as JSON.
     *
     * This is a hook that allows to read any messages arriving via the
     * in JSON format.
     *
     * @param callback Callback to be called for each incoming message.
     *        To drop a message, return 'false' from the callback.
     */
    InterceptJsonHandle subscribe_incoming_messages_json(const InterceptJsonCallback& callback);

    /**
     * @brief Unsubscribe from incoming messages as JSON
     */
    void unsubscribe_incoming_messages_json(InterceptJsonHandle handle);

    /**3
     * @brief Intercept outgoing messages as JSON.
     *
     * This is a hook that allows to read any messages arriving via the
     * in JSON format.
     *
     * @param callback Callback to be called for each outgoing message.
     *        To drop a message, return 'false' from the callback.
     */
    InterceptJsonHandle subscribe_outgoing_messages_json(const InterceptJsonCallback& callback);

    /**
     * @brief Unsubscribe from outgoing messages as JSON
     */
    void unsubscribe_outgoing_messages_json(InterceptJsonHandle handle);

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

    /**
     * @brief Callback type for raw bytes subscriptions.
     */
    using RawBytesCallback = std::function<void(const char* bytes, size_t length)>;

    /**
     * @brief Handle type for raw bytes subscriptions.
     */
    using RawBytesHandle = Handle<const char*, size_t>;

    /**
     * @brief Pass received raw MAVLink bytes.
     *
     * This allows passing raw MAVLink message bytes into MAVSDK to be processel.
     * The bytes can contain one or more MAVLink messages.
     *
     * @note Before using this, run add_any_connection("raw://")
     *
     * This goes together with subscribe_to_bytes_to_send.
     *
     * @param bytes Pointer to raw MAVLink message bytes.
     * @param length Number of bytes to send.
     */
    void pass_received_raw_bytes(const char* bytes, size_t length);

    /**
     * @brief Subscribe to raw bytes to be sent.
     *
     * This allows getting MAVLink bytes that need to be sent out.
     *
     * @note Before using this, run add_any_connection("raw://")
     *
     * This goes together with pass_received_raw_bytes.
     * The bytes contain one mavlink message at a time.
     *
     * @param callback Callback to be called with outgoing raw bytes.
     * @return Handle to unsubscribe again.
     */
    RawBytesHandle subscribe_raw_bytes_to_be_sent(RawBytesCallback callback);

    /**
     * @brief Unsubscribe from raw bytes to be sent.
     *
     * @param handle Handle from subscribe_raw_bytes_to_be_sent.
     */
    void unsubscribe_raw_bytes_to_be_sent(RawBytesHandle handle);

private:
    static constexpr int DEFAULT_SYSTEM_ID_AUTOPILOT = 1;
    static constexpr int DEFAULT_COMPONENT_ID_AUTOPILOT = MAV_COMP_ID_AUTOPILOT1;
    static constexpr int DEFAULT_SYSTEM_ID_GCS = 245;
    static constexpr int DEFAULT_COMPONENT_ID_GCS = MAV_COMP_ID_MISSIONPLANNER;
    static constexpr int DEFAULT_SYSTEM_ID_CC = 1;
    static constexpr int DEFAULT_COMPONENT_ID_CC = MAV_COMP_ID_PATHPLANNER;
    static constexpr int DEFAULT_SYSTEM_ID_CAMERA = 1;
    static constexpr int DEFAULT_COMPONENT_ID_CAMERA = MAV_COMP_ID_CAMERA;
    static constexpr int DEFAULT_SYSTEM_ID_GIMBAL = 1;
    static constexpr int DEFAULT_COMPONENT_ID_GIMBAL = MAV_COMP_ID_GIMBAL;
    static constexpr int DEFAULT_SYSTEM_ID_REMOTEID = 1;
    static constexpr int DEFAULT_COMPONENT_ID_REMOTEID = MAV_COMP_ID_ODID_TXRX_1;

    /* @private. */
    std::shared_ptr<MavsdkImpl> _impl{};

    friend ServerPluginImplBase;

    // Non-copyable
    Mavsdk(const Mavsdk&) = delete;
    const Mavsdk& operator=(const Mavsdk&) = delete;
};

} // namespace mavsdk
