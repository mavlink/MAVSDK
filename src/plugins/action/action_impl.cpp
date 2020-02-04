#include "action_impl.h"
#include "mavsdk_impl.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <cmath>
#include <future>

namespace mavsdk {

using namespace std::placeholders; // for `_1`

ActionImpl::ActionImpl(System& system) : PluginImplBase(system)
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
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        1.0,
        nullptr,
        MAVLinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void ActionImpl::disable() {}

Action::Result ActionImpl::arm() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    arm_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::disarm() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    disarm_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::kill() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    kill_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::reboot() const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
    command.params.param1 = 1.0f; // reboot autopilot
    command.params.param2 = 1.0f; // reboot onboard computer
    command.params.param3 = 1.0f; // reboot camera
    command.params.param4 = 1.0f; // reboot gimbal
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

Action::Result ActionImpl::shutdown() const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
    command.params.param1 = 2.0f; // shutdown autopilot
    command.params.param2 = 2.0f; // shutdown onboard computer
    command.params.param3 = 2.0f; // shutdown camera
    command.params.param4 = 2.0f; // shutdown gimbal
    command.target_component_id = _parent->get_autopilot_id();

    return action_result_from_command_result(_parent->send_command(command));
}

Action::Result ActionImpl::takeoff() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    takeoff_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::land() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    land_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::return_to_launch() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    return_to_launch_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::goto_location(
    double latitude_deg, double longitude_deg, float altitude_amsl_m, float yaw_deg)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_REPOSITION;
    command.target_component_id = _parent->get_autopilot_id();
    command.params.param4 = to_rad_from_deg(yaw_deg);
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_amsl_m;

    return action_result_from_command_result(_parent->send_command(command));
}

Action::Result ActionImpl::transition_to_fixedwing() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    transition_to_fixedwing_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

Action::Result ActionImpl::transition_to_multicopter() const
{
    auto prom = std::promise<Action::Result>();
    auto fut = prom.get_future();

    transition_to_multicopter_async([&prom](Action::Result result) { prom.set_value(result); });

    return fut.get();
}

void ActionImpl::arm_async(const Action::result_callback_t& callback) const
{
    auto send_arm_command = [this, callback]() {
        MAVLinkCommands::CommandLong command{};

        command.command = MAV_CMD_COMPONENT_ARM_DISARM;
        command.params.param1 = 1.0f; // arm
        command.target_component_id = _parent->get_autopilot_id();

        _parent->send_command_async(
            command, [this, callback](MAVLinkCommands::Result result, float) {
                command_result_callback(result, callback);
            });
    };

    if (_parent->get_flight_mode() == SystemImpl::FlightMode::MISSION ||
        _parent->get_flight_mode() == SystemImpl::FlightMode::RETURN_TO_LAUNCH) {
        _parent->set_flight_mode_async(
            SystemImpl::FlightMode::HOLD,
            [callback, send_arm_command](MAVLinkCommands::Result result, float) {
                Action::Result action_result = action_result_from_command_result(result);
                if (action_result != Action::Result::SUCCESS) {
                    if (callback) {
                        callback(action_result);
                    }
                }
                send_arm_command();
            });
        return;
    }

    send_arm_command();
}

void ActionImpl::disarm_async(const Action::result_callback_t& callback) const
{
    Action::Result ret = disarming_allowed();
    if (ret != Action::Result::SUCCESS) {
        if (callback) {
            callback(ret);
        }
        return;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 0.0f; // disarm
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command, [this, callback](MAVLinkCommands::Result result, float) {
        command_result_callback(result, callback);
    });
}

void ActionImpl::kill_async(const Action::result_callback_t& callback) const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.params.param1 = 0.0f; // kill
    command.params.param2 = 21196.f; // magic number to enforce in-air
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command, [this, callback](MAVLinkCommands::Result result, float) {
        command_result_callback(result, callback);
    });
}

void ActionImpl::takeoff_async(const Action::result_callback_t& callback) const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_NAV_TAKEOFF;
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command, [this, callback](MAVLinkCommands::Result result, float) {
        command_result_callback(result, callback);
    });
}

void ActionImpl::land_async(const Action::result_callback_t& callback) const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_NAV_LAND;
    command.params.param4 = NAN; // Don't change yaw.
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command, [this, callback](MAVLinkCommands::Result result, float) {
        command_result_callback(result, callback);
    });
}

