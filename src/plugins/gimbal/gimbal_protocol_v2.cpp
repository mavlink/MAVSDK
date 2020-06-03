#include "gimbal_protocol_v2.h"
#include "gimbal_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalProtocolV2::GimbalProtocolV2(
    SystemImpl& system_impl, const mavlink_gimbal_manager_information_t& information) :
    GimbalProtocolBase(system_impl)
{
    set_gimbal_information(information);
}

void GimbalProtocolV2::set_gimbal_information(
    const mavlink_gimbal_manager_information_t& information)
{
    _gimbal_device_id = information.gimbal_device_id;
}

Gimbal::Result GimbalProtocolV2::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_rad = 0.0f;
    const float pitch_rad = to_rad_from_deg(pitch_deg);
    const float yaw_rad = to_rad_from_deg(yaw_deg);

    float quaternion[4];
    mavlink_euler_to_quaternion(roll_rad, pitch_rad, yaw_rad, quaternion);

    const uint32_t flags = GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK;

    mavlink_message_t message;
    mavlink_msg_gimbal_manager_set_attitude_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &message,
        _system_impl.get_system_id(),
        _system_impl.get_autopilot_id(), // FIXME: this is hard-coded to autopilot for now
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

Gimbal::Result GimbalProtocolV2::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    UNUSED(gimbal_mode);

    return Gimbal::Result::Unsupported;
}

void GimbalProtocolV2::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    UNUSED(gimbal_mode);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback(
            [temp_callback]() { temp_callback(Gimbal::Result::Unsupported); });
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
