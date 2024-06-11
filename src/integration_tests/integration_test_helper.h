#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>
#include <gtest/gtest.h>
#include "log.h"
#include "unused.h"

// Autostarting SITL is not supported on Windows

template<typename Rep, typename Period>
bool poll_condition_with_timeout(
    std::function<bool()> fun, std::chrono::duration<Rep, Period> duration)
{
    // We need millisecond resolution for sleeping.
    const std::chrono::milliseconds duration_ms(duration);

    unsigned iteration = 0;
    while (!fun()) {
        std::this_thread::sleep_for(duration_ms / 100);
        if (iteration++ >= 100) {
            return false;
        }
    }
    return true;
}
