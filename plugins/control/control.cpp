#include "control.h"
#include "control_impl.h"

namespace dronelink {

Control::Control(ControlImpl *impl) :
    PluginBase(),
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


} // namespace dronelink
