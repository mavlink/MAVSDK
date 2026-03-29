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

// All std::formatter specializations below are defined inside namespace std
// so that GCC's compile-time format-string checker (_Checking_scanner /
// basic_format_string) can find the constrained partial specialisations via
// normal lookup rather than relying on ADL from a definition in the global
// namespace.  Defining them outside namespace std with the qualified name
// (struct std::formatter<T>) is technically legal C++ but GCC 13/14's
// consteval format-string validation does not consistently apply constrained
// partial specialisations that were introduced that way, causing hard errors
// even though the types do have operator<<.
namespace std {

// std::formatter for std::filesystem::path is not available until C++26.
// Provide a specialization here so paths can be passed directly to std::format.
template<> struct formatter<filesystem::path> : formatter<string> {
    template<typename FormatContext>
    auto format(const filesystem::path& p, FormatContext& ctx) const
    {
        return formatter<string>::format(p.string(), ctx);
    }
};

// Formatter for scoped enum types (enum class) that provide operator<<.
// All MAVSDK plugin Result, Mode, Status, etc. enums are generated with
// operator<< so they print their named value (e.g. "Success") rather than a
// raw integer.  Use an explicit FormatContext template parameter (not
// abbreviated `auto&`) for maximum GCC compatibility.
template<typename T>
    requires(
        is_enum_v<T> && !is_convertible_v<T, underlying_type_t<T>> &&
        requires(ostream& os, T v) { os << v; })
struct formatter<T> : formatter<string> {
    template<typename FormatContext>
    auto format(T val, FormatContext& ctx) const
    {
        ostringstream ss;
        ss << val;
        return formatter<string>::format(ss.str(), ctx);
    }
};

// Formatter for class/struct types that provide operator<< but have no native
// std::formatter specialization (e.g. MissionPlan, MissionItem, ParamValue).
template<typename T>
    requires(
        is_class_v<T> && !is_same_v<remove_cvref_t<T>, string> &&
        !is_same_v<remove_cvref_t<T>, string_view> &&
        !is_same_v<remove_cvref_t<T>, filesystem::path> &&
        requires(ostream& os, const T& v) { os << v; })
struct formatter<T> : formatter<string> {
    template<typename FormatContext>
    auto format(const T& val, FormatContext& ctx) const
    {
        ostringstream ss;
        ss << val;
        return formatter<string>::format(ss.str(), ctx);
    }
};

} // namespace std

#endif // MAVSDK_HAS_STD_FORMAT
