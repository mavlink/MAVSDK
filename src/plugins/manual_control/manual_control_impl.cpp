#include "manual_control_impl.h"
#include <future>

namespace mavsdk {

ManualControlImpl::ManualControlImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ManualControlImpl::~ManualControlImpl()
{
    _parent->unregister_plugin(this);
}

void ManualControlImpl::init() {}

void ManualControlImpl::deinit() {}

void ManualControlImpl::enable() {}

void ManualControlImpl::disable() {}

void ManualControlImpl::start_position_control_async(const ManualControl::ResultCallback callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::Posctl, [this, callback](MAVLinkCommands::Result result, float) {
            command_result_callback(result, callback);
        });
}

ManualControl::Result ManualControlImpl::start_position_control()
{
    auto prom = std::promise<ManualControl::Result>();
    auto fut = prom.get_future();

    start_position_control_async([&prom](ManualControl::Result result) { prom.set_value(result); });

    return fut.get();
}

void ManualControlImpl::start_altitude_control_async(const ManualControl::ResultCallback callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::Altctl, [this, callback](MAVLinkCommands::Result result, float) {
            command_result_callback(result, callback);
        });
}

ManualControl::Result ManualControlImpl::start_altitude_control()
{
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

    // No buttons supported yet.
    const uint16_t buttons = 0;

    mavlink_message_t message;
    mavlink_msg_manual_control_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        static_cast<int16_t>(x * 1000),
        static_cast<int16_t>(y * 1000),
        static_cast<int16_t>(z * 1000),
        static_cast<int16_t>(r * 1000),
        buttons);
    return _parent->send_message(message) ? ManualControl::Result::Success :
                                            ManualControl::Result::ConnectionError;
}

ManualControl::Result
ManualControlImpl::manual_control_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::Success:
            return ManualControl::Result::Success;
        case MAVLinkCommands::Result::NoSystem:
            return ManualControl::Result::NoSystem;
        case MAVLinkCommands::Result::ConnectionError:
            return ManualControl::Result::ConnectionError;
        case MAVLinkCommands::Result::Busy:
            return ManualControl::Result::Busy;
        case MAVLinkCommands::Result::CommandDenied:
            return ManualControl::Result::CommandDenied;
        case MAVLinkCommands::Result::Timeout:
            return ManualControl::Result::Timeout;
        default:
            return ManualControl::Result::Unknown;
    }
}

void ManualControlImpl::command_result_callback(
    MAVLinkCommands::Result command_result, const ManualControl::ResultCallback& callback)
{
    ManualControl::Result action_result = manual_control_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _parent->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk
