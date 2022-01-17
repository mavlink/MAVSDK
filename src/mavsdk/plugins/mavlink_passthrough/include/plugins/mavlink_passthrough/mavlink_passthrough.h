#pragma once

#include <memory>
#include <string>
#include <functional>

// This plugin provides/includes the mavlink 2.0 header files.
#include "mavlink_include.h"
#include "mavsdk/plugin_base.h"

namespace mavsdk {

class System;
class MavlinkPassthroughImpl;

/**
 * @brief The MavlinkPassthrough class provides direct MAVLink access
 *
 * "With great power comes great responsibility." - This plugin allows
 * you to send and receive MAVLink messages. There is no checking or
 * safe-guards, you're on your own, and you have been warned.
 */
class MavlinkPassthrough : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mavlink_passthrough = MavlinkPassthrough(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit MavlinkPassthrough(System& system); // deprecated

    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mavlink_passthrough = MavlinkPassthrough(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit MavlinkPassthrough(std::shared_ptr<System> system); // new

    /**
     * @brief Destructor (internal use only).
     */
    ~MavlinkPassthrough();

    /**
     * @brief Possible results returned for requests.
     */
    enum class Result {
        Unknown, /**< @brief Unknown error. */
        Success, /**< @brief Success. */
        ConnectionError, /**< @brief Connection error. */
        CommandNoSystem, /**< @brief System not available. */
        CommandBusy, /**< @brief System is busy. */
        CommandDenied, /**< @brief Command has been denied. */
        CommandUnsupported, /**< @brief Command is not supported. */
        CommandTimeout, /**< @brief A timeout happened. */
    };

    /**
     * @brief Stream operator to print information about a `MavlinkPassthrough::Result`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream& operator<<(std::ostream& str, MavlinkPassthrough::Result const& result);

    /**
     * @brief Send message.
     *
     * @return result of the request.
     */
    Result send_message(mavlink_message_t& message);

    /**
     * @brief Type for MAVLink command_long.
     */
    struct CommandLong {
        uint8_t target_sysid; /**< @brief System ID to send command to */
        uint8_t target_compid; /**< @brief Component ID to send command to */
        uint16_t command; /**< @brief command to send. */
        float param1; /**< @brief param1. */
        float param2; /**< @brief param2. */
        float param3; /**< @brief param3. */
        float param4; /**< @brief param4. */
        float param5; /**< @brief param5. */
        float param6; /**< @brief param6. */
        float param7; /**< @brief param7. */
    };

    /**
     * @brief Type for MAVLink command_int.
     */
    struct CommandInt {
        uint8_t target_sysid; /**< @brief System ID to send command to */
        uint8_t target_compid; /**< @brief Component ID to send command to */
        uint16_t command; /**< @brief command to send. */
        MAV_FRAME frame; /**< Frame of command. */
        float param1; /**< @brief param1. */
        float param2; /**< @brief param2. */
        float param3; /**< @brief param3. */
        float param4; /**< @brief param4. */
        int32_t x; /**< @brief x. */
        int32_t y; /**< @brief y. */
        float z; /**< @brief z. */
    };

    /**
     * @brief Send a MAVLink command_long.
     *
     * @param command Command to send.
     *
     * @return result of the request.
     */
    Result send_command_long(const CommandLong& command);

    /**
     * @brief Send a MAVLink command_long.
     *
     * @param command Command to send.
     *
     * @return result of the request.
     */
    Result send_command_int(const CommandInt& command);

    /**
     * @brief Subscribe to messages using message ID.
     *
     * This means that all future messages being received will trigger the
     * callback to be called. To stop the subscription, call this method with
     * `nullptr` as the argument.
     *
     * @param message_id The MAVLink message ID.
     * @param callback Callback to be called for message subscription.
     */
    void subscribe_message_async(
        uint16_t message_id, std::function<void(const mavlink_message_t&)> callback);

    /**
     * @brief Get our own system ID.
     *
     * @return our own system ID.
     */
    uint8_t get_our_sysid() const;

    /**
     * @brief Get our own component ID.
     *
     * @return our own component ID.
     */
    uint8_t get_our_compid() const;

    /**
     * @brief Get system ID of target.
     *
     * @return system ID of target.
     */
    uint8_t get_target_sysid() const;

    /**
     * @brief Get target component ID.
     *
     * This defaults to the component ID of the autopilot (1) if available
     * and otherwise to all components (0).
     *
     * @return component ID of target.
     */
    uint8_t get_target_compid() const;

    /**
     * @brief Intercept incoming messages.
     *
     * This is a hook which allows to change or drop MAVLink messages as they
     * are received before they get forwarded to the core and the other plugins.
     *
     * @note This functioniality is provided primarily for testing in order to
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
     * @note This functioniality is provided primarily for testing in order to
     * simulate packet drops or actors not adhering to the MAVLink protocols.
     *
     * @param callback Callback to be called for each outgoing message.
     *        To drop a message, return 'false' from the callback.
     */
    void intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback);

    /**
     * @brief Copy Constructor (object is not copyable).
     */
    MavlinkPassthrough(const MavlinkPassthrough&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const MavlinkPassthrough& operator=(const MavlinkPassthrough&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MavlinkPassthroughImpl> _impl;
};

} // namespace mavsdk
