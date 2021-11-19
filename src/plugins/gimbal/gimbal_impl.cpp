#include "gimbal_impl.h"
#include "global_include.h"
#include "gimbal_protocol_v1.h"
#include "gimbal_protocol_v2.h"
#include <chrono>
#include <cmath>
#include <functional>
#include <thread>

namespace mavsdk {

GimbalImpl::GimbalImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GimbalImpl::GimbalImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GimbalImpl::~GimbalImpl()
{
    _parent->unregister_plugin(this);
}

void GimbalImpl::init() {}

void GimbalImpl::deinit() {}

void GimbalImpl::enable() {}

Gimbal::Result GimbalImpl::prepare()
{
    auto prom = std::make_shared<std::promise<Gimbal::Result>>();
    auto ret = prom->get_future();

    prepare_async([&prom](Gimbal::Result result) { prom->set_value(result); });

    return ret.get();
}

void GimbalImpl::prepare_async(const Gimbal::ResultCallback& callback)
{
    if (_gimbal_protocol != nullptr) {
        _parent->call_user_callback([callback]() { callback(Gimbal::Result::Success); });
    } else {
        _parent->request_message().request(
            MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION,
            [this,
             callback](MavlinkCommandSender::Result result, const mavlink_message_t& message) {
                if (result == MavlinkCommandSender::Result::Success) {
                    mavlink_gimbal_manager_information_t gimbal_manager_information;
                    mavlink_msg_gimbal_manager_information_decode(
                        &message, &gimbal_manager_information);

                    LogDebug() << "Gimbal manager information for gimbal device "
                               << static_cast<int>(gimbal_manager_information.gimbal_device_id)
                               << " was discovered";

                    _gimbal_protocol.reset(new GimbalProtocolV2(
                        *_parent, gimbal_manager_information, message.sysid, message.compid));

                    _parent->call_user_callback(
                        [callback]() { callback(Gimbal::Result::Success); });
                } else {
                    _parent->call_user_callback([callback, result]() {
                        callback(gimbal_result_from_command_result(result));
                    });
                }
            },
            0); // any component
    }
} // namespace mavsdk

void GimbalImpl::disable()
{
    _gimbal_protocol.reset(nullptr);
}

Gimbal::Result GimbalImpl::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return Gimbal::Result::Error;
    }

    return _gimbal_protocol->set_pitch_and_yaw(pitch_deg, yaw_deg);
}

void GimbalImpl::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        _parent->call_user_callback([callback]() {
            LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
            callback(Gimbal::Result::Error);
        });

        return;
    }

    _gimbal_protocol->set_pitch_and_yaw_async(pitch_deg, yaw_deg, callback);
}

Gimbal::Result GimbalImpl::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s)
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return Gimbal::Result::Error;
    }

    return _gimbal_protocol->set_pitch_rate_and_yaw_rate(pitch_rate_deg_s, yaw_rate_deg_s);
}

void GimbalImpl::set_pitch_rate_and_yaw_rate_async(
    float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        _parent->call_user_callback([callback]() {
            LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
            callback(Gimbal::Result::Error);
        });

        return;
    }

    _gimbal_protocol->set_pitch_rate_and_yaw_rate_async(pitch_rate_deg_s, yaw_rate_deg_s, callback);
}

Gimbal::Result GimbalImpl::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return Gimbal::Result::Error;
    }

    return _gimbal_protocol->set_mode(gimbal_mode);
}

void GimbalImpl::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        _parent->call_user_callback([callback]() {
            LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
            callback(Gimbal::Result::Error);
        });

        return;
    }

    _gimbal_protocol->set_mode_async(gimbal_mode, callback);
}

Gimbal::Result
GimbalImpl::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return Gimbal::Result::Error;
    }

    return _gimbal_protocol->set_roi_location(latitude_deg, longitude_deg, altitude_m);
}

void GimbalImpl::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        _parent->call_user_callback([callback]() {
            LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
            callback(Gimbal::Result::Error);
        });

        return;
    }

    _gimbal_protocol->set_roi_location_async(latitude_deg, longitude_deg, altitude_m, callback);
}

Gimbal::Result GimbalImpl::take_control(Gimbal::ControlMode control_mode)
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return Gimbal::Result::Error;
    }

    return _gimbal_protocol->take_control(control_mode);
}

void GimbalImpl::take_control_async(
    Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        _parent->call_user_callback([callback]() {
            LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
            callback(Gimbal::Result::Error);
        });

        return;
    }

    _gimbal_protocol->take_control_async(control_mode, callback);
}

Gimbal::Result GimbalImpl::release_control()
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return Gimbal::Result::Error;
    }

    return _gimbal_protocol->release_control();
}

void GimbalImpl::release_control_async(Gimbal::ResultCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        _parent->call_user_callback([callback]() {
            LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
            callback(Gimbal::Result::Error);
        });

        return;
    }

    _gimbal_protocol->release_control_async(callback);
}

Gimbal::ControlStatus GimbalImpl::control()
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";
        return {};
    }

    return _gimbal_protocol->control();
}

void GimbalImpl::subscribe_control(Gimbal::ControlCallback callback)
{
    if (_gimbal_protocol == nullptr) {
        LogErr() << "Gimbal plugin not prepared! Call `prepare()` first!";

        return;
    }

    _gimbal_protocol->control_async(callback);
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
        case MavlinkCommandSender::Result::CommandDenied:
        default:
            return Gimbal::Result::Error;
    }
}

} // namespace mavsdk
