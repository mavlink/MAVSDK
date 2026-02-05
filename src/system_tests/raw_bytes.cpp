#include "log.h"
#include "mavsdk.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include "mavlink_include.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <future>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, RawBytesSendReceive)
{
    // Test 1: Verify subscribe_raw_bytes_to_be_sent captures outgoing messages
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    ASSERT_EQ(mavsdk_autopilot.add_any_connection("raw://"), ConnectionResult::Success);

    auto telemetry_server = std::make_shared<TelemetryServer>(mavsdk_autopilot.server_component());

    // Set up to capture outgoing raw bytes
    auto prom = std::promise<void>();
    auto fut = prom.get_future();
    std::atomic<bool> message_captured{false};

    // Subscribe to outgoing raw bytes
    auto handle = mavsdk_autopilot.subscribe_raw_bytes_to_be_sent(
        [&prom, &message_captured](const char* bytes, size_t length) {
            // Parse the bytes
            mavlink_message_t msg = {};
            mavlink_status_t status = {};

            for (size_t i = 0; i < length; ++i) {
                if (mavlink_parse_char(
                        MAVLINK_COMM_0, static_cast<uint8_t>(bytes[i]), &msg, &status)) {
                    LogInfo() << "Captured outgoing message ID " << msg.msgid;

                    // Look for GLOBAL_POSITION_INT message
                    if (msg.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT &&
                        !message_captured.exchange(true)) {
                        LogInfo() << "Captured GLOBAL_POSITION_INT being sent";
                        prom.set_value();
                    }
                }
            }
        });

    // Give subscription time to be registered
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Publish a position which should trigger outgoing message
    TelemetryServer::Position position{};
    position.latitude_deg = 47.3977;
    position.longitude_deg = 8.5456;
    position.absolute_altitude_m = 488.0f;
    position.relative_altitude_m = 10.0f;

    TelemetryServer::VelocityNed velocity{};
    velocity.north_m_s = 0.0f;
    velocity.east_m_s = 0.0f;
    velocity.down_m_s = 0.0f;

    TelemetryServer::Heading heading{};
    heading.heading_deg = 90.0f;

    LogInfo() << "Publishing position via TelemetryServer...";
    telemetry_server->publish_position(position, velocity, heading);

    // Wait for the message to be captured
    auto result = fut.wait_for(std::chrono::seconds(2));
    ASSERT_EQ(result, std::future_status::ready)
        << "Did not capture GLOBAL_POSITION_INT through raw bytes subscription";

    mavsdk_autopilot.unsubscribe_raw_bytes_to_be_sent(handle);

    // Test 2: Verify pass_received_raw_bytes processes incoming messages
    Mavsdk mavsdk_gcs{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk_gcs.add_any_connection("raw://"), ConnectionResult::Success);

    // Create a PING message
    mavlink_message_t ping_msg;
    mavlink_ping_t ping;
    ping.time_usec = 123456;
    ping.seq = 42;
    ping.target_system = 0; // Broadcast
    ping.target_component = 0;

    mavlink_msg_ping_encode_chan(1, 1, MAVLINK_COMM_NUM_BUFFERS - 1, &ping_msg, &ping);

    // Serialize to bytes
    std::vector<uint8_t> ping_bytes(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(ping_bytes.data(), &ping_msg);
    ping_bytes.resize(len);

    LogInfo() << "Passing " << len << " raw bytes (PING) into MAVSDK...";

    // This should be processed and create a system
    mavsdk_gcs.pass_received_raw_bytes(reinterpret_cast<const char*>(ping_bytes.data()), len);

    // Give it time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify a system was created from the ping
    auto systems = mavsdk_gcs.systems();
    ASSERT_FALSE(systems.empty()) << "No system was created from received raw bytes";
    ASSERT_EQ(systems[0]->get_system_id(), 1) << "System ID mismatch";
    LogInfo() << "Successfully created system from raw bytes with ID "
              << systems[0]->get_system_id();
}
