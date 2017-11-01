#include "call_every_handler.h"
#include <gtest/gtest.h>
#include <atomic>
#include "log.h"

using namespace dronecore;

TEST(CallEveryHandler, Single)
{
    CallEveryHandler ceh;

    int num_called = 0;

    void *cookie = nullptr;
    ceh.add([&num_called]() { ++num_called; }, 0.1f, &cookie);

    for (int i = 0; i < 11; ++i) {
        dc_sleep_for(std::chrono::milliseconds(10));
        ceh.run_once();
    }
    EXPECT_EQ(num_called, 1);

    UNUSED(cookie);
}

TEST(CallEveryHandler, Multiple)
{
    CallEveryHandler ceh;

    int num_called = 0;

    void *cookie = nullptr;
    ceh.add([&num_called]() { ++num_called; }, 0.1f, &cookie);

    for (int i = 0; i < 10; ++i) {
        dc_sleep_for(std::chrono::milliseconds(100));
        ceh.run_once();
    }
    EXPECT_EQ(num_called, 10);

    num_called = 0;
    ceh.change(0.2f, cookie);

    for (int i = 0; i < 20; ++i) {
        dc_sleep_for(std::chrono::milliseconds(100));
        ceh.run_once();
    }

    EXPECT_EQ(num_called, 10);

    num_called = 0;
    ceh.remove(cookie);
    dc_sleep_for(std::chrono::milliseconds(100));
    ceh.run_once();
    EXPECT_EQ(num_called, 0);
}

TEST(CallEveryHandler, InParallel)
{
    CallEveryHandler ceh;

    int num_called1 = 0;
    int num_called2 = 0;

    void *cookie1 = nullptr;
    void *cookie2 = nullptr;
    ceh.add([&num_called1]() { ++num_called1; }, 0.1f, &cookie1);
    ceh.add([&num_called2]() { ++num_called2; }, 0.2f, &cookie2);

    for (int i = 0; i < 10; ++i) {
        dc_sleep_for(std::chrono::milliseconds(100));
        ceh.run_once();
    }

    EXPECT_EQ(num_called1, 10);
    EXPECT_EQ(num_called2, 5);

    num_called1 = 0;
    num_called2 = 0;

    ceh.change(0.4f, cookie1);
    ceh.change(0.1f, cookie2);

    for (int i = 0; i < 10; ++i) {
        dc_sleep_for(std::chrono::milliseconds(100));
        ceh.run_once();
    }

    EXPECT_EQ(num_called1, 2);
    EXPECT_EQ(num_called2, 10);
}

TEST(CallEveryHandler, Reset)
{
    CallEveryHandler ceh;

    int num_called = 0;

    void *cookie = nullptr;
    ceh.add([&num_called]() { ++num_called; }, 0.1f, &cookie);

    for (int i = 0; i < 8; ++i) {
        dc_sleep_for(std::chrono::milliseconds(10));
        ceh.run_once();
        if (i == 8) {
        }
    }
    EXPECT_EQ(num_called, 0);

    ceh.reset(cookie);

    for (int i = 0; i < 8; ++i) {
        dc_sleep_for(std::chrono::milliseconds(10));
        ceh.run_once();
    }
    EXPECT_EQ(num_called, 0);

    for (int i = 0; i < 3; ++i) {
        dc_sleep_for(std::chrono::milliseconds(10));
        ceh.run_once();
    }
    EXPECT_EQ(num_called, 1);
}
