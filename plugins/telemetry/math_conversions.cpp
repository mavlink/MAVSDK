#include "global_include.h"
#include "math_conversions.h"
#include <cmath>

namespace dronecore {

// TODO: this needs to be unit tested.

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion)
{
    auto &q = quaternion;

    Telemetry::EulerAngle euler_angle{
        to_deg_from_rad(
            atan2f(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y))),
        to_deg_from_rad(asinf(2.0f * (q.w * q.y - q.z * q.x))),
        to_deg_from_rad(
            atan2f(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)))};
    return euler_angle;
}

Telemetry::Quaternion to_quaternion_from_euler_angle(Telemetry::EulerAngle euler_angle)
{
    const double cos_phi_2 = cos(double(euler_angle.roll_deg) / 2.0);
    const double sin_phi_2 = sin(double(euler_angle.roll_deg) / 2.0);
    const double cos_theta_2 = cos(double(euler_angle.pitch_deg) / 2.0);
    const double sin_theta_2 = sin(double(euler_angle.pitch_deg) / 2.0);
    const double cos_psi_2 = cos(double(euler_angle.yaw_deg) / 2.0);
    const double sin_psi_2 = sin(double(euler_angle.yaw_deg) / 2.0);

    // Need to disable astyle for this block.
    // *INDENT-OFF*
    Telemetry::Quaternion quaternion{
        (float(cos_phi_2 * cos_theta_2 * cos_psi_2 + sin_phi_2 * sin_theta_2 * sin_psi_2)),
        (float(sin_phi_2 * cos_theta_2 * cos_psi_2 - cos_phi_2 * sin_theta_2 * sin_psi_2)),
        (float(cos_phi_2 * sin_theta_2 * cos_psi_2 + sin_phi_2 * cos_theta_2 * sin_psi_2)),
        (float(cos_phi_2 * cos_theta_2 * sin_psi_2 - sin_phi_2 * sin_theta_2 * cos_psi_2))};
    // *INDENT-ON*

    return quaternion;
}

} // namespace dronecore
