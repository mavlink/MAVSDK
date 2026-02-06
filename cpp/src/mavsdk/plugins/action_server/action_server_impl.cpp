#include "action_server_impl.h"
#include "unused.h"
#include "flight_mode.h"
#include "callback_list.tpp"
#include "px4_custom_mode.h"

namespace mavsdk {

template class CallbackList<ActionServer::Result, ActionServer::ArmDisarm>;
template class CallbackList<ActionServer::Result, ActionServer::FlightMode>;
template class CallbackList<ActionServer::Result, bool>;

ActionServer::FlightMode
ActionServerImpl::telemetry_flight_mode_from_flight_mode(FlightMode flight_mode)
{
    switch (flight_mode) {
        case FlightMode::Ready:
            return ActionServer::FlightMode::Ready;
        case FlightMode::Takeoff:
            return ActionServer::FlightMode::Takeoff;
        case FlightMode::Hold:
            return ActionServer::FlightMode::Hold;
        case FlightMode::Mission:
            return ActionServer::FlightMode::Mission;
        case FlightMode::ReturnToLaunch:
            return ActionServer::FlightMode::ReturnToLaunch;
        case FlightMode::Land:
            return ActionServer::FlightMode::Land;
        case FlightMode::Offboard:
            return ActionServer::FlightMode::Offboard;
        case FlightMode::FollowMe:
            return ActionServer::FlightMode::FollowMe;
        case FlightMode::Manual:
            return ActionServer::FlightMode::Manual;
        case FlightMode::Posctl:
            return ActionServer::FlightMode::Posctl;
        case FlightMode::Altctl:
            return ActionServer::FlightMode::Altctl;
        case FlightMode::Acro:
            return ActionServer::FlightMode::Acro;
        case FlightMode::Stabilized:
            return ActionServer::FlightMode::Stabilized;
        default:
            return ActionServer::FlightMode::Unknown;
    }
}

uint32_t ActionServerImpl::to_px4_mode_from_flight_mode(ActionServer::FlightMode flight_mode)
{
    px4::px4_custom_mode px4_mode;
    switch (flight_mode) {
        case ActionServer::FlightMode::Ready:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_READY;
            break;
        case ActionServer::FlightMode::Takeoff:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case ActionServer::FlightMode::Hold:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case ActionServer::FlightMode::Mission:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case ActionServer::FlightMode::ReturnToLaunch:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case ActionServer::FlightMode::Land:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case ActionServer::FlightMode::Offboard:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            px4_mode.sub_mode = 0;
            break;
        case ActionServer::FlightMode::FollowMe:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
            px4_mode.sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case ActionServer::FlightMode::Manual:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_MANUAL;
            px4_mode.sub_mode = 0;
            break;
        case ActionServer::FlightMode::Posctl:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_POSCTL;
            px4_mode.sub_mode = 0;
            break;
        case ActionServer::FlightMode::Altctl:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_ALTCTL;
            px4_mode.sub_mode = 0;
            break;
        case ActionServer::FlightMode::Acro:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_ACRO;
            px4_mode.sub_mode = 0;
            break;
        case ActionServer::FlightMode::Stabilized:
            px4_mode.main_mode = px4::PX4_CUSTOM_MAIN_MODE_STABILIZED;
            px4_mode.sub_mode = 0;
            break;
        default:
            px4_mode.main_mode = 0; // unknown
            px4_mode.sub_mode = 0;
            break;
    }
    return px4_mode.data;
}

ActionServerImpl::ActionServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
}

