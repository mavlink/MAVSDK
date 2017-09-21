#include "timeout_handler.h"
#include <gtest/gtest.h>
#include <atomic>

using namespace dronecore;

TEST(TimeoutHandler, Timeout)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    void *cookie = nullptr;
    th.add([&timeout_happened]() {
        timeout_happened = true;
    }, 0.5, &cookie);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);

    UNUSED(cookie);
}

TEST(TimeoutHandler, TimeoutNoCookie)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    // This time we supply nullptr and don't want a cookie.
    th.add([&timeout_happened]() {
        timeout_happened = true;
    }, 0.5, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);
}

TEST(TimeoutHandler, CallTimeoutInTimeoutCallback)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    void *cookie1 = nullptr;
    void *cookie2 = nullptr;
    th.add([&th, &timeout_happened, &cookie2]() {
        timeout_happened = true;
        // This tests the case where we want to set yet another timeout when we
        // are called because of a timeout. This tests if there are no locking
        // issues.
        th.add([]() {
        }, 5.0, &cookie2);
    }, 0.5, &cookie1);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);

    UNUSED(cookie1);
    UNUSED(cookie2);
}

TEST(TimeoutHandler, TimeoutRefreshed)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    void *cookie = nullptr;
    th.add([&timeout_happened]() {
        timeout_happened = true;
    }, 0.5, &cookie);

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    th.refresh(cookie);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    th.run_once();
    EXPECT_TRUE(timeout_happened);

    UNUSED(cookie);
}

TEST(TimeoutHandler, TimeoutRemoved)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    void *cookie = nullptr;
    th.add([&timeout_happened]() {
        timeout_happened = true;
    }, 0.5, &cookie);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    th.remove(cookie);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
}

TEST(TimeoutHandler, TimeoutRemovedDuringCallback)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    void *cookie = nullptr;
    th.add([&th, &cookie, &timeout_happened]() {
        // This is evil but can potentially happen. We remove our own timeout while
        // being called.
        th.remove(cookie);
        timeout_happened = true;
    }, 0.5, &cookie);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);
}

TEST(TimeoutHandler, TimeoutUpdate)
{
    TimeoutHandler th;

    bool timeout_happened = false;

    void *cookie = nullptr;
    th.add([&timeout_happened]() {
        timeout_happened = true;
    }, 0.5, &cookie);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);

    th.update(1.0, cookie);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_FALSE(timeout_happened);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);
}
