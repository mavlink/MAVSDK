#include "plugins/gimbal/gimbal_device.h"
#include "gimbal_device_impl.h"

namespace mavsdk {

GimbalDevice::GimbalDevice(System& system) : PluginBase(), _impl{new GimbalDeviceImpl(system)} {}

GimbalDevice::~GimbalDevice() {}

GimbalDevice::Result GimbalDevice::set_attitude(int operation_flags,
        float quat_w, float quat_x, float quat_y, float quat_z,
        float angular_velocity_x, float angular_velocity_y, float angular_velocity_z)
{
    return _impl->set_attitude(operation_flags, quat_w, quat_x, quat_y, quat_z,
            angular_velocity_x, angular_velocity_y, angular_velocity_z);
}

void GimbalDevice::set_attitude_async(int operation_flags,
        float quat_w, float quat_x, float quat_y, float quat_z,
        float angular_velocity_x, float angular_velocity_y, float angular_velocity_z,
        result_callback_t callback)
{
    _impl->set_attitude_async(operation_flags, quat_w, quat_x, quat_y, quat_z,
            angular_velocity_x, angular_velocity_y, angular_velocity_z, callback);
}

void GimbalDevice::post_autopilot_state(uint64_t timestamp,
        float quat_w, float quat_x, float quat_y, float quat_z,
        uint32_t quat_estimated_delay_us,
        float vel_x, float vel_y, float vel_z, uint32_t vel_estimated_delay_us,
        float feed_forward_angular_velocity_z)
{
    _impl->post_autopilot_state(timestamp, quat_w, quat_x, quat_y, quat_z,
            quat_estimated_delay_us,
            vel_x, vel_y, vel_z, vel_estimated_delay_us,
            feed_forward_angular_velocity_z);
}

const char* GimbalDevice::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::ERROR:
            return "Error";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace mavsdk
