#pragma once

#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>
#include "log_callback.h"

// Pull in the std::format detection macro and formatter specializations
// for MAVSDK types (scoped enums and streamable structs).
#include "std_format.h"

#if !MAVSDK_HAS_STD_FORMAT
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

// Requires at least one format argument (FirstArg) to avoid ambiguity with the
// string_view overload below. This spelling is compatible with C++11 through C++20.
template<typename FirstArg, typename... RestArgs>
void log_message(
    log::Level level,
    const char* filename,
    int line,
    const char* fmt,
    FirstArg&& first,
    RestArgs&&... rest)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    std::ostringstream os;
    detail::format_into(os, fmt, std::forward<FirstArg>(first), std::forward<RestArgs>(rest)...);
    emit_log(level, os.str(), filename, line);
}

inline void log_message(log::Level level, const char* filename, int line, std::string_view msg)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    emit_log(level, std::string(msg), filename, line);
}

#endif // MAVSDK_HAS_STD_FORMAT

} // namespace mavsdk
