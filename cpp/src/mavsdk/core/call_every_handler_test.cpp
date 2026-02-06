#include "call_every_handler.h"
#include "unused.h"
#include <gtest/gtest.h>

#ifdef FAKE_TIME
#define Time FakeTime
#endif

using namespace mavsdk;

TEST(CallEveryHandler, Single)
{
    Time time{};

    CallEveryHandler ceh(time);

    int num_called = 0;

    auto cookie = ceh.add([&num_called]() { ++num_called; }, 0.1);

    for (int i = 0; i < 11; ++i) {
        time.sleep_for(std::chrono::milliseconds(10));
        ceh.run_once();
    }
    EXPECT_EQ(num_called, 2);

    UNUSED(cookie);
}

TEST(CallEveryHandler, Multiple)
{
    Time time{};
    CallEveryHandler ceh(time);

    int num_called = 0;

    auto cookie = ceh.add([&num_called]() { ++num_called; }, 0.1);

    for (int i = 0; i < 10; ++i) {
        ceh.run_once();
        time.sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_EQ(num_called, 10);

    num_called = 0;
    ceh.change(0.2, cookie);

    for (int i = 0; i < 20; ++i) {
        time.sleep_for(std::chrono::milliseconds(100));
        ceh.run_once();
    }

    EXPECT_EQ(num_called, 10);

    num_called = 0;
    ceh.remove(cookie);
    time.sleep_for(std::chrono::milliseconds(100));
    ceh.run_once();
    EXPECT_EQ(num_called, 0);
}

TEST(CallEveryHandler, InParallel)
{
    Time time{};
    CallEveryHandler ceh(time);

    int num_called1 = 0;
    int num_called2 = 0;

    auto cookie1 = ceh.add([&num_called1]() { ++num_called1; }, 0.1);
    auto cookie2 = ceh.add([&num_called2]() { ++num_called2; }, 0.2);

    for (int i = 0; i < 10; ++i) {
        ceh.run_once();
        time.sleep_for(std::chrono::milliseconds(100));
    }

    EXPECT_EQ(num_called1, 10);
    EXPECT_EQ(num_called2, 5);

    num_called1 = 0;
    num_called2 = 0;

    ceh.change(0.4, cookie1);
    ceh.change(0.1, cookie2);

    for (int i = 0; i < 10; ++i) {
        ceh.run_once();
        time.sleep_for(std::chrono::milliseconds(100));
    }

    EXPECT_EQ(num_called1, 2);
    EXPECT_EQ(num_called2, 10);
}

TEST(CallEveryHandler, Reset)
{
    Time time{};
    CallEveryHandler ceh(time);

    int num_called = 0;

    auto cookie = ceh.add([&num_called]() { ++num_called; }, 0.1);

    for (int i = 0; i < 8; ++i) {
        ceh.run_once();
        time.sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(num_called, 1);

    ceh.reset(cookie);

    for (int i = 0; i < 8; ++i) {
        ceh.run_once();
        time.sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(num_called, 1);

    for (int i = 0; i < 3; ++i) {
        ceh.run_once();
        time.sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(num_called, 2);
}

TEST(CallEveryHandler, CallImmediately)
{
    Time time{};
    CallEveryHandler ceh(time);

    int num_called = 0;

    auto cookie = ceh.add([&num_called]() { ++num_called; }, 0.1);
    UNUSED(cookie);

    for (int i = 0; i < 1; ++i) {
        ceh.run_once();
    }
    EXPECT_EQ(num_called, 1);
}

TEST(CallEveryHandler, NextHandlerRemovedDuringCallback)
{
    Time time{};
    CallEveryHandler ceh(time);

    CallEveryHandler::Cookie cookie2{};

    CallEveryHandler::Cookie cookie1 = ceh.add(
        [&ceh, &cookie2]() {
            // This is evil but can potentially happen. We remove the other handler while
            // being called. This triggers that the iterator is invalid and could cause a segfault
            // if not handled properly.
            ceh.remove(cookie2);
        },
        0.1);

    cookie2 = ceh.add([]() {}, 0.1);

    time.sleep_for(std::chrono::milliseconds(200));
    ceh.run_once();

    UNUSED(cookie1);
}

TEST(CallEveryHandler, AllHandlersRemovedDuringCallback)
{
    Time time{};
    CallEveryHandler ceh(time);

    CallEveryHandler::Cookie cookie1{};
    CallEveryHandler::Cookie cookie2{};
    CallEveryHandler::Cookie cookie3{};

    cookie1 = ceh.add(
        [&ceh, &cookie1, &cookie2, &cookie3]() {
            // This is even more evil. We remove all handlers, including ourselves,
            // while being called. This tests if there are no iterator invalidation issues.
            ceh.remove(cookie1);
            ceh.remove(cookie2);
            ceh.remove(cookie3);
        },
        0.1);

    cookie2 = ceh.add([]() {}, 0.1);
    cookie3 = ceh.add([]() {}, 0.1);

    time.sleep_for(std::chrono::milliseconds(200));
    ceh.run_once();
}
