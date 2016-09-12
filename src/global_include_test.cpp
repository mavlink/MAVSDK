#include "global_include.h"
#include <gtest/gtest.h>
#include <unistd.h>
#include <chrono>
#include <ctime>

using namespace dronelink;

TEST(GlobalInclude, SteadyTimeIncreasing)
{
    using std::chrono::steady_clock;

    dl_time_t time_before = steady_time();
    usleep(100000);
    dl_time_t time_now = steady_time();

    ASSERT_GT(time_now, time_before);
}

TEST(GlobalInclude, ElapsedAboutRight)
{
    dl_time_t time_before = steady_time();
    usleep(100000);
    double seconds_elapsed = elapsed_s(time_before);

    ASSERT_GT(seconds_elapsed, 0.09);
    ASSERT_LT(seconds_elapsed, 0.11);
}

TEST(GlobalInclude, SteadyTimeInFuture)
{
    dl_time_t in_future = steady_time_in_future(0.1);
    usleep(80000);
    dl_time_t now = steady_time();
    ASSERT_LT(now, in_future);
    usleep(40000);
    now = steady_time();
    ASSERT_GT(now, in_future);
}


