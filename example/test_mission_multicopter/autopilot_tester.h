#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/path_checker/path_checker.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "gtest/gtest.h"
#include <chrono>
#include <memory>
#include <thread>

class AutopilotTester {
public:
    [[nodiscard]] bool connect(const std::string uri);
    [[nodiscard]] bool wait_until_ready();
    // void subscribe_path_checker();
    [[nodiscard]] bool set_takeoff_altitude(const float altitude_m);
    [[nodiscard]] bool arm();
    [[nodiscard]] bool takeoff();
    [[nodiscard]] bool land();
    [[nodiscard]] bool wait_until_disarmed();

private:
    mavsdk::Mavsdk _mavsdk{};
    std::unique_ptr<mavsdk::Telemetry> _telemetry{};
    std::unique_ptr<mavsdk::Action> _action{};
};

template<typename Rep, typename Period>
bool poll_condition_with_timeout(
    std::function<bool()> fun, std::chrono::duration<Rep, Period> duration)
{
    // We need millisecond resolution for sleeping.
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
