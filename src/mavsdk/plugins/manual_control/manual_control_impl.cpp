#include "manual_control_impl.h"
#include <future>

namespace mavsdk {

ManualControlImpl::ManualControlImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

ManualControlImpl::ManualControlImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

ManualControlImpl::~ManualControlImpl()
{
    _system_impl->unregister_plugin(this);
}

void ManualControlImpl::init() {}

void ManualControlImpl::deinit() {}

void ManualControlImpl::enable() {}

void ManualControlImpl::disable() {}

void ManualControlImpl::start_position_control_async(const ManualControl::ResultCallback callback)
{
    if (_input == Input::NotSet) {
        if (callback) {
            auto temp_callback = callback;
            _system_impl->call_user_callback(
                [temp_callback]() { temp_callback(ManualControl::Result::InputNotSet); });
        }
        return;
    }

    _system_impl->set_flight_mode_async(
        FlightMode::Posctl, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

ManualControl::Result ManualControlImpl::start_position_control()
{
    if (_input == Input::NotSet) {
        return ManualControl::Result::InputNotSet;
    }

    auto prom = std::promise<ManualControl::Result>();
    auto fut = prom.get_future();

    start_position_control_async([&prom](ManualControl::Result result) { prom.set_value(result); });

    return fut.get();
}

void ManualControlImpl::start_altitude_control_async(const ManualControl::ResultCallback callback)
{
    if (_input == Input::NotSet) {
        if (callback) {
            auto temp_callback = callback;
            _system_impl->call_user_callback(
                [temp_callback]() { temp_callback(ManualControl::Result::InputNotSet); });
        }
        return;
    }
    _system_impl->set_flight_mode_async(
        FlightMode::Altctl, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

ManualControl::Result ManualControlImpl::start_altitude_control()
{
    if (_input == Input::NotSet) {
        return ManualControl::Result::InputNotSet;
    }
    auto prom = std::promise<ManualControl::Result>();
    auto fut = prom.get_future();

    start_altitude_control_async([&prom](ManualControl::Result result) { prom.set_value(result); });

    return fut.get();
}

ManualControl::Result
ManualControlImpl::set_manual_control_input(float x, float y, float z, float r)
{
    if (x > 1.f || x < -1.f || y > 1.f || y < -1.f || z > 1.f || z < 0.f || r > 1.f || r < -1.f) {
        return ManualControl::Result::InputOutOfRange;
    }

    if (_input == Input::NotSet) {
        _input = Input::Set;
    }

    // No buttons/extensions supported yet.
    const uint16_t buttons = 0;
    const uint16_t buttons2 = 0;
    const uint8_t enabled_extensions = 0;
    const int16_t pitch_only_axis = 0;
    const int16_t roll_only_axis = 0;

    mavlink_message_t message;
    mavlink_msg_manual_control_pack(
        _system_impl->get_own_system_id(),
        _system_impl->get_own_component_id(),
        &message,
        _system_impl->get_system_id(),
        static_cast<int16_t>(x * 1000),
        static_cast<int16_t>(y * 1000),
        static_cast<int16_t>(z * 1000),
        static_cast<int16_t>(r * 1000),
        buttons,
        buttons2,
        enabled_extensions,
        pitch_only_axis,
        roll_only_axis);
    return _system_impl->send_message(message) ? ManualControl::Result::Success :
                                                 ManualControl::Result::ConnectionError;
}

ManualControl::Result
ManualControlImpl::manual_control_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return ManualControl::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return ManualControl::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return ManualControl::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return ManualControl::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return ManualControl::Result::CommandDenied;
        case MavlinkCommandSender::Result::Timeout:
            return ManualControl::Result::Timeout;
        default:
            return ManualControl::Result::Unknown;
    }
}

void ManualControlImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const ManualControl::ResultCallback& callback)
{
    ManualControl::Result action_result = manual_control_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _system_impl->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk
