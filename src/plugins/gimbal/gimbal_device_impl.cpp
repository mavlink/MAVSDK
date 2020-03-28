#include "gimbal_device_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalDeviceImpl::GimbalDeviceImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GimbalDeviceImpl::~GimbalDeviceImpl()
{
    _parent->unregister_plugin(this);
}

void GimbalDeviceImpl::init() {}

void GimbalDeviceImpl::deinit() {}

void GimbalDeviceImpl::enable() {}

void GimbalDeviceImpl::disable() {}

void GimbalDeviceImpl::set_device_id(uint8_t system_id, uint8_t component_id)
{
    _address = {system_id, component_id};
}

GimbalDevice::Result GimbalDeviceImpl::set_attitude(int operation_flags,
        float quat_w, float quat_x, float quat_y, float quat_z,
        float angular_velocity_x, float angular_velocity_y, float angular_velocity_z)
{
    mavlink_message_t message;
    float q[4] = {quat_w, quat_x, quat_y, quat_z};

    mavlink_msg_gimbal_device_set_attitude_pack(
            _address.system_id,
            _address.component_id,
            &message,
            operation_flags,
            q,
            angular_velocity_x,
            angular_velocity_y,
            angular_velocity_z);
}

} // namespace mavsdk
