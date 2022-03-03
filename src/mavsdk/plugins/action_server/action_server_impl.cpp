#include "action_server_impl.h"
#include "unused.h"

namespace mavsdk {

ActionServer::FlightMode telemetry_flight_mode_from_flight_mode(SystemImpl::FlightMode flight_mode)
{
    switch (flight_mode) {
        case SystemImpl::FlightMode::Ready:
            return ActionServer::FlightMode::Ready;
        case SystemImpl::FlightMode::Takeoff:
            return ActionServer::FlightMode::Takeoff;
        case SystemImpl::FlightMode::Hold:
            return ActionServer::FlightMode::Hold;
        case SystemImpl::FlightMode::Mission:
            return ActionServer::FlightMode::Mission;
        case SystemImpl::FlightMode::ReturnToLaunch:
            return ActionServer::FlightMode::ReturnToLaunch;
        case SystemImpl::FlightMode::Land:
            return ActionServer::FlightMode::Land;
        case SystemImpl::FlightMode::Offboard:
            return ActionServer::FlightMode::Offboard;
        case SystemImpl::FlightMode::FollowMe:
            return ActionServer::FlightMode::FollowMe;
        case SystemImpl::FlightMode::Manual:
            return ActionServer::FlightMode::Manual;
        case SystemImpl::FlightMode::Posctl:
            return ActionServer::FlightMode::Posctl;
        case SystemImpl::FlightMode::Altctl:
            return ActionServer::FlightMode::Altctl;
        case SystemImpl::FlightMode::Acro:
            return ActionServer::FlightMode::Acro;
        case SystemImpl::FlightMode::Stabilized:
            return ActionServer::FlightMode::Stabilized;
        default:
            return ActionServer::FlightMode::Unknown;
    }
}

ActionServerImpl::ActionServerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ActionServerImpl::ActionServerImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _parent->register_plugin(this);
}

ActionServerImpl::~ActionServerImpl()
{
    _parent->unregister_plugin(this);
}

void ActionServerImpl::init()
{
    _parent->enable_sending_autopilot_version();

    // Arming / Disarm / Kill
    _parent->register_mavlink_command_handler(
        MAV_CMD_COMPONENT_ARM_DISARM,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            ActionServer::ArmDisarm armDisarm{
                command.params.param1 == 1, command.params.param2 == 21196};

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
                _parent->set_server_armed(armDisarm.arm);
            }

            auto result = (request_ack == MAV_RESULT::MAV_RESULT_ACCEPTED) ?
                              ActionServer::Result::Success :
                              ActionServer::Result::CommandDenied;

            _parent->call_user_callback([this, armDisarm, result]() {
                if (_arm_disarm_callback) {
                    _arm_disarm_callback(result, armDisarm);
                }
            });

            return _parent->make_command_ack_message(command, request_ack);
        },
        this);

    _parent->register_mavlink_command_handler(
        MAV_CMD_NAV_TAKEOFF,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            if (_allow_takeoff) {
                if (_takeoff_callback) {
                    _parent->call_user_callback(
                        [this]() { _takeoff_callback(ActionServer::Result::Success, true); });
                }

                return _parent->make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
            } else {
                if (_takeoff_callback) {
                    _parent->call_user_callback([this]() {
                        _takeoff_callback(ActionServer::Result::CommandDenied, false);
                    });
                }

                return _parent->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
            }
        },
        this);

    // Flight mode
    _parent->register_mavlink_command_handler(
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
                px4_custom_mode px4_mode{};
                px4_mode.main_mode = custom_mode;
                px4_mode.sub_mode = sub_custom_mode;
                auto system_flight_mode = _parent->to_flight_mode_from_custom_mode(px4_mode.data);
                request_flight_mode = telemetry_flight_mode_from_flight_mode(system_flight_mode);
            } else {
                // TO DO: non PX4 flight modes...
                // Just bug out now if not using PX4 modes
                _parent->call_user_callback([this, request_flight_mode]() {
                    if (_flight_mode_change_callback) {
                        _flight_mode_change_callback(
                            ActionServer::Result::ParameterError, request_flight_mode);
                    }
                });

                return _parent->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
            }

            bool allow_mode = false;
            switch (request_flight_mode) {
                case ActionServer::FlightMode::Manual:
                    allow_mode = true;
                    break;
                case ActionServer::FlightMode::Mission:
                    allow_mode = _allowed_flight_modes.can_auto_mode;
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

            // PX4...
            px4_custom_mode px4_mode{};
            px4_mode.data = _parent->get_custom_mode();

            if (allow_mode) {
                px4_mode.main_mode = custom_mode;
                px4_mode.sub_mode = sub_custom_mode;
                _parent->set_custom_mode(px4_mode.data);
            }

            _parent->call_user_callback([this, allow_mode, request_flight_mode]() {
                if (_flight_mode_change_callback) {
                    _flight_mode_change_callback(
                        allow_mode ? ActionServer::Result::Success :
                                     ActionServer::Result::CommandDenied,
                        request_flight_mode);
                }
            });

            return _parent->make_command_ack_message(
                command,
                allow_mode ? MAV_RESULT::MAV_RESULT_ACCEPTED : MAV_RESULT_TEMPORARILY_REJECTED);
        },
        this);
}

void ActionServerImpl::deinit() {}

void ActionServerImpl::enable() {}

void ActionServerImpl::disable() {}

void ActionServerImpl::subscribe_arm_disarm(ActionServer::ArmDisarmCallback callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _arm_disarm_callback = callback;
}

void ActionServerImpl::subscribe_flight_mode_change(ActionServer::FlightModeChangeCallback callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _flight_mode_change_callback = callback;
}

void ActionServerImpl::subscribe_takeoff(ActionServer::TakeoffCallback callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _takeoff_callback = callback;
}

void ActionServerImpl::subscribe_land(ActionServer::LandCallback callback)
{
    UNUSED(callback);
}

void ActionServerImpl::subscribe_reboot(ActionServer::RebootCallback callback)
{
    UNUSED(callback);
}

void ActionServerImpl::subscribe_shutdown(ActionServer::ShutdownCallback callback)
{
    UNUSED(callback);
}

void ActionServerImpl::subscribe_terminate(ActionServer::TerminateCallback callback)
{
    UNUSED(callback);
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

} // namespace mavsdk