void ActionImpl::return_to_launch_async(const Action::result_callback_t& callback) const
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::RETURN_TO_LAUNCH,
        [this, callback](MAVLinkCommands::Result result, float) {
            command_result_callback(result, callback);
        });
}

void ActionImpl::transition_to_fixedwing_async(const Action::result_callback_t& callback) const
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

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_VTOL_TRANSITION;
    command.params.param1 = float(MAV_VTOL_STATE_FW);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command, [this, callback](MAVLinkCommands::Result result, float) {
        command_result_callback(result, callback);
    });
}

void ActionImpl::transition_to_multicopter_async(const Action::result_callback_t& callback) const
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
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_VTOL_TRANSITION;
    command.params.param1 = float(MAV_VTOL_STATE_MC);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(command, [this, callback](MAVLinkCommands::Result result, float) {
        command_result_callback(result, callback);
    });
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

void ActionImpl::process_extended_sys_state(const mavlink_message_t& message)
{
    mavlink_extended_sys_state_t extended_sys_state;
    mavlink_msg_extended_sys_state_decode(&message, &extended_sys_state);
    if (extended_sys_state.landed_state == MAV_LANDED_STATE_IN_AIR ||
        extended_sys_state.landed_state == MAV_LANDED_STATE_TAKEOFF ||
        extended_sys_state.landed_state == MAV_LANDED_STATE_LANDING) {
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

Action::Result ActionImpl::set_takeoff_altitude(float relative_altitude_m)
{
    const MAVLinkParameters::Result result =
        _parent->set_param_float(TAKEOFF_ALT_PARAM, relative_altitude_m);
    return (result == MAVLinkParameters::Result::SUCCESS) ? Action::Result::SUCCESS :
                                                            Action::Result::PARAMETER_ERROR;
}

std::pair<Action::Result, float> ActionImpl::get_takeoff_altitude() const
{
    auto result = _parent->get_param_float(TAKEOFF_ALT_PARAM);
    return std::make_pair<>(
        (result.first == MAVLinkParameters::Result::SUCCESS) ? Action::Result::SUCCESS :
                                                               Action::Result::PARAMETER_ERROR,
        result.second);
}

Action::Result ActionImpl::set_max_speed(float speed_m_s)
{
    const MAVLinkParameters::Result result = _parent->set_param_float(MAX_SPEED_PARAM, speed_m_s);
    return (result == MAVLinkParameters::Result::SUCCESS) ? Action::Result::SUCCESS :
                                                            Action::Result::PARAMETER_ERROR;
}

std::pair<Action::Result, float> ActionImpl::get_max_speed() const
{
    auto result = _parent->get_param_float(MAX_SPEED_PARAM);
    return std::make_pair<>(
        (result.first == MAVLinkParameters::Result::SUCCESS) ? Action::Result::SUCCESS :
                                                               Action::Result::PARAMETER_ERROR,
        result.second);
}

Action::Result ActionImpl::set_return_to_launch_return_altitude(float relative_altitude_m)
{
    const MAVLinkParameters::Result result =
        _parent->set_param_float(RTL_RETURN_ALTITUDE_PARAM, relative_altitude_m);
    return (result == MAVLinkParameters::Result::SUCCESS) ? Action::Result::SUCCESS :
                                                            Action::Result::PARAMETER_ERROR;
}

std::pair<Action::Result, float> ActionImpl::get_return_to_launch_return_altitude() const
{
    auto result = _parent->get_param_float(RTL_RETURN_ALTITUDE_PARAM);
    return std::make_pair<>(
        (result.first == MAVLinkParameters::Result::SUCCESS) ? Action::Result::SUCCESS :
                                                               Action::Result::PARAMETER_ERROR,
        result.second);
}

Action::Result ActionImpl::action_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Action::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return Action::Result::NO_SYSTEM;
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return Action::Result::CONNECTION_ERROR;
        case MAVLinkCommands::Result::BUSY:
            return Action::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Action::Result::COMMAND_DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
            return Action::Result::TIMEOUT;
        default:
            return Action::Result::UNKNOWN;
    }
}

void ActionImpl::command_result_callback(
    MAVLinkCommands::Result command_result, const Action::result_callback_t& callback) const
{
    Action::Result action_result = action_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _parent->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk
