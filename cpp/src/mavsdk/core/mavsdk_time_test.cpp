#include "mavsdk_time.h"
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
