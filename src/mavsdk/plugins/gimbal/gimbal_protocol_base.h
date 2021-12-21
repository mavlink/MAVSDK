#pragma once

#include "plugins/gimbal/gimbal.h"
#include "plugin_impl_base.h"
#include "system_impl.h"

namespace mavsdk {

class GimbalProtocolBase {
public:
    GimbalProtocolBase(SystemImpl& system_impl) : _system_impl(system_impl) {}
    virtual ~GimbalProtocolBase() = default;

    virtual Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg) = 0;
    virtual void
    set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result
    set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s) = 0;

    virtual void set_pitch_rate_and_yaw_rate_async(
        float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result set_mode(const Gimbal::GimbalMode gimbal_mode) = 0;
    virtual void
    set_mode_async(const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result
    set_roi_location(double latitude_deg, double longitude_deg, float altitude_m) = 0;
    virtual void set_roi_location_async(
        double latitude_deg,
        double longitude_deg,
        float altitude_m,
        Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result take_control(Gimbal::ControlMode control_mode) = 0;
    virtual void
    take_control_async(Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::Result release_control() = 0;
    virtual void release_control_async(Gimbal::ResultCallback callback) = 0;

    virtual Gimbal::ControlStatus control() = 0;
    virtual void control_async(Gimbal::ControlCallback callback) = 0;

protected:
    SystemImpl& _system_impl;
};

} // namespace mavsdk
