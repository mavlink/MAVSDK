#include "blocking_calls.hpp"
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(BlockingCalls, ReturnsCompletedValue)
{
    BlockingCalls blocking_calls;

    std::thread completer;
    const int result = blocking_calls.wait(
        [&](auto complete) {
            completer = std::thread([complete]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                complete(42);
            });
        },
        -1);
    completer.join();

    EXPECT_EQ(result, 42);
}

TEST(BlockingCalls, CompletedInlineReturnsValue)
{
    BlockingCalls blocking_calls;
    EXPECT_EQ(blocking_calls.wait([](auto complete) { complete(7); }, -1), 7);
}

TEST(BlockingCalls, ReleaseEndsWait)
{
    auto blocking_calls = std::make_unique<BlockingCalls>();

    // Kept so it can still be called after the release, like a late io-thread callback.
    auto late_complete = std::make_shared<std::promise<std::function<void(int)>>>();
    auto late_complete_future = late_complete->get_future();

    auto result = std::make_shared<std::promise<int>>();
    auto result_future = result->get_future();
    auto* raw = blocking_calls.get();
    std::thread([raw, late_complete, result]() {
        result->set_value(raw->wait(
            [&](auto complete) { late_complete->set_value(std::function<void(int)>(complete)); },
            -1));
    }).detach();

    ASSERT_EQ(late_complete_future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    auto complete = late_complete_future.get();
    ASSERT_NE(result_future.wait_for(std::chrono::milliseconds(50)), std::future_status::ready);

    // Release and destroy the owner while the call is blocked, as a destructor does.
    EXPECT_EQ(blocking_calls->release_all(), 1u);
    blocking_calls.reset();

    ASSERT_EQ(result_future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_EQ(result_future.get(), -1);

    // A late completion is ignored.
    complete(42);
}

TEST(BlockingCalls, WaitAfterReleaseReturnsImmediately)
{
    BlockingCalls blocking_calls;
    EXPECT_EQ(blocking_calls.release_all(), 0u);

    bool started = false;
    EXPECT_EQ(blocking_calls.wait([&](auto) { started = true; }, -1), -1);
    EXPECT_FALSE(started);
}

TEST(BlockingCalls, ReleaseAfterCompletionDoesNothing)
{
    BlockingCalls blocking_calls;
    EXPECT_EQ(blocking_calls.wait([](auto complete) { complete(3); }, -1), 3);
    EXPECT_EQ(blocking_calls.release_all(), 0u);
}
