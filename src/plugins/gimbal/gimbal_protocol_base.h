#pragma once

#include "plugins/gimbal/gimbal.h"
#include "plugin_impl_base.h"
#include "system_impl.h"

namespace mavsdk {

class GimbalProtocolBase {
public:
    GimbalProtocolBase(SystemImpl& system_impl) :
        _system_impl(system_impl) {}
    virtual ~GimbalProtocolBase() = default;

    virtual Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg) = 0;

    virtual void set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result set_mode(const Gimbal::GimbalMode gimbal_mode) = 0;

    virtual void set_mode_async(const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result set_roi_location(double latitude_deg, double longitude_deg, float altitude_m) = 0;

    virtual void set_roi_location_async(
        double latitude_deg,
        double longitude_deg,
        float altitude_m,
        Gimbal::ResultCallback callback) = 0;

protected:
    SystemImpl &_system_impl;
};

} // namespace mavsdk
