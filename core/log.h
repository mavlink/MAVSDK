#pragma once

#include <sstream>
#include "global_include.h"

#if ANDROID
#include <android/log.h>
#else
#include <iostream>
#include <ctime>
#endif

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_GRAY    "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#ifndef WINDOWS
// Remove path and extract only filename.
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define __FILENAME__  __FILE__
#endif

// For release builds, don't show debug printfs, and just discard it into the NullStream.
class NullStream
{
public:
    template<typename TPrintable>
    NullStream &operator<<(TPrintable const &)
    {
        /* no-op */
        static NullStream nothing;
        return nothing;
    }
};

#if DEBUG
#define LogDebug() LogDebugDetailed(__FILENAME__, __LINE__)
#else
#define LogDebug() NullStream()
#endif


#define LogInfo() LogInfoDetailed(__FILENAME__, __LINE__)
#define LogWarn() LogWarnDetailed(__FILENAME__, __LINE__)
#define LogErr() LogErrDetailed(__FILENAME__, __LINE__)


namespace dronecore {

class LogDetailed
{
public:
    LogDetailed(const char *filename, int filenumber) : _s(),
        _caller_filename(filename),
        _caller_filenumber(filenumber)
    {}

    template <typename T>
    LogDetailed &operator << (const T &x)
    {
        _s << x;
        return *this;
    }

    virtual ~LogDetailed()
    {
#if ANDROID
        switch (_log_level) {
            case LogLevel::Debug:
                __android_log_print(ANDROID_LOG_DEBUG, "DroneCore", "%s", _s.str().c_str());
                break;
            case LogLevel::Info:
                __android_log_print(ANDROID_LOG_INFO, "DroneCore", "%s", _s.str().c_str());
                break;
            case LogLevel::Warn:
                __android_log_print(ANDROID_LOG_WARN, "DroneCore", "%s", _s.str().c_str());
                break;
            case LogLevel::Err:
                __android_log_print(ANDROID_LOG_ERROR, "DroneCore", "%s", _s.str().c_str());
                break;
        }
        UNUSED(_caller_filename);
        UNUSED(_caller_filenumber);
#else

        // Time output taken from:
        // https://stackoverflow.com/questions/16357999#answer-16358264


        time_t rawtime;
        time(&rawtime);
        struct tm *timeinfo = localtime(&rawtime);
        char time_buffer[10] {}; // We need 8 characters + \0
        strftime(time_buffer, sizeof(time_buffer), "%I:%M:%S", timeinfo);
        std::cout << "[" << time_buffer;

        switch (_log_level) {
            case LogLevel::Debug:
                std::cout << "|Debug] ";
                break;
            case LogLevel::Info:
                std::cout << ANSI_COLOR_BLUE;
                std::cout << "|Info ] ";
                break;
            case LogLevel::Warn:
                std::cout << ANSI_COLOR_YELLOW;
                std::cout << "|Warn ] ";
                break;
            case LogLevel::Err:
                std::cout << ANSI_COLOR_RED;
                std::cout << "|Error] ";
                break;
        }

        std::cout << _s.str();
        std::cout << " (" << _caller_filename << ":" << _caller_filenumber << ")";

        switch (_log_level) {
            case LogLevel::Debug:
                break;
            case LogLevel::Info:
            // FALLTHROUGH
            case LogLevel::Warn:
            // FALLTHROUGH
            case LogLevel::Err:
                std::cout << ANSI_COLOR_RESET;
                break;
        }

        std::cout << std::endl;
#endif
    }

protected:
    enum LogLevel {
        Debug,
        Info,
        Warn,
        Err
    } _log_level;

private:
    std::stringstream _s;
    const char *_caller_filename;
    int _caller_filenumber;
};

class LogDebugDetailed : public LogDetailed
{
public:
    LogDebugDetailed(const char *filename, int filenumber) :
        LogDetailed(filename, filenumber)
    {
        _log_level = LogLevel::Debug;
    }
};

class LogInfoDetailed : public LogDetailed
{
public:
    LogInfoDetailed(const char *filename, int filenumber) :
        LogDetailed(filename, filenumber)
    {
        _log_level = LogLevel::Info;
    }
};

class LogWarnDetailed : public LogDetailed
{
public:
    LogWarnDetailed(const char *filename, int filenumber) :
        LogDetailed(filename, filenumber)
    {
        _log_level = LogLevel::Warn;
    }
};

class LogErrDetailed : public LogDetailed
{
public:
    LogErrDetailed(const char *filename, int filenumber) :
        LogDetailed(filename, filenumber)
    {
        _log_level = LogLevel::Err;
    }
};

} // namespace dronecore
