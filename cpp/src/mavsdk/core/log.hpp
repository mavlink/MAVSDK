/*
 * INTERNAL HEADER - DO NOT INCLUDE IN PUBLIC HEADERS
 *
 * This header contains logging functionality that uses fmt internally.
 * It should only be included by internal MAVSDK implementation files,
 * never by public headers that examples might include.
 *
 * The logging macros (LogDebug, LogInfo, LogWarn, LogErr) should only be
 * used in internal implementation files (.cpp files and *_impl.h files).
 */
#pragma once

#include <mutex>
#include <string>
#include <string_view>
#include "log_callback.hpp"
#include "mavsdk_export.hpp"

// fmt is used everywhere for logging across all platforms.
#include <fmt/format.h>
#include <fmt/ostream.h>

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

// Mutex moved to log.cpp to avoid inlining issues
MAVSDK_TEST_EXPORT std::mutex& get_log_mutex();

enum class Color { Red, Green, Yellow, Blue, Gray, Reset };

MAVSDK_TEST_EXPORT void set_color(Color color);

MAVSDK_PUBLIC void
emit_log(log::Level level, const std::string& message, const char* filename, int line);

template<typename... Args>
void log_message(
    log::Level level,
    const char* filename,
    int line,
    fmt::format_string<Args...> fmt_str,
    Args&&... args)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    emit_log(level, fmt::format(fmt_str, std::forward<Args>(args)...), filename, line);
}

// Overload for a plain string with no format args (avoids treating the string as a
// format string at compile time, which would reject strings containing braces).
inline void log_message(log::Level level, const char* filename, int line, std::string_view msg)
{
    std::lock_guard<std::mutex> lock(get_log_mutex());
    emit_log(level, std::string(msg), filename, line);
}

} // namespace mavsdk
