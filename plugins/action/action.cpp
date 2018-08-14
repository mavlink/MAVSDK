#include "plugins/action/action.h"
#include "action_impl.h"

namespace dronecode_sdk {

Action::Action(System &system) : PluginBase(), _impl{new ActionImpl(system)} {}

Action::~Action() {}

ActionResult Action::arm() const
{
    return _impl->arm();
}

ActionResult Action::disarm() const
{
    return _impl->disarm();
}

ActionResult Action::kill() const
{
    return _impl->kill();
}

ActionResult Action::takeoff() const
{
    return _impl->takeoff();
}

ActionResult Action::land() const
{
    return _impl->land();
}

ActionResult Action::goto_location(double latitude_deg, double longitude_deg, float altitude_amsl_m, float yaw_rad)
{
    return _impl->goto_location(latitude_deg, longitude_deg, altitude_amsl_m, yaw_rad);
}

ActionResult Action::return_to_launch() const
{
    return _impl->return_to_launch();
}

ActionResult Action::transition_to_fixedwing() const
{
    return _impl->transition_to_fixedwing();
}

ActionResult Action::transition_to_multicopter() const
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

ActionResult Action::set_takeoff_altitude(float relative_altitude_m)
{
    return _impl->set_takeoff_altitude(relative_altitude_m);
}

std::pair<ActionResult, float> Action::get_takeoff_altitude() const
{
    return _impl->get_takeoff_altitude();
}

ActionResult Action::set_max_speed(float speed_m_s)
{
    return _impl->set_max_speed(speed_m_s);
}

std::pair<ActionResult, float> Action::get_max_speed() const
{
    return _impl->get_max_speed();
}

ActionResult Action::set_return_to_launch_return_altitude(float relative_altitude_m)
{
    return _impl->set_return_to_launch_return_altitude(relative_altitude_m);
}

std::pair<ActionResult, float> Action::get_return_to_launch_return_altitude() const
{
    return _impl->get_return_to_launch_return_altitude();
}

} // namespace dronecode_sdk
