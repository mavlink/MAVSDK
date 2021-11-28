#include "action_server_impl.h"

namespace mavsdk {

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

void ActionServerImpl::init() {}

void ActionServerImpl::deinit() {}

void ActionServerImpl::enable() {}

void ActionServerImpl::disable() {}



void ActionServerImpl::subscribe_arm_disarm(ActionServer::ArmDisarmCallback callback)
{
    
    UNUSED(callback);
}





void ActionServerImpl::subscribe_flight_mode_change(ActionServer::FlightModeChangeCallback callback)
{
    
    UNUSED(callback);
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
    
    UNUSED(armable);
    
    UNUSED(force_armable);
    

    // TODO :)
    return {};
}





ActionServer::Result ActionServerImpl::set_disarmable(bool disarmable, bool force_disarmable)
{
    
    UNUSED(disarmable);
    
    UNUSED(force_disarmable);
    

    // TODO :)
    return {};
}





ActionServer::Result ActionServerImpl::set_allowable_flight_modes(ActionServer::AllowableFlightModes flight_modes)
{
    
    UNUSED(flight_modes);
    

    // TODO :)
    return {};
}





ActionServer::AllowableFlightModes ActionServerImpl::get_allowable_flight_modes()
{
    

    // TODO :)
    return {};
}



} // namespace mavsdk