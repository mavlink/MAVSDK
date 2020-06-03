#include "gimbal_protocol_v2.h"
#include "gimbal_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalProtocolV2::GimbalProtocolV2(SystemImpl& system_impl) : GimbalProtocolBase(system_impl) {}

Gimbal::Result GimbalProtocolV2::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    UNUSED(pitch_deg);
    UNUSED(yaw_deg);

    return Gimbal::Result::Unsupported;
}

void GimbalProtocolV2::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    UNUSED(pitch_deg);
    UNUSED(yaw_deg);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback([temp_callback]() {
            temp_callback(Gimbal::Result::Unsupported);
        });
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
        _system_impl.call_user_callback([temp_callback]() {
            temp_callback(Gimbal::Result::Unsupported);
        });
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
        _system_impl.call_user_callback([temp_callback]() {
            temp_callback(Gimbal::Result::Unsupported);
        });
    }
}

} // namespace mavsdk
