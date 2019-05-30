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

void Offboard::set_position_ned(Offboard::PositionNEDYaw position_ned_yaw)
{
    return _impl->set_position_ned(position_ned_yaw);
}

void Offboard::set_velocity_ned(Offboard::VelocityNEDYaw velocity_ned_yaw)
{
    return _impl->set_velocity_ned(velocity_ned_yaw);
}

void Offboard::set_velocity_body(Offboard::VelocityBodyYawspeed velocity_body_yawspeed)
{
    return _impl->set_velocity_body(velocity_body_yawspeed);
}

void Offboard::set_attitude(Offboard::Attitude attitude)
{
    return _impl->set_attitude(attitude);
}

void Offboard::set_attitude_rate(Offboard::AttitudeRate attitude_rate)
{
    return _impl->set_attitude_rate(attitude_rate);
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
        case Result::NO_SETPOINT_SET:
            return "No setpoint set";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

bool operator==(const Offboard::Attitude &lhs, const Offboard::Attitude &rhs)
{
    return lhs.roll_deg == rhs.roll_deg && lhs.pitch_deg == rhs.pitch_deg &&
           lhs.yaw_deg == rhs.yaw_deg && lhs.thrust_value == rhs.thrust_value;
}

std::ostream &operator<<(std::ostream &str, Offboard::Attitude const &attitude)
{
    return str << "[roll_deg: " << attitude.roll_deg << ", pitch_deg: " << attitude.pitch_deg
               << ", yaw_deg: " << attitude.yaw_deg << ", thrust_value " << attitude.thrust_value
               << "]";
}
bool operator==(const Offboard::AttitudeRate &lhs, const Offboard::AttitudeRate &rhs)
{
    return lhs.roll_deg_s == rhs.roll_deg_s && lhs.pitch_deg_s == rhs.pitch_deg_s &&
           lhs.yaw_deg_s == rhs.yaw_deg_s && lhs.thrust_value == rhs.thrust_value;
}

std::ostream &operator<<(std::ostream &str, Offboard::AttitudeRate const &attitude_rate)
{
    return str << "[roll_deg_s: " << attitude_rate.roll_deg_s
               << ", pitch_deg_s: " << attitude_rate.pitch_deg_s
               << ", yaw_deg_s: " << attitude_rate.yaw_deg_s
               << ", thrust_value: " << attitude_rate.thrust_value << "]";
}

bool operator==(const Offboard::PositionNEDYaw &lhs, const Offboard::PositionNEDYaw &rhs)
{
    return lhs.north_m == rhs.north_m && lhs.east_m == rhs.east_m && lhs.down_m == rhs.down_m &&
           lhs.yaw_deg == rhs.yaw_deg;
}

std::ostream &operator<<(std::ostream &str, Offboard::PositionNEDYaw const &position_ned_yaw)
{
    return str << "[north_m: " << position_ned_yaw.north_m
               << ", east_m: " << position_ned_yaw.east_m << ", down_m: " << position_ned_yaw.down_m
               << ", yaw_deg: " << position_ned_yaw.yaw_deg << "]";
}

bool operator==(const Offboard::VelocityBodyYawspeed &lhs,
                const Offboard::VelocityBodyYawspeed &rhs)
{
    return lhs.forward_m_s == rhs.forward_m_s && lhs.right_m_s == rhs.right_m_s &&
           lhs.down_m_s == rhs.down_m_s && lhs.yawspeed_deg_s == rhs.yawspeed_deg_s;
}

std::ostream &operator<<(std::ostream &str,
                         Offboard::VelocityBodyYawspeed const &velocity_body_yawspeed)
{
    return str << "[forward_m_s: " << velocity_body_yawspeed.forward_m_s
               << ", right_m_s: " << velocity_body_yawspeed.right_m_s
               << ", down_m_s: " << velocity_body_yawspeed.down_m_s
               << ", yawspeed_deg_s: " << velocity_body_yawspeed.yawspeed_deg_s << "]";
}

bool operator==(const Offboard::VelocityNEDYaw &lhs, const Offboard::VelocityNEDYaw &rhs)
{
    return lhs.north_m_s == rhs.north_m_s && lhs.east_m_s == rhs.east_m_s &&
           lhs.down_m_s == rhs.down_m_s && lhs.yaw_deg == rhs.yaw_deg;
}

std::ostream &operator<<(std::ostream &str, Offboard::VelocityNEDYaw const &velocity_ned_yaw)
{
    return str << "[north_m_s: " << velocity_ned_yaw.north_m_s
               << ", east_m_s: " << velocity_ned_yaw.east_m_s
               << ", down_m_s: " << velocity_ned_yaw.down_m_s
               << ", yaw_deg: " << velocity_ned_yaw.yaw_deg << "]";
}

} // namespace dronecode_sdk
