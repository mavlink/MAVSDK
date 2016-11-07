#include "offboard.h"
#include "offboard_impl.h"

namespace dronelink {

Offboard::Offboard(OffboardImpl *impl) :
    _impl(impl)
{
}

Offboard::~Offboard()
{
}

Offboard::Result Offboard::start() const
{
    return _impl->start();
}

Offboard::Result Offboard::stop() const
{
    return _impl->stop();
}

void Offboard::start_async(result_callback_t callback)
{
    _impl->start_async(callback);
}

void Offboard::stop_async(result_callback_t callback)
{
    _impl->stop_async(callback);
}

void Offboard::set_velocity(VelocityNEDYaw velocity_ned_yaw)
{
    return _impl->set_velocity(velocity_ned_yaw);
}

} // namespace dronelink
