#include "log.h"
#include "mavsdk.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, MavlinkSeqSequential)
{
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("tcpin://0.0.0.0:17012"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("tcpout://127.0.0.1:17012"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    std::mutex seq_mutex;
    std::vector<uint8_t> seq_numbers;

    constexpr unsigned num_messages = 10;

    // Intercept all incoming messages on the ground station and record
    // sequence numbers from the autopilot system (sysid 1, compid 1).
    mavsdk_groundstation.intercept_incoming_messages_async([&](mavlink_message_t& message) -> bool {
        if (message.sysid == 1 && message.compid == MAV_COMP_ID_AUTOPILOT1) {
            std::lock_guard<std::mutex> lock(seq_mutex);
            seq_numbers.push_back(message.seq);
        }
        return true;
    });

    // Collect messages (heartbeats at 1 Hz plus any other traffic).
    for (unsigned i = 0; i < 150; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(seq_mutex);
        if (seq_numbers.size() >= num_messages) {
            break;
        }
    }

    // Stop intercepting.
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);

    std::lock_guard<std::mutex> lock(seq_mutex);
    ASSERT_GE(seq_numbers.size(), num_messages);

    // Verify that all messages from the autopilot component have strictly
    // sequential seq numbers (incrementing by 1 with uint8_t wrapping).
    for (unsigned i = 1; i < num_messages; ++i) {
        uint8_t expected = static_cast<uint8_t>(seq_numbers[i - 1] + 1);
        EXPECT_EQ(seq_numbers[i], expected)
            << "seq[" << i - 1 << "]=" << static_cast<int>(seq_numbers[i - 1]) << " seq[" << i
            << "]=" << static_cast<int>(seq_numbers[i]);
    }
}
