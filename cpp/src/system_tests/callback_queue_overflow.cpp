#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/action/action.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/telemetry_server/telemetry_server.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;
using namespace std::chrono_literals;

// User callbacks are delivered through one bounded queue, and blocking API calls depend on
// it: Action::arm() waits on a promise that is only satisfied when the command-result
// callback runs. So a subscriber that cannot keep up must not be able to push that result
// out of the queue -- if it does, arm() never returns.
//
// This floods the queue with telemetry deliveries behind a deliberately slow subscriber and
// then checks that arm() still completes.

namespace {

constexpr auto slow_callback_duration = 20ms;

// Published continuously rather than in one burst: a burst drains while the command is in
// flight, and the queue has to still be saturated at the moment the command result is
// queued, since that is what the overflow policy acts on.

TelemetryServer::Position make_position()
{
    TelemetryServer::Position position{};
    position.latitude_deg = 47.3977;
    position.longitude_deg = 8.5456;
    position.absolute_altitude_m = 488.0f;
    position.relative_altitude_m = 10.0f;
    return position;
}

} // namespace

TEST(CallbackQueueOverflow, SlowSubscriberDoesNotStallBlockingCall)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17010"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17010"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    auto telemetry_server = TelemetryServer{mavsdk_autopilot.server_component()};
    EXPECT_EQ(action_server.set_armable(true, true), ActionServer::Result::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    auto telemetry = Telemetry{system};
    auto action = Action{system};

    // A subscriber that cannot keep up. Every delivery holds the callback thread for a
    // while, so the queue behind it grows.
    //
    // The counter is shared rather than captured by reference: callbacks that are already
    // queued keep their own copy of this lambda and run until the Mavsdk instance is torn
    // down, which happens after every local declared below it here goes away. Capturing a
    // stack variable would be a use-after-scope, and unsubscribing would not save it --
    // deliveries already on the queue hold their own copy of the callback.
    auto positions_received = std::make_shared<std::atomic<unsigned>>(0);
    telemetry.subscribe_position([positions_received](Telemetry::Position) {
        std::this_thread::sleep_for(slow_callback_duration);
        ++(*positions_received);
    });

    std::atomic<bool> stop_publishing{false};
    std::atomic<unsigned> positions_published{0};
    std::thread publisher([&]() {
        TelemetryServer::VelocityNed velocity{};
        TelemetryServer::Heading heading{};
        heading.heading_deg = 90.0;
        while (!stop_publishing) {
            telemetry_server.publish_position(make_position(), velocity, heading);
            ++positions_published;
            std::this_thread::sleep_for(1ms);
        }
    });

    // Give the queue time to back up behind the slow subscriber and stay that way.
    std::this_thread::sleep_for(2s);

    // The command result has to get through regardless of how backed up the telemetry is.
    // Run arm() on its own thread so a regression shows up as a failure here rather than
    // hanging the whole suite.
    auto arm_result = std::async(std::launch::async, [&action]() { return action.arm(); });

    const auto arrived = arm_result.wait_for(20s);
    stop_publishing = true;
    publisher.join();

    ASSERT_EQ(arrived, std::future_status::ready)
        << "arm() never returned: its result callback was dropped from the user callback queue";
    EXPECT_EQ(arm_result.get(), Action::Result::Success);

    LogInfo(
        "Published {}, delivered {} position updates",
        positions_published.load(),
        positions_received->load());
}
