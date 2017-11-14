#include "global_include.h"
#include "action_impl.h"
#include "dronecore_impl.h"
#include "telemetry.h"
#include "px4_custom_mode.h"

namespace dronecore {

using namespace std::placeholders; // for `_1`

ActionImpl::ActionImpl()
{
}

ActionImpl::~ActionImpl()
{

}

void ActionImpl::init()
{
    // We need the system state.
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        std::bind(&ActionImpl::process_extended_sys_state, this, _1), this);

    // And we need to make sure the system state is actually sent.
    // We use the async call here because we should not block in the init call because
    // we won't receive an answer anyway in init because the receive loop is not
    // called while we are being created here.
    _parent->set_msg_rate_async(MAVLINK_MSG_ID_EXTENDED_SYS_STATE, 1.0, nullptr,
                                MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

Action::Result ActionImpl::arm() const
{
    Action::Result ret = arming_allowed();
    if (ret != Action::Result::SUCCESS) {
        return ret;
    }

    // Go to LOITER mode first.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    ret = action_result_from_command_result(
              _parent->send_command_with_ack(
                  MAV_CMD_DO_SET_MODE,
                  MavlinkCommands::Params {float(mode),
                                           float(custom_mode),
                                           float(custom_sub_mode),
                                           NAN, NAN, NAN, NAN},
                  MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));

    if (ret != Action::Result::SUCCESS) {
        return ret;
    }

    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_COMPONENT_ARM_DISARM,
                   MavlinkCommands::Params {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Action::Result ActionImpl::disarm() const
{
    Action::Result ret = disarming_allowed();
    if (ret != Action::Result::SUCCESS) {
        return ret;
    }

    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_COMPONENT_ARM_DISARM,
                   MavlinkCommands::Params {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Action::Result ActionImpl::kill() const
{
    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_COMPONENT_ARM_DISARM,
                   MavlinkCommands::Params {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Action::Result ActionImpl::takeoff() const
{
    Action::Result ret = taking_off_allowed();
    if (ret != Action::Result::SUCCESS) {
        return ret;
    }

    // Go to LOITER mode first.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    ret = action_result_from_command_result(
              _parent->send_command_with_ack(
                  MAV_CMD_DO_SET_MODE,
                  MavlinkCommands::Params {float(mode),
                                           float(custom_mode),
                                           float(custom_sub_mode),
                                           NAN, NAN, NAN, NAN},
                  MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));

    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_NAV_TAKEOFF,
                   MavlinkCommands::Params {NAN, NAN, NAN, NAN, NAN, NAN,
                                            _relative_takeoff_altitude_m},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Action::Result ActionImpl::land() const
{
    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_NAV_LAND,
                   MavlinkCommands::Params {NAN, NAN, NAN, NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Action::Result ActionImpl::return_to_launch() const
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED |
                   flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;

    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_MODE,
                   MavlinkCommands::Params {float(mode),
                                            float(custom_mode),
                                            float(custom_sub_mode),
                                            NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Action::Result ActionImpl::transition_to_fixedwing() const
{
    if (!_vtol_transition_support_known) {
        return Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN;
    }

    if (!_vtol_transition_possible) {
        return Action::Result::NO_VTOL_TRANSITION_SUPPORT;
    }

    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_VTOL_TRANSITION,
                   MavlinkCommands::Params {float(MAV_VTOL_STATE_FW)},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

void ActionImpl::transition_to_fixedwing_async(const Action::result_callback_t &callback)
{
    if (!_vtol_transition_support_known) {
        if (callback) {
            callback(Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN);
        }
        return;
    }

    if (!_vtol_transition_possible) {
        if (callback) {
            callback(Action::Result::NO_VTOL_TRANSITION_SUPPORT);
        }
        return;
    }

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_VTOL_TRANSITION,
        MavlinkCommands::Params {float(MAV_VTOL_STATE_FW)},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

Action::Result ActionImpl::transition_to_multicopter() const
{
    if (!_vtol_transition_support_known) {
        return Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN;
    }

    if (!_vtol_transition_possible) {
        return Action::Result::NO_VTOL_TRANSITION_SUPPORT;
    }

    return action_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_VTOL_TRANSITION,
                   MavlinkCommands::Params {float(MAV_VTOL_STATE_MC)},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

void ActionImpl::transition_to_multicopter_async(const Action::result_callback_t &callback)
{
    if (!_vtol_transition_support_known) {
        if (callback) {
            callback(Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN);
        }
        return;
    }

    if (!_vtol_transition_possible) {
        if (callback) {
            callback(Action::Result::NO_VTOL_TRANSITION_SUPPORT);
        }
        return;
    }

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_VTOL_TRANSITION,
        MavlinkCommands::Params {float(MAV_VTOL_STATE_MC)},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::arm_async(const Action::result_callback_t &callback)
{
    Action::Result ret = arming_allowed();
    if (ret != Action::Result::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }

    loiter_before_arm_async(callback);

}

void ActionImpl::arm_async_continued(MavlinkCommands::Result previous_result,
                                     const Action::result_callback_t &callback)
{
    if (previous_result != MavlinkCommands::Result::SUCCESS) {
        command_result_callback(previous_result, callback);
        return;
    }

    _parent->send_command_with_ack_async(
        MAV_CMD_COMPONENT_ARM_DISARM,
        MavlinkCommands::Params {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::disarm_async(const Action::result_callback_t &callback)
{
    Action::Result ret = disarming_allowed();
    if (ret != Action::Result::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }

    _parent->send_command_with_ack_async(
        MAV_CMD_COMPONENT_ARM_DISARM,
        MavlinkCommands::Params {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::kill_async(const Action::result_callback_t &callback)
{
    _parent->send_command_with_ack_async(
        MAV_CMD_COMPONENT_ARM_DISARM,
        MavlinkCommands::Params {0.0f, NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::takeoff_async(const Action::result_callback_t &callback)
{
    Action::Result ret = taking_off_allowed();
    if (ret != Action::Result::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }

    loiter_before_takeoff_async(callback);
}


void ActionImpl::takeoff_async_continued(MavlinkCommands::Result previous_result,
                                         const Action::result_callback_t &callback)
{
    if (previous_result != MavlinkCommands::Result::SUCCESS) {
        command_result_callback(previous_result, callback);
        return;
    }

    _parent->send_command_with_ack_async(
        MAV_CMD_NAV_TAKEOFF,
        MavlinkCommands::Params {NAN, NAN, NAN, NAN, NAN, NAN,
                                 _relative_takeoff_altitude_m},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::land_async(const Action::result_callback_t &callback)
{
    _parent->send_command_with_ack_async(
        MAV_CMD_NAV_LAND,
        MavlinkCommands::Params {NAN, NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::return_to_launch_async(const Action::result_callback_t &callback)
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED |
                   flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::command_result_callback,
                  _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

Action::Result ActionImpl::arming_allowed() const
{
    if (!_in_air_state_known) {
        return Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN;
    }

    if (_in_air) {
        return Action::Result::COMMAND_DENIED_NOT_LANDED;
    }

    return Action::Result::SUCCESS;
}

Action::Result ActionImpl::taking_off_allowed() const
{
    if (!_in_air_state_known) {
        return Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN;
    }

    if (_in_air) {
        return Action::Result::COMMAND_DENIED_NOT_LANDED;
    }

    return Action::Result::SUCCESS;
}

Action::Result ActionImpl::disarming_allowed() const
{
    if (!_in_air_state_known) {
        return Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN;
    }

    if (_in_air) {
        return Action::Result::COMMAND_DENIED_NOT_LANDED;
    }

    return Action::Result::SUCCESS;
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

    if (extended_sys_state.vtol_state != MAV_VTOL_STATE_UNDEFINED) {
        _vtol_transition_possible = true;
    } else {
        _vtol_transition_possible = false;
    }
    _vtol_transition_support_known = true;
}

void ActionImpl::loiter_before_takeoff_async(const Action::result_callback_t &callback)
{
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::takeoff_async_continued, this, _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::loiter_before_arm_async(const Action::result_callback_t &callback)
{
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&ActionImpl::arm_async_continued, this, _1,
                  callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}


void ActionImpl::set_takeoff_altitude(float relative_altitude_m)
{
    _parent->set_param_float_async("MIS_TAKEOFF_ALT", relative_altitude_m,
                                   std::bind(&ActionImpl::receive_takeoff_alt_param,
                                             this, _1, relative_altitude_m));

}

void ActionImpl::receive_takeoff_alt_param(bool success, float new_relative_altitude_m)
{
    if (success) {
        // TODO: This should not be buffered like this, the param system
        //       needs some refactoring.
        _relative_takeoff_altitude_m = new_relative_altitude_m;
    }
}

float ActionImpl::get_takeoff_altitude_m() const
{
    return _relative_takeoff_altitude_m;
}

void ActionImpl::set_max_speed(float speed_m_s)
{
    // TODO: add retries
    //const int retries = 3;
    _parent->set_param_float_async("MPC_XY_CRUISE", speed_m_s,
                                   std::bind(&ActionImpl::receive_max_speed_result, this, _1,
                                             speed_m_s));
}

void ActionImpl::receive_max_speed_result(bool success, float new_speed_m_s)
{
    if (!success) {
        LogErr() << "setting max speed param failed";
        return;
    }
    _max_speed_m_s = new_speed_m_s;
}

float ActionImpl::get_max_speed_m_s() const
{
    return _max_speed_m_s;
}

Action::Result
ActionImpl::action_result_from_command_result(MavlinkCommands::Result result)
{
    switch (result) {
        case MavlinkCommands::Result::SUCCESS:
            return Action::Result::SUCCESS;
        case MavlinkCommands::Result::NO_DEVICE:
            return Action::Result::NO_DEVICE;
        case MavlinkCommands::Result::CONNECTION_ERROR:
            return Action::Result::CONNECTION_ERROR;
        case MavlinkCommands::Result::BUSY:
            return Action::Result::BUSY;
        case MavlinkCommands::Result::COMMAND_DENIED:
            return Action::Result::COMMAND_DENIED;
        case MavlinkCommands::Result::TIMEOUT:
            return Action::Result::TIMEOUT;
        default:
            return Action::Result::UNKNOWN;
    }
}

void ActionImpl::command_result_callback(MavlinkCommands::Result command_result,
                                         const Action::result_callback_t &callback)
{
    Action::Result action_result = action_result_from_command_result(command_result);

    if (callback) {
        callback(action_result);
    }
}


} // namespace dronecore
