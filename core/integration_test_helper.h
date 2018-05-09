#pragma once

#include <gtest/gtest.h>
#include "global_include.h"
#include "log.h"

// Autostarting SITL is not supported on Windows

class SitlTest : public testing::Test
{
protected:
    virtual void SetUp() override
    {
#ifndef WINDOWS
        const int ret = system("./start_px4_sitl.sh");
        if (ret != 0) {
            dronecore::LogErr() << "./start_px4_sitl.sh failed, giving up.";
            abort();
        }
        // We need to wait a bit until it's up and running.
        std::this_thread::sleep_for(std::chrono::seconds(3));
#else
        dronecore::LogErr() << "Auto-starting SITL not supported on Windows.";
#endif
    }
    virtual void TearDown() override
    {
#ifndef WINDOWS
        // Don't rush this either.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const int ret = system("./stop_px4_sitl.sh");
        if (ret != 0) {
            dronecore::LogErr() << "./stop_px4_sitl.sh failed, giving up.";
            abort();
        }
#else
        dronecore::LogErr() << "Auto-starting SITL not supported on Windows.";
#endif
    }
};
