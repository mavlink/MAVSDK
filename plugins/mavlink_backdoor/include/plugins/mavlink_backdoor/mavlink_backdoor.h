#pragma once

#include <memory>
#include <string>
#include <functional>

#include <dronecode_sdk/plugin_base.h>

namespace dronecode_sdk {

class System;
class MavlinkBackdoorImpl;

/**
 * @brief The MavlinkBackdoor class provides direct MAVLink access
 *
 * "With great power comes great responsibility." - This plugin allows
 * you to send and receive MAVLink messages. There is no checking or
 * safe-guards, you're on your own, and you have been warned.
 */
class MavlinkBackdoor : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mavlink_backdoor = std::make_shared<MavlinkBackdoor>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit MavlinkBackdoor(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~MavlinkBackdoor();

    /**
     * @brief Possible results returned for requests.
     */
    enum class Result {
        UNKNOWN, /**< @brief Unknown error. */
        SUCCESS, /**< @brief Success. */
        CONNECTION_ERROR /**< @brief Connection error. */
    };

    /**
     * @brief Returns a human-readable English string for `MavlinkBackdoor::Result`.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the `MavlinkBackdoor::Result`.
     */
    std::string result_str(Result result);

    /**
     * @brief Send message.
     *
     * @return result of the request.
     */
    Result send_message(/*const mavlink_message_t& message*/);

    /**
     * @brief Subscribe to messages using message ID.
     *
     * This means that all future messages being received will trigger the
     * callback to be called. To stop the subscription, call this method with
     * `nullptr` as the argument.
     *
     * @param callback Callback to be called for message subscription.
     */
    void subscribe_message_async(std::function<void(/*const mavlink_message_t&*/)> callback);

    /**
     * @brief Copy Constructor (object is not copyable).
     */
    MavlinkBackdoor(const MavlinkBackdoor &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const MavlinkBackdoor &operator=(const MavlinkBackdoor &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MavlinkBackdoorImpl> _impl;
};

} // namespace dronecode_sdk
