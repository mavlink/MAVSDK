#include "mavlink_direct_impl.hpp"
#include <mav/Message.h>
#include <mav/MessageSet.h>
#include "log.hpp"
#include "connection.hpp"
#include "libmav_conversions.hpp"
#include "callback_list.tpp"
#include "mavsdk_export.h"

#include <array>
#include <future>
#include <memory>

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

            _callbacks.queue(mavlink_direct_message, [this](const auto& func) {
                _system_impl->call_user_callback(func);
            });
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

std::pair<MavlinkDirect::Result, std::optional<mav::Message>>
MavlinkDirectImpl::create_libmav_message(
    const MavlinkDirect::MavlinkMessage& message, uint32_t& target_system_id) const
{
    target_system_id = 0;

    // Get access to the MessageSet through the system
    auto& message_set = _system_impl->get_message_set();

    // Create libmav message from the message name
    auto libmav_message_opt = message_set.create(message.message_name);
    if (!libmav_message_opt) {
        LogErr("Failed to create message: {}", message.message_name);
        return {MavlinkDirect::Result::InvalidMessage, std::nullopt};
    }

    if (_debugging) {
        LogDebug(
            "Created message {} with ID: {}",
            message.message_name,
            libmav_message_opt.value().id());
    }

    auto& libmav_message = libmav_message_opt.value();

    // Convert JSON fields to libmav message fields (shared core helper)
    if (!json_to_libmav_message(message.fields_json, libmav_message)) {
        LogErr("Failed to convert JSON fields to libmav message for {}", message.message_name);
        return {MavlinkDirect::Result::InvalidField, std::nullopt};
    }

    if (_debugging) {
        LogDebug("Successfully populated fields for {}", message.message_name);
    }

    // Set target system/component if specified.
    // Component IDs are 8 bit on the wire.
    if (message.target_component_id > 255) {
        LogErr("target_component_id {} out of range (max 255)", message.target_component_id);
        return {MavlinkDirect::Result::InvalidField, std::nullopt};
    }
    // Targets above 255 go in the extended header, the payload gets a sentinel.
    if (message.target_system_id != 0 &&
        libmav_message.set("target_system", mavlink_msg_target_field(message.target_system_id)) ==
            mav::MessageResult::Success) {
        target_system_id = message.target_system_id;
    }
    if (message.target_component_id != 0) {
        // For messages that have target_component field, set it
        libmav_message.set("target_component", static_cast<uint8_t>(message.target_component_id));
    }

    return {MavlinkDirect::Result::Success, std::move(libmav_message_opt)};
}

MavlinkDirect::Result MavlinkDirectImpl::send_message(MavlinkDirect::MavlinkMessage message)
{
    uint32_t target_system_id = 0;
    auto [result, libmav_message_opt] = create_libmav_message(message, target_system_id);
    if (result != MavlinkDirect::Result::Success) {
        return result;
    }
    auto& libmav_message = libmav_message_opt.value();

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

    return MavlinkDirect::Result::Success;
}

