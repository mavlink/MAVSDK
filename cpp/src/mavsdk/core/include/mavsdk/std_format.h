#pragma once

// Detect whether std::format is usable on this platform.
// Conditions for disabling std::format:
//   1. The <format> header is not present (e.g. GCC < 13 cross-toolchains).
//   2. The feature-test macro __cpp_lib_format is not defined.
//   3. iOS / iOS-simulator deployment target < 16.3: Apple's libc++ std::format
//      floating-point support calls std::to_chars which is only available
//      from iOS 16.3 / iPadOS 16.3 onward.
#if !defined(__has_include) || !__has_include(<format>)
#define MAVSDK_HAS_STD_FORMAT 0
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && (__IPHONE_OS_VERSION_MIN_REQUIRED < 160300)
#define MAVSDK_HAS_STD_FORMAT 0
#elif defined(__cpp_lib_format)
#define MAVSDK_HAS_STD_FORMAT 1
#else
#define MAVSDK_HAS_STD_FORMAT 0
#endif

#if MAVSDK_HAS_STD_FORMAT
#include <format>
#include <filesystem>
#include <sstream>
#include <string>
#include <type_traits>

// std::formatter for std::filesystem::path is not available until C++26.
// Provide a specialization here so paths can be passed directly to std::format.
template<> struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const std::filesystem::path& p, FormatContext& ctx) const
    {
        return std::formatter<std::string>::format(p.string(), ctx);
    }
};

// Formatter for scoped enum types (enum class) that provide operator<<.
// All MAVSDK plugin Result, Mode, Status, etc. enums are generated with operator<<
// so they print their named value (e.g. "Success") rather than a raw integer.
template<typename T>
    requires(
        std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>> &&
        requires(std::ostream& os, T v) { os << v; })
struct std::formatter<T> : std::formatter<std::string> {
    auto format(T val, auto& ctx) const
    {
        std::ostringstream ss;
        ss << val;
        return std::formatter<std::string>::format(ss.str(), ctx);
    }
};

// Formatter for class/struct types that provide operator<< but have no native
// std::formatter specialization (e.g. MissionPlan, MissionItem, ParamValue).
template<typename T>
    requires(
        std::is_class_v<T> && !std::is_same_v<std::remove_cvref_t<T>, std::string> &&
        !std::is_same_v<std::remove_cvref_t<T>, std::string_view> &&
        !std::is_same_v<std::remove_cvref_t<T>, std::filesystem::path> &&
        requires(std::ostream& os, const T& v) { os << v; })
struct std::formatter<T> : std::formatter<std::string> {
    auto format(const T& val, auto& ctx) const
    {
        std::ostringstream ss;
        ss << val;
        return std::formatter<std::string>::format(ss.str(), ctx);
    }
};

#endif // MAVSDK_HAS_STD_FORMAT
