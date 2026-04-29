#pragma once

#include "mavsdk_export.h"

namespace mavsdk {

/**
 * @brief Quaternion (Hamilton convention) representing a rotation.
 *
 * Components are ordered (w, x, y, z), where w is the scalar part. By MAVSDK
 * convention this represents the rotation from local NED earth frame to body
 * FRD frame.
 */
struct Quaternion {
    float w; /**< @brief Scalar component. */
    float x; /**< @brief Vector component along the x-axis. */
    float y; /**< @brief Vector component along the y-axis. */
    float z; /**< @brief Vector component along the z-axis. */
};

/**
 * @brief Euler angles (roll, pitch, yaw) in degrees, ZYX intrinsic convention.
 */
struct EulerAngle {
    float roll_deg; /**< @brief Roll angle in degrees, around body x-axis. */
    float pitch_deg; /**< @brief Pitch angle in degrees, around body y-axis. */
    float yaw_deg; /**< @brief Yaw angle in degrees, around body z-axis. */
};

/**
 * @brief Equality comparison for Quaternion. NaN-aware: two NaN components compare equal.
 */
MAVSDK_PUBLIC bool operator==(const Quaternion& lhs, const Quaternion& rhs);

/**
 * @brief Equality comparison for EulerAngle. NaN-aware: two NaN components compare equal.
 */
MAVSDK_PUBLIC bool operator==(const EulerAngle& lhs, const EulerAngle& rhs);

/**
 * @brief Convert a Quaternion to EulerAngle in degrees (ZYX Tait-Bryan).
 *
 * The argument to asinf is clamped to [-1, 1] so floating-point rounding
 * cannot drive the pitch to NaN at gimbal lock.
 *
 * @param quaternion Input Quaternion (does not need to be exactly normalised).
 * @return EulerAngle with roll, pitch, yaw in degrees.
 */
MAVSDK_PUBLIC EulerAngle to_euler_angle_from_quaternion(Quaternion quaternion);

/**
 * @brief Convert an EulerAngle (degrees) to a Quaternion (ZYX Tait-Bryan).
 *
 * @param euler_angle Input EulerAngle in degrees.
 * @return Normalised Quaternion.
 */
MAVSDK_PUBLIC Quaternion to_quaternion_from_euler_angle(EulerAngle euler_angle);

/**
 * @brief Hamilton product of two Quaternions; composes rotations.
 *
 * (lhs * rhs) applies rhs first, then lhs.
 *
 * @param lhs Left-hand side Quaternion.
 * @param rhs Right-hand side Quaternion.
 * @return Composed Quaternion.
 */
MAVSDK_PUBLIC Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

// Instead of using the constant from math.h or cmath we define it ourselves. This way
// we don't import all the other C math functions and make sure to use the C++ functions
// from the standard library (e.g. std::abs() instead of abs()).
/**
 * @brief Pi (double precision).
 */
constexpr double PI = 3.14159265358979323846;

/**
 * @brief Convert degrees to radians.
 *
 * @tparam T Numeric type.
 * @param deg Angle in degrees.
 * @return Angle in radians.
 */
template<typename T> constexpr T to_rad_from_deg(T deg)
{
    return static_cast<T>(PI) / static_cast<T>(180.0) * deg;
}

/**
 * @brief Convert radians to degrees.
 *
 * @tparam T Numeric type.
 * @param rad Angle in radians.
 * @return Angle in degrees.
 */
template<typename T> constexpr T to_deg_from_rad(T rad)
{
    return static_cast<T>(180.0) / static_cast<T>(PI) * rad;
}

/**
 * @brief Clamp a value to a closed interval [min, max].
 *
 * @tparam T Numeric type comparable with operator<.
 * @param input Value to clamp.
 * @param min Lower bound (inclusive).
 * @param max Upper bound (inclusive).
 * @return input clamped to [min, max].
 */
template<typename T> constexpr T constrain(T input, T min, T max)
{
    return (input > max) ? max : (input < min) ? min : input;
}

} // namespace mavsdk
