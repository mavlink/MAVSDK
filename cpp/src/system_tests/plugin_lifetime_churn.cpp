#include "log.hpp"
#include "mavsdk.hpp"
#include "mavlink_include.hpp"
#include "plugins/telemetry/telemetry.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;
using namespace std::chrono_literals;

// Plugins are owned by user code and destroyed on a user thread, while SystemImpl calls
// enable()/disable() on them from the io thread when the system connects and disconnects.
// SystemImpl keeps them as raw PluginImplBase*, so those two things have to be serialized
// against each other -- otherwise the io thread can call into a plugin that a user thread is
// in the middle of destroying, and unregister_plugin()'s disable() can run concurrently with
// set_disconnected()'s.
//
// Nothing else in the suite creates and destroys plugins while the link goes up and down, so
// this exercises that specific overlap. It is a race, so a plain build passing proves little;
// the point is to give ThreadSanitizer something to look at. Run it under TSan (and ASan).

namespace {

// Short enough that a gap in the heartbeats disconnects us quickly, so the test can flap the
// link many times without taking forever.
constexpr double heartbeat_timeout_s = 0.05;
constexpr auto silence_for_disconnect = 90ms;

// Churn for a fixed duration rather than a fixed number of rounds, so that the plugin
// lifetimes and the link flaps overlap for the whole run instead of one outlasting the other.
constexpr auto churn_duration = 5s;

std::vector<char> make_heartbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack_chan(
        1, // system id of the "autopilot" we are pretending to hear from
        MAV_COMP_ID_AUTOPILOT1,
        MAVLINK_COMM_NUM_BUFFERS - 1,
        &message,
        MAV_TYPE_QUADROTOR,
        MAV_AUTOPILOT_PX4,
        MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
        0,
        MAV_STATE_STANDBY);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    const auto length = mavlink_msg_to_send_buffer(buffer.data(), &message);

    return std::vector<char>(buffer.begin(), buffer.begin() + length);
}

} // namespace

TEST(PluginLifetime, ChurnPluginsWhileLinkFlaps)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk_groundstation.add_any_connection("raw://"), ConnectionResult::Success);

    mavsdk_groundstation.set_heartbeat_timeout_s(heartbeat_timeout_s);

    const auto heartbeat = make_heartbeat();

    std::atomic<bool> stop{false};
    std::atomic<unsigned> flaps{0};

    // Drive the link up and down: a burst of heartbeats connects the system (io thread runs
    // set_connected() -> enable() on every plugin), then silence times it out
    // (set_disconnected() -> disable() on every plugin).
    std::thread flapper([&]() {
        while (!stop) {
            for (int i = 0; i < 2 && !stop; ++i) {
                mavsdk_groundstation.pass_received_raw_bytes(heartbeat.data(), heartbeat.size());
                std::this_thread::sleep_for(10ms);
            }
            std::this_thread::sleep_for(silence_for_disconnect);
            ++flaps;
        }
    });

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "system never showed up";
    auto system = maybe_system.value();

    // Meanwhile, create and destroy a plugin over and over on this thread. Telemetry is a
    // good candidate: enable() registers a call_every and disable() removes it, so both
    // touch the same members the io thread would.
    unsigned rounds = 0;
    const auto deadline = std::chrono::steady_clock::now() + churn_duration;
    while (std::chrono::steady_clock::now() < deadline && !::testing::Test::HasFailure()) {
        Telemetry telemetry{system};

        // Keep each plugin alive for less than a flap cycle, and vary it, so that the
        // destructor lands at many different points relative to the connect and disconnect
        // transitions rather than settling into lockstep with them.
        std::this_thread::sleep_for(std::chrono::milliseconds(1 + (rounds % 17)));
        ++rounds;
    }

    stop = true;
    flapper.join();

    LogInfo("Completed {} plugin rounds over {} link flaps", rounds, flaps.load());

    // The system must still be usable: nothing should have been torn down that shouldn't be.
    EXPECT_NE(system->get_system_id(), 0);
}
