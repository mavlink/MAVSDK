#include <functional>
#include "mavlink_passthrough_impl.h"
#include "system.h"
#include "global_include.h"

namespace mavsdk {

MavlinkPassthroughImpl::MavlinkPassthroughImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MavlinkPassthroughImpl::~MavlinkPassthroughImpl()
{
    _parent->unregister_plugin(this);
}

void MavlinkPassthroughImpl::init() {}

void MavlinkPassthroughImpl::deinit()
{
    _parent->intercept_incoming_messages(nullptr);
    _parent->intercept_outgoing_messages(nullptr);
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MavlinkPassthroughImpl::enable() {}

void MavlinkPassthroughImpl::disable() {}

MavlinkPassthrough::Result MavlinkPassthroughImpl::send_message(mavlink_message_t& message)
{
    if (!_parent->send_message(message)) {
        return MavlinkPassthrough::Result::ConnectionError;
    }
    return MavlinkPassthrough::Result::Success;
}

MavlinkPassthrough::Result
MavlinkPassthroughImpl::send_command_long(const MavlinkPassthrough::CommandLong& command)
{
    MAVLinkCommands::CommandLong command_internal{};
    command_internal.target_system_id = command.target_sysid;
    command_internal.target_component_id = command.target_compid;
    command_internal.command = command.command;
    command_internal.params.param1 = command.param1;
    command_internal.params.param2 = command.param2;
    command_internal.params.param3 = command.param3;
    command_internal.params.param4 = command.param4;
    command_internal.params.param5 = command.param5;
    command_internal.params.param6 = command.param6;
    command_internal.params.param7 = command.param7;

    return to_mavlink_passthrough_result_from_mavlink_commands_result(
        _parent->send_command(command_internal));
}

MavlinkPassthrough::Result
MavlinkPassthroughImpl::send_command_int(const MavlinkPassthrough::CommandInt& command)
{
    MAVLinkCommands::CommandInt command_internal{};
    command_internal.target_system_id = command.target_sysid;
    command_internal.target_component_id = command.target_compid;
    command_internal.frame = command.frame;
    command_internal.command = command.command;
    command_internal.params.param1 = command.param1;
    command_internal.params.param2 = command.param2;
    command_internal.params.param3 = command.param3;
    command_internal.params.param4 = command.param4;
    command_internal.params.x = command.x;
    command_internal.params.y = command.y;
    command_internal.params.z = command.z;

    return to_mavlink_passthrough_result_from_mavlink_commands_result(
        _parent->send_command(command_internal));
}

MavlinkPassthrough::Result
MavlinkPassthroughImpl::to_mavlink_passthrough_result_from_mavlink_commands_result(
    MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::Success:
            return MavlinkPassthrough::Result::Success;
        case MAVLinkCommands::Result::NoSystem:
            return MavlinkPassthrough::Result::CommandNoSystem;
        case MAVLinkCommands::Result::ConnectionError:
            return MavlinkPassthrough::Result::ConnectionError;
        case MAVLinkCommands::Result::Busy:
            return MavlinkPassthrough::Result::CommandBusy;
        case MAVLinkCommands::Result::CommandDenied:
            return MavlinkPassthrough::Result::CommandDenied;
        case MAVLinkCommands::Result::Unsupported:
            return MavlinkPassthrough::Result::CommandUnsupported;
        case MAVLinkCommands::Result::Timeout:
            return MavlinkPassthrough::Result::CommandTimeout;
        default:
            // FALLTHROUGH
        case MAVLinkCommands::Result::InProgress: // FIXME: currently not expected
                                                  // FALLTHROUGH
        case MAVLinkCommands::Result::UnknownError:
            return MavlinkPassthrough::Result::Unknown;
    }
}

void MavlinkPassthroughImpl::subscribe_message_async(
    uint16_t message_id, std::function<void(const mavlink_message_t&)> callback)
{
    if (callback == nullptr) {
        _parent->unregister_mavlink_message_handler(message_id, this);
    } else {
        auto temp_callback = callback;
        _parent->register_mavlink_message_handler(
            message_id,
            [this, temp_callback](const mavlink_message_t& message) {
                _parent->call_user_callback([temp_callback, message]() { temp_callback(message); });
            },
            this);
    }
}

uint8_t MavlinkPassthroughImpl::get_our_sysid() const
{
    return _parent->get_own_system_id();
}

uint8_t MavlinkPassthroughImpl::get_our_compid() const
{
    return _parent->get_own_component_id();
}

uint8_t MavlinkPassthroughImpl::get_target_sysid() const
{
    return _parent->get_system_id();
}

uint8_t MavlinkPassthroughImpl::get_target_compid() const
{
    return _parent->get_autopilot_id();
}

void MavlinkPassthroughImpl::intercept_incoming_messages_async(
    std::function<bool(mavlink_message_t&)> callback)
{
    _parent->intercept_incoming_messages(callback);
}

void MavlinkPassthroughImpl::intercept_outgoing_messages_async(
    std::function<bool(mavlink_message_t&)> callback)
{
    _parent->intercept_outgoing_messages(callback);
}

} // namespace mavsdk
