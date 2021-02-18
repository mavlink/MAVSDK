#include "tracking_server_impl.h"

namespace mavsdk {

TrackingServerImpl::TrackingServerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

TrackingServerImpl::TrackingServerImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

TrackingServerImpl::~TrackingServerImpl()
{
    _parent->unregister_plugin(this);
}

void TrackingServerImpl::init() {}

void TrackingServerImpl::deinit() {}

void TrackingServerImpl::enable() {}

void TrackingServerImpl::disable() {}

void TrackingServerImpl::set_tracking_point_status(TrackingServer::TrackPoint tracked_point)
{
    UNUSED(tracked_point);

    // TODO :)
    ;
}

void TrackingServerImpl::set_tracking_rectangle_status(
    TrackingServer::TrackRectangle tracked_rectangle)
{
    UNUSED(tracked_rectangle);

    // TODO :)
    ;
}

void TrackingServerImpl::set_tracking_off_status()
{
    // TODO :)
    ;
}

void TrackingServerImpl::subscribe_tracking_point_command(
    TrackingServer::TrackingPointCommandCallback callback)
{
    UNUSED(callback);
}

void TrackingServerImpl::subscribe_tracking_rectangle_command(
    TrackingServer::TrackingRectangleCommandCallback callback)
{
    UNUSED(callback);
}

void TrackingServerImpl::subscribe_tracking_off_command(
    TrackingServer::TrackingOffCommandCallback callback)
{
    UNUSED(callback);
}

TrackingServer::Result
TrackingServerImpl::respond_tracking_point_command(TrackingServer::CommandAnswer command_answer)
{
    UNUSED(command_answer);

    // TODO :)
    return {};
}

TrackingServer::Result
TrackingServerImpl::respond_tracking_rectangle_command(TrackingServer::CommandAnswer command_answer)
{
    UNUSED(command_answer);

    // TODO :)
    return {};
}

TrackingServer::Result
TrackingServerImpl::respond_tracking_off_command(TrackingServer::CommandAnswer command_answer)
{
    UNUSED(command_answer);

    // TODO :)
    return {};
}

} // namespace mavsdk