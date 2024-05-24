#pragma once

#include <bitset>
#include <cstddef>
#include <mutex>
#include <sstream>
#include "log_callback.h"

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

#define LogDebug() LogDebugDetailed(FILENAME, __LINE__)
#define LogInfo() LogInfoDetailed(FILENAME, __LINE__)
#define LogWarn() LogWarnDetailed(FILENAME, __LINE__)
#define LogErr() LogErrDetailed(FILENAME, __LINE__)

namespace mavsdk {

static std::mutex log_mutex_{};

std::ostream& operator<<(std::ostream& os, std::byte b);

enum class Color { Red, Green, Yellow, Blue, Gray, Reset };

void set_color(Color color);

class LogDetailed {
public:
    LogDetailed(const char* filename, int filenumber) :
        _lock_guard(log_mutex_),
        _s(),
        _caller_filename(filename),
        _caller_filenumber(filenumber)
    {}

    template<typename T> LogDetailed& operator<<(const T& x)
    {
        _s << x;
        return *this;
    }

    virtual
#if defined(__has_feature)
#if __has_feature(thread_sanitizer)
        __attribute__((no_sanitize("thread")))
#endif
#endif
        ~LogDetailed()
    {
        if (log::get_callback() &&
            log::get_callback()(_log_level, _s.str(), _caller_filename, _caller_filenumber)) {
            return;
        }

#if ANDROID
        switch (_log_level) {
            case log::Level::Debug:
                __android_log_print(ANDROID_LOG_DEBUG, "Mavsdk", "%s", _s.str().c_str());
                break;
            case log::Level::Info:
                __android_log_print(ANDROID_LOG_INFO, "Mavsdk", "%s", _s.str().c_str());
                break;
            case log::Level::Warn:
                __android_log_print(ANDROID_LOG_WARN, "Mavsdk", "%s", _s.str().c_str());
                break;
            case log::Level::Err:
                __android_log_print(ANDROID_LOG_ERROR, "Mavsdk", "%s", _s.str().c_str());
                break;
        }
        // Unused:
        (void)_caller_filename;
        (void)_caller_filenumber;
#else

        switch (_log_level) {
            case log::Level::Debug:
                set_color(Color::Green);
                break;
            case log::Level::Info:
                set_color(Color::Blue);
                break;
            case log::Level::Warn:
                set_color(Color::Yellow);
                break;
            case log::Level::Err:
                set_color(Color::Red);
                break;
        }

        // Time output taken from:
        // https://stackoverflow.com/questions/16357999#answer-16358264
        time_t rawtime;
        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);
        char time_buffer[10]{}; // We need 8 characters + \0
        strftime(time_buffer, sizeof(time_buffer), "%I:%M:%S", timeinfo);
        std::cout << "[" << time_buffer;

        switch (_log_level) {
            case log::Level::Debug:
                std::cout << "|Debug] ";
                break;
            case log::Level::Info:
                std::cout << "|Info ] ";
                break;
            case log::Level::Warn:
                std::cout << "|Warn ] ";
                break;
            case log::Level::Err:
                std::cout << "|Error] ";
                break;
        }

        set_color(Color::Reset);

        std::cout << _s.str();
        std::cout << " (" << _caller_filename << ":" << std::dec << _caller_filenumber << ")";

        std::cout << '\n';
#endif
    }

    LogDetailed(const mavsdk::LogDetailed&) = delete;
    void operator=(const mavsdk::LogDetailed&) = delete;

protected:
    log::Level _log_level = log::Level::Debug;

private:
    std::lock_guard<std::mutex> _lock_guard;

    std::stringstream _s;
    const char* _caller_filename;
    int _caller_filenumber;
};

class LogDebugDetailed : public LogDetailed {
public:
    LogDebugDetailed(const char* filename, int filenumber) : LogDetailed(filename, filenumber)
    {
        _log_level = log::Level::Debug;
    }
};

class LogInfoDetailed : public LogDetailed {
public:
    LogInfoDetailed(const char* filename, int filenumber) : LogDetailed(filename, filenumber)
    {
        _log_level = log::Level::Info;
    }
};

class LogWarnDetailed : public LogDetailed {
public:
    LogWarnDetailed(const char* filename, int filenumber) : LogDetailed(filename, filenumber)
    {
        _log_level = log::Level::Warn;
    }
};

class LogErrDetailed : public LogDetailed {
public:
    LogErrDetailed(const char* filename, int filenumber) : LogDetailed(filename, filenumber)
    {
        _log_level = log::Level::Err;
    }
};

} // namespace mavsdk
