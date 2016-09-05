#pragma once

#include "telemetry.h"

namespace dronelink {

float to_deg_from_rad(float rad);
float to_rad_from_deg(float deg);

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion);
Telemetry::Quaternion to_quaternion_from_euler_angle(Telemetry::EulerAngle euler_angle);

} // namespace dronelink
