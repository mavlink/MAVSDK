#include "action_server_impl.h"

namespace mavsdk {

ActionServer::FlightMode
telemetry_flight_mode_from_flight_mode(SystemImpl::FlightMode flight_mode)
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

ActionServerImpl::ActionServerImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ActionServerImpl::~ActionServerImpl()
{
    _parent->unregister_plugin(this);
}

void ActionServerImpl::init() {
        // Arming / Disarm / Kill
        _parent->register_mavlink_command_handler(
        MAV_CMD_COMPONENT_ARM_DISARM,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            ActionServer::ArmDisarm armDisarm{
                command.params.param1 == 1,
                command.params.param2 == 21196};

            // Check arm states - Ugly.
            uint8_t request_ack = MAV_RESULT_UNSUPPORTED;
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
                _parent->set_server_armed(armDisarm.arm == 1);
            }

            auto result = (request_ack == MAV_RESULT::MAV_RESULT_ACCEPTED) ?
                              ActionServer::Result::Success :
                              ActionServer::Result::CommandDenied;

            _parent->call_user_callback(
                [this, armDisarm, result]() { _arm_disarm_callback(result, armDisarm); });

            mavlink_message_t msg;
            mavlink_msg_command_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &msg,
                command.command,
                request_ack,
                100,
                0,
                command.origin_system_id,
                command.origin_component_id);
            return msg;
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
                // TO DO non PX4 flight modes...
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
                _flight_mode_change_callback(
                    allow_mode ? ActionServer::Result::Success :
                                 ActionServer::Result::CommandDenied,
                    request_flight_mode);
            });

            mavlink_message_t msg;
            mavlink_msg_command_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &msg,
                command.command,
                allow_mode ? MAV_RESULT::MAV_RESULT_ACCEPTED : MAV_RESULT_TEMPORARILY_REJECTED,
                100,
                0,
                command.origin_system_id,
                command.origin_component_id);
            return msg;
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
    
    UNUSED(callback);
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
    
    UNUSED(allow_takeoff);
    

    // TODO :)
    return {};
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





ActionServer::Result ActionServerImpl::set_allowable_flight_modes(ActionServer::AllowableFlightModes flight_modes)
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