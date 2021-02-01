#pragma once

#include <condition_variable>
#include <mutex>

#include "plugins/gimbal/gimbal.h"
#include "gimbal_protocol_base.h"

namespace mavsdk {

class GimbalProtocolV1 : public GimbalProtocolBase {
public:
    GimbalProtocolV1(SystemImpl& system_impl);
    ~GimbalProtocolV1() = default;

    Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg) override;
    void set_pitch_and_yaw_async(
        float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback) override;

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
    void take_control_async(Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback) override;

    Gimbal::Result release_control() override;
    void release_control_async(Gimbal::ResultCallback callback) override;

    Gimbal::ControlMode control() override;
    void control_async(Gimbal::ControlCallback callback) override;

private:
    static float to_float_gimbal_mode(const Gimbal::GimbalMode gimbal_mode);

    Gimbal::ControlMode _current_control_mode = Gimbal::ControlMode::None;
    Gimbal::ControlCallback _control_callback;

    std::condition_variable _control_thread_cv;
    std::mutex _control_thread_mutex;
};

} // namespace mavsdk
