#include "global_include.h"
#include <gtest/gtest.h>
#include <chrono>
#include <ctime>
#include <cmath>

using namespace dronecore;

TEST(GlobalInclude, SteadyTimeIncreasing)
{
    using std::chrono::steady_clock;

    dl_time_t time_before = steady_time();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    dl_time_t time_now = steady_time();

    ASSERT_GT(time_now, time_before);
}

TEST(GlobalInclude, ElapsedSinceAboutRight)
{
    dl_time_t time_before = steady_time();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double seconds_elapsed = elapsed_since_s(time_before);

    ASSERT_GT(seconds_elapsed, 0.09);
    ASSERT_LT(seconds_elapsed, 0.12); // we need some slack for macOS CI on travis.
}

TEST(GlobalInclude, SteadyTimeInFuture)
{
    dl_time_t in_future = steady_time_in_future(0.1);
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    dl_time_t now = steady_time();
    ASSERT_LT(now, in_future);
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    now = steady_time();
    ASSERT_GT(now, in_future);
}

TEST(GlobalInclude, ElapsedIncreasing)
{
    double before = elapsed_s();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    double now = elapsed_s();
    ASSERT_GT(now, before);
}

TEST(GlobalInclude, RadDeg)
{
    ASSERT_DOUBLE_EQ(0.0, to_rad_from_deg(0.0));
    ASSERT_DOUBLE_EQ(M_PI / 2.0, to_rad_from_deg(90.0));
    ASSERT_DOUBLE_EQ(M_PI, to_rad_from_deg(180.0));
    ASSERT_DOUBLE_EQ(-M_PI, to_rad_from_deg(-180.0));
    ASSERT_DOUBLE_EQ(2.0 * M_PI, to_rad_from_deg(360.0));

    ASSERT_DOUBLE_EQ(0.0, to_deg_from_rad(0.0));
    ASSERT_DOUBLE_EQ(90.0, to_deg_from_rad(M_PI / 2.0));
    ASSERT_DOUBLE_EQ(180, to_deg_from_rad(M_PI));
    ASSERT_DOUBLE_EQ(-180, to_deg_from_rad(-M_PI));
    ASSERT_DOUBLE_EQ(360.0, to_deg_from_rad(2.0 * M_PI));
}

TEST(GlobalInclude, FloatComparisons)
{
    EXPECT_TRUE(are_equal(0.0, 0.0));
    EXPECT_TRUE(are_equal(0.0f, 0.0f));
    EXPECT_FALSE(are_equal(0.0001, 0.0002));
    EXPECT_FALSE(are_equal(0.0001f, 0.0002f));
    EXPECT_FALSE(are_equal(1e20, 1e-20));
    EXPECT_FALSE(are_equal(1e20f, 1e-20f));
}
