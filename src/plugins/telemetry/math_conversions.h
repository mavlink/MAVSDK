#pragma once

#include "plugins/telemetry/telemetry.h"

namespace mavsdk {

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion);
Telemetry::Quaternion to_quaternion_from_euler_angle(Telemetry::EulerAngle euler_angle);

} // namespace mavsdk