ActionServerImpl::~ActionServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void ActionServerImpl::init()
{
    _send_version_cookie = _server_component_impl->add_call_every(
        [this]() { _server_component_impl->send_autopilot_version(); }, 1.0);

    // Arming / Disarm / Kill
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_COMPONENT_ARM_DISARM,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            ActionServer::ArmDisarm armDisarm{
                command.params.param1 == 1, command.params.param2 == 21196};

            std::lock_guard<std::mutex> lock(_callback_mutex);

            // Check arm states - Ugly.
            auto request_ack = MAV_RESULT_UNSUPPORTED;
            bool force = armDisarm.force;
            if (armDisarm.arm) {
                request_ack = (_armable || (force && _force_armable)) ?
                                  MAV_RESULT::MAV_RESULT_ACCEPTED :
                                  MAV_RESULT_TEMPORARILY_REJECTED;
            } else {
                request_ack = (_disarmable || (force && _force_disarmable)) ?
                                  MAV_RESULT::MAV_RESULT_ACCEPTED :
                                  MAV_RESULT_TEMPORARILY_REJECTED;
            }

            if (request_ack == MAV_RESULT::MAV_RESULT_ACCEPTED) {
                set_server_armed(armDisarm.arm);
            }

            auto result = (request_ack == MAV_RESULT::MAV_RESULT_ACCEPTED) ?
                              ActionServer::Result::Success :
                              ActionServer::Result::CommandDenied;

            _arm_disarm_callbacks.queue(result, armDisarm, [this](const auto& func) {
                _server_component_impl->call_user_callback(func);
            });

            return _server_component_impl->make_command_ack_message(command, request_ack);
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_NAV_TAKEOFF,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            std::lock_guard<std::mutex> lock(_callback_mutex);

            if (_allow_takeoff) {
                _takeoff_callbacks.queue(
                    ActionServer::Result::Success, true, [this](const auto& func) {
                        _server_component_impl->call_user_callback(func);
                    });

                return _server_component_impl->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_ACCEPTED);
            } else {
                _takeoff_callbacks.queue(
                    ActionServer::Result::CommandDenied, false, [this](const auto& func) {
                        _server_component_impl->call_user_callback(func);
                    });

                return _server_component_impl->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
            }
        },
        this);

    // Flight mode (long)
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_DO_SET_MODE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            auto base_mode = static_cast<uint8_t>(command.params.param1);
            auto is_custom = (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) ==
                             MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
            ActionServer::FlightMode request_flight_mode = ActionServer::FlightMode::Unknown;

            auto custom_mode = static_cast<uint8_t>(command.params.param2);
            auto sub_custom_mode = static_cast<uint8_t>(command.params.param3);

            if (is_custom) {
                // PX4 uses custom modes
                px4::px4_custom_mode px4_mode{};
                px4_mode.main_mode = custom_mode;
                px4_mode.sub_mode = sub_custom_mode;
                auto system_flight_mode = to_flight_mode_from_px4_mode(px4_mode.data);
                request_flight_mode = telemetry_flight_mode_from_flight_mode(system_flight_mode);
            } else {
                // TO DO: non PX4 flight modes...
                // Just bug out now if not using PX4 modes
                std::lock_guard<std::mutex> callback_lock(_callback_mutex);
                _flight_mode_change_callbacks.queue(
                    ActionServer::Result::ParameterError,
                    request_flight_mode,
                    [this](const auto& func) { _server_component_impl->call_user_callback(func); });

                return _server_component_impl->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
            }

            bool allow_mode = false;
            {
                std::lock_guard<std::mutex> flight_mode_lock(_flight_mode_mutex);
                switch (request_flight_mode) {
                    case ActionServer::FlightMode::Manual:
                        allow_mode = true;
                        break;
                    case ActionServer::FlightMode::Mission:
                        allow_mode = _allowed_flight_modes.can_auto_mode;
                        break;
                    case ActionServer::FlightMode::ReturnToLaunch:
                        allow_mode = _allowed_flight_modes.can_auto_rtl_mode;
                        break;
                    case ActionServer::FlightMode::Takeoff:
                        allow_mode = _allowed_flight_modes.can_auto_takeoff_mode;
                        break;
                    case ActionServer::FlightMode::Land:
                        allow_mode = _allowed_flight_modes.can_auto_land_mode;
                        break;
                    case ActionServer::FlightMode::Stabilized:
                        allow_mode = _allowed_flight_modes.can_stabilize_mode;
                        break;
                    case ActionServer::FlightMode::Offboard:
                        allow_mode = _allowed_flight_modes.can_guided_mode;
                        break;
                    default:
                        allow_mode = false;
                        break;
                }
            }

            // PX4...
            px4::px4_custom_mode px4_mode{};
            px4_mode.data = get_custom_mode();

            if (allow_mode) {
                px4_mode.main_mode = custom_mode;
                px4_mode.sub_mode = sub_custom_mode;
                uint8_t system_base_mode = get_base_mode();
                system_base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
                set_base_mode(system_base_mode);
                set_custom_mode(px4_mode.data);
            }

            {
                std::lock_guard<std::mutex> callback_lock(_callback_mutex);
                _flight_mode_change_callbacks.queue(
                    allow_mode ? ActionServer::Result::Success :
                                 ActionServer::Result::CommandDenied,
                    request_flight_mode,
                    [this](const auto& func) { _server_component_impl->call_user_callback(func); });
            }

            return _server_component_impl->make_command_ack_message(
                command,
                allow_mode ? MAV_RESULT::MAV_RESULT_ACCEPTED : MAV_RESULT_TEMPORARILY_REJECTED);
        },
        this);

    // Flight mode (legacy int, standard for PX4)
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SET_MODE,
        [this](const mavlink_message_t& message) {
            mavlink_set_mode_t set_mode;
            mavlink_msg_set_mode_decode(&message, &set_mode);

            auto base_mode = set_mode.base_mode;
            auto is_custom = (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) ==
                             MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
            ActionServer::FlightMode request_flight_mode = ActionServer::FlightMode::Unknown;

            std::lock_guard<std::mutex> lock(_callback_mutex);

            if (is_custom) {
                // for now, custom mode is assumed to be PX4
                auto system_flight_mode = to_flight_mode_from_px4_mode(set_mode.custom_mode);
                request_flight_mode = telemetry_flight_mode_from_flight_mode(system_flight_mode);
            } else {
                // TO DO: non PX4 flight modes...
                // Just bug out now if not using PX4 modes
                _flight_mode_change_callbacks.queue(
                    ActionServer::Result::ParameterError,
                    request_flight_mode,
                    [this](const auto& func) { _server_component_impl->call_user_callback(func); });

                // make_command_ack_message
                mavlink_command_ack_t command_ack{};
                command_ack.command = MAVLINK_MSG_ID_SET_MODE;
                command_ack.result = MAV_RESULT::MAV_RESULT_UNSUPPORTED;
                command_ack.progress = std::numeric_limits<uint8_t>::max();
                command_ack.result_param2 = 0;
                command_ack.target_system = message.sysid;
                command_ack.target_component = message.compid;
                _server_component_impl->send_command_ack(command_ack);
            }

            bool allow_mode = false;
            switch (request_flight_mode) {
                case ActionServer::FlightMode::Manual:
                    allow_mode = true;
                    break;
                case ActionServer::FlightMode::Mission:
                    allow_mode = _allowed_flight_modes.can_auto_mode;
                    break;
                case ActionServer::FlightMode::ReturnToLaunch:
                    allow_mode = _allowed_flight_modes.can_auto_rtl_mode;
                    break;
                case ActionServer::FlightMode::Takeoff:
                    allow_mode = _allowed_flight_modes.can_auto_takeoff_mode;
                    break;
                case ActionServer::FlightMode::Land:
                    allow_mode = _allowed_flight_modes.can_auto_land_mode;
                    break;
                case ActionServer::FlightMode::Stabilized:
                    allow_mode = _allowed_flight_modes.can_stabilize_mode;
                    break;
                case ActionServer::FlightMode::Offboard:
                    allow_mode = _allowed_flight_modes.can_guided_mode;
                    break;
                default:
                    allow_mode = false;
                    break;
            }

            if (allow_mode) {
                uint8_t system_base_mode = get_base_mode();
                system_base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
                set_base_mode(system_base_mode);
                set_custom_mode(set_mode.custom_mode);
            }

            _flight_mode_change_callbacks.queue(
                allow_mode ? ActionServer::Result::Success : ActionServer::Result::CommandDenied,
                request_flight_mode,
                [this](const auto& func) { _server_component_impl->call_user_callback(func); });

            // make_command_ack_message
            mavlink_command_ack_t command_ack{};
            command_ack.command = MAVLINK_MSG_ID_SET_MODE;
            command_ack.result =
                allow_mode ? MAV_RESULT::MAV_RESULT_ACCEPTED : MAV_RESULT_TEMPORARILY_REJECTED;
            command_ack.progress = std::numeric_limits<uint8_t>::max();
            command_ack.result_param2 = 0;
            command_ack.target_system = message.sysid;
            command_ack.target_component = message.compid;
            _server_component_impl->send_command_ack(command_ack);
        },
        this);
}

