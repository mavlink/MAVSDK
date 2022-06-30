#pragma once

#include <condition_variable>
#include <mutex>

#include "plugins/gimbal/gimbal.h"
#include "gimbal_protocol_base.h"

namespace mavsdk {

class GimbalProtocolV1 : public GimbalProtocolBase {
public:
    GimbalProtocolV1(SystemImpl& system_impl);
    ~GimbalProtocolV1() override;

    Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg) override;
    void set_pitch_and_yaw_async(
        float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback) override;

    Gimbal::Result
    set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s) override;

    void set_pitch_rate_and_yaw_rate_async(
        float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback) override;

    Gimbal::Result set_mode(const Gimbal::GimbalMode gimbal_mode) override;
    void
    set_mode_async(const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback) override;

    Gimbal::Result
    set_roi_location(double latitude_deg, double longitude_deg, float altitude_m) override;
    void set_roi_location_async(
        double latitude_deg,
        double longitude_deg,
        float altitude_m,
        Gimbal::ResultCallback callback) override;

    Gimbal::Result take_control(Gimbal::ControlMode control_mode) override;
    void
    take_control_async(Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback) override;

    Gimbal::Result release_control() override;
    void release_control_async(Gimbal::ResultCallback callback) override;

    Gimbal::ControlStatus control() override;
    void control_async(Gimbal::ControlCallback callback) override;

private:
    static float to_float_gimbal_mode(const Gimbal::GimbalMode gimbal_mode);

    Gimbal::ControlStatus _current_control_status{Gimbal::ControlMode::None, 0, 0, 0, 0};
    Gimbal::ControlCallback _control_callback;

    void* _control_cookie{nullptr};
};

} // namespace mavsdk
