#include "mavsdk_time.hpp"
#include <gtest/gtest.h>
#include <chrono>

#ifdef FAKE_TIME
#define Time FakeTime
#endif

using namespace mavsdk;

TEST(Time, SteadyTimeIncreasing)
{
    Time time{};

    SteadyTimePoint time_before = time.steady_time();
    time.sleep_for(std::chrono::milliseconds(100));
    SteadyTimePoint time_now = time.steady_time();

    ASSERT_GT(time_now, time_before);
}

TEST(Time, ElapsedSinceAboutRight)
{
    Time time{};
    SteadyTimePoint time_before = time.steady_time();
    time.sleep_for(std::chrono::milliseconds(100));
    double seconds_elapsed = time.elapsed_since_s(time_before);

    ASSERT_GT(seconds_elapsed, 0.09);
    ASSERT_LT(seconds_elapsed, 0.12); // we need some slack for macOS CI on travis.
}

TEST(Time, SteadyTimeInFuture)
{
    Time time{};
    SteadyTimePoint in_future = time.steady_time_in_future(0.1);
    time.sleep_for(std::chrono::milliseconds(80));
    SteadyTimePoint now = time.steady_time();
    ASSERT_LT(now, in_future);
    time.sleep_for(std::chrono::milliseconds(40));
    now = time.steady_time();
    ASSERT_GT(now, in_future);
}

TEST(Time, ElapsedIncreasing)
{
    Time time{};
    double before = time.elapsed_s();
    time.sleep_for(std::chrono::milliseconds(10));
    double now = time.elapsed_s();
    ASSERT_GT(now, before);
}

TEST(Time, ShiftSteadyTimeByPositiveAndNegative)
{
    Time time{};
    SteadyTimePoint base = time.steady_time();

    SteadyTimePoint shifted = base;
    Time::shift_steady_time_by(shifted, 0.25);
    auto forward_ms = std::chrono::duration_cast<std::chrono::milliseconds>(shifted - base).count();
    EXPECT_EQ(forward_ms, 250);

    SteadyTimePoint back = shifted;
    Time::shift_steady_time_by(back, -0.25);
    auto back_ms = std::chrono::duration_cast<std::chrono::milliseconds>(back - base).count();
    EXPECT_EQ(back_ms, 0);
}

TEST(Time, SteadyTimeAdvancesWithSleep)
{
    // Use steady_time() so this is valid under FAKE_TIME (where Time is
    // #define'd to FakeTime and sleep_for advances the fake clock only).
    // elapsed_ms()/elapsed_us() always read the wall steady_clock and would
    // not advance during FakeTime::sleep_for.
    Time time{};
    const SteadyTimePoint t1 = time.steady_time();
    time.sleep_for(std::chrono::milliseconds(5));
    const SteadyTimePoint t2 = time.steady_time();
    EXPECT_GT(t2, t1);
    const auto advanced_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    EXPECT_GE(advanced_us, 5000);
}

TEST(FakeTime, SleepAdvancesWithoutWallClock)
{
    FakeTime ft{};
    SteadyTimePoint t0 = ft.steady_time();
    ft.sleep_for(std::chrono::milliseconds(250));
    SteadyTimePoint t1 = ft.steady_time();
    // FakeTime adds ~50us overhead per sleep
    auto advanced = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    EXPECT_GE(advanced, 250);
    EXPECT_LT(advanced, 260);
}

TEST(AutopilotTime, ShiftAndTimeIn)
{
    AutopilotTime at{};
    SystemTimePoint local = SystemTimePoint(std::chrono::milliseconds(1000000));
    AutopilotTimePoint before = at.time_in(local);

    at.shift_time_by(std::chrono::milliseconds(500));
    AutopilotTimePoint after = at.time_in(local);

    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
    EXPECT_EQ(delta, 500);
}