void ActionServerImpl::deinit()
{
    _server_component_impl->unregister_all_mavlink_command_handlers(this);
    _server_component_impl->remove_call_every(_send_version_cookie);
    _server_component_impl->unregister_all_mavlink_command_handlers(this);
}

ActionServer::ArmDisarmHandle
ActionServerImpl::subscribe_arm_disarm(const ActionServer::ArmDisarmCallback& callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _arm_disarm_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_arm_disarm(ActionServer::ArmDisarmHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _arm_disarm_callbacks.unsubscribe(handle);
}

ActionServer::FlightModeChangeHandle ActionServerImpl::subscribe_flight_mode_change(
    const ActionServer::FlightModeChangeCallback& callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _flight_mode_change_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_flight_mode_change(ActionServer::FlightModeChangeHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _flight_mode_change_callbacks.unsubscribe(handle);
}

ActionServer::TakeoffHandle
ActionServerImpl::subscribe_takeoff(const ActionServer::TakeoffCallback& callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _takeoff_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_takeoff(ActionServer::TakeoffHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _takeoff_callbacks.unsubscribe(handle);
}

ActionServer::LandHandle
ActionServerImpl::subscribe_land(const ActionServer::LandCallback& callback)
{
    // TODO: implement
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _land_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_land(ActionServer::LandHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _land_callbacks.unsubscribe(handle);
}

ActionServer::RebootHandle
ActionServerImpl::subscribe_reboot(const ActionServer::RebootCallback& callback)
{
    // TODO: implement
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _reboot_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_reboot(ActionServer::RebootHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _reboot_callbacks.unsubscribe(handle);
}

ActionServer::ShutdownHandle
ActionServerImpl::subscribe_shutdown(const ActionServer::ShutdownCallback& callback)
{
    // TODO: implement
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _shutdown_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_shutdown(ActionServer::ShutdownHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _shutdown_callbacks.unsubscribe(handle);
}

ActionServer::TerminateHandle
ActionServerImpl::subscribe_terminate(const ActionServer::TerminateCallback& callback)
{
    // TODO: implement
    std::lock_guard<std::mutex> lock(_callback_mutex);
    return _terminate_callbacks.subscribe(callback);
}

void ActionServerImpl::unsubscribe_terminate(ActionServer::TerminateHandle handle)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _terminate_callbacks.unsubscribe(handle);
}

ActionServer::Result ActionServerImpl::set_allow_takeoff(bool allow_takeoff)
{
    _allow_takeoff = allow_takeoff;
    return ActionServer::Result::Success;
}

ActionServer::Result ActionServerImpl::set_armable(bool armable, bool force_armable)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _armable = armable;
    _force_armable = force_armable;
    return ActionServer::Result::Success;
}

ActionServer::Result ActionServerImpl::set_disarmable(bool disarmable, bool force_disarmable)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _disarmable = disarmable;
    _force_disarmable = force_disarmable;
    return ActionServer::Result::Success;
}

ActionServer::Result
ActionServerImpl::set_allowable_flight_modes(ActionServer::AllowableFlightModes flight_modes)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _allowed_flight_modes = flight_modes;
    return ActionServer::Result::Success;
}

ActionServer::AllowableFlightModes ActionServerImpl::get_allowable_flight_modes()
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    return _allowed_flight_modes;
}

ActionServer::Result ActionServerImpl::set_armed_state(bool armed)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    ActionServer::ArmDisarm arm_disarm{};

    arm_disarm.arm = armed;
    arm_disarm.force = true;
    set_server_armed(armed);
    _arm_disarm_callbacks.queue(
        ActionServer::Result::Success, arm_disarm, [this](const auto& func) {
            _server_component_impl->call_user_callback(func);
        });
    return ActionServer::Result::Success;
}

