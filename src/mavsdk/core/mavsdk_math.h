#pragma once

namespace mavsdk {

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
