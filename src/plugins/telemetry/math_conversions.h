#pragma once

#include "plugins/telemetry/telemetry.h"

namespace dronecode_sdk {

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion);
Telemetry::Quaternion to_quaternion_from_euler_angle(Telemetry::EulerAngle euler_angle);

} // namespace dronecode_sdk
