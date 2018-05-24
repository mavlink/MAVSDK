#pragma once

#include "telemetry.h"

namespace dronecore {

Telemetry::EulerAngle to_euler_angle_from_quaternion(Telemetry::Quaternion quaternion);
Telemetry::Quaternion to_quaternion_from_euler_angle(Telemetry::EulerAngle euler_angle);

}// namespace dronecore
