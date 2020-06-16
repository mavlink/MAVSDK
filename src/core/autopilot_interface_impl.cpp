#include "autopilot_interface_impl.h"
#include "node.h"
#include "mavlink_commands.h"
#include "px4_custom_mode.h"


namespace mavsdk {

using namespace std::placeholders;

AutopilotInterfaceImpl::AutopilotInterfaceImpl(std::shared_ptr<NodeImpl> node_impl) :
    _uuid{node_impl->get_system_id()},
    _node_impl{node_impl}
{
    _node_impl->register_mavlink_message_handler(
            MAVLINK_MSG_ID_AUTOPILOT_VERSION, 
            std::bind(&AutopilotInterfaceImpl::process_autopilot_version, this, _1),
            this);
    _node_impl->register_mavlink_message_handler(
            MAVLINK_MSG_ID_HEARTBEAT,
            std::bind(&AutopilotInterfaceImpl::process_heartbeat, this, _1),
            this);

    send_autopilot_version_request();
}

uint64_t AutopilotInterfaceImpl::get_uuid() const
{
    //return 1234;
    return _uuid;
};

void AutopilotInterfaceImpl::send_autopilot_version_request()
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES;
    command.params.param1 = 1.0f;

    _node_impl->send_command_async(command, nullptr);
}

void AutopilotInterfaceImpl::send_flight_information_request()
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_REQUEST_FLIGHT_INFORMATION;
    command.params.param1 = 1.0f;

    _node_impl->send_command_async(command, nullptr);
}

void AutopilotInterfaceImpl::process_autopilot_version(const mavlink_message_t& message)
{
    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    if (autopilot_version.uid != 0) {
        _uuid = autopilot_version.uid;
    }
}

void AutopilotInterfaceImpl::register_mavlink_message_handler(
        uint16_t msg_id, mavlink_message_handler_t callback, const void* cookie)
{
    _node_impl->register_mavlink_message_handler(msg_id, callback, cookie);
}

void AutopilotInterfaceImpl::unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie)
{
    _node_impl->unregister_mavlink_message_handler(msg_id, cookie);
}

void AutopilotInterfaceImpl::unregister_all_mavlink_message_handlers(const void *cookie)
{
    _node_impl->unregister_all_mavlink_message_handlers(cookie);
}

void AutopilotInterfaceImpl::register_timeout_handler(std::function<void()> callback, double duration_s, void** cookie)
{
    _node_impl->register_timeout_handler(callback, duration_s, cookie);
}

void AutopilotInterfaceImpl::refresh_timeout_handler(const void* cookie)
{
    _node_impl->refresh_timeout_handler(cookie);
}

void AutopilotInterfaceImpl::unregister_timeout_handler(const void* cookie)
{
    _node_impl->unregister_timeout_handler(cookie);
}

uint8_t AutopilotInterfaceImpl::get_system_id()
{
    return _node_impl->get_system_id();
}

uint8_t AutopilotInterfaceImpl::get_component_id()
{
    return _node_impl->get_component_id();
}
uint8_t AutopilotInterfaceImpl::get_own_system_id()
{
    return _node_impl->get_own_system_id();
}
uint8_t AutopilotInterfaceImpl::get_own_component_id()
{
    return _node_impl->get_own_component_id();
}

bool AutopilotInterfaceImpl::send_message(mavlink_message_t& message)
{
    return _node_impl->send_message(message);
}

MAVLinkCommands::Result AutopilotInterfaceImpl::send_command(MAVLinkCommands::CommandLong& command)
{
    return _node_impl->send_command(command);
}

MAVLinkCommands::Result AutopilotInterfaceImpl::send_command(MAVLinkCommands::CommandInt& command)
{
    return _node_impl->send_command(command);
}

void AutopilotInterfaceImpl::send_command_async(
        MAVLinkCommands::CommandLong& command, const commandResultCallback callback)
{
    _node_impl->send_command_async(command, callback);
}

void AutopilotInterfaceImpl::send_command_async(
        MAVLinkCommands::CommandInt& command, const commandResultCallback callback)
{
    _node_impl->send_command_async(command, callback);
}

MAVLinkCommands::Result AutopilotInterfaceImpl::set_msg_rate(
        uint16_t message_id, double rate_hz)
{
    return _node_impl->set_msg_rate(message_id, rate_hz);
}

void AutopilotInterfaceImpl::set_msg_rate_async(
        uint16_t message_id,
        double rate_hz,
        commandResultCallback callback)
{
    _node_impl->set_msg_rate_async(message_id, rate_hz, callback);
}

void AutopilotInterfaceImpl::register_plugin(PluginImplBase* plugin_impl)
{
    _node_impl->register_plugin(plugin_impl);
}

void AutopilotInterfaceImpl::unregister_plugin(PluginImplBase* plugin_impl)
{
    _node_impl->register_plugin(plugin_impl);
}

void AutopilotInterfaceImpl::call_user_callback(const std::function<void()>& func)
{
    _node_impl->call_user_callback(func);
}

void AutopilotInterfaceImpl::intercept_incoming_messages(std::function<bool(mavlink_message_t&)> callback)
{
    _node_impl->intercept_incoming_messages(callback);
}

void AutopilotInterfaceImpl::intercept_outgoing_messages(std::function<bool(mavlink_message_t&)> callback)
{
    _node_impl->intercept_outgoing_messages(callback);
}

bool AutopilotInterfaceImpl::is_connected()
{
    return _node_impl->is_connected();
}

void AutopilotInterfaceImpl::add_call_every(std::function<void()> callback, float interval_s, void** cookie)
{
    _node_impl->add_call_every(callback, interval_s, cookie);
}

void AutopilotInterfaceImpl::change_call_every(float interval_s, void* cookie)
{
    _node_impl->change_call_every(interval_s, cookie);
}

void AutopilotInterfaceImpl::reset_call_every(void* cookie)
{
    _node_impl->reset_call_every(cookie);
}

void AutopilotInterfaceImpl::remove_call_every(void* cookie)
{
    _node_impl->remove_call_every(cookie);
}

