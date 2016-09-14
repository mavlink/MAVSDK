#include "global_include.h"
#include "action_impl.h"
#include "dronelink_impl.h"
#include "telemetry.h"
#include <unistd.h>

namespace dronelink {

ActionImpl::ActionImpl() :
    _in_air_state_known(false),
    _in_air(false)
{
}

ActionImpl::~ActionImpl()
{

}

void ActionImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        std::bind(&ActionImpl::process_extended_sys_state, this, _1), (void *)this);
}

void ActionImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers((void *)this);
}

Action::Result ActionImpl::arm() const
{
    if (!is_arm_allowed()) {
        return Action::Result::COMMAND_DENIED;
    }

    return action_result_from_command_result(
        _parent->send_command_with_ack(MAV_CMD_COMPONENT_ARM_DISARM,
                                       {1.0f, NAN, NAN, NAN, NAN, NAN, NAN}));
}

Action::Result ActionImpl::disarm() const
{
    if (!is_disarm_allowed()) {
        return Action::Result::COMMAND_DENIED;
    }

    return action_result_from_command_result(
        _parent->send_command_with_ack(MAV_CMD_COMPONENT_ARM_DISARM,
                                       {0.0f, NAN, NAN, NAN, NAN, NAN, NAN}));
}

Action::Result ActionImpl::kill() const
{
    return action_result_from_command_result(
        _parent->send_command_with_ack(MAV_CMD_COMPONENT_ARM_DISARM,
                                       {0.0f, NAN, NAN, NAN, NAN, NAN, NAN}));
}

Action::Result ActionImpl::takeoff() const
{
    return action_result_from_command_result(
        _parent->send_command_with_ack(MAV_CMD_NAV_TAKEOFF,
                                       {NAN, NAN, NAN, NAN, NAN, NAN, NAN}));
}

Action::Result ActionImpl::land() const
{
    return action_result_from_command_result(
        _parent->send_command_with_ack(MAV_CMD_NAV_LAND,
                                       {NAN, NAN, NAN, NAN, NAN, NAN, NAN}));
}

Action::Result ActionImpl::return_to_land() const
{
    uint8_t mode = MAV_MODE_AUTO_ARMED | VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t custom_mode = PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = PX4_CUSTOM_SUB_MODE_AUTO_RTL;

    return action_result_from_command_result(
        _parent->send_command_with_ack(MAV_CMD_DO_SET_MODE,
                                       {float(mode),
                                        float(custom_mode),
                                        float(custom_sub_mode),
                                        NAN, NAN, NAN, NAN}));
}

void ActionImpl::arm_async(Action::CallbackData callback_data)
{
    // TODO: support void *user
    if (!is_arm_allowed()) {
        report_result(callback_data, Action::Result::COMMAND_DENIED);
        return;
    }

    _parent->send_command_with_ack_async(MAV_CMD_COMPONENT_ARM_DISARM,
                                         {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {&command_result_callback, (void *)&callback_data});
}

void ActionImpl::disarm_async(Action::CallbackData callback_data)
{
    // TODO: support void *user
    if (!is_disarm_allowed()) {
        report_result(callback_data, Action::Result::COMMAND_DENIED);
        return;
    }

    _parent->send_command_with_ack_async(MAV_CMD_COMPONENT_ARM_DISARM,
                                         {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {&command_result_callback, (void *)&callback_data});
}

void ActionImpl::kill_async(Action::CallbackData callback_data)
{
    // TODO: support void *user
    _parent->send_command_with_ack_async(MAV_CMD_COMPONENT_ARM_DISARM,
                                         {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {&command_result_callback, (void *)&callback_data});
}

void ActionImpl::takeoff_async(Action::CallbackData callback_data)
{
    // TODO: support void *user
    _parent->send_command_with_ack_async(MAV_CMD_NAV_TAKEOFF,
                                         {NAN, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {&command_result_callback, (void *)&callback_data});
}

void ActionImpl::land_async(Action::CallbackData callback_data)
{
    // TODO: support void *user
    _parent->send_command_with_ack_async(MAV_CMD_NAV_LAND,
                                         {NAN, NAN, NAN, NAN, NAN, NAN, NAN},
                                         {&command_result_callback, (void *)&callback_data});
}

void ActionImpl::return_to_land_async(Action::CallbackData callback_data)
{

    uint8_t mode = MAV_MODE_AUTO_ARMED | VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t custom_mode = PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = PX4_CUSTOM_SUB_MODE_AUTO_RTL;

    _parent->send_command_with_ack_async(MAV_CMD_DO_SET_MODE,
                                         {float(mode),
                                          float(custom_mode),
                                          float(custom_sub_mode),
                                          NAN, NAN, NAN, NAN},
                                         {&command_result_callback, (void *)&callback_data});
}

bool ActionImpl::is_arm_allowed() const
{
    if (!_in_air_state_known) {
        return false;
    }

    if (_in_air) {
        return false;
    }

    return true;
}

bool ActionImpl::is_disarm_allowed() const
{
    if (!_in_air_state_known) {
        Debug() << "in air state unknown";
        return false;
    }

    if (_in_air) {
        Debug() << "still in air";
        return false;
    }

    return true;
}

void ActionImpl::process_extended_sys_state(const mavlink_message_t &message)
{
    mavlink_extended_sys_state_t extended_sys_state;
    mavlink_msg_extended_sys_state_decode(&message, &extended_sys_state);
    if (extended_sys_state.landed_state == MAV_LANDED_STATE_IN_AIR) {
        _in_air = true;
    } else if (extended_sys_state.landed_state == MAV_LANDED_STATE_ON_GROUND) {
        _in_air = false;
    }
    _in_air_state_known = true;
}

void ActionImpl::report_result(Action::CallbackData callback_data, Action::Result result)
{
    if (callback_data.callback == nullptr) {
        return;
    }

    callback_data.callback(result, callback_data.user);
}

Action::Result
ActionImpl::action_result_from_command_result(DeviceImpl::CommandResult result)
{
    switch (result) {
        case DeviceImpl::CommandResult::SUCCESS:
            return Action::Result::SUCCESS;
        case DeviceImpl::CommandResult::NO_DEVICE:
            return Action::Result::NO_DEVICE;
        case DeviceImpl::CommandResult::CONNECTION_ERROR:
             return Action::Result::CONNECTION_ERROR;
        case DeviceImpl::CommandResult::BUSY:
             return Action::Result::BUSY;
        case DeviceImpl::CommandResult::COMMAND_DENIED:
             return Action::Result::COMMAND_DENIED;
        case DeviceImpl::CommandResult::TIMEOUT:
            return Action::Result::TIMEOUT;
        default:
            return Action::Result::UNKNOWN;
    }
}

void ActionImpl::command_result_callback(DeviceImpl::CommandResult command_result,
                                         void *user)
{
    Action::Result action_result = action_result_from_command_result(command_result);

    Action::CallbackData *callback_data = reinterpret_cast<Action::CallbackData *>(user);

    callback_data->callback(action_result, callback_data->user);
}

} // namespace dronelink
