#include "winch_impl.h"
#include "system.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<Winch::Status>;

WinchImpl::WinchImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

WinchImpl::WinchImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

WinchImpl::~WinchImpl()
{
    _system_impl->unregister_plugin(this);
}

void WinchImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_WINCH_STATUS,
        [this](const mavlink_message_t& message) { process_status(message); },
        this);
}

Winch::StatusHandle WinchImpl::subscribe_status(const Winch::StatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    return _status_subscriptions.subscribe(callback);
}

void WinchImpl::unsubscribe_status(Winch::StatusHandle handle)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _status_subscriptions.unsubscribe(handle);
}

void WinchImpl::set_status(Winch::Status status)
{
    std::lock_guard<std::mutex> lock(_status_mutex);
    _status = status;
}

Winch::Status WinchImpl::status() const
{
    std::lock_guard<std::mutex> lock(_status_mutex);
    return _status;
}

void WinchImpl::process_status(const mavlink_message_t& message)
{
    mavlink_winch_status_t mavlink_winch_status;
    mavlink_msg_winch_status_decode(&message, &mavlink_winch_status);

    Winch::StatusFlags status_flags;
    status_flags.healthy =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_HEALTHY;
    status_flags.fully_retracted =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_FULLY_RETRACTED;
    status_flags.moving =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_MOVING;
    status_flags.clutch_engaged =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_CLUTCH_ENGAGED;
    status_flags.locked =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_LOCKED;
    status_flags.dropping =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_DROPPING;
    status_flags.arresting =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_ARRESTING;
    status_flags.ground_sense =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_GROUND_SENSE;
    status_flags.retracting =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_RETRACTING;
    status_flags.redeliver =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_REDELIVER;
    status_flags.abandon_line =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_ABANDON_LINE;
    status_flags.locking =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_LOCKING;
    status_flags.load_line =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_LOAD_LINE;
    status_flags.load_payload =
        mavlink_winch_status.status & MAV_WINCH_STATUS_FLAG::MAV_WINCH_STATUS_LOAD_PAYLOAD;

    Winch::Status new_status;
    new_status.time_usec = mavlink_winch_status.time_usec;
    new_status.line_length_m = mavlink_winch_status.line_length;
    new_status.speed_m_s = mavlink_winch_status.speed;
    new_status.tension_kg = mavlink_winch_status.tension;
    new_status.voltage_v = mavlink_winch_status.voltage;
    new_status.current_a = mavlink_winch_status.current;
    new_status.temperature_c = mavlink_winch_status.temperature;
    new_status.status_flags = status_flags;

    set_status(new_status);

    {
        std::lock_guard<std::mutex> lock(_subscription_mutex);
        _status_subscriptions.queue(
            status(), [this](const auto& func) { _system_impl->call_user_callback(func); });
    }
}

void WinchImpl::deinit() {}

void WinchImpl::enable() {}

void WinchImpl::disable() {}

void WinchImpl::relax_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::Relaxed);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::relax(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    relax_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::relative_length_control_async(
    uint32_t instance, float length, float rate, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::RelativeLengthControl);
    command.params.maybe_param3 = length;
    command.params.maybe_param4 = rate;

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::relative_length_control(uint32_t instance, float length, float rate)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    relative_length_control_async(
        instance, length, rate, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::rate_control_async(
    uint32_t instance, float rate, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::RateControl);
    command.params.maybe_param4 = rate;

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::rate_control(uint32_t instance, float rate)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    rate_control_async(instance, rate, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::lock_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::Lock);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::lock(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    lock_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::deliver_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::Deliver);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::deliver(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    deliver_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::hold_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::Hold);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::hold(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    hold_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::retract_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::Retract);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::retract(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    retract_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::load_line_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::LoadLine);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::load_line(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    load_line_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::abandon_line_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::AbandonLine);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::abandon_line(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    abandon_line_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchImpl::load_payload_async(uint32_t instance, const Winch::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Winch::WinchAction::LoadPayload);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Winch::Result WinchImpl::load_payload(uint32_t instance)
{
    auto prom = std::promise<Winch::Result>();
    auto fut = prom.get_future();

    load_payload_async(instance, [&prom](Winch::Result result) { prom.set_value(result); });

    return fut.get();
}

Winch::Result WinchImpl::winch_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return Winch::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Winch::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            // Fallthrough
        case MavlinkCommandSender::Result::Timeout:
            return Winch::Result::Timeout;
        case MavlinkCommandSender::Result::Busy:
            return Winch::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            // Fallthrough
        case MavlinkCommandSender::Result::TemporarilyRejected:
            // Fallthrough
        case MavlinkCommandSender::Result::Failed:
            return Winch::Result::Failed;
        case MavlinkCommandSender::Result::Unsupported:
            return Winch::Result::Unsupported;
        default:
            return Winch::Result::Unknown;
    }
}

void WinchImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const Winch::ResultCallback& callback) const
{
    Winch::Result action_result = winch_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _system_impl->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk