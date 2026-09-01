#include "timeout_handler.hpp"
#include "unused.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <gtest/gtest.h>

#ifdef FAKE_TIME
#define Time FakeTime
#endif

using namespace mavsdk;

TEST(TimeoutHandler, Timeout)
{
    Time time;
    TimeoutHandler th(time);

    bool timeout_happened = false;

    TimeoutHandler::Cookie cookie = th.add([&timeout_happened]() { timeout_happened = true; }, 0.5);

    time.sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    time.sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);

    UNUSED(cookie);
}

TEST(TimeoutHandler, CallTimeoutInTimeoutCallback)
{
    Time time;
    TimeoutHandler th(time);

    bool timeout_happened = false;

    TimeoutHandler::Cookie cookie1{};
    TimeoutHandler::Cookie cookie2{};
    cookie1 = th.add(
        [&th, &timeout_happened, &cookie2]() {
            timeout_happened = true;
            // This tests the case where we want to set yet another timeout when we
            // are called because of a timeout. This tests if there are no locking
            // issues.
            cookie2 = th.add([]() {}, 5.0);
        },
        0.5);

    time.sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    time.sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);

    UNUSED(cookie1);
    UNUSED(cookie2);
}

TEST(TimeoutHandler, TimeoutRefreshed)
{
    Time time{};
    TimeoutHandler th(time);

    bool timeout_happened = false;

    auto cookie = th.add([&timeout_happened]() { timeout_happened = true; }, 0.5);

    time.sleep_for(std::chrono::milliseconds(400));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    th.refresh(cookie);
    time.sleep_for(std::chrono::milliseconds(300));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    time.sleep_for(std::chrono::milliseconds(300));
    th.run_once();
    EXPECT_TRUE(timeout_happened);

    UNUSED(cookie);
}

TEST(TimeoutHandler, TimeoutRemoved)
{
    Time time{};
    TimeoutHandler th(time);

    bool timeout_happened = false;

    auto cookie = th.add([&timeout_happened]() { timeout_happened = true; }, 0.5);

    time.sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    th.remove(cookie);
    time.sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
}

TEST(TimeoutHandler, TimeoutRemovedDuringCallback)
{
    Time time{};
    TimeoutHandler th(time);

    bool timeout_happened = false;

    TimeoutHandler::Cookie cookie = th.add(
        [&th, &cookie, &timeout_happened]() {
            // This is evil but can potentially happen. We remove our own timeout while
            // being called.
            th.remove(cookie);
            timeout_happened = true;
        },
        0.5);

    time.sleep_for(std::chrono::milliseconds(250));
    th.run_once();
    EXPECT_FALSE(timeout_happened);
    time.sleep_for(std::chrono::milliseconds(500));
    th.run_once();
    EXPECT_TRUE(timeout_happened);
}

TEST(TimeoutHandler, NextTimeoutRemovedDuringCallback)
{
    Time time{};
    TimeoutHandler th(time);

    TimeoutHandler::Cookie cookie2{};

    TimeoutHandler::Cookie cookie1 = th.add(
        [&th, &cookie2]() {
            // This is evil but can potentially happen. We remove the other timer while
            // being called. This triggers that the iterator is invalid and causes a segfault
            // if not handled properly.
            th.remove(cookie2);
        },
        0.5);

    cookie2 = th.add([]() {}, 0.5);

    time.sleep_for(std::chrono::milliseconds(1000));
    th.run_once();

    UNUSED(cookie1);
}

TEST(TimeoutHandler, AllTimeoutsRemovedDuringCallback)
{
    Time time{};
    TimeoutHandler th(time);

    TimeoutHandler::Cookie cookie1{};
    TimeoutHandler::Cookie cookie2{};
    TimeoutHandler::Cookie cookie3{};

    cookie1 = th.add(
        [&th, &cookie1, &cookie2, &cookie3]() {
            // Mirror CallEveryHandler stress: remove every cookie including self
            // while the first timeout fires.
            th.remove(cookie1);
            th.remove(cookie2);
            th.remove(cookie3);
        },
        0.5);

    cookie2 = th.add([]() {}, 0.5);
    cookie3 = th.add([]() {}, 0.5);

    time.sleep_for(std::chrono::milliseconds(1000));
    th.run_once();
}

TEST(TimeoutHandler, RemoveUnknownCookieIsNoOp)
{
    Time time{};
    TimeoutHandler th(time);
    bool fired = false;
    auto cookie = th.add([&fired]() { fired = true; }, 0.2);
    th.remove(static_cast<TimeoutHandler::Cookie>(999999));
    time.sleep_for(std::chrono::milliseconds(400));
    th.run_once();
    EXPECT_TRUE(fired);
    UNUSED(cookie);
}

TEST(TimeoutHandler, RefreshAfterFireDoesNotResurrect)
{
    Time time{};
    TimeoutHandler th(time);
    int fires = 0;
    auto cookie = th.add([&fires]() { ++fires; }, 0.2);
    time.sleep_for(std::chrono::milliseconds(400));
    th.run_once();
    EXPECT_EQ(fires, 1);
    // Cookie already consumed; refresh must not re-arm a timeout.
    th.refresh(cookie);
    time.sleep_for(std::chrono::milliseconds(400));
    th.run_once();
    EXPECT_EQ(fires, 1);
}

TEST(TimeoutHandler, RemovingDueTimeoutDuringCallbackPreventsIt)
{
    Time time{};
    TimeoutHandler th(time);

    bool second_happened = false;
    TimeoutHandler::Cookie cookie2{};

    // Both timeouts are due in the same run_once(). The first one removes the second,
    // which has to actually stop it from being called.
    TimeoutHandler::Cookie cookie1 = th.add([&th, &cookie2]() { th.remove(cookie2); }, 0.5);
    cookie2 = th.add([&second_happened]() { second_happened = true; }, 0.5);

    time.sleep_for(std::chrono::milliseconds(1000));
    th.run_once();

    EXPECT_FALSE(second_happened);

    UNUSED(cookie1);
}

TEST(TimeoutHandler, RemoveBlockingWaitsForRunningCallback)
{
    Time time{};
    TimeoutHandler th(time);

    std::promise<void> callback_entered;
    std::atomic<bool> release{false};
    std::atomic<bool> callback_returned{false};

    auto cookie = th.add(
        [&]() {
            callback_entered.set_value();
            while (!release) {
                std::this_thread::yield();
            }
            callback_returned = true;
        },
        0.5);

    time.sleep_for(std::chrono::milliseconds(1000));

    std::thread runner([&th]() { th.run_once(); });

    // Only call remove_blocking() once we know the callback is actually running.
    callback_entered.get_future().wait();

    std::thread releaser([&release]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        release = true;
    });

    th.remove_blocking(cookie);

    // The whole point: remove_blocking() does not return while the callback is running.
    EXPECT_TRUE(callback_returned);

    releaser.join();
    runner.join();
}

TEST(TimeoutHandler, RemoveBlockingFromOwnCallbackDoesNotDeadlock)
{
    Time time{};
    TimeoutHandler th(time);

    bool timeout_happened = false;
    TimeoutHandler::Cookie cookie{};

    cookie = th.add(
        [&th, &cookie, &timeout_happened]() {
            timeout_happened = true;
            // Waiting for ourselves here would hang forever.
            th.remove_blocking(cookie);
        },
        0.5);

    time.sleep_for(std::chrono::milliseconds(1000));
    th.run_once();

    EXPECT_TRUE(timeout_happened);
}

TEST(TimeoutHandler, RemoveBlockingUnsetCookieReturns)
{
    Time time{};
    TimeoutHandler th(time);

    // A default-initialised cookie means "never registered". Nothing is executing, so this
    // has to return rather than wait for a callback that does not exist.
    TimeoutHandler::Cookie never_registered{};
    th.remove_blocking(never_registered);
    th.remove_blocking(static_cast<TimeoutHandler::Cookie>(999999));
}
