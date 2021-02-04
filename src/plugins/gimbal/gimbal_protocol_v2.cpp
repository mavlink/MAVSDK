#include "gimbal_protocol_v2.h"
#include "gimbal_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalProtocolV2::GimbalProtocolV2(
    SystemImpl& system_impl,
    const mavlink_gimbal_manager_information_t& information,
    uint8_t gimbal_manager_sysid,
    uint8_t gimbal_manager_compid) :
    GimbalProtocolBase(system_impl),
    _gimbal_device_id(information.gimbal_device_id),
    _gimbal_manager_sysid(gimbal_manager_sysid),
    _gimbal_manager_compid(gimbal_manager_compid)
{}

Gimbal::Result GimbalProtocolV2::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_rad = 0.0f;
    const float pitch_rad = to_rad_from_deg(pitch_deg);
    const float yaw_rad = to_rad_from_deg(yaw_deg);

    float quaternion[4];
    mavlink_euler_to_quaternion(roll_rad, pitch_rad, yaw_rad, quaternion);

    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((_gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    mavlink_message_t message;
    mavlink_msg_gimbal_manager_set_attitude_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &message,
        _gimbal_manager_sysid,
        _gimbal_manager_compid,
        flags,
        _gimbal_device_id,
        quaternion,
        NAN,
        NAN,
        NAN);

    return _system_impl.send_message(message) ? Gimbal::Result::Success : Gimbal::Result::Error;
}

void GimbalProtocolV2::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    Gimbal::Result result = set_pitch_and_yaw(pitch_deg, yaw_deg);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback([temp_callback, result]() { temp_callback(result); });
    }
}

Gimbal::Result
GimbalProtocolV2::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s)
{
    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((_gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    const float quaternion[4] = {NAN, NAN, NAN, NAN};

    mavlink_message_t message;
    mavlink_msg_gimbal_manager_set_attitude_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &message,
        _gimbal_manager_sysid,
        _gimbal_manager_compid,
        flags,
        _gimbal_device_id,
        quaternion,
        0.0f,
        to_rad_from_deg(pitch_rate_deg_s),
        to_rad_from_deg(yaw_rate_deg_s));

    return _system_impl.send_message(message) ? Gimbal::Result::Success : Gimbal::Result::Error;
}

void GimbalProtocolV2::set_pitch_rate_and_yaw_rate_async(
    float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    Gimbal::Result result = set_pitch_rate_and_yaw_rate(pitch_rate_deg_s, yaw_rate_deg_s);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback([temp_callback, result]() { temp_callback(result); });
    }
}

Gimbal::Result GimbalProtocolV2::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    _gimbal_mode = gimbal_mode;
    return Gimbal::Result::Success;
}

void GimbalProtocolV2::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    _gimbal_mode = gimbal_mode;

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback(
            [temp_callback]() { temp_callback(Gimbal::Result::Success); });
    }
}

Gimbal::Result
GimbalProtocolV2::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    UNUSED(latitude_deg);
    UNUSED(longitude_deg);
    UNUSED(altitude_m);

    return Gimbal::Result::Unsupported;
}

void GimbalProtocolV2::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    UNUSED(latitude_deg);
    UNUSED(longitude_deg);
    UNUSED(altitude_m);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback(
            [temp_callback]() { temp_callback(Gimbal::Result::Unsupported); });
    }
}

} // namespace mavsdk
