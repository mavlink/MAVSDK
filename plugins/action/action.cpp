#include "action.h"
#include "action_impl.h"

namespace dronelink {

Action::Action(ActionImpl *impl) :
    _impl(impl)
{
}

Action::~Action()
{
}

Result Action::arm() const
{
    return _impl->arm();
}

Result Action::disarm() const
{
    return _impl->disarm();
}

Result Action::kill() const
{
    return _impl->kill();
}

Result Action::takeoff() const
{
    return _impl->takeoff();
}

Result Action::land() const
{
    return _impl->land();
}

Result Action::return_to_land() const
{
    return _impl->return_to_land();
}

void Action::arm_async(result_callback_t callback)
{
    return _impl->arm_async(callback);
}

void Action::disarm_async(result_callback_t callback)
{
    return _impl->disarm_async(callback);
}

void Action::kill_async(result_callback_t callback)
{
    return _impl->kill_async(callback);
}

void Action::takeoff_async(result_callback_t callback)
{
    return _impl->takeoff_async(callback);
}

void Action::land_async(result_callback_t callback)
{
    return _impl->land_async(callback);
}

void Action::return_to_land_async(result_callback_t callback)
{
    return _impl->return_to_land_async(callback);
}


} // namespace dronelink
