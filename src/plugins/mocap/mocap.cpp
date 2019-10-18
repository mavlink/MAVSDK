#include <cmath>
#include <limits>
#include <array>

#include "plugins/mocap/mocap.h"
#include "mocap_impl.h"

namespace mavsdk {

Mocap::Mocap(System& system) : PluginBase(), _impl{new MocapImpl(system)} {}

Mocap::~Mocap() {}

Mocap::Result Mocap::set_vision_position_estimate(VisionPositionEstimate vision_position_estimate)
{
    return _impl->set_vision_position_estimate(vision_position_estimate);
}

Mocap::Result Mocap::set_attitude_position_mocap(AttitudePositionMocap attitude_position_mocap)
{
    return _impl->set_attitude_position_mocap(attitude_position_mocap);
}

Mocap::Result Mocap::set_odometry(const Odometry& odometry)
{
    return _impl->set_odometry(odometry);
}

const char* Mocap::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::NO_SYSTEM:
            return "No system";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::INVALID_REQUEST_DATA:
            return "Invalid data in gRPC request";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

bool operator==(const Mocap::Quaternion& lhs, const Mocap::Quaternion& rhs)
{
    return lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

std::ostream& operator<<(std::ostream& str, Mocap::Quaternion const& quaternion)
{
    return str << "[w: " << quaternion.w << ", x: " << quaternion.x << ", y: " << quaternion.y
               << ", z: " << quaternion.z << "]";
}

bool operator==(const Mocap::VisionPositionEstimate& lhs, const Mocap::VisionPositionEstimate& rhs)
{
    // FixMe: Should we check time_usec and reset_counter equality?
    return (
        lhs.time_usec == rhs.time_usec && lhs.position_body == rhs.position_body &&
        lhs.angle_body == rhs.angle_body && lhs.pose_covariance == rhs.pose_covariance);
}

std::ostream&
operator<<(std::ostream& str, Mocap::VisionPositionEstimate const& vision_position_estimate)
{
    str << "[time_usec: " << vision_position_estimate.time_usec
        << ", position_body: " << vision_position_estimate.position_body
        << ", angle_body: " << vision_position_estimate.angle_body
        << ", pose_covariance: " << vision_position_estimate.pose_covariance
        << ", reset_counter: " << static_cast<int>(vision_position_estimate.reset_counter) << "]";
    return str;
}

bool operator==(const Mocap::AttitudePositionMocap& lhs, const Mocap::AttitudePositionMocap& rhs)
{
    // FixMe: Should we check time_usec equality?
    return (
        lhs.time_usec == rhs.time_usec && lhs.q == rhs.q &&
        lhs.position_body == rhs.position_body && lhs.pose_covariance == rhs.pose_covariance);
}

std::ostream&
operator<<(std::ostream& str, Mocap::AttitudePositionMocap const& attitude_position_mocap)
{
    str << "[time_usec: " << attitude_position_mocap.time_usec
        << ", q: " << attitude_position_mocap.q
        << ", position_body: " << attitude_position_mocap.position_body
        << ", pose_covariance: " << attitude_position_mocap.pose_covariance << "]";
    return str;
}

bool operator==(const Mocap::Odometry& lhs, const Mocap::Odometry& rhs)
{
    return (
        lhs.time_usec == rhs.time_usec && lhs.frame_id == rhs.frame_id &&
        lhs.position_body == rhs.position_body && lhs.q == rhs.q &&
        lhs.speed_body == rhs.speed_body &&
        lhs.angular_velocity_body == rhs.angular_velocity_body &&
        lhs.pose_covariance == rhs.pose_covariance &&
        lhs.velocity_covariance == rhs.velocity_covariance);
}

std::ostream& operator<<(std::ostream& str, Mocap::Odometry const& odometry)
{
    return str << "[time_usec: " << odometry.time_usec
               << ", frame_id: " << static_cast<int>(odometry.frame_id)
               << ", position_body: " << odometry.position_body << ", q: " << odometry.q
               << ", speed_body: " << odometry.speed_body
               << ", angular_velocity_body: " << odometry.angular_velocity_body
               << ", pose_covariance: " << odometry.pose_covariance
               << ", velocity_covariance: " << odometry.velocity_covariance;
}

bool operator==(const Mocap::PositionBody& lhs, const Mocap::PositionBody& rhs)
{
    return (
        std::abs(lhs.x_m - rhs.x_m) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.y_m - rhs.y_m) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.z_m - rhs.z_m) <= std::numeric_limits<float>::epsilon());
}

