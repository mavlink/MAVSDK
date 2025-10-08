#include "log.h"
#include "mavsdk.h"
#include "mavlink_include.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <future>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, RawBytesHeartbeat)
{
    // Create a MAVSDK instance that will receive and respond to raw bytes
    Mavsdk mavsdk_responder{Mavsdk::Configuration{ComponentType::Autopilot}};

    // Add a raw connection - this will automatically enable heartbeats
    ASSERT_EQ(mavsdk_responder.add_any_connection("raw://"), ConnectionResult::Success);

    // Set up a promise/future to capture the heartbeat response
    auto prom = std::promise<void>();
    auto fut = prom.get_future();
    bool heartbeat_received = false;

    // Subscribe to raw bytes to capture the heartbeat response
    auto handle = mavsdk_responder.subscribe_raw_bytes([&prom, &heartbeat_received](
                                                           const char* bytes, size_t length) {
        LogInfo() << "Received " << length << " raw bytes";

        // Parse to check if this is a heartbeat
        mavlink_message_t msg;
        mavlink_status_t status;

        for (size_t i = 0; i < length; ++i) {
            if (mavlink_parse_char(MAVLINK_COMM_0, static_cast<uint8_t>(bytes[i]), &msg, &status)) {
                if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                    LogInfo() << "Captured HEARTBEAT message from system " << (int)msg.sysid
                              << ", component " << (int)msg.compid;
                    if (!heartbeat_received) {
                        heartbeat_received = true;
                        prom.set_value();
                    }
                    return;
                }
            }
        }
    });

    // Give subscription time to be registered
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Create a raw heartbeat message
    mavlink_message_t heartbeat_msg;
    mavlink_heartbeat_t heartbeat;
    heartbeat.type = MAV_TYPE_GCS;
    heartbeat.autopilot = MAV_AUTOPILOT_INVALID;
    heartbeat.base_mode = 0;
    heartbeat.custom_mode = 0;
    heartbeat.system_status = MAV_STATE_ACTIVE;
    heartbeat.mavlink_version = 3;

    mavlink_msg_heartbeat_encode(245, 190, &heartbeat_msg, &heartbeat);

    // Serialize the heartbeat to raw bytes
    std::vector<uint8_t> heartbeat_bytes(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(heartbeat_bytes.data(), &heartbeat_msg);
    heartbeat_bytes.resize(len);

    // Send periodic heartbeats at 1 Hz to keep the system alive
    std::atomic<bool> should_send_heartbeats{true};
    std::thread heartbeat_thread([&]() {
        while (should_send_heartbeats) {
            LogInfo() << "Sending raw heartbeat (" << len << " bytes)...";
            mavsdk_responder.send_raw_bytes(
                reinterpret_cast<const char*>(heartbeat_bytes.data()), len);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // Wait for heartbeat response (responder should send heartbeats automatically)
    // The subscription callback should be triggered when heartbeats are sent out
    auto status = fut.wait_for(std::chrono::seconds(5));
    ASSERT_EQ(status, std::future_status::ready)
        << "Did not receive heartbeat response within timeout";

    // Stop sending heartbeats and clean up
    should_send_heartbeats = false;
    heartbeat_thread.join();

    mavsdk_responder.unsubscribe_raw_bytes(handle);
}
