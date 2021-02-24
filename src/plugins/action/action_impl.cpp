#include "action_impl.h"

namespace mavsdk {

ActionImpl::ActionImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ActionImpl::ActionImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ActionImpl::ActionImpl(SystemImpl* system) : PluginImplBase(system_impl)
{
    _parent->register_plugin(this);
}

ActionImpl::~ActionImpl()
{
    _parent->unregister_plugin(this);
}

void ActionImpl::init() {}

void ActionImpl::deinit() {}

void ActionImpl::enable() {}

void ActionImpl::disable() {}



void ActionImpl::arm_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::arm()
{
    

    // TODO :)
    return {};
}



void ActionImpl::disarm_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::disarm()
{
    

    // TODO :)
    return {};
}



void ActionImpl::takeoff_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::takeoff()
{
    

    // TODO :)
    return {};
}



void ActionImpl::land_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::land()
{
    

    // TODO :)
    return {};
}



void ActionImpl::reboot_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::reboot()
{
    

    // TODO :)
    return {};
}



void ActionImpl::shutdown_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::shutdown()
{
    

    // TODO :)
    return {};
}



void ActionImpl::terminate_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::terminate()
{
    

    // TODO :)
    return {};
}



void ActionImpl::kill_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::kill()
{
    

    // TODO :)
    return {};
}



void ActionImpl::return_to_launch_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::return_to_launch()
{
    

    // TODO :)
    return {};
}



void ActionImpl::goto_location_async(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg, const Action::ResultCallback callback)
{
    
    UNUSED(latitude_deg);
    
    UNUSED(longitude_deg);
    
    UNUSED(absolute_altitude_m);
    
    UNUSED(yaw_deg);
    
    UNUSED(callback);
}



Action::Result ActionImpl::goto_location(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg)
{
    
    UNUSED(latitude_deg);
    
    UNUSED(longitude_deg);
    
    UNUSED(absolute_altitude_m);
    
    UNUSED(yaw_deg);
    

    // TODO :)
    return {};
}



void ActionImpl::do_orbit_async(float radius_m, float velocity_ms, Action::OrbitYawBehavior yaw_behavior, double latitude_deg, double longitude_deg, double absolute_altitude_m, const Action::ResultCallback callback)
{
    
    UNUSED(radius_m);
    
    UNUSED(velocity_ms);
    
    UNUSED(yaw_behavior);
    
    UNUSED(latitude_deg);
    
    UNUSED(longitude_deg);
    
    UNUSED(absolute_altitude_m);
    
    UNUSED(callback);
}



Action::Result ActionImpl::do_orbit(float radius_m, float velocity_ms, Action::OrbitYawBehavior yaw_behavior, double latitude_deg, double longitude_deg, double absolute_altitude_m)
{
    
    UNUSED(radius_m);
    
    UNUSED(velocity_ms);
    
    UNUSED(yaw_behavior);
    
    UNUSED(latitude_deg);
    
    UNUSED(longitude_deg);
    
    UNUSED(absolute_altitude_m);
    

    // TODO :)
    return {};
}



void ActionImpl::transition_to_fixedwing_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::transition_to_fixedwing()
{
    

    // TODO :)
    return {};
}



void ActionImpl::transition_to_multicopter_async(const Action::ResultCallback callback)
{
    
    UNUSED(callback);
}



Action::Result ActionImpl::transition_to_multicopter()
{
    

    // TODO :)
    return {};
}



void Action::get_takeoff_altitude_async(const GetTakeoffAltitudeCallback callback)
{
    
    UNUSED(callback);

    // TODO :)
}



std::pair<Action::Result, float> Action::get_takeoff_altitude() const
{
    

    // TODO :)
    return {};
}



void ActionImpl::set_takeoff_altitude_async(float altitude, const Action::ResultCallback callback)
{
    
    UNUSED(altitude);
    
    UNUSED(callback);
}



Action::Result ActionImpl::set_takeoff_altitude(float altitude)
{
    
    UNUSED(altitude);
    

    // TODO :)
    return {};
}



void Action::get_maximum_speed_async(const GetMaximumSpeedCallback callback)
{
    
    UNUSED(callback);

    // TODO :)
}



std::pair<Action::Result, float> Action::get_maximum_speed() const
{
    

    // TODO :)
    return {};
}



void ActionImpl::set_maximum_speed_async(float speed, const Action::ResultCallback callback)
{
    
    UNUSED(speed);
    
    UNUSED(callback);
}



Action::Result ActionImpl::set_maximum_speed(float speed)
{
    
    UNUSED(speed);
    

    // TODO :)
    return {};
}



void Action::get_return_to_launch_altitude_async(const GetReturnToLaunchAltitudeCallback callback)
{
    
    UNUSED(callback);

    // TODO :)
}



std::pair<Action::Result, float> Action::get_return_to_launch_altitude() const
{
    

    // TODO :)
    return {};
}



void ActionImpl::set_return_to_launch_altitude_async(float relative_altitude_m, const Action::ResultCallback callback)
{
    
    UNUSED(relative_altitude_m);
    
    UNUSED(callback);
}



Action::Result ActionImpl::set_return_to_launch_altitude(float relative_altitude_m)
{
    
    UNUSED(relative_altitude_m);
    

    // TODO :)
    return {};
}



} // namespace mavsdk