MavlinkDirect::MessageHandle MavlinkDirectImpl::subscribe_message(
    std::string message_name, const MavlinkDirect::MessageCallback& callback)
{
    if (message_name.empty()) {
        return _callbacks.subscribe(callback);
    }

    return _callbacks.subscribe(
        callback, [message_name](const MavlinkDirect::MavlinkMessage& message) {
            return message.message_name == message_name;
        });
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

void MavlinkDirectImpl::send_command_async(
    MavlinkDirect::MavlinkMessage command, const MavlinkDirect::SendCommandCallback& callback)
{
    auto report_error = [this, callback](MavlinkDirect::Result result) {
        if (callback) {
            _system_impl->call_user_callback(
                [callback, result]() { callback(result, MavlinkDirect::MavlinkMessage{}); });
        }
    };

    if (command.message_name != "COMMAND_LONG" && command.message_name != "COMMAND_INT") {
        LogErr("send_command needs COMMAND_LONG or COMMAND_INT, not {}", command.message_name);
        report_error(MavlinkDirect::Result::InvalidMessage);
        return;
    }

    // The target system is always the system this plugin is for.
    uint32_t target_system_id = 0;
    auto [result, libmav_message_opt] = create_libmav_message(command, target_system_id);
    if (result != MavlinkDirect::Result::Success) {
        report_error(result);
        return;
    }
    const auto& libmav_message = libmav_message_opt.value();

    // The final ack is handed to us on the io thread before the result callback is queued.
    auto final_ack = std::make_shared<std::optional<MavlinkDirect::MavlinkMessage>>();

    // Not capturing this, the plugin may be gone by the time the ack arrives.
    auto final_ack_callback = [weak_system_impl = std::weak_ptr<SystemImpl>(_system_impl),
                               final_ack](const mavlink_message_t& message) {
        auto system_impl = weak_system_impl.lock();
        if (!system_impl) {
            return;
        }
        auto maybe_json_message = system_impl->to_json_message(message);
        if (!maybe_json_message) {
            return;
        }
        MavlinkDirect::MavlinkMessage ack;
        ack.message_name = maybe_json_message->message_name;
        ack.system_id = maybe_json_message->system_id;
        ack.component_id = maybe_json_message->component_id;
        ack.target_system_id = maybe_json_message->target_system_id;
        ack.target_component_id = maybe_json_message->target_component_id;
        ack.fields_json = maybe_json_message->fields_json;
        *final_ack = std::move(ack);
    };

    auto result_callback = [callback,
                            final_ack](MavlinkCommandSender::Result command_result, float) {
        if (command_result == MavlinkCommandSender::Result::InProgress) {
            return;
        }
        if (!callback) {
            return;
        }

        switch (command_result) {
            case MavlinkCommandSender::Result::Success:
            case MavlinkCommandSender::Result::Denied:
            case MavlinkCommandSender::Result::Unsupported:
            case MavlinkCommandSender::Result::TemporarilyRejected:
            case MavlinkCommandSender::Result::Failed:
            case MavlinkCommandSender::Result::Cancelled:
                // Any final ack counts as success, the outcome is in the ack itself.
                if (final_ack->has_value()) {
                    callback(MavlinkDirect::Result::Success, final_ack->value());
                } else {
                    callback(MavlinkDirect::Result::Error, MavlinkDirect::MavlinkMessage{});
                }
                break;
            case MavlinkCommandSender::Result::Timeout:
                callback(MavlinkDirect::Result::Timeout, MavlinkDirect::MavlinkMessage{});
                break;
            case MavlinkCommandSender::Result::NoSystem:
                callback(MavlinkDirect::Result::NoSystem, MavlinkDirect::MavlinkMessage{});
                break;
            case MavlinkCommandSender::Result::ConnectionError:
                callback(MavlinkDirect::Result::ConnectionError, MavlinkDirect::MavlinkMessage{});
                break;
            default:
                callback(MavlinkDirect::Result::Error, MavlinkDirect::MavlinkMessage{});
                break;
        }
    };

    uint16_t command_id = 0;
    uint8_t target_component = 0;
    libmav_message.get("command", command_id);
    libmav_message.get("target_component", target_component);

    if (command.message_name == "COMMAND_LONG") {
        MavlinkCommandSender::CommandLong command_long{};
        command_long.command = command_id;
        command_long.target_component_id = target_component;
        libmav_message.get("confirmation", command_long.confirmation);

        std::array<std::optional<float>*, 7> params{
            &command_long.params.maybe_param1,
            &command_long.params.maybe_param2,
            &command_long.params.maybe_param3,
            &command_long.params.maybe_param4,
            &command_long.params.maybe_param5,
            &command_long.params.maybe_param6,
            &command_long.params.maybe_param7};
        for (size_t i = 0; i < params.size(); ++i) {
            float value = 0.0f;
            libmav_message.get("param" + std::to_string(i + 1), value);
            *params[i] = value;
        }

        _system_impl->send_command_async(command_long, result_callback, final_ack_callback);
    } else {
        MavlinkCommandSender::CommandInt command_int{};
        command_int.command = command_id;
        command_int.target_component_id = target_component;

        uint8_t frame = 0;
        uint8_t current = 0;
        uint8_t autocontinue = 0;
        libmav_message.get("frame", frame);
        libmav_message.get("current", current);
        libmav_message.get("autocontinue", autocontinue);
        command_int.frame = static_cast<MAV_FRAME>(frame);
        command_int.current = current != 0;
        command_int.autocontinue = autocontinue != 0;

        std::array<std::optional<float>*, 4> params{
            &command_int.params.maybe_param1,
            &command_int.params.maybe_param2,
            &command_int.params.maybe_param3,
            &command_int.params.maybe_param4};
        for (size_t i = 0; i < params.size(); ++i) {
            float value = 0.0f;
            libmav_message.get("param" + std::to_string(i + 1), value);
            *params[i] = value;
        }
        libmav_message.get("x", command_int.params.x);
        libmav_message.get("y", command_int.params.y);
        float z = 0.0f;
        libmav_message.get("z", z);
        command_int.params.maybe_z = z;

        _system_impl->send_command_async(command_int, result_callback, final_ack_callback);
    }
}

std::pair<MavlinkDirect::Result, MavlinkDirect::MavlinkMessage>
MavlinkDirectImpl::send_command(MavlinkDirect::MavlinkMessage command)
{
    auto prom = std::make_shared<
        std::promise<std::pair<MavlinkDirect::Result, MavlinkDirect::MavlinkMessage>>>();
    auto fut = prom->get_future();

    send_command_async(
        std::move(command),
        [prom](MavlinkDirect::Result result, MavlinkDirect::MavlinkMessage ack) {
            prom->set_value({result, std::move(ack)});
        });

    return fut.get();
}

} // namespace mavsdk
