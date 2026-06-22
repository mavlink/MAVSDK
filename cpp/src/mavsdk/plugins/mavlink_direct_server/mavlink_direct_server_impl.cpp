#include "mavlink_direct_server_impl.hpp"

#include "libmav_conversions.hpp"
#include "log.hpp"

#include <mav/Message.h>
#include <mav/MessageSet.h>
#include <cstdlib>
#include <cstring>

namespace mavsdk {

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

void MavlinkDirectServerImpl::init() {}

void MavlinkDirectServerImpl::deinit() {}

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
        libmav_message.set(
            "target_component_id", static_cast<uint8_t>(message.target_component_id));
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
