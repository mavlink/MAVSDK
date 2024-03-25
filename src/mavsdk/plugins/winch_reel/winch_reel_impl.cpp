#include "winch_reel_impl.h"
#include "system.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<WinchReel::Status>;

// Note: The Watts Reel winch uses component ID 69, instead of the standard 169
// This is because the Reel was developed before the mavlink standard.
uint8_t winch_comp_id = 69;

WinchReelImpl::WinchReelImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

WinchReelImpl::WinchReelImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

WinchReelImpl::~WinchReelImpl()
{
    _system_impl->unregister_plugin(this);
}

void WinchReelImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_WINCH_STATUS,
        [this](const mavlink_message_t& message) { process_status(message); },
        this);
}

WinchReel::StatusHandle WinchReelImpl::subscribe_status(const WinchReel::StatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    return _status_subscriptions.subscribe(callback);
}

void WinchReelImpl::unsubscribe_status(WinchReel::StatusHandle handle)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _status_subscriptions.unsubscribe(handle);
}

void WinchReelImpl::set_status(WinchReel::Status status)
{
    std::lock_guard<std::mutex> lock(_status_mutex);
    _status = status;
}

WinchReel::Status WinchReelImpl::status() const
{
    std::lock_guard<std::mutex> lock(_status_mutex);
    return _status;
}

void WinchReelImpl::process_status(const mavlink_message_t& message)
{
    mavlink_winch_status_t mavlink_winch_status;
    mavlink_msg_winch_status_decode(&message, &mavlink_winch_status);

    WinchReel::Status new_status;
    new_status.time_usec = mavlink_winch_status.time_usec;
    new_status.line_length_m = mavlink_winch_status.line_length;
    new_status.speed_m_s = mavlink_winch_status.speed;
    new_status.tension_kg = mavlink_winch_status.tension;
    new_status.voltage_v = mavlink_winch_status.voltage;
    new_status.current_a = mavlink_winch_status.current;
    new_status.temperature_c = mavlink_winch_status.temperature;
    new_status.status_enum = static_cast<WinchReel::StatusEnum>(mavlink_winch_status.status);

    set_status(new_status);

    {
        std::lock_guard<std::mutex> lock(_subscription_mutex);
        _status_subscriptions.queue(
            status(), [this](const auto& func) { _system_impl->call_user_callback(func); });
    }
}

void WinchReelImpl::deinit() {}

void WinchReelImpl::enable() {}

void WinchReelImpl::disable() {}

void WinchReelImpl::relax_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::Relaxed);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::relax(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    relax_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchReelImpl::lock_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::Lock);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::lock(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    lock_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchReelImpl::deliver_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::Deliver);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::deliver(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    deliver_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchReelImpl::hold_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::Hold);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::hold(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    hold_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchReelImpl::retract_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::Retract);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::retract(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    retract_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchReelImpl::load_line_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::LoadLine);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::load_line(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    load_line_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

void WinchReelImpl::load_payload_async(uint32_t instance, const WinchReel::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_WINCH;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(WinchReel::WinchAction::LoadPayload);

    command.target_component_id = winch_comp_id;

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

WinchReel::Result WinchReelImpl::load_payload(uint32_t instance)
{
    auto prom = std::promise<WinchReel::Result>();
    auto fut = prom.get_future();

    load_payload_async(instance, [&prom](WinchReel::Result result) { prom.set_value(result); });

    return fut.get();
}

WinchReel::Result WinchReelImpl::winch_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return WinchReel::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return WinchReel::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            // Fallthrough
        case MavlinkCommandSender::Result::Timeout:
            return WinchReel::Result::Timeout;
        case MavlinkCommandSender::Result::Busy:
            return WinchReel::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            // Fallthrough
        case MavlinkCommandSender::Result::TemporarilyRejected:
            // Fallthrough
        case MavlinkCommandSender::Result::Failed:
            return WinchReel::Result::Failed;
        case MavlinkCommandSender::Result::Unsupported:
            return WinchReel::Result::Unsupported;
        default:
            return WinchReel::Result::Unknown;
    }
}

void WinchReelImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const WinchReel::ResultCallback& callback) const
{
    WinchReel::Result action_result = winch_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _system_impl->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk