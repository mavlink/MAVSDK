#include "mavsdk.hpp"
#include "mavlink_include.hpp"

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <optional>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;

namespace {

using Result = std::optional<std::shared_ptr<System>>;

std::vector<char> make_heartbeat(uint8_t component_id, uint8_t type, uint8_t autopilot)
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack_chan(
        1,
        component_id,
        MAVLINK_COMM_NUM_BUFFERS - 1,
        &message,
        type,
        autopilot,
        MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
        0,
        MAV_STATE_STANDBY);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    const auto length = mavlink_msg_to_send_buffer(buffer.data(), &message);

    return std::vector<char>(buffer.begin(), buffer.begin() + length);
}

void pass_autopilot_heartbeat(Mavsdk& mavsdk)
{
    const auto bytes =
        make_heartbeat(MAV_COMP_ID_AUTOPILOT1, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4);
    mavsdk.pass_received_raw_bytes(bytes.data(), bytes.size());
}

void pass_companion_heartbeat(Mavsdk& mavsdk)
{
    const auto bytes = make_heartbeat(
        MAV_COMP_ID_ONBOARD_COMPUTER, MAV_TYPE_ONBOARD_CONTROLLER, MAV_AUTOPILOT_INVALID);
    mavsdk.pass_received_raw_bytes(bytes.data(), bytes.size());
}

// Collects the result of first_autopilot_async() and counts how often it was delivered.
struct Collector {
    std::promise<Result> promise{};
    std::future<Result> future{promise.get_future()};
    std::atomic<int> calls{0};

    Mavsdk::FirstAutopilotCallback callback()
    {
        return [this](Result result) {
            if (++calls == 1) {
                promise.set_value(std::move(result));
            }
        };
    }
};

} // namespace

TEST(FirstAutopilotAsync, FindsAutopilotAlreadyThere)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);
    pass_autopilot_heartbeat(mavsdk);
    ASSERT_TRUE(mavsdk.first_autopilot(2.0));

    auto collector = std::make_shared<Collector>();
    const auto caller = std::this_thread::get_id();
    auto called_from = std::make_shared<std::thread::id>();
    mavsdk.first_autopilot_async(5.0, [collector, called_from](Result result) {
        *called_from = std::this_thread::get_id();
        collector->callback()(std::move(result));
    });

    ASSERT_EQ(collector->future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    auto result = collector->future.get();
    ASSERT_TRUE(result);
    EXPECT_TRUE(result.value()->has_autopilot());
    EXPECT_NE(*called_from, caller) << "the callback must not be called from within the call";
}

TEST(FirstAutopilotAsync, FindsAutopilotThatAppearsLater)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    auto collector = std::make_shared<Collector>();
    mavsdk.first_autopilot_async(5.0, collector->callback());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(collector->calls.load(), 0);

    pass_autopilot_heartbeat(mavsdk);
    ASSERT_EQ(collector->future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_TRUE(collector->future.get());
}

TEST(FirstAutopilotAsync, FindsAutopilotJoiningConnectedSystem)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);
    pass_companion_heartbeat(mavsdk);
    ASSERT_FALSE(mavsdk.first_autopilot(0.5));

    auto collector = std::make_shared<Collector>();
    mavsdk.first_autopilot_async(5.0, collector->callback());
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    pass_autopilot_heartbeat(mavsdk);
    ASSERT_EQ(collector->future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_TRUE(collector->future.get());
}

TEST(FirstAutopilotAsync, TimesOutOnce)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    auto collector = std::make_shared<Collector>();
    const auto start = std::chrono::steady_clock::now();
    mavsdk.first_autopilot_async(0.5, collector->callback());

    ASSERT_EQ(collector->future.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    EXPECT_FALSE(collector->future.get());
    EXPECT_GE(std::chrono::steady_clock::now() - start, std::chrono::milliseconds(400));

    // An autopilot showing up after the timeout must not produce a second call.
    pass_autopilot_heartbeat(mavsdk);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    EXPECT_EQ(collector->calls.load(), 1);
}

