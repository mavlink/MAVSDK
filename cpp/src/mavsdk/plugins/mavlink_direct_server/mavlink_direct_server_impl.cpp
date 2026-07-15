#include "mavlink_direct_server_impl.hpp"

#include "libmav_conversions.hpp"
#include "log.hpp"
#include "callback_list.tpp"
#include "mavsdk_export.h"

#include <mav/Message.h>
#include <mav/MessageSet.h>
#include <cstdlib>
#include <cstring>

namespace mavsdk {

template class MAVSDK_TEMPL_INST CallbackList<MavlinkDirectServer::MavlinkMessage>;

MavlinkDirectServerImpl::MavlinkDirectServerImpl(
    std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
    _server_component_impl->register_plugin(this);
}

MavlinkDirectServerImpl::~MavlinkDirectServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void MavlinkDirectServerImpl::init()
{
    // Subscribe to all incoming messages and distribute to our internal CallbackList.
    _server_subscription = _server_component_impl->register_libmav_message_handler(
        "", // Empty string means all messages
        [this](const Mavsdk::MavlinkMessage& message) {
            MavlinkDirectServer::MavlinkMessage mavlink_direct_message;
            mavlink_direct_message.message_name = message.message_name;
            mavlink_direct_message.system_id = message.system_id;
            mavlink_direct_message.component_id = message.component_id;
            mavlink_direct_message.target_system_id = message.target_system_id;
            mavlink_direct_message.target_component_id = message.target_component_id;
            mavlink_direct_message.fields_json = message.fields_json;

            _callbacks(mavlink_direct_message);
        });
}

void MavlinkDirectServerImpl::deinit()
{
    // Stop dispatching to our own subscribers.
    _callbacks.clear();

    // Unregister from the server component BEFORE this object is destroyed. The registered
    // callback captures 'this' and reaches into _callbacks on the io thread;
    // unregister_libmav_message_handler() is blocking, so once it returns the io thread can
    // no longer invoke it against a freed MavlinkDirectServerImpl (see MavlinkDirectImpl).
    if (_server_subscription.valid()) {
        _server_component_impl->unregister_libmav_message_handler(_server_subscription);
        _server_subscription = {};
    }
}

MavlinkDirectServer::Result
MavlinkDirectServerImpl::send_message(MavlinkDirectServer::MavlinkMessage message)
{
    // Get access to the MessageSet shared across the Mavsdk instance.
    auto& message_set = _server_component_impl->get_message_set();

    // Create libmav message from the message name.
    auto libmav_message_opt = message_set.create(message.message_name);
    if (!libmav_message_opt) {
        LogErr("Failed to create message: {}", message.message_name);
        return MavlinkDirectServer::Result::InvalidMessage;
    }

    if (_debugging) {
        LogDebug(
            "Created message {} with ID: {}",
            message.message_name,
            libmav_message_opt.value().id());
    }

    auto libmav_message = libmav_message_opt.value();

    // Convert JSON fields to libmav message fields (shared core helper).
    if (!json_to_libmav_message(message.fields_json, libmav_message)) {
        LogErr("Failed to convert JSON fields to libmav message for {}", message.message_name);
        return MavlinkDirectServer::Result::InvalidField;
    }

    if (_debugging) {
        LogDebug("Successfully populated fields for {}", message.message_name);
    }

    // Set target system/component if specified (0 means broadcast).
    if (message.target_system_id != 0) {
        libmav_message.set("target_system", static_cast<uint8_t>(message.target_system_id));
    }
    if (message.target_component_id != 0) {
        libmav_message.set("target_component", static_cast<uint8_t>(message.target_component_id));
    }

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t mavlink_message;

        auto payload_view = libmav_message.getPayloadView();
        const uint8_t payload_length = libmav_message.getPayloadLength();

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

    return MavlinkDirectServer::Result::Success;
}

MavlinkDirectServer::MessageHandle MavlinkDirectServerImpl::subscribe_message(
    std::string message_name, const MavlinkDirectServer::MessageCallback& callback)
{
    auto filtering_callback = [this, message_name, callback](
                                  const MavlinkDirectServer::MavlinkMessage& message) {
        if (!message_name.empty() && message_name != message.message_name) {
            return;
        }
        _server_component_impl->call_user_callback([callback, message]() { callback(message); });
    };

    return _callbacks.subscribe(filtering_callback);
}

void MavlinkDirectServerImpl::unsubscribe_message(MavlinkDirectServer::MessageHandle handle)
{
    _callbacks.unsubscribe(handle);
}

MavlinkDirectServer::Result MavlinkDirectServerImpl::load_custom_xml(std::string xml_content)
{
    if (_debugging) {
        LogDebug("Loading custom XML definitions...");
    }

    if (_server_component_impl->load_custom_xml_to_message_set(xml_content)) {
        if (_debugging) {
            LogDebug("Successfully loaded custom XML definitions");
        }
        return MavlinkDirectServer::Result::Success;
    }

    LogErr("Failed to load custom XML definitions");
    return MavlinkDirectServer::Result::Error;
}

} // namespace mavsdk
