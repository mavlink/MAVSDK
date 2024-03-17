#include "mavsdk_math.h"
#include "math_conversions.h"
#include <cmath>

namespace mavsdk {

bool operator==(const Quaternion& lhs, const Quaternion& rhs)
{
    return ((std::isnan(rhs.w) && std::isnan(lhs.w)) || rhs.w == lhs.w) &&
           ((std::isnan(rhs.x) && std::isnan(lhs.x)) || rhs.x == lhs.x) &&
           ((std::isnan(rhs.y) && std::isnan(lhs.y)) || rhs.y == lhs.y) &&
           ((std::isnan(rhs.z) && std::isnan(lhs.z)) || rhs.z == lhs.z);
}

bool operator==(const EulerAngle& lhs, const EulerAngle& rhs)
{
    return ((std::isnan(rhs.roll_deg) && std::isnan(lhs.roll_deg)) ||
            rhs.roll_deg == lhs.roll_deg) &&
           ((std::isnan(rhs.pitch_deg) && std::isnan(lhs.pitch_deg)) ||
            rhs.pitch_deg == lhs.pitch_deg) &&
           ((std::isnan(rhs.yaw_deg) && std::isnan(lhs.yaw_deg)) || rhs.yaw_deg == lhs.yaw_deg);
}

EulerAngle to_euler_angle_from_quaternion(Quaternion quaternion)
{
    auto& q = quaternion;

    EulerAngle euler_angle;
    euler_angle.roll_deg = to_deg_from_rad(
        atan2f(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)));
    euler_angle.pitch_deg = to_deg_from_rad(asinf(2.0f * (q.w * q.y - q.z * q.x)));
    euler_angle.yaw_deg = to_deg_from_rad(
        atan2f(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)));

    return euler_angle;
}

Quaternion to_quaternion_from_euler_angle(EulerAngle euler_angle)
{
    const double cos_phi_2 = cos(double(to_rad_from_deg(euler_angle.roll_deg)) / 2.0);
    const double sin_phi_2 = sin(double(to_rad_from_deg(euler_angle.roll_deg)) / 2.0);
    const double cos_theta_2 = cos(double(to_rad_from_deg(euler_angle.pitch_deg)) / 2.0);
    const double sin_theta_2 = sin(double(to_rad_from_deg(euler_angle.pitch_deg)) / 2.0);
    const double cos_psi_2 = cos(double(to_rad_from_deg(euler_angle.yaw_deg)) / 2.0);
    const double sin_psi_2 = sin(double(to_rad_from_deg(euler_angle.yaw_deg)) / 2.0);

    Quaternion quaternion;
    quaternion.w = float(cos_phi_2 * cos_theta_2 * cos_psi_2 + sin_phi_2 * sin_theta_2 * sin_psi_2);
    quaternion.x = float(sin_phi_2 * cos_theta_2 * cos_psi_2 - cos_phi_2 * sin_theta_2 * sin_psi_2);
    quaternion.y = float(cos_phi_2 * sin_theta_2 * cos_psi_2 + sin_phi_2 * cos_theta_2 * sin_psi_2);
    quaternion.z = float(cos_phi_2 * cos_theta_2 * sin_psi_2 - sin_phi_2 * sin_theta_2 * cos_psi_2);

    return quaternion;
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion result;
    result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
    result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
    result.y = lhs.w * rhs.y + lhs.y * rhs.w - lhs.x * rhs.z + lhs.z * rhs.x;
    result.z = lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y + lhs.y * rhs.x;

    return result;
}

} // namespace mavsdk
