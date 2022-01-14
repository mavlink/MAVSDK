#include "mavsdk_math.h"
#include "math_conversions.h"
#include <cmath>

namespace mavsdk {

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion)
{
    auto& q = quaternion;

    Telemetry::EulerAngle euler_angle;
    euler_angle.roll_deg = to_deg_from_rad(
        atan2f(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)));
    euler_angle.pitch_deg = to_deg_from_rad(asinf(2.0f * (q.w * q.y - q.z * q.x)));
    euler_angle.yaw_deg = to_deg_from_rad(
        atan2f(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)));

    euler_angle.timestamp_us = quaternion.timestamp_us;

    return euler_angle;
}

Telemetry::Quaternion to_quaternion_from_euler_angle(Telemetry::EulerAngle euler_angle)
{
    const double cos_phi_2 = cos(double(to_rad_from_deg(euler_angle.roll_deg)) / 2.0);
    const double sin_phi_2 = sin(double(to_rad_from_deg(euler_angle.roll_deg)) / 2.0);
    const double cos_theta_2 = cos(double(to_rad_from_deg(euler_angle.pitch_deg)) / 2.0);
    const double sin_theta_2 = sin(double(to_rad_from_deg(euler_angle.pitch_deg)) / 2.0);
    const double cos_psi_2 = cos(double(to_rad_from_deg(euler_angle.yaw_deg)) / 2.0);
    const double sin_psi_2 = sin(double(to_rad_from_deg(euler_angle.yaw_deg)) / 2.0);

    Telemetry::Quaternion quaternion;
    quaternion.w = float(cos_phi_2 * cos_theta_2 * cos_psi_2 + sin_phi_2 * sin_theta_2 * sin_psi_2);
    quaternion.x = float(sin_phi_2 * cos_theta_2 * cos_psi_2 - cos_phi_2 * sin_theta_2 * sin_psi_2);
    quaternion.y = float(cos_phi_2 * sin_theta_2 * cos_psi_2 + sin_phi_2 * cos_theta_2 * sin_psi_2);
    quaternion.z = float(cos_phi_2 * cos_theta_2 * sin_psi_2 - sin_phi_2 * sin_theta_2 * cos_psi_2);

    quaternion.timestamp_us = euler_angle.timestamp_us;

    return quaternion;
}

} // namespace mavsdk
