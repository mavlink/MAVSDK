#pragma once

#include "telemetry_structs.h"

namespace dronecore {

EulerAngle to_euler_angle_from_quaternion(Quaternion quaternion);
Quaternion to_quaternion_from_euler_angle(EulerAngle euler_angle);

} // namespace dronecore
