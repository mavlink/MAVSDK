#include "offboard.h"
#include "offboard_impl.h"

namespace dronecore {

Offboard::Offboard(System &system) :
    PluginBase(),
    _impl { new OffboardImpl(system) }
{
}

Offboard::~Offboard()
{
}

Offboard::Result Offboard::start()
{
    return _impl->start();
}

Offboard::Result Offboard::stop()
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

bool Offboard::is_active() const
{
    return _impl->is_active();
}

void Offboard::set_velocity_ned(Offboard::VelocityNEDYaw velocity_ned_yaw)
{
    return _impl->set_velocity_ned(velocity_ned_yaw);
}

void Offboard::set_velocity_body(Offboard::VelocityBodyYawspeed velocity_body_yawspeed)
{
    return _impl->set_velocity_body(velocity_body_yawspeed);
}

const char *Offboard::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::NO_SYSTEM:
            return "No system";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::BUSY:
            return "Busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecore
