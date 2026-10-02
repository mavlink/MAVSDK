#include "mavsdk.hpp"
#include "mavlink_include.hpp"

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;

// A system does not have to show up with its autopilot. A companion computer, camera or
// gimbal on the same system ID can be heard first, for example because it boots faster than
// the flight controller. The system is then connected without an autopilot, and the autopilot
// component only joins it later. That still has to count as the autopilot appearing.

namespace {

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

std::vector<char> make_companion_heartbeat()
{
    return make_heartbeat(
        MAV_COMP_ID_ONBOARD_COMPUTER, MAV_TYPE_ONBOARD_CONTROLLER, MAV_AUTOPILOT_INVALID);
}

std::vector<char> make_autopilot_heartbeat()
{
    return make_heartbeat(MAV_COMP_ID_AUTOPILOT1, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4);
}

void pass(Mavsdk& mavsdk, const std::vector<char>& bytes)
{
    mavsdk.pass_received_raw_bytes(bytes.data(), bytes.size());
}

} // namespace

TEST(FirstAutopilotLateComponent, AutopilotJoiningConnectedSystemIsFound)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    pass(mavsdk, make_companion_heartbeat());

    // The companion alone connects system 1, but there is no autopilot yet.
    EXPECT_FALSE(mavsdk.first_autopilot(0.5));
    ASSERT_EQ(mavsdk.systems().size(), 1);
    ASSERT_TRUE(mavsdk.systems().at(0)->is_connected());
    ASSERT_FALSE(mavsdk.systems().at(0)->has_autopilot());

    auto waiting =
        std::async(std::launch::async, [&mavsdk]() { return mavsdk.first_autopilot(5.0); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    const auto start = std::chrono::steady_clock::now();
    pass(mavsdk, make_autopilot_heartbeat());

    auto maybe_system = waiting.get();
    ASSERT_TRUE(maybe_system) << "the autopilot joining an already connected system was missed";
    EXPECT_TRUE(maybe_system.value()->has_autopilot());
    // Found because the autopilot showed up, not just before the timeout.
    EXPECT_LT(std::chrono::steady_clock::now() - start, std::chrono::seconds(2));
}

TEST(FirstAutopilotLateComponent, AutopilotVersionRequestedWhenAutopilotJoins)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    // Look for MAV_CMD_REQUEST_MESSAGE(AUTOPILOT_VERSION) to the autopilot in what goes out.
    auto requested = std::make_shared<std::atomic<bool>>(false);
    auto raw_handle = mavsdk.subscribe_raw_bytes_to_be_sent([requested](
                                                                const char* bytes, size_t length) {
        constexpr uint8_t channel = MAVLINK_COMM_NUM_BUFFERS - 2;
        mavlink_message_t message;
        mavlink_status_t status;
        for (size_t i = 0; i < length; ++i) {
            if (mavlink_parse_char(channel, static_cast<uint8_t>(bytes[i]), &message, &status) &&
                message.msgid == MAVLINK_MSG_ID_COMMAND_LONG) {
                mavlink_command_long_t command;
                mavlink_msg_command_long_decode(&message, &command);
                if (command.command == MAV_CMD_REQUEST_MESSAGE &&
                    static_cast<uint32_t>(command.param1) == MAVLINK_MSG_ID_AUTOPILOT_VERSION &&
                    command.target_component == MAV_COMP_ID_AUTOPILOT1) {
                    requested->store(true);
                }
            }
        }
    });

    pass(mavsdk, make_companion_heartbeat());
    ASSERT_FALSE(mavsdk.first_autopilot(0.5));
    EXPECT_FALSE(requested->load()) << "nothing to ask for before there is an autopilot";

    pass(mavsdk, make_autopilot_heartbeat());

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!requested->load() && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_TRUE(requested->load())
        << "AUTOPILOT_VERSION was never requested from the late autopilot";

    mavsdk.unsubscribe_raw_bytes_to_be_sent(raw_handle);
}
