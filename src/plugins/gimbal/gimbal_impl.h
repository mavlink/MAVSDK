#pragma once

#include "plugins/gimbal/gimbal.h"
#include "gimbal_v1.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class GimbalImpl : public PluginImplBase {
public:
    GimbalImpl(System& system);
    ~GimbalImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

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

    static Gimbal::Result gimbal_result_from_command_result(MAVLinkCommands::Result command_result);

    static void receive_command_result(
        MAVLinkCommands::Result command_result, const Gimbal::ResultCallback& callback);

    // Non-copyable
    GimbalImpl(const GimbalImpl&) = delete;
    const GimbalImpl& operator=(const GimbalImpl&) = delete;

private:
    enum class Protocol {
        Unknown,
        Version1,
        Version2,
    } _protocol{Protocol::Unknown};

    GimbalV1 _gimbal_v1;

    void* _protocol_cookie{nullptr};

    void receive_protocol_timeout();
    void process_gimbal_manager_information(const mavlink_message_t& message);
};

} // namespace mavsdk
