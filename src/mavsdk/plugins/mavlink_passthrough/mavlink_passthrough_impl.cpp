#include <functional>
#include "mavlink_passthrough_impl.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "system.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<const mavlink_message_t&>;

MavlinkPassthroughImpl::MavlinkPassthroughImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

MavlinkPassthroughImpl::MavlinkPassthroughImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

MavlinkPassthroughImpl::~MavlinkPassthroughImpl()
{
    _system_impl->unregister_plugin(this);
}

void MavlinkPassthroughImpl::init() {}

void MavlinkPassthroughImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers(this);
    _message_subscriptions.clear();
}

void MavlinkPassthroughImpl::enable() {}

void MavlinkPassthroughImpl::disable() {}

MavlinkPassthrough::Result MavlinkPassthroughImpl::send_message(mavlink_message_t& message)
{
    if (!_system_impl->send_message(message)) {
        return MavlinkPassthrough::Result::ConnectionError;
    }
    return MavlinkPassthrough::Result::Success;
}

MavlinkPassthrough::Result MavlinkPassthroughImpl::queue_message(
    std::function<mavlink_message_t(MavlinkAddress mavlink_address, uint8_t channel)> fun)
{
    return _system_impl->queue_message(fun) ? MavlinkPassthrough::Result::Success :
                                              MavlinkPassthrough::Result::ConnectionError;
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
        _system_impl->send_command(command_internal));
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
        _system_impl->send_command(command_internal));
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

std::pair<MavlinkPassthrough::Result, int32_t> MavlinkPassthroughImpl::get_param_int(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    auto result = _system_impl->get_param_int(name, maybe_component_id, extended);
    auto translated_result = to_mavlink_passthrough_result_from_mavlink_params_result(result.first);

    return std::make_pair(translated_result, result.second);
}

std::pair<MavlinkPassthrough::Result, float> MavlinkPassthroughImpl::get_param_float(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    auto result = _system_impl->get_param_float(name, maybe_component_id, extended);
    auto translated_result = to_mavlink_passthrough_result_from_mavlink_params_result(result.first);

    return std::make_pair(translated_result, result.second);
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

MavlinkPassthrough::Result
MavlinkPassthroughImpl::to_mavlink_passthrough_result_from_mavlink_params_result(
    MavlinkParameterClient::Result result)
{
    switch (result) {
        case MavlinkParameterClient::Result::Success:
            return MavlinkPassthrough::Result::Success;
        case MavlinkParameterClient::Result::Timeout:
            return MavlinkPassthrough::Result::CommandTimeout;
        case MavlinkParameterClient::Result::ConnectionError:
            return MavlinkPassthrough::Result::ConnectionError;
        case MavlinkParameterClient::Result::WrongType:
            return MavlinkPassthrough::Result::ParamWrongType;
        case MavlinkParameterClient::Result::ParamNameTooLong:
            return MavlinkPassthrough::Result::ParamNameTooLong;
        case MavlinkParameterClient::Result::ParamValueTooLong:
            return MavlinkPassthrough::Result::ParamValueTooLong;
        case MavlinkParameterClient::Result::NotFound:
            return MavlinkPassthrough::Result::ParamNotFound;
        case MavlinkParameterClient::Result::ValueUnsupported:
            return MavlinkPassthrough::Result::ParamValueUnsupported;
        case MavlinkParameterClient::Result::Failed:
            return MavlinkPassthrough::Result::CommandFailed;
        default:
            // FALLTHROUGH
        case MavlinkParameterClient::Result::UnknownError:
            return MavlinkPassthrough::Result::Unknown;
    }
}

MavlinkPassthrough::MessageHandle MavlinkPassthroughImpl::subscribe_message(
    uint16_t message_id, const MavlinkPassthrough::MessageCallback& callback)
{
    if (_message_subscriptions.find(message_id) == _message_subscriptions.end()) {
        _system_impl->register_mavlink_message_handler(
            message_id,
            [this](const mavlink_message_t& message) { receive_mavlink_message(message); },
            this);
    }

    return _message_subscriptions[message_id].subscribe(callback);
}

void MavlinkPassthroughImpl::unsubscribe_message(
    uint16_t message_id, MavlinkPassthrough::MessageHandle handle)
{
    auto it = _message_subscriptions.find(message_id);
    if (it != _message_subscriptions.end()) {
        it->second.unsubscribe(handle);
    }
}

void MavlinkPassthroughImpl::receive_mavlink_message(const mavlink_message_t& message)
{
    _message_subscriptions[message.msgid].queue(
        message, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

uint8_t MavlinkPassthroughImpl::get_our_sysid() const
{
    return _system_impl->get_own_system_id();
}

uint8_t MavlinkPassthroughImpl::get_our_compid() const
{
    return _system_impl->get_own_component_id();
}

uint8_t MavlinkPassthroughImpl::get_target_sysid() const
{
    return _system_impl->get_system_id();
}

uint8_t MavlinkPassthroughImpl::get_target_compid() const
{
    return _system_impl->get_autopilot_id();
}

} // namespace mavsdk
