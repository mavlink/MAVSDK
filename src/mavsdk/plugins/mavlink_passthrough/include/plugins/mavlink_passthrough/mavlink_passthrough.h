#pragma once

#include <memory>
#include <string>
#include <functional>
#include <optional>

// This plugin provides/includes the mavlink 2.0 header files.
#include "mavlink_include.h"
#include "plugin_base.h"
#include "handle.h"
#include "deprecated.h"
#include "mavlink_address.h"

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
        CommandTemporarilyRejected, /**< @brief Command has been rejected for now. */
        CommandFailed, /**< @brief Command has failed. */
        ParamWrongType, /**< @brief Wrong type for requested param. */
        ParamNameTooLong, /**< @brief Param name too long. */
        ParamValueTooLong, /**< @brief Param value too long. */
        ParamNotFound, /**< @brief Param not found. */
        ParamValueUnsupported, /**< @brief Param value unsupported. */
    };

    /**
     * @brief Stream operator to print information about a `MavlinkPassthrough::Result`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream& operator<<(std::ostream& str, MavlinkPassthrough::Result const& result);

    /**
     * @brief Send message (deprecated).
     *
     * @note This interface is deprecated. Instead the method queue_message() should be used.
     *
     * @return result of the request.
     */
    DEPRECATED Result send_message(mavlink_message_t& message);

    /**
     * @brief Send message by queueing it.
     *
     * @note This interface replaces the previous send_message method.
     *
     * The interface changed in order to prevent accessing the internal MAVLink status from
     * different threads and to make sure the seq numbers are not unique to Mavsdk instances
     * and server components.
     *
     * @param fun Function which is (immediately) executed to send a message. It is passed the
     *            mavlink_address and channel, both data required to send a message using
     *            mavlink_message_xx_pack_chan().
     *
     * @return result of request
     */
    Result queue_message(
        std::function<mavlink_message_t(MavlinkAddress mavlink_address, uint8_t channel)> fun);

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
     * @brief Create a command_ack.
     *
     * @param target_sysid Target system ID where to send command_ack to.
     * @param target_compid Target component ID where to send command_ack to.
     * @param command Command to respond to.
     * @param result Result of command.
     *
     * @return message to send.
     */
    mavlink_message_t make_command_ack_message(
        const uint8_t target_sysid,
        const uint8_t target_compid,
        const uint16_t command,
        MAV_RESULT result);

    /**
     * @brief Request param (int).
     */
    std::pair<Result, int32_t> get_param_int(
        const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended);

    /**
     * @brief Request param (float).
     */
    std::pair<Result, float> get_param_float(
        const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended);

    /**
     * @brief Callback type for message subscriptions.
     */
    using MessageCallback = std::function<void(const mavlink_message_t&)>;

    /**
     * @brief Handle type for subscribe_message_async.
     */
    using MessageHandle = Handle<const mavlink_message_t&>;

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
    MessageHandle subscribe_message(uint16_t message_id, const MessageCallback& callback);

    /**
     * @brief Unsubscribe from subscribe_message.
     *
     * @param message_id The MAVLink message ID.
     * @param handle The handle returned from subscribe_message.
     */
    void unsubscribe_message(uint16_t message_id, MessageHandle handle);

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
