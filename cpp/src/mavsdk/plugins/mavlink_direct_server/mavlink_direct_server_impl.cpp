#include "mavlink_direct_server_impl.hpp"

#include "libmav_conversions.hpp"
#include "log.hpp"
#include "callback_list.tpp"
#include "mavsdk_export.h"

#include <mav/Message.h>
#include <mav/MessageSet.h>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <optional>

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

            _callbacks.queue(mavlink_direct_message, [this](const auto& func) {
                _server_component_impl->call_user_callback(func);
            });
        });
}

void MavlinkDirectServerImpl::deinit()
{
    // Blocking with respect to the receiver, so no command handler runs after this.
    _server_component_impl->unregister_all_mavlink_command_handlers(this);
    {
        std::lock_guard<std::mutex> lock(_command_subscriptions_mutex);
        _command_subscriptions.clear();
        _command_subscription_counts.clear();
    }
    _command_callbacks.clear();

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
    // Component IDs are 8 bit on the wire.
    if (message.target_component_id > 255) {
        LogErr("target_component_id {} out of range (max 255)", message.target_component_id);
        return MavlinkDirectServer::Result::InvalidField;
    }
    // Targets above 255 go in the extended header, the payload gets a sentinel.
    uint32_t target_system_id = 0;
    if (message.target_system_id != 0 &&
        libmav_message.set("target_system", mavlink_msg_target_field(message.target_system_id)) ==
            mav::MessageResult::Success) {
        target_system_id = message.target_system_id;
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

        mavlink_finalize_message_chan_target(
            &mavlink_message,
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            payload_length,
            libmav_message.type().maxPayloadSize(),
            libmav_message.type().crcExtra(),
            target_system_id);

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
    if (message_name.empty()) {
        return _callbacks.subscribe(callback);
    }

    return _callbacks.subscribe(
        callback, [message_name](const MavlinkDirectServer::MavlinkMessage& message) {
            return message.message_name == message_name;
        });
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

static std::optional<uint16_t> command_id_from_json(const std::string& fields_json)
{
    auto json = nlohmann::json::parse(fields_json, nullptr, false);
    if (json.is_discarded() || !json.is_object() || !json.contains("command") ||
        !json["command"].is_number_unsigned()) {
        return std::nullopt;
    }
    return json["command"].get<uint16_t>();
}

MavlinkDirectServer::CommandHandle MavlinkDirectServerImpl::subscribe_command(
    uint32_t command, const MavlinkDirectServer::CommandCallback& callback)
{
    if (command > std::numeric_limits<uint16_t>::max()) {
        LogErr("Command {} out of range", command);
        return {};
    }
    const auto command_id = static_cast<uint16_t>(command);

    std::lock_guard<std::mutex> lock(_command_subscriptions_mutex);

    auto handle = _command_callbacks.subscribe(
        callback, [command_id](const MavlinkDirectServer::MavlinkMessage& message) {
            return command_id_from_json(message.fields_json) == command_id;
        });
    _command_subscriptions[handle] = command_id;

    // The receiver is told only once per command ID that we handle it.
    if (++_command_subscription_counts[command_id] == 1) {
        register_command_handlers(command_id);
    }

    return handle;
}

void MavlinkDirectServerImpl::unsubscribe_command(MavlinkDirectServer::CommandHandle handle)
{
    std::lock_guard<std::mutex> lock(_command_subscriptions_mutex);

    auto it = _command_subscriptions.find(handle);
    if (it == _command_subscriptions.end()) {
        return;
    }
    const auto command_id = it->second;
    _command_subscriptions.erase(it);
    _command_callbacks.unsubscribe(handle);

    if (--_command_subscription_counts[command_id] == 0) {
        _command_subscription_counts.erase(command_id);
        _server_component_impl->unregister_mavlink_command_handler(command_id, this);
    }
}

void MavlinkDirectServerImpl::register_command_handlers(uint16_t command_id)
{
    // These run on the io thread while the receiver holds its table lock, so they must not
    // call user code directly. Returning nullopt means the subscriber sends the ack.
    _server_component_impl->register_mavlink_command_handler(
        command_id,
        [this](const MavlinkCommandReceiver::CommandLong& command)
            -> std::optional<mavlink_command_ack_t> {
            // Re-encode with a local status so no channel sequence number is used up.
            mavlink_status_t status{};
            mavlink_message_t message;
            mavlink_msg_command_long_pack_status(
                command.origin_system_id,
                command.origin_component_id,
                &status,
                &message,
                command.target_system_id,
                command.target_component_id,
                command.command,
                command.confirmation,
                command.params.param1,
                command.params.param2,
                command.params.param3,
                command.params.param4,
                command.params.param5,
                command.params.param6,
                command.params.param7);
            queue_command(message);
            return std::nullopt;
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        command_id,
        [this](const MavlinkCommandReceiver::CommandInt& command)
            -> std::optional<mavlink_command_ack_t> {
            mavlink_status_t status{};
            mavlink_message_t message;
            mavlink_msg_command_int_pack_status(
                command.origin_system_id,
                command.origin_component_id,
                &status,
                &message,
                command.target_system_id,
                command.target_component_id,
                static_cast<uint8_t>(command.frame),
                command.command,
                command.current ? 1 : 0,
                command.autocontinue ? 1 : 0,
                command.params.param1,
                command.params.param2,
                command.params.param3,
                command.params.param4,
                command.params.x,
                command.params.y,
                command.params.z);
            queue_command(message);
            return std::nullopt;
        },
        this);
}

void MavlinkDirectServerImpl::queue_command(const mavlink_message_t& message)
{
    auto maybe_json_message = _server_component_impl->to_json_message(message);
    if (!maybe_json_message) {
        LogErr("Could not convert received command to JSON");
        return;
    }

    MavlinkDirectServer::MavlinkMessage command;
    command.message_name = maybe_json_message->message_name;
    command.system_id = maybe_json_message->system_id;
    command.component_id = maybe_json_message->component_id;
    command.target_system_id = maybe_json_message->target_system_id;
    command.target_component_id = maybe_json_message->target_component_id;
    command.fields_json = maybe_json_message->fields_json;

    _command_callbacks.queue(
        command, [this](const auto& func) { _server_component_impl->call_user_callback(func); });
}

} // namespace mavsdk
