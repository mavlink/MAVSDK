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
    // Use only async-signal-safe functions in signal handler
    const char msg1[] = "=== CRASH DETECTED ===\n";
    (void)write(STDERR_FILENO, msg1, sizeof(msg1) - 1);

    // Format signal info using async-signal-safe functions
    char sig_msg[100];
    const char* signal_name = strsignal(sig);
    int len = 0;
    if (signal_name) {
        len = snprintf(sig_msg, sizeof(sig_msg), "Signal: %s (%d)\n", signal_name, sig);
    } else {
        len = snprintf(sig_msg, sizeof(sig_msg), "Signal: %d\n", sig);
    }
    if (len > 0 && len < (int)sizeof(sig_msg)) {
        (void)write(STDERR_FILENO, sig_msg, len);
    }

    // Process ID
    char pid_msg[50];
    len = snprintf(pid_msg, sizeof(pid_msg), "Process: %d\n", getpid());
    if (len > 0 && len < (int)sizeof(pid_msg)) {
        (void)write(STDERR_FILENO, pid_msg, len);
    }

    // Stack trace using async-signal-safe backtrace_symbols_fd
#if defined(__linux__) || defined(__APPLE__)
    const char trace_msg[] = "Stack trace (backtrace):\n";
    (void)write(STDERR_FILENO, trace_msg, sizeof(trace_msg) - 1);

    void* array[20];
    size_t size = backtrace(array, 20);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

    const char msg2[] = "=== END CRASH INFO ===\n";
    (void)write(STDERR_FILENO, msg2, sizeof(msg2) - 1);

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
