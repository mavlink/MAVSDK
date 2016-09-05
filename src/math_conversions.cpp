#include "math_conversions.h"
#include <cmath>

namespace dronelink {

float to_deg_from_rad(float rad)
{
    return rad*180.0f/M_PI;
}

float to_rad_from_deg(float deg)
{
    return deg/180.0f*M_PI;
}

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion)
{
    auto &v = quaternion.vec;

    Telemetry::EulerAngle euler_angle({
        to_deg_from_rad(
            atan2f(2.0f * (v[0] * v[1] + v[2] * v[3]), 1.0f - 2.0f * (v[1] * v[1] + v[2] * v[2]))),
        to_deg_from_rad(
            asinf(2.0f * (v[0] * v[2] - v[3] * v[1]))),
        to_deg_from_rad(
            atan2f(2.0f * (v[0] * v[3] + v[1] * v[2]), 1.0f - 2.0f * (v[2] * v[2] + v[3] * v[3])))
    });
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

    Telemetry::Quaternion quaternion({
        (float(cos_phi_2 * cos_theta_2 * cos_psi_2 + sin_phi_2 * sin_theta_2 * sin_psi_2)),
        (float(sin_phi_2 * cos_theta_2 * cos_psi_2 - cos_phi_2 * sin_theta_2 * sin_psi_2)),
        (float(cos_phi_2 * sin_theta_2 * cos_psi_2 + sin_phi_2 * cos_theta_2 * sin_psi_2)),
        (float(cos_phi_2 * cos_theta_2 * sin_psi_2 - sin_phi_2 * sin_theta_2 * cos_psi_2))
    });

    return quaternion;
}


} // namespace dronelink