ActionServer::Result ActionServerImpl::set_flight_mode(ActionServer::FlightMode flight_mode)
{
    ActionServer::Result res = set_flight_mode_internal(flight_mode);
    _flight_mode_change_callbacks.queue(res, flight_mode, [this](const auto& func) {
        _server_component_impl->call_user_callback(func);
    });

    return res;
}

ActionServer::Result
ActionServerImpl::set_flight_mode_internal(ActionServer::FlightMode flight_mode)
{
    // note: currently on receipt of MAV_CMD_DO_SET_MODE/MAV_CMD_SET_MODE, we error out if the mode
    // is *not* PX4 custom. For symmetry we will also convert from FlightMode to
    // PX4 custom when set directly.
    std::lock_guard<std::mutex> lock(_callback_mutex);

    px4::px4_custom_mode px4_mode{};
    px4_mode.data = to_px4_mode_from_flight_mode(flight_mode);

    uint8_t base_mode = get_base_mode();
    base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    set_base_mode(base_mode);
    set_custom_mode(px4_mode.data);
    return ActionServer::Result::Success;
}

void ActionServerImpl::set_base_mode(uint8_t base_mode)
{
    _server_component_impl->set_base_mode(base_mode);
}

uint8_t ActionServerImpl::get_base_mode() const
{
    return _server_component_impl->get_base_mode();
}

void ActionServerImpl::set_custom_mode(uint32_t custom_mode)
{
    _server_component_impl->set_custom_mode(custom_mode);
}

uint32_t ActionServerImpl::get_custom_mode() const
{
    return _server_component_impl->get_custom_mode();
}

void ActionServerImpl::set_server_armed(bool armed)
{
    uint8_t base_mode = get_base_mode();
    if (armed) {
        base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    } else {
        base_mode &= ~MAV_MODE_FLAG_SAFETY_ARMED;
    }
    set_base_mode(base_mode);
}

} // namespace mavsdk