bool operator!=(const Mocap::PositionBody& lhs, const Mocap::PositionBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Mocap::PositionBody const& position_body)
{
    return str << "[x_m: " << position_body.x_m << ", y_m: " << position_body.y_m
               << ", z_m: " << position_body.z_m << "]";
}

bool operator==(const Mocap::AngleBody& lhs, const Mocap::AngleBody& rhs)
{
    return (
        std::abs(lhs.roll_rad - rhs.roll_rad) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.pitch_rad - rhs.pitch_rad) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.yaw_rad - rhs.yaw_rad) <= std::numeric_limits<float>::epsilon());
}

bool operator!=(const Mocap::AngleBody& lhs, const Mocap::AngleBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Mocap::AngleBody const& angle_body)
{
    return str << "[roll_rad: " << angle_body.roll_rad << ", pitch_rad: " << angle_body.pitch_rad
               << ", yaw_rad: " << angle_body.yaw_rad << "]";
}

bool operator==(const Mocap::SpeedBody& lhs, const Mocap::SpeedBody& rhs)
{
    return (
        std::abs(lhs.x_m_s - rhs.x_m_s) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.y_m_s - rhs.y_m_s) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.z_m_s - rhs.z_m_s) <= std::numeric_limits<float>::epsilon());
}

bool operator!=(const Mocap::SpeedBody& lhs, const Mocap::SpeedBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Mocap::SpeedBody const& speed_body)
{
    return str << "[x_m_s: " << speed_body.x_m_s << ", y_m_s: " << speed_body.y_m_s
               << ", z_m_s: " << speed_body.z_m_s << "]";
}

bool operator==(const Mocap::Covariance& lhs, const Mocap::Covariance& rhs)
{
    const bool lhs_first_nan = std::isnan(lhs[0]);
    const bool rhs_first_nan = std::isnan(rhs[0]);

    if (lhs_first_nan != rhs_first_nan) {
        return false;
    }

    if (!lhs_first_nan) {
        for (int i = 0; i < 21; i++) {
            if (std::abs(lhs[i] - rhs[i]) > std::numeric_limits<float>::epsilon())
                return false;
        }
    }
    return true;
}

bool operator!=(const Mocap::Covariance& lhs, const Mocap::Covariance& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Mocap::Covariance const& covariance)
{
    str << "[";
    for (unsigned i = 0; i < 21; i++) {
        str << covariance[i];
        if (i != 20) {
            str << ", ";
        } else {
            str << "]";
        }
    }
    return str;
}

bool operator==(const Mocap::AngularVelocityBody& lhs, const Mocap::AngularVelocityBody& rhs)
{
    return (
        std::abs(lhs.roll_rad_s - rhs.roll_rad_s) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.pitch_rad_s - rhs.pitch_rad_s) <= std::numeric_limits<float>::epsilon() &&
        std::abs(lhs.yaw_rad_s - rhs.yaw_rad_s) <= std::numeric_limits<float>::epsilon());
}

bool operator!=(const Mocap::AngularVelocityBody& lhs, const Mocap::AngularVelocityBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Mocap::AngularVelocityBody const& angular_velocity_body)
{
    return str << "[roll_rad_s: " << angular_velocity_body.roll_rad_s
               << ", pitch_rad_s: " << angular_velocity_body.pitch_rad_s
               << ", yaw_rad_s: " << angular_velocity_body.yaw_rad_s << "]";
}

} // namespace mavsdk
