#include <signal.h>
#include "log.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

#if defined(ENABLE_CPPTRACE)
#include <cpptrace/cpptrace.hpp>
#endif

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    // Use default signal handling to allow core dumps

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
