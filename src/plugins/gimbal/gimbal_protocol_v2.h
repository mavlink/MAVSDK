#pragma once

#include "plugins/gimbal/gimbal.h"
#include "gimbal_protocol_base.h"

namespace mavsdk {

class GimbalProtocolV2 : public GimbalProtocolBase {
public:
    GimbalProtocolV2(
        SystemImpl& system_impl,
        const mavlink_gimbal_manager_information_t& information,
        uint8_t gimbal_manager_sysid,
        uint8_t gimbal_manager_compid);
    ~GimbalProtocolV2() = default;

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

private:
    uint8_t _gimbal_manager_sysid;
    uint8_t _gimbal_manager_compid;
    uint8_t _gimbal_device_id;

    Gimbal::GimbalMode _gimbal_mode{Gimbal::GimbalMode::YawFollow};
};

} // namespace mavsdk
