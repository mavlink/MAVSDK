#pragma once

#include "plugins/gimbal/gimbal_device.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class GimbalDeviceImpl : public PluginImplBase {
public:
    GimbalDeviceImpl(System& system);
    ~GimbalDeviceImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    GimbalDevice::Result set_attitude(int operation_flags,
            float quat_w, float quat_x, float quat_y, float quat_z,
            float angular_velocity_x, float angular_velocity_y, float angular_velocity_z);

    void set_attitude_async(int operation_flags,
            float quat_w, float quat_x, float quat_y, float quat_z,
            float angular_velocity_x, float angular_velocity_y, float angular_velocity_z,
            GimbalDevice::result_callback_t callback);

    void post_autopilot_state(uint64_t timestamp,
            float quat_w, float quat_x, float quat_y, float quat_z,
            uint32_t quat_estimated_delay_us,
            float vel_x, float vel_y, float vel_z, uint32_t vel_estimated_delay_us,
            float feed_forward_angular_velocity_z);

    // Non-copyable
    GimbalDeviceImpl(const GimbalDeviceImpl&) = delete;
    const GimbalDeviceImpl& operator=(const GimbalDeviceImpl&) = delete;
private:
    static GimbalDevice::Result gimbal_device_result_from_command_result(MAVLinkCommands::Result command_result);

    static void receive_command_result(
            MAVLinkCommands::Result command_result, const GimbalDevice::result_callback_t& callback);
};

} // namespace mavsdk
