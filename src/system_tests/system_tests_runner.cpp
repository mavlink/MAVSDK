#include <signal.h>
#include "log.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

#if defined(ENABLE_CPPTRACE)
#include <cpptrace/cpptrace.hpp>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <execinfo.h>
#include <unistd.h>
#endif

#include <gtest/gtest.h>

void crash_handler(int sig)
{
    const char* signal_name = strsignal(sig);
    mavsdk::LogErr() << "=== CRASH DETECTED ===";
    mavsdk::LogErr() << "Signal: " << signal_name << " (" << sig << ")";
    mavsdk::LogErr() << "Process: " << getpid();

    // Try cpptrace first (best quality stack trace)
#if defined(ENABLE_CPPTRACE)
    mavsdk::LogErr() << "Stack trace (cpptrace):";
    cpptrace::generate_trace().print();
#endif

    // Fallback to backtrace if available
#if defined(__linux__) || defined(__APPLE__)
    mavsdk::LogErr() << "Stack trace (backtrace):";
    void* array[20];
    size_t size = backtrace(array, 20);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

    mavsdk::LogErr() << "=== END CRASH INFO ===";

    // Force immediate exit to prevent hanging
    _exit(128 + sig);
}

int main(int argc, char** argv)
{
    // Always install crash handler, regardless of cpptrace availability
    signal(SIGSEGV, crash_handler);
    signal(SIGQUIT, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGILL, crash_handler);
    signal(SIGBUS, crash_handler);
    signal(SIGPIPE, crash_handler);
    signal(SIGFPE, crash_handler);

    // Log environment info for CI debugging
    mavsdk::LogInfo() << "=== Test Environment Info ===";
    mavsdk::LogInfo() << "PID: " << getpid();
    if (const char* ci = getenv("CI")) {
        mavsdk::LogInfo() << "CI Environment: " << ci;
    }
    if (const char* runner = getenv("GITHUB_ACTIONS")) {
        mavsdk::LogInfo() << "GitHub Actions: " << runner;
    }
    mavsdk::LogInfo() << "cpptrace support: "
#if defined(ENABLE_CPPTRACE)
                      << "enabled";
#else
                      << "disabled";
#endif
    mavsdk::LogInfo() << "=== Starting Tests ===";

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
