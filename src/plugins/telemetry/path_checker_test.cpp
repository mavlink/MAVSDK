#include <atomic>
#include <cmath>
#include <gtest/gtest.h>

#include "global_include.h"
#include "log.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/telemetry/path_checker.h"

using namespace mavsdk;
using namespace mavsdk::telemetry::path_checker;

TEST(TelemetryPathCheckerLine, OnWithMargin)
{
    const auto line = Line({10.f, 5.f, 0.f}, {10.f, 0.f, 0.f});
    const auto big_margin = 0.5f;
    const auto small_margin = 0.1f;

    EXPECT_TRUE(line.is_on({9.5f, 2.5f, 0.f}, big_margin));
    EXPECT_FALSE(line.is_on({9.5f, 2.5f, 0.f}, small_margin));
}

TEST(TelemetryPathCheckerLine, OnExtension)
{
    const auto line = Line({10.f, 5.f, 0.f}, {10.f, 0.f, 0.f});
    const auto big_margin = 0.5f;
    const auto small_margin = 0.1f;

    EXPECT_TRUE(line.is_on({10.f, 5.2f, 0.f}, big_margin));
    EXPECT_FALSE(line.is_on({10.f, 5.2f, 0.f}, small_margin));

    EXPECT_FALSE(line.is_on({10.f, 7.5f, 0.f}, big_margin));
    EXPECT_FALSE(line.is_on({10.f, -2.5f, 0.f}, big_margin));
}

#if 0
TEST(TelemetryPathChecker, TakeoffSegmentValid)
{
    auto takeoff_segment = std::unique_ptr<TakeoffSegment>(new TakeoffSegment());
    takeoff_segment->set_takeoff_altitude(2.5f);

    Telemetry::PositionNED too_low = {0.0f, 0.0f, -1.0f}; // 1 meter up
    EXPECT_TRUE(takeoff_segment->check_position_ned(too_low));
}

TEST(TelemetryPathChecker, TakeoffSegmentTooLow)
{
    auto takeoff_segment = std::unique_ptr<TakeoffSegment>(new TakeoffSegment());
    takeoff_segment->set_takeoff_altitude(2.5f);

    Telemetry::PositionNED too_low = {0.0f, 0.0f, 1.0f}; // 1 meter down
    EXPECT_FALSE(takeoff_segment->check_position_ned(too_low));
}

TEST(TelemetryPathChecker, TakeoffSegmentTooHigh)
{
    auto takeoff_segment = std::unique_ptr<TakeoffSegment>(new TakeoffSegment());
    takeoff_segment->set_takeoff_altitude(2.5f);

    Telemetry::PositionNED too_high = {0.0f, 0.0f, -4.0f}; // 4 meters up
    EXPECT_FALSE(takeoff_segment->check_position_ned(too_high));
}
#endif