TEST(FirstAutopilotAsync, ZeroTimeoutChecksOnce)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    auto none = std::make_shared<Collector>();
    mavsdk.first_autopilot_async(0.0, none->callback());
    ASSERT_EQ(none->future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_FALSE(none->future.get());

    pass_autopilot_heartbeat(mavsdk);
    ASSERT_TRUE(mavsdk.first_autopilot(2.0));

    auto found = std::make_shared<Collector>();
    mavsdk.first_autopilot_async(0.0, found->callback());
    ASSERT_EQ(found->future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_TRUE(found->future.get());
}

TEST(FirstAutopilotAsync, CancelledWaitIsNotCalled)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    auto collector = std::make_shared<Collector>();
    const auto handle = mavsdk.first_autopilot_async(0.5, collector->callback());
    mavsdk.cancel_first_autopilot(handle);
    // Cancelling again, or after completion, does nothing.
    mavsdk.cancel_first_autopilot(handle);

    // Neither the autopilot nor the timeout may get through anymore.
    pass_autopilot_heartbeat(mavsdk);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_EQ(collector->calls.load(), 0);
}

TEST(FirstAutopilotAsync, CancelAfterCompletionDoesNothing)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    auto collector = std::make_shared<Collector>();
    const auto handle = mavsdk.first_autopilot_async(0.2, collector->callback());
    ASSERT_EQ(collector->future.wait_for(std::chrono::seconds(2)), std::future_status::ready);

    mavsdk.cancel_first_autopilot(handle);
    EXPECT_EQ(collector->calls.load(), 1);
}

TEST(FirstAutopilotAsync, CancelFromWithinCallback)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    // Handed over through a promise, so the callback can't read it before it is set.
    auto handle_promise = std::make_shared<std::promise<Mavsdk::FirstAutopilotHandle>>();
    auto handle_future = handle_promise->get_future().share();
    auto done = std::make_shared<std::promise<void>>();
    handle_promise->set_value(
        mavsdk.first_autopilot_async(0.2, [&mavsdk, handle_future, done](Result) {
            // Must not wait for itself.
            mavsdk.cancel_first_autopilot(handle_future.get());
            done->set_value();
        }));

    EXPECT_EQ(done->get_future().wait_for(std::chrono::seconds(2)), std::future_status::ready);
}

TEST(FirstAutopilotAsync, DestroyWhileWaiting)
{
    auto collector = std::make_shared<Collector>();
    {
        Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
        ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);
        // A connected system without an autopilot, so the wait also watches its components.
        pass_companion_heartbeat(mavsdk);
        ASSERT_FALSE(mavsdk.first_autopilot(0.5));

        mavsdk.first_autopilot_async(30.0, collector->callback());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // Destroyed while still waiting: neither crashes nor aborts for outliving references.
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(collector->calls.load(), 0);
}

TEST(FirstAutopilotAsync, DestroyWithResultQueued)
{
    // The result is decided on MAVSDK's threads and delivered on the callback thread. If the
    // Mavsdk goes away in between, the queued result must not hold the System alive past it.
    for (int i = 0; i < 20; ++i) {
        // Counts only, and lets the result go again: whether the callback still runs before the
        // Mavsdk is destroyed is a race, and keeping the System here would make this test
        // outlive it -- the very mistake abort_if_references_outlive_us() aborts on.
        auto calls = std::make_shared<std::atomic<int>>(0);
        {
            Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
            ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);
            pass_autopilot_heartbeat(mavsdk);
            ASSERT_TRUE(mavsdk.first_autopilot(2.0));
            mavsdk.first_autopilot_async(5.0, [calls](Result) { ++(*calls); });
        }
        EXPECT_LE(calls->load(), 1) << "the result must be delivered at most once";
    }
}
