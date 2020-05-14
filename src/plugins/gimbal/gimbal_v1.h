#pragma once

#include "plugins/gimbal/gimbal.h"
#include "plugin_impl_base.h"
#include "system_impl.h"

namespace mavsdk {

class GimbalV1 {
public:
    GimbalV1(SystemImpl& system_impl);
    ~GimbalV1() = default;

    Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg);

    void set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback);

    Gimbal::Result set_mode(const Gimbal::GimbalMode gimbal_mode);

    void set_mode_async(const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback);

    Gimbal::Result set_roi_location(double latitude_deg, double longitude_deg, float altitude_m);

    void set_roi_location_async(
        double latitude_deg,
        double longitude_deg,
        float altitude_m,
        Gimbal::ResultCallback callback);

private:
    static float to_float_gimbal_mode(const Gimbal::GimbalMode gimbal_mode);

    SystemImpl &_system_impl;
};

} // namespace mavsdk
