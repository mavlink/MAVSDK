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

// Instead of using the constant from math.h or cmath we define it ourselves. This way
// we don't import all the other C math functions and make sure to use the C++ functions
// from the standard library (e.g. std::abs() instead of abs()).
constexpr double PI = 3.14159265358979323846;

template<typename T> constexpr T to_rad_from_deg(T deg)
{
    return static_cast<T>(PI) / static_cast<T>(180.0) * deg;
}

template<typename T> constexpr T to_deg_from_rad(T rad)
{
    return static_cast<T>(180.0) / static_cast<T>(PI) * rad;
}

template<typename T> constexpr T constrain(T input, T min, T max)
{
    return (input > max) ? max : (input < min) ? min : input;
}

} // namespace mavsdk
