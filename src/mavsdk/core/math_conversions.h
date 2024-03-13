#pragma once

namespace mavsdk {

struct Quaternion {
    float w;
    float x;
    float y;
    float z;
};

struct EulerAngle {
    float roll_deg;
    float pitch_deg;
    float yaw_deg;
};

bool operator==(const Quaternion& lhs, const Quaternion& rhs);
bool operator==(const EulerAngle& lhs, const EulerAngle& rhs);

EulerAngle to_euler_angle_from_quaternion(Quaternion quaternion);
Quaternion to_quaternion_from_euler_angle(EulerAngle euler_angle);

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

} // namespace mavsdk
