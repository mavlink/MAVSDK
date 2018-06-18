#include "plugins/offboard/offboard.h"
#include "offboard_impl.h"

namespace dronecode_sdk {

Offboard::Offboard(System &system) : PluginBase(), _impl{new OffboardImpl(system)} {}

Offboard::~Offboard() {}

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

void Offboard::set_position_ned_yaw(Offboard::PositionNEDYaw position_ned_yaw)
{
    return _impl->set_position_ned_yaw(position_ned_yaw);
}

void Offboard::set_velocity_altitude_ned_yaw(VelocityAltitudeNEDYaw velocity_altitude_ned_yaw)
{
    return _impl->set_velocity_altitude_ned_yaw(velocity_altitude_ned_yaw);
}

void Offboard::set_position_climbrate_ned_yaw(PositionClimbRateNEDYaw position_climbrate_ned_yaw)
{
    return _impl->set_position_climbrate_ned_yaw(position_climbrate_ned_yaw);
}

void Offboard::set_position_climbrate_ned_yawspeed(
    PositionClimbRateNEDYawspeed position_climbrate_ned_yawspeed)
{
    return _impl->set_position_climbrate_ned_yawspeed(position_climbrate_ned_yawspeed);
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

} // namespace dronecode_sdk