void AutopilotInterfaceImpl::process_heartbeat(const mavlink_message_t& message)
{
    mavlink_heartbeat_t  heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    if (message.sysid == get_system_id() && message.compid == get_component_id()) {
        _armed = ((heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false);
        _hitl_enabled = ((heartbeat.base_mode & MAV_MODE_FLAG_HIL_ENABLED) ? true : false);

        // TODO custom mode to flight mode
    }
}

AutopilotInterfaceImpl::FlightMode AutopilotInterfaceImpl::get_flight_mode() const
{
    return _flight_mode;
}

MAVLinkCommands::Result AutopilotInterfaceImpl::set_flight_mode(
        FlightMode system_mode)
{
    std::pair<MAVLinkCommands::Result, MAVLinkCommands::CommandLong> result =
        make_command_flight_mode(system_mode);

    if (result.first != MAVLinkCommands::Result::Success) {
        return result.first;
    }

    return send_command(result.second);
}

void AutopilotInterfaceImpl::set_flight_mode_async(
        FlightMode system_mode, commandResultCallback callback)
{
    std::pair<MAVLinkCommands::Result, MAVLinkCommands::CommandLong> result =
        make_command_flight_mode(system_mode);

    if (result.first != MAVLinkCommands::Result::Success) {
        if (callback) {
            callback(result.first, NAN);
        }
    }

    send_command_async(result.second, callback);
}

std::pair<MAVLinkCommands::Result, MAVLinkCommands::CommandLong>
AutopilotInterfaceImpl::make_command_flight_mode(FlightMode flight_mode)
{
    const uint8_t flag_safety_armed = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;

    const uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed | flag_hitl_enabled;

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = 0;

    switch (flight_mode) {
        case FlightMode::Hold:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case FlightMode::ReturnToLaunch:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case FlightMode::Takeoff:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case FlightMode::Land:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case FlightMode::Mission:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case FlightMode::FollowMe:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case FlightMode::Offboard:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            break;
        case FlightMode::Manual:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_MANUAL;
            break;
        case FlightMode::Posctl:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_POSCTL;
            break;
        case FlightMode::Altctl:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_ALTCTL;
            break;
        case FlightMode::Rattitude:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE;
            break;
        case FlightMode::Acro:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_ACRO;
            break;
        case FlightMode::Stabilized:
            custom_sub_mode = px4::PX4_CUSTOM_MAIN_MODE_STABILIZED;
            break;
        default:
            LogErr() << "Unknown Flight mode.";
            MAVLinkCommands::CommandLong empty_command{};
            return std::make_pair<>(MAVLinkCommands::Result::UnknownError, empty_command);
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_SET_MODE;
    command.params.param1 = float(mode);
    command.params.param2 = float(custom_mode);
    command.params.param3 = float(custom_sub_mode);

    return std::make_pair<>(MAVLinkCommands::Result::Success, command);
}

MAVLinkCommands::CommandLong
AutopilotInterfaceImpl::make_command_msg_rate(uint16_t message_id, double rate_hz)
{
    return _node_impl->make_command_msg_rate(message_id, rate_hz);
}

MAVLinkParameters::Result
AutopilotInterfaceImpl::set_param_float(const std::string& name, float value)
{
    return _node_impl->set_param_float(name, value);
}

MAVLinkParameters::Result
AutopilotInterfaceImpl::set_param_int(const std::string& name, int32_t value)
{
    return _node_impl->set_param_int(name, value);
}

MAVLinkParameters::Result
AutopilotInterfaceImpl::set_param_ext_float(const std::string& name, float value)
{
    return _node_impl->set_param_ext_float(name, value);
}

MAVLinkParameters::Result
AutopilotInterfaceImpl::set_param_ext_int(const std::string& name, int32_t value)
{
    return _node_impl->set_param_ext_int(name, value);
}

void AutopilotInterfaceImpl::set_param_float_async(
        const std::string& name, float value, success_t callback, const void* cookie)
{
    _node_impl->set_param_float_async(name, value, callback, cookie);
}

void AutopilotInterfaceImpl::set_param_int_async(
        const std::string& name, int32_t value, success_t callback, const void* cookie)
{
    _node_impl->set_param_int_async(name, value, callback, cookie);
}

void AutopilotInterfaceImpl::set_param_ext_float_async(
        const std::string& name, float value, success_t callback, const void* cookie)
{
    _node_impl->set_param_ext_float_async(name, value, callback, cookie);
}

void AutopilotInterfaceImpl::set_param_ext_int_async(
        const std::string& name, int32_t value, success_t callback, const void* cookie)
{
    _node_impl->set_param_ext_int_async(name, value, callback, cookie);
}

std::pair<MAVLinkParameters::Result, float>
AutopilotInterfaceImpl::get_param_float(const std::string& name)
{
    return _node_impl->get_param_float(name);
}

std::pair<MAVLinkParameters::Result, int>
AutopilotInterfaceImpl::get_param_int(const std::string& name)
{
    return _node_impl->get_param_int(name);
}

std::pair<MAVLinkParameters::Result, float>
AutopilotInterfaceImpl::get_param_ext_float(const std::string& name)
{
    return _node_impl->get_param_ext_float(name);
}

std::pair<MAVLinkParameters::Result, int>
AutopilotInterfaceImpl::get_param_ext_int(const std::string& name)
{
    return _node_impl->get_param_ext_int(name);
}

void AutopilotInterfaceImpl::get_param_float_async(
        const std::string& name, get_param_float_callback_t callback, const void* cookie)
{
    _node_impl->get_param_float_async(name, callback, cookie);
}

void AutopilotInterfaceImpl::get_param_int_async(
        const std::string& name, get_param_int_callback_t callback, const void* cookie)
{
    _node_impl->get_param_int_async(name, callback, cookie);
}

void AutopilotInterfaceImpl::get_param_ext_float_async(
        const std::string& name, get_param_float_callback_t callback, const void* cookie)
{
    _node_impl->get_param_ext_float_async(name, callback, cookie);
}

void AutopilotInterfaceImpl::get_param_ext_int_async(
        const std::string& name, get_param_int_callback_t callback, const void* cookie)
{
    _node_impl->get_param_ext_int_async(name, callback, cookie);
}

} // namespace mavsdk
