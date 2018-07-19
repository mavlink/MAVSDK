#include "action_impl.h"
#include "dronecode_sdk_impl.h"
#include "global_include.h"
#include "px4_custom_mode.h"

namespace dronecode_sdk {

using namespace std::placeholders; // for `_1`

ActionImpl::ActionImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ActionImpl::~ActionImpl()
{
    _parent->unregister_plugin(this);
}

void ActionImpl::init()
{
    // We need the system state.
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        std::bind(&ActionImpl::process_extended_sys_state, this, _1),
        this);
}

void ActionImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void ActionImpl::enable()
{
    // And we need to make sure the system state is actually sent.
    // We use the async call here because we should not block in the init call because
    // we won't receive an answer anyway in init because the receive loop is not
    // called while we are being created here.
    _parent->set_msg_rate_async(MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
                                1.0,
                                nullptr,
                                MAVLinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);

    _parent->get_param_float_async(TAKEOFF_ALT_PARAM,
                                   std::bind(&ActionImpl::receive_takeoff_altitude_param,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
    _parent->get_param_float_async(MAX_SPEED_PARAM,
                                   std::bind(&ActionImpl::receive_max_speed_param,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
    _parent->get_param_float_async(TAKEOFF_ALT_PARAM,
                                   std::bind(&ActionImpl::receive_rtl_return_altitude_param,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
}

void ActionImpl::disable() {}

ActionResult ActionImpl::arm() const
{
    ActionResult ret = arming_allowed();
    if (ret != ActionResult::SUCCESS) {
        return ret;
    }

    // Go to LOITER mode first.
    ret = action_result_from_command_result(_parent->set_flight_mode(SystemImpl::FlightMode::HOLD));

    if (ret != ActionResult::SUCCESS) {
        return ret;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 1.0f; // arm
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

ActionResult ActionImpl::disarm() const
{
    ActionResult ret = disarming_allowed();
    if (ret != ActionResult::SUCCESS) {
        return ret;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 0.0f; // disarm
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

ActionResult ActionImpl::kill() const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 0.0f; // kill
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

ActionResult ActionImpl::takeoff() const
{
    ActionResult ret = taking_off_allowed();
    if (ret != ActionResult::SUCCESS) {
        return ret;
    }

    // Go to LOITER mode first.
    ret = action_result_from_command_result(_parent->set_flight_mode(SystemImpl::FlightMode::HOLD));

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_NAV_TAKEOFF;
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

ActionResult ActionImpl::land() const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_NAV_LAND;
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

ActionResult ActionImpl::return_to_launch() const
{
    return action_result_from_command_result(
        _parent->set_flight_mode(SystemImpl::FlightMode::RETURN_TO_LAUNCH));
}

ActionResult ActionImpl::transition_to_fixedwing() const
{
    if (!_vtol_transition_support_known) {
        return ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN;
    }

    if (!_vtol_transition_possible) {
        return ActionResult::NO_VTOL_TRANSITION_SUPPORT;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_VTOL_TRANSITION;
    command.params.param1 = float(MAV_VTOL_STATE_FW);
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

void ActionImpl::transition_to_fixedwing_async(const Action::result_callback_t &callback)
{
    if (!_vtol_transition_support_known) {
        if (callback) {
            callback(ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN);
        }
        return;
    }

    if (!_vtol_transition_possible) {
        if (callback) {
            callback(ActionResult::NO_VTOL_TRANSITION_SUPPORT);
        }
        return;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_VTOL_TRANSITION;
    command.params.param1 = float(MAV_VTOL_STATE_FW);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

ActionResult ActionImpl::transition_to_multicopter() const
{
    if (!_vtol_transition_support_known) {
        return ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN;
    }

    if (!_vtol_transition_possible) {
        return ActionResult::NO_VTOL_TRANSITION_SUPPORT;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_VTOL_TRANSITION;
    command.params.param1 = float(MAV_VTOL_STATE_MC);
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

void ActionImpl::transition_to_multicopter_async(const Action::result_callback_t &callback)
{
    if (!_vtol_transition_support_known) {
        if (callback) {
            callback(ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN);
        }
        return;
    }

    if (!_vtol_transition_possible) {
        if (callback) {
            callback(ActionResult::NO_VTOL_TRANSITION_SUPPORT);
        }
        return;
    }
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_VTOL_TRANSITION;
    command.params.param1 = float(MAV_VTOL_STATE_MC);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

void ActionImpl::arm_async(const Action::result_callback_t &callback)
{
    ActionResult ret = arming_allowed();
    if (ret != ActionResult::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }

    loiter_before_arm_async(callback);
}

void ActionImpl::arm_async_continued(MAVLinkCommands::Result previous_result,
                                     const Action::result_callback_t &callback)
{
    if (previous_result != MAVLinkCommands::Result::SUCCESS) {
        command_result_callback(previous_result, callback);
        return;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 1.0f; // arm
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

void ActionImpl::disarm_async(const Action::result_callback_t &callback)
{
    ActionResult ret = disarming_allowed();
    if (ret != ActionResult::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 0.0f; // disarm
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

void ActionImpl::kill_async(const Action::result_callback_t &callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 0.0f; // kill
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

void ActionImpl::takeoff_async(const Action::result_callback_t &callback)
{
    ActionResult ret = taking_off_allowed();
    if (ret != ActionResult::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }

    loiter_before_takeoff_async(callback);
}

void ActionImpl::takeoff_async_continued(MAVLinkCommands::Result previous_result,
                                         const Action::result_callback_t &callback)
{
    if (previous_result != MAVLinkCommands::Result::SUCCESS) {
        command_result_callback(previous_result, callback);
        return;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_NAV_TAKEOFF;
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

void ActionImpl::land_async(const Action::result_callback_t &callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_NAV_LAND;
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command,
                                std::bind(&ActionImpl::command_result_callback, _1, callback));
}

void ActionImpl::return_to_launch_async(const Action::result_callback_t &callback)
{
    _parent->set_flight_mode_async(SystemImpl::FlightMode::RETURN_TO_LAUNCH,
                                   std::bind(&ActionImpl::command_result_callback, _1, callback));
}

ActionResult ActionImpl::arming_allowed() const
{
    if (!_in_air_state_known) {
        return ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN;
    }

    if (_in_air) {
        return ActionResult::COMMAND_DENIED_NOT_LANDED;
    }

    return ActionResult::SUCCESS;
}

ActionResult ActionImpl::taking_off_allowed() const
{
    if (!_in_air_state_known) {
        return ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN;
    }

    if (_in_air) {
        return ActionResult::COMMAND_DENIED_NOT_LANDED;
    }

    return ActionResult::SUCCESS;
}

ActionResult ActionImpl::disarming_allowed() const
{
    if (!_in_air_state_known) {
        return ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN;
    }

    if (_in_air) {
        return ActionResult::COMMAND_DENIED_NOT_LANDED;
    }

    return ActionResult::SUCCESS;
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
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::HOLD,
        std::bind(&ActionImpl::takeoff_async_continued, this, _1, callback));
}

void ActionImpl::loiter_before_arm_async(const Action::result_callback_t &callback)
{
    _parent->set_flight_mode_async(SystemImpl::FlightMode::HOLD,
                                   std::bind(&ActionImpl::arm_async_continued, this, _1, callback));
}

ActionResult ActionImpl::set_takeoff_altitude(float relative_altitude_m)
{
    const bool success = _parent->set_param_float(TAKEOFF_ALT_PARAM, relative_altitude_m);
    return success ? ActionResult::SUCCESS : ActionResult::PARAMETER_ERROR;
}

void ActionImpl::receive_takeoff_altitude_param(bool success, float new_relative_altitude_m)
{
    if (success) {
        _relative_takeoff_altitude_m = new_relative_altitude_m;
    }
}

std::pair<ActionResult, float> ActionImpl::get_takeoff_altitude() const
{
    auto result = _parent->get_param_float(TAKEOFF_ALT_PARAM);
    return std::make_pair<>(result.first ? ActionResult::SUCCESS : ActionResult::PARAMETER_ERROR,
                            result.second);
}

ActionResult ActionImpl::set_max_speed(float speed_m_s)
{
    const bool success = _parent->set_param_float(MAX_SPEED_PARAM, speed_m_s);
    return success ? ActionResult::SUCCESS : ActionResult::PARAMETER_ERROR;
}

void ActionImpl::receive_max_speed_param(bool success, float new_speed_m_s)
{
    if (!success) {
        LogErr() << "setting max speed param failed";
        return;
    }
    _max_speed_m_s = new_speed_m_s;
}

void ActionImpl::receive_rtl_return_altitude_param(bool success, float new_rtl_return_alt_m)
{
    if (!success) {
        LogErr() << "setting RTL return alt failed";
        return;
    }
    _rtl_return_alt_m = new_rtl_return_alt_m;
}

std::pair<ActionResult, float> ActionImpl::get_max_speed() const
{
    auto result = _parent->get_param_float(MAX_SPEED_PARAM);
    return std::make_pair<>(result.first ? ActionResult::SUCCESS : ActionResult::PARAMETER_ERROR,
                            result.second);
}

ActionResult ActionImpl::set_return_to_launch_return_altitude(float relative_altitude_m)
{
    const bool success = _parent->set_param_float(RTL_RETURN_ALTITUDE_PARAM, relative_altitude_m);
    return success ? ActionResult::SUCCESS : ActionResult::PARAMETER_ERROR;
}

std::pair<ActionResult, float> ActionImpl::get_return_to_launch_return_altitude() const
{
    auto result = _parent->get_param_float(RTL_RETURN_ALTITUDE_PARAM);
    return std::make_pair<>(result.first ? ActionResult::SUCCESS : ActionResult::PARAMETER_ERROR,
                            result.second);
}

ActionResult ActionImpl::action_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return ActionResult::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return ActionResult::NO_SYSTEM;
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return ActionResult::CONNECTION_ERROR;
        case MAVLinkCommands::Result::BUSY:
            return ActionResult::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return ActionResult::COMMAND_DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
            return ActionResult::TIMEOUT;
        default:
            return ActionResult::UNKNOWN;
    }
}

void ActionImpl::command_result_callback(MAVLinkCommands::Result command_result,
                                         const Action::result_callback_t &callback)
{
    ActionResult action_result = action_result_from_command_result(command_result);

    if (callback) {
        callback(action_result);
    }
}

} // namespace dronecode_sdk
