#include "global_include.h"
#include "control_impl.h"
#include "dronelink_impl.h"
#include <unistd.h>

namespace dronelink {

ControlImpl::ControlImpl()
{
}

ControlImpl::~ControlImpl()
{

}

void ControlImpl::init()
{

}

void ControlImpl::deinit()
{

}

Result ControlImpl::arm() const
{
    return _parent->send_command_with_ack(MAV_CMD_COMPONENT_ARM_DISARM,
                                          {1.0f, NAN, NAN, NAN, NAN, NAN, NAN});
}

Result ControlImpl::disarm() const
{
    return _parent->send_command_with_ack(MAV_CMD_COMPONENT_ARM_DISARM,
                                          {0.0f, NAN, NAN, NAN, NAN, NAN, NAN});
}

Result ControlImpl::takeoff() const
{
    return _parent->send_command_with_ack(MAV_CMD_NAV_TAKEOFF,
                                          {NAN, NAN, NAN, NAN, NAN, NAN, NAN});
}

Result ControlImpl::land() const
{
    return _parent->send_command_with_ack(MAV_CMD_NAV_LAND,
                                          {NAN, NAN, NAN, NAN, NAN, NAN, NAN});
}

Result ControlImpl::return_to_land() const
{
    uint8_t mode = MAV_MODE_AUTO_ARMED | VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t custom_mode = PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = PX4_CUSTOM_SUB_MODE_AUTO_RTL;

    return _parent->send_command_with_ack(MAV_CMD_DO_SET_MODE,
                                          {float(mode),
                                           float(custom_mode),
                                           float(custom_sub_mode),
                                           NAN, NAN, NAN, NAN});
}

void ControlImpl::arm_async(result_callback_t callback)
{
    _parent->send_command_with_ack_async(MAV_CMD_COMPONENT_ARM_DISARM,
                                         {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {callback, nullptr});
}

void ControlImpl::disarm_async(result_callback_t callback)
{
    _parent->send_command_with_ack_async(MAV_CMD_COMPONENT_ARM_DISARM,
                                         {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {callback, nullptr});
}

void ControlImpl::takeoff_async(result_callback_t callback)
{
    _parent->send_command_with_ack_async(MAV_CMD_NAV_TAKEOFF,
                                         {NAN, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {callback, nullptr});
}

void ControlImpl::land_async(result_callback_t callback)
{
    _parent->send_command_with_ack_async(MAV_CMD_NAV_LAND,
                                         {NAN, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {callback, nullptr});
}

void ControlImpl::return_to_land_async(result_callback_t callback)
{

    uint8_t mode = MAV_MODE_AUTO_ARMED | VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t custom_mode = PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = PX4_CUSTOM_SUB_MODE_AUTO_RTL;

    _parent->send_command_with_ack_async(MAV_CMD_DO_SET_MODE,
                                         {float(mode),
                                          float(custom_mode),
                                          float(custom_sub_mode),
                                          NAN, NAN, NAN, NAN},
                                         {callback, nullptr});
}

} // namespace dronelink
