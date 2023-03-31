#include "gimbal_impl.h"
#include "gimbal_protocol_v1.h"
#include "gimbal_protocol_v2.h"
#include "callback_list.tpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <thread>

namespace mavsdk {

template class CallbackList<Gimbal::ControlStatus>;

GimbalImpl::GimbalImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

GimbalImpl::GimbalImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

GimbalImpl::~GimbalImpl()
{
    _system_impl->unregister_plugin(this);
}

void GimbalImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION,
        [this](const mavlink_message_t& message) { process_gimbal_manager_information(message); },
        this);
}

void GimbalImpl::deinit() {}

void GimbalImpl::enable()
{
    _system_impl->register_timeout_handler(
        [this]() { receive_protocol_timeout(); }, 1.0, &_protocol_cookie);

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_REQUEST_MESSAGE;
    command.params.maybe_param1 = {static_cast<float>(MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION)};
    command.target_component_id = 0; // any component
    _system_impl->send_command_async(command, nullptr);
}

void GimbalImpl::disable()
{
    _gimbal_protocol.reset(nullptr);
}

void GimbalImpl::receive_protocol_timeout()
{
    // We did not receive a GIMBAL_MANAGER_INFORMATION in time, so we have to
    // assume Version2 is not available.
    LogDebug() << "Falling back to Gimbal Version 1";
    _gimbal_protocol.reset(new GimbalProtocolV1(*_system_impl));
    _protocol_cookie = nullptr;
}

void GimbalImpl::process_gimbal_manager_information(const mavlink_message_t& message)
{
    mavlink_gimbal_manager_information_t gimbal_manager_information;
    mavlink_msg_gimbal_manager_information_decode(&message, &gimbal_manager_information);

    if (_protocol_cookie != nullptr) {
        LogDebug() << "Using Gimbal Version 2 as gimbal manager information for gimbal device "
                   << static_cast<int>(gimbal_manager_information.gimbal_device_id)
                   << " was discovered";

        _system_impl->unregister_timeout_handler(_protocol_cookie);
        _protocol_cookie = nullptr;
        _gimbal_protocol.reset(new GimbalProtocolV2(
            *_system_impl, gimbal_manager_information, message.sysid, message.compid));
    }
}

Gimbal::Result GimbalImpl::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    wait_for_protocol();

    return _gimbal_protocol->set_pitch_and_yaw(pitch_deg, yaw_deg);
}

void GimbalImpl::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    wait_for_protocol_async(
        [=]() { _gimbal_protocol->set_pitch_and_yaw_async(pitch_deg, yaw_deg, callback); });
}

Gimbal::Result GimbalImpl::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s)
{
    wait_for_protocol();

    return _gimbal_protocol->set_pitch_rate_and_yaw_rate(pitch_rate_deg_s, yaw_rate_deg_s);
}

void GimbalImpl::set_pitch_rate_and_yaw_rate_async(
    float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback)
{
    wait_for_protocol_async([=]() {
        _gimbal_protocol->set_pitch_rate_and_yaw_rate_async(
            pitch_rate_deg_s, yaw_rate_deg_s, callback);
    });
}

Gimbal::Result GimbalImpl::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    wait_for_protocol();
    return _gimbal_protocol->set_mode(gimbal_mode);
}

void GimbalImpl::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    wait_for_protocol_async([=]() { _gimbal_protocol->set_mode_async(gimbal_mode, callback); });
}

Gimbal::Result
GimbalImpl::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    wait_for_protocol();
    return _gimbal_protocol->set_roi_location(latitude_deg, longitude_deg, altitude_m);
}

void GimbalImpl::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    wait_for_protocol_async([=]() {
        _gimbal_protocol->set_roi_location_async(latitude_deg, longitude_deg, altitude_m, callback);
    });
}

Gimbal::Result GimbalImpl::take_control(Gimbal::ControlMode control_mode)
{
    wait_for_protocol();
    return _gimbal_protocol->take_control(control_mode);
}

void GimbalImpl::take_control_async(
    Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback)
{
    wait_for_protocol_async(
        [=]() { _gimbal_protocol->take_control_async(control_mode, callback); });
}

Gimbal::Result GimbalImpl::release_control()
{
    wait_for_protocol();
    return _gimbal_protocol->release_control();
}

void GimbalImpl::release_control_async(Gimbal::ResultCallback callback)
{
    wait_for_protocol_async([=]() { _gimbal_protocol->release_control_async(callback); });
}

Gimbal::ControlStatus GimbalImpl::control()
{
    wait_for_protocol();
    return _gimbal_protocol->control();
}

Gimbal::ControlHandle GimbalImpl::subscribe_control(const Gimbal::ControlCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    bool need_to_register_callback = _control_subscriptions.empty();
    auto handle = _control_subscriptions.subscribe(callback);

    if (need_to_register_callback) {
        wait_for_protocol_async([=]() {
            _gimbal_protocol->control_async([this](Gimbal::ControlStatus status) {
                _control_subscriptions.queue(
                    status, [this](const auto& func) { _system_impl->call_user_callback(func); });
            });
        });
    }
    return handle;
}

void GimbalImpl::unsubscribe_control(Gimbal::ControlHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _control_subscriptions.unsubscribe(handle);

    if (_control_subscriptions.empty()) {
        wait_for_protocol_async([=]() { _gimbal_protocol->control_async(nullptr); });
    }
}

void GimbalImpl::wait_for_protocol()
{
    while (_gimbal_protocol == nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void GimbalImpl::wait_for_protocol_async(std::function<void()> callback)
{
    wait_for_protocol();
    callback();
}

void GimbalImpl::receive_command_result(
    MavlinkCommandSender::Result command_result, const Gimbal::ResultCallback& callback)
{
    Gimbal::Result gimbal_result = gimbal_result_from_command_result(command_result);

    if (callback) {
        callback(gimbal_result);
    }
}

Gimbal::Result
GimbalImpl::gimbal_result_from_command_result(MavlinkCommandSender::Result command_result)
{
    switch (command_result) {
        case MavlinkCommandSender::Result::Success:
            return Gimbal::Result::Success;
        case MavlinkCommandSender::Result::Timeout:
            return Gimbal::Result::Timeout;
        case MavlinkCommandSender::Result::NoSystem:
        case MavlinkCommandSender::Result::ConnectionError:
        case MavlinkCommandSender::Result::Busy:
        case MavlinkCommandSender::Result::Denied:
        case MavlinkCommandSender::Result::TemporarilyRejected:
        default:
            return Gimbal::Result::Error;
    }
}

} // namespace mavsdk
