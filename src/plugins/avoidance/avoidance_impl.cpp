#include "avoidance_impl.h"
#include "global_include.h"
#include "log.h"

namespace mavsdk {

AvoidanceImpl::AvoidanceImpl(System& system) : PluginImplBase(system), _mavlink_avoidance_item_messages()
{
    _parent->register_plugin(this);
}

AvoidanceImpl::~AvoidanceImpl()
{
    _parent->unregister_plugin(this);
}

void AvoidanceImpl::init() {}

void AvoidanceImpl::deinit() {}

void AvoidanceImpl::enable() {}

void AvoidanceImpl::disable() {}

void AvoidanceImpl::process_trajectory_representation_waypoints(const mavlink_message_t& message)
{

}

void AvoidanceImpl::receive_trajectory_representation_waypoints_async(
    Avoidance::trajectory_representation_waypoints_callback_t& callback)
{
    _trajectory_representation_waypoints_subscription = callback;
}

} // namespace mavsdk
