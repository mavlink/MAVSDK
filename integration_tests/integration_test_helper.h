#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <gtest/gtest.h>
#include "global_include.h"
#include "log.h"

// Autostarting SITL is not supported on Windows

class SitlTest : public testing::Test {
protected:
    virtual void SetUp() override
    {
#ifndef WINDOWS
        const int ret = system("./start_px4_sitl.sh");
        if (ret != 0) {
            dronecode_sdk::LogErr() << "./start_px4_sitl.sh failed, giving up.";
            abort();
        }
        // We need to wait a bit until it's up and running.
        std::this_thread::sleep_for(std::chrono::seconds(3));
#else
        dronecode_sdk::LogErr() << "Auto-starting SITL not supported on Windows.";
#endif
    }
    virtual void TearDown() override
    {
#ifndef WINDOWS
        // Don't rush this either.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const int ret = system("./stop_px4_sitl.sh");
        if (ret != 0) {
            dronecode_sdk::LogErr() << "./stop_px4_sitl.sh failed, giving up.";
            abort();
        }
#else
        dronecode_sdk::LogErr() << "Auto-starting SITL not supported on Windows.";
#endif
    }
};

template<typename Rep, typename Period>
bool poll_condition_with_timeout(std::function<bool()> fun,
                                 std::chrono::duration<Rep, Period> duration)
{
    // We need at millisecond resolution for sleeping.
    const std::chrono::milliseconds duration_ms(duration);

    unsigned iteration = 0;
    while (!fun()) {
        std::this_thread::sleep_for(duration_ms / 10);
        if (iteration++ >= 10) {
            return false;
        }
    }
    return true;
}
