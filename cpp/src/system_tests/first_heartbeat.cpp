#include "mavsdk.hpp"
#include "mavlink_include.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;

// The first heartbeat from an unknown system is what creates the System for it. It also has
// to be delivered to that System, otherwise nothing marks it connected and discovery waits
// for the next one -- a whole heartbeat interval, typically a second, for no reason.

namespace {

std::vector<char> make_heartbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack_chan(
        1,
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

TEST(FirstHeartbeat, SingleHeartbeatConnectsSystem)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    const auto heartbeat = make_heartbeat();

    // Exactly one. If the System that this creates does not also receive it, nothing calls
    // set_connected() and first_autopilot() below waits for a heartbeat that never comes.
    mavsdk.pass_received_raw_bytes(heartbeat.data(), heartbeat.size());

    auto maybe_system = mavsdk.first_autopilot(2.0);
    ASSERT_TRUE(maybe_system) << "a single heartbeat did not connect the system";
    EXPECT_TRUE(maybe_system.value()->is_connected());
}

TEST(FirstHeartbeat, NoErrorBeforeUdpRemoteIsKnown)
{
    // A companion computer sends heartbeats right away, before the udpin connection has
    // heard from anyone and therefore has nowhere to send them to.
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::CompanionComputer}};

    auto error_count = std::make_shared<std::atomic<int>>(0);
    auto handle = mavsdk.subscribe_connection_errors(
        [error_count](const Mavsdk::ConnectionError&) { ++(*error_count); });

    ASSERT_EQ(mavsdk.add_any_connection("udpin://127.0.0.1:17030"), ConnectionResult::Success);

    // Long enough for the immediate heartbeat and the next one.
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    mavsdk.unsubscribe_connection_errors(handle);
    EXPECT_EQ(error_count->load(), 0);
}
