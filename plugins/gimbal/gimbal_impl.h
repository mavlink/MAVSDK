#pragma once

#include "system.h"
#include "mavlink_system.h"
#include "gimbal.h"
#include "plugin_impl_base.h"

namespace dronecore {

class GimbalImpl : public PluginImplBase
{
public:
    GimbalImpl(System &system);
    ~GimbalImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg);

    void set_pitch_and_yaw_async(float pitch_deg, float yaw_deg,
                                 Gimbal::result_callback_t callback);

    Gimbal::Result set_roi_location(double latitude_deg, double longitude_deg, float altitude_m);

    void set_roi_location_async(double latitude_deg, double longitude_deg, float altitude_m,
                                Gimbal::result_callback_t callback);

    // Non-copyable
    GimbalImpl(const GimbalImpl &) = delete;
    const GimbalImpl &operator=(const GimbalImpl &) = delete;

private:
    static Gimbal::Result gimbal_result_from_command_result(MAVLinkCommands::Result
                                                            command_result);

    static void receive_command_result(MAVLinkCommands::Result command_result,
                                       const Gimbal::result_callback_t &callback);
};


} // namespace dronecore
