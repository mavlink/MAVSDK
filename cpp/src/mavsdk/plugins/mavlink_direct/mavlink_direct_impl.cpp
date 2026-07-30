#include "mavlink_direct_impl.hpp"
#include <mav/Message.h>
#include <mav/MessageSet.h>
#include "log.hpp"
#include "connection.hpp"
#include "libmav_conversions.hpp"
#include "callback_list.tpp"
#include "mavsdk_export.h"

namespace mavsdk {

template class MAVSDK_TEMPL_INST CallbackList<MavlinkDirect::MavlinkMessage>;

MavlinkDirectImpl::MavlinkDirectImpl(System& system) : PluginImplBase(system)
{
    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
    _system_impl->register_plugin(this);
}

MavlinkDirectImpl::MavlinkDirectImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
    _system_impl->register_plugin(this);
}

MavlinkDirectImpl::~MavlinkDirectImpl()
{
    _system_impl->unregister_plugin(this);
}

void MavlinkDirectImpl::init()
{
    // Subscribe to all messages from SystemImpl and distribute to internal CallbackList
    _system_subscription = _system_impl->register_libmav_message_handler(
        "", // Empty string means all messages
        [this](const Mavsdk::MavlinkMessage& message) {
            // Convert Mavsdk::MavlinkMessage to MavlinkDirect::MavlinkMessage
            MavlinkDirect::MavlinkMessage mavlink_direct_message;
            mavlink_direct_message.message_name = message.message_name;
            mavlink_direct_message.system_id = message.system_id;
            mavlink_direct_message.component_id = message.component_id;
            mavlink_direct_message.target_system_id = message.target_system_id;
            mavlink_direct_message.target_component_id = message.target_component_id;
            mavlink_direct_message.fields_json = message.fields_json;

            _callbacks(mavlink_direct_message);
        });
}

void MavlinkDirectImpl::deinit()
{
    // Stop dispatching to our own subscribers.
    _callbacks.clear();

    // Crucially, unregister our handler from the system BEFORE this object is destroyed.
    // The registered callback captures 'this' and reaches into _callbacks, and it runs on
    // the io thread. unregister_libmav_message_handler() is blocking: once it returns the
    // io thread can no longer invoke that callback, so it can't touch a freed
    // MavlinkDirectImpl (previously a teardown use-after-free, only visible as a rare
    // io-thread crash).
    if (_system_subscription.valid()) {
        _system_impl->unregister_libmav_message_handler(_system_subscription);
        _system_subscription = {};
    }
}

void MavlinkDirectImpl::enable() {}

void MavlinkDirectImpl::disable() {}

MavlinkDirect::Result MavlinkDirectImpl::send_message(MavlinkDirect::MavlinkMessage message)
{
    // Get access to the MessageSet through the system
    auto& message_set = _system_impl->get_message_set();

    // Create libmav message from the message name
    auto libmav_message_opt = message_set.create(message.message_name);
    if (!libmav_message_opt) {
        LogErr("Failed to create message: {}", message.message_name);
        return MavlinkDirect::Result::InvalidMessage; // Message type not found
    }

    if (_debugging) {
        LogDebug(
            "Created message {} with ID: {}",
            message.message_name,
            libmav_message_opt.value().id());
    }

    auto libmav_message = libmav_message_opt.value();

    // Convert JSON fields to libmav message fields (shared core helper)
    if (!json_to_libmav_message(message.fields_json, libmav_message)) {
        LogErr("Failed to convert JSON fields to libmav message for {}", message.message_name);
        return MavlinkDirect::Result::InvalidField; // JSON conversion failed
    }

    if (_debugging) {
        LogDebug("Successfully populated fields for {}", message.message_name);
    }

    // Set target system/component if specified
    if (message.target_system_id != 0) {
        // For messages that have target_system field, set it
        libmav_message.set("target_system", static_cast<uint8_t>(message.target_system_id));
    }
    if (message.target_component_id != 0) {
        // For messages that have target_component field, set it
        libmav_message.set("target_component", static_cast<uint8_t>(message.target_component_id));
    }

    if (_debugging) {
        LogDebug("Sending {} via unified libmav API", message.message_name);
    }

    _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t mavlink_message;

        // Use clean libmav helper methods to get payload data
        auto payload_view = libmav_message.getPayloadView();
        const uint8_t payload_length = libmav_message.getPayloadLength();

        // Set up the mavlink_message_t structure
        mavlink_message.msgid = libmav_message.id();
        mavlink_message.len = payload_length;
        memcpy(mavlink_message.payload64, payload_view.first, payload_length);

        mavlink_finalize_message_chan(
            &mavlink_message,
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            payload_length,
            libmav_message.type().maxPayloadSize(),
            libmav_message.type().crcExtra());

        return mavlink_message;
    });

    if (_debugging) {
        LogDebug("Successfully sent {} as raw data", message.message_name);
    }

    return MavlinkDirect::Result::Success;
}

MavlinkDirect::MessageHandle MavlinkDirectImpl::subscribe_message(
    std::string message_name, const MavlinkDirect::MessageCallback& callback)
{
    // filtering_callback is called synchronously on the I/O thread (via _callbacks()/exec()
    // in init()).  It captures 'this' only to reach call_user_callback, and is never
    // enqueued anywhere — so there is no lifetime issue.  deinit() calls _callbacks.clear()
    // before unregistering the system handler, which blocks until any in-flight exec()
    // completes; after that, filtering_callback will never be called again.
    auto filtering_callback =
        [this, message_name, callback](const MavlinkDirect::MavlinkMessage& message) {
            if (!message_name.empty() && message_name != message.message_name) {
                return;
            }
            _system_impl->call_user_callback([callback, message]() { callback(message); });
        };

    // Subscribe to internal CallbackList and return handle directly
    return _callbacks.subscribe(filtering_callback);
}

void MavlinkDirectImpl::unsubscribe_message(MavlinkDirect::MessageHandle handle)
{
    // Unregister from internal CallbackList directly
    _callbacks.unsubscribe(handle);
}

MavlinkDirect::Result MavlinkDirectImpl::load_custom_xml(const std::string& xml_content)
{
    if (_debugging) {
        LogDebug("Loading custom XML definitions...");
    }

    // Load the custom XML into the MessageSet with thread safety
    // This uses the thread-safe method that protects against concurrent read operations
    bool success = _system_impl->load_custom_xml_to_message_set(xml_content);

    if (success) {
        if (_debugging) {
            LogDebug("Successfully loaded custom XML definitions");
        }
        return MavlinkDirect::Result::Success;
    } else {
        LogErr("Failed to load custom XML definitions");
        return MavlinkDirect::Result::Error;
    }
}

} // namespace mavsdk
