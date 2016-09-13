#include "control.h"
#include "control_impl.h"

namespace dronelink {

Control::Control(ControlImpl *impl) :
    _impl(impl)
{
}

Control::~Control()
{
}

Result Control::arm() const
{
    return _impl->arm();
}

Result Control::disarm() const
{
    return _impl->disarm();
}

Result Control::takeoff() const
{
    return _impl->takeoff();
}

Result Control::land() const
{
    return _impl->land();
}

Result Control::return_to_land() const
{
    return _impl->return_to_land();
}

void Control::arm_async(result_callback_t callback)
{
    return _impl->arm_async(callback);
}

void Control::disarm_async(result_callback_t callback)
{
    return _impl->disarm_async(callback);
}

void Control::takeoff_async(result_callback_t callback)
{
    return _impl->takeoff_async(callback);
}

void Control::land_async(result_callback_t callback)
{
    return _impl->land_async(callback);
}

void Control::return_to_land_async(result_callback_t callback)
{
    return _impl->return_to_land_async(callback);
}


} // namespace dronelink
