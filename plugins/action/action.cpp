#include "plugins/action/action.h"
#include "action_impl.h"

namespace dronecode_sdk {

Action::Action(System &system) : PluginBase(), _impl{new ActionImpl(system)} {}

Action::~Action() {}

Action::Result Action::arm() const
{
    return _impl->arm();
}

Action::Result Action::disarm() const
{
    return _impl->disarm();
}

Action::Result Action::kill() const
{
    return _impl->kill();
}

Action::Result Action::reboot() const
{
    return _impl->reboot();
}

Action::Result Action::takeoff() const
{
    return _impl->takeoff();
}

Action::Result Action::land() const
{
    return _impl->land();
}

Action::Result Action::goto_location(double latitude_deg,
                                     double longitude_deg,
                                     float altitude_amsl_m,
                                     float yaw_deg)
{
    return _impl->goto_location(latitude_deg, longitude_deg, altitude_amsl_m, yaw_deg);
}

Action::Result Action::return_to_launch() const
{
    return _impl->return_to_launch();
}

Action::Result Action::transition_to_fixedwing() const
{
    return _impl->transition_to_fixedwing();
}

Action::Result Action::transition_to_multicopter() const
{
    return _impl->transition_to_multicopter();
}

void Action::arm_async(result_callback_t callback)
{
    _impl->arm_async(callback);
}

void Action::disarm_async(result_callback_t callback)
{
    _impl->disarm_async(callback);
}

void Action::kill_async(result_callback_t callback)
{
    _impl->kill_async(callback);
}

void Action::takeoff_async(result_callback_t callback)
{
    _impl->takeoff_async(callback);
}

void Action::land_async(result_callback_t callback)
{
    _impl->land_async(callback);
}

void Action::return_to_launch_async(result_callback_t callback)
{
    _impl->return_to_launch_async(callback);
}

void Action::transition_to_multicopter_async(result_callback_t callback)
{
    _impl->transition_to_multicopter_async(callback);
}

void Action::transition_to_fixedwing_async(result_callback_t callback)
{
    _impl->transition_to_fixedwing_async(callback);
}

Action::Result Action::set_takeoff_altitude(float relative_altitude_m)
{
    return _impl->set_takeoff_altitude(relative_altitude_m);
}

std::pair<Action::Result, float> Action::get_takeoff_altitude() const
{
    return _impl->get_takeoff_altitude();
}

Action::Result Action::set_max_speed(float speed_m_s)
{
    return _impl->set_max_speed(speed_m_s);
}

std::pair<Action::Result, float> Action::get_max_speed() const
{
    return _impl->get_max_speed();
}

Action::Result Action::set_return_to_launch_return_altitude(float relative_altitude_m)
{
    return _impl->set_return_to_launch_return_altitude(relative_altitude_m);
}

std::pair<Action::Result, float> Action::get_return_to_launch_return_altitude() const
{
    return _impl->get_return_to_launch_return_altitude();
}

const char *Action::result_str(Action::Result result)
{
    switch (result) {
        case Action::Result::SUCCESS:
            return "Success";
        case Action::Result::NO_SYSTEM:
            return "No system";
        case Action::Result::CONNECTION_ERROR:
            return "Connection error";
        case Action::Result::BUSY:
            return "Busy";
        case Action::Result::COMMAND_DENIED:
            return "Command denied";
        case Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN:
            return "Command denied, landed state is unknown";
        case Action::Result::COMMAND_DENIED_NOT_LANDED:
            return "Command denied, not landed";
        case Action::Result::TIMEOUT:
            return "Timeout";
        case Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN:
            return "VTOL transition unknown";
        case Action::Result::NO_VTOL_TRANSITION_SUPPORT:
            return "No VTOL transition support";
        case Action::Result::PARAMETER_ERROR:
            return "Parameter error";
        case Action::Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecode_sdk
