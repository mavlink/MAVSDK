#pragma once

#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>
#include "log_callback.h"

// Detect whether std::format is usable on this platform.
// Conditions for disabling std::format:
//   1. The <format> header is not present (e.g. GCC < 13 cross-toolchains).
//   2. The feature-test macro __cpp_lib_format is not defined.
//   3. iOS / iOS-simulator deployment target < 16.3: Apple's libc++ std::format
//      floating-point support calls std::to_chars which is only available
//      from iOS 16.3 / iPadOS 16.3 onward.
#if !defined(__has_include) || !__has_include(<format>)
#  define MAVSDK_HAS_STD_FORMAT 0
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && (__IPHONE_OS_VERSION_MIN_REQUIRED < 160300)
#  define MAVSDK_HAS_STD_FORMAT 0
#elif defined(__cpp_lib_format)
#  define MAVSDK_HAS_STD_FORMAT 1
#else
#  define MAVSDK_HAS_STD_FORMAT 0
#endif

#if MAVSDK_HAS_STD_FORMAT
#include <format>
#include <sstream>

// std::formatter for std::filesystem::path is not available until C++26.
// Provide a specialization here so paths can be passed directly to Log macros.
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
    requires(std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>> &&
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
// std::formatter specialization (e.g. ParamValue).
template<typename T>
    requires(std::is_class_v<T> &&
             !std::is_same_v<std::remove_cvref_t<T>, std::string> &&
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

#else
// Fallback for platforms without std::format: use ostringstream-based {} substitution.
#include <sstream>
#endif

#if defined(ANDROID)
#include <android/log.h>
#else
#include <iostream>
#include <ctime>
#endif

#if !defined(WINDOWS)
// Remove path and extract only filename.
#define FILENAME \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define FILENAME __FILE__
#endif

#define call_user_callback(...) call_user_callback_located(FILENAME, __LINE__, __VA_ARGS__)

#define LogDebug(...) \
    ::mavsdk::log_message(::mavsdk::log::Level::Debug, FILENAME, __LINE__, __VA_ARGS__)
#define LogInfo(...) \
    ::mavsdk::log_message(::mavsdk::log::Level::Info, FILENAME, __LINE__, __VA_ARGS__)
#define LogWarn(...) \
    ::mavsdk::log_message(::mavsdk::log::Level::Warn, FILENAME, __LINE__, __VA_ARGS__)
#define LogErr(...) \
    ::mavsdk::log_message(::mavsdk::log::Level::Err, FILENAME, __LINE__, __VA_ARGS__)

namespace mavsdk {

std::mutex& get_log_mutex();

enum class Color { Red, Green, Yellow, Blue, Gray, Reset };

void set_color(Color color);

void emit_log(log::Level level, const std::string& message, const char* filename, int line);

#if MAVSDK_HAS_STD_FORMAT

template<typename... Args>
void log_message(
    log::Level level,
    const char* filename,
    int line,
    std::format_string<Args...> fmt,
    Args&&... args)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    emit_log(level, std::format(fmt, std::forward<Args>(args)...), filename, line);
}

// Overload for a plain string with no format args (avoids treating the string as a
// format string at compile time, which would reject strings containing braces).
inline void log_message(log::Level level, const char* filename, int line, std::string_view msg)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    emit_log(level, std::string(msg), filename, line);
}

#else // MAVSDK_HAS_STD_FORMAT

// Fallback implementation: substitute {} placeholders via ostringstream.
namespace detail {
inline void format_into(std::ostream& os, std::string_view fmt)
{
    os << fmt;
}

template<typename T, typename... Rest>
void format_into(std::ostream& os, std::string_view fmt, T&& arg, Rest&&... rest)
{
    auto pos = fmt.find("{}");
    if (pos == std::string_view::npos) {
        os << fmt;
        return;
    }
    os << fmt.substr(0, pos);
    os << arg;
    format_into(os, fmt.substr(pos + 2), std::forward<Rest>(rest)...);
}
} // namespace detail

template<typename... Args>
    requires(sizeof...(Args) > 0)
void log_message(
    log::Level level,
    const char* filename,
    int line,
    const char* fmt,
    Args&&... args)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    std::ostringstream os;
    detail::format_into(os, fmt, std::forward<Args>(args)...);
    emit_log(level, os.str(), filename, line);
}

inline void log_message(log::Level level, const char* filename, int line, std::string_view msg)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    emit_log(level, std::string(msg), filename, line);
}

#endif // MAVSDK_HAS_STD_FORMAT

} // namespace mavsdk
