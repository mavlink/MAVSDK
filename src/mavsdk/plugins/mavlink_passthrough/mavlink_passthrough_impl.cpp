#include <functional>
#include "mavlink_passthrough_impl.h"
#include "system.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<const mavlink_message_t&>;

MavlinkPassthroughImpl::MavlinkPassthroughImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MavlinkPassthroughImpl::MavlinkPassthroughImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
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
    _parent->unregister_all_mavlink_message_handlers(this);
    _message_subscriptions.clear();
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
    MavlinkCommandSender::CommandLong command_internal{};
    command_internal.target_system_id = command.target_sysid;
    command_internal.target_component_id = command.target_compid;
    command_internal.command = command.command;
    command_internal.params.maybe_param1 = command.param1;
    command_internal.params.maybe_param2 = command.param2;
    command_internal.params.maybe_param3 = command.param3;
    command_internal.params.maybe_param4 = command.param4;
    command_internal.params.maybe_param5 = command.param5;
    command_internal.params.maybe_param6 = command.param6;
    command_internal.params.maybe_param7 = command.param7;

    return to_mavlink_passthrough_result_from_mavlink_commands_result(
        _parent->send_command(command_internal));
}

MavlinkPassthrough::Result
MavlinkPassthroughImpl::send_command_int(const MavlinkPassthrough::CommandInt& command)
{
    MavlinkCommandSender::CommandInt command_internal{};
    command_internal.target_system_id = command.target_sysid;
    command_internal.target_component_id = command.target_compid;
    command_internal.frame = command.frame;
    command_internal.command = command.command;
    command_internal.params.maybe_param1 = command.param1;
    command_internal.params.maybe_param2 = command.param2;
    command_internal.params.maybe_param3 = command.param3;
    command_internal.params.maybe_param4 = command.param4;
    command_internal.params.x = command.x;
    command_internal.params.y = command.y;
    command_internal.params.maybe_z = command.z;

    return to_mavlink_passthrough_result_from_mavlink_commands_result(
        _parent->send_command(command_internal));
}

mavlink_message_t MavlinkPassthroughImpl::make_command_ack_message(
    const uint8_t target_sysid,
    const uint8_t target_compid,
    const uint16_t command,
    MAV_RESULT result)
{
    /* copied over from system impl */
    const uint8_t progress = std::numeric_limits<uint8_t>::max();
    const uint8_t result_param2 = 0;

    mavlink_message_t msg{};
    mavlink_msg_command_ack_pack(
        get_our_sysid(),
        get_our_compid(),
        &msg,
        command,
        result,
        progress,
        result_param2,
        target_sysid,
        target_compid);
    return msg;
}

MavlinkPassthrough::Result
MavlinkPassthroughImpl::to_mavlink_passthrough_result_from_mavlink_commands_result(
    MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return MavlinkPassthrough::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return MavlinkPassthrough::Result::CommandNoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return MavlinkPassthrough::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return MavlinkPassthrough::Result::CommandBusy;
        case MavlinkCommandSender::Result::Denied:
            return MavlinkPassthrough::Result::CommandDenied;
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return MavlinkPassthrough::Result::CommandTemporarilyRejected;
        case MavlinkCommandSender::Result::Unsupported:
            return MavlinkPassthrough::Result::CommandUnsupported;
        case MavlinkCommandSender::Result::Failed:
            return MavlinkPassthrough::Result::CommandFailed;
        case MavlinkCommandSender::Result::Timeout:
            return MavlinkPassthrough::Result::CommandTimeout;
        default:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::InProgress: // FIXME: currently not expected
                                                       // FALLTHROUGH
        case MavlinkCommandSender::Result::UnknownError:
            return MavlinkPassthrough::Result::Unknown;
    }
}

MavlinkPassthrough::MessageHandle MavlinkPassthroughImpl::subscribe_message(
    uint16_t message_id, const MavlinkPassthrough::MessageCallback& callback)
{
    if (_message_subscriptions.find(message_id) == _message_subscriptions.end()) {
        _parent->register_mavlink_message_handler(
            message_id,
            [this](const mavlink_message_t& message) { receive_mavlink_message(message); },
            this);
    }

    return _message_subscriptions[message_id].subscribe(callback);
}

void MavlinkPassthroughImpl::unsubscribe_message(MavlinkPassthrough::MessageHandle handle)
{
    // We don't know which subscription holds the handle, so we have to go
    // through all of them.
    for (auto& subscription : _message_subscriptions) {
        subscription.second.unsubscribe(handle);
    }
}

void MavlinkPassthroughImpl::receive_mavlink_message(const mavlink_message_t& message)
{
    _message_subscriptions[message.msgid].queue(
        message, [this](const auto& func) { _parent->call_user_callback(func); });
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

} // namespace mavsdk
