#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <string>
#include <gtest/gtest.h>
#include "global_include.h"
#include "log.h"

// Autostarting SITL is not supported on Windows

class SitlTest : public testing::Test {
protected:
    void StartPX4(const std::string &model)
    {
#ifndef WINDOWS
        const int ret = system((std::string("./start_px4_sitl.sh ") + model).c_str());
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

    void StopPX4()
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

    virtual void SetUp() override { StartPX4(determineModel()); }

    virtual void TearDown() override { StopPX4(); }

private:
    static std::string determineModel()
    {
        // If no model name is appended to the test name, we use the default which is iris.
        std::string model_name = "iris";

        const std::string &test_name =
            ::testing::UnitTest::GetInstance()->current_test_info()->name();
        const size_t pos = test_name.find_first_of('_', 0);
        if (pos != std::string::npos) {
            model_name = test_name.substr(pos + 1, test_name.length() - pos - 1);
        }

        dronecode_sdk::LogDebug() << "Model chosen: '" << model_name << "'";
        return model_name;
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
