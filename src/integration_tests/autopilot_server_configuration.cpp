#include "log.h"
#include "future"
#include "chrono"
#include "mavsdk.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include <gtest/gtest.h>
#include <thread>

using namespace mavsdk;

TEST(SystemTest, Configuration)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};

    Mavsdk::Configuration config{ComponentType::Autopilot};
    MAV_TYPE TEST_VARIABLE = MAV_TYPE_GROUND_ROVER;
    config.set_mav_type(TEST_VARIABLE);
    Mavsdk mavsdk_autopilot{config};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto mavlink_passthrough = MavlinkPassthrough{system};

    std::promise<void> promise;
    auto future = promise.get_future();
    mavlink_passthrough.subscribe_message(
        MAVLINK_MSG_ID_HEARTBEAT, [&promise, TEST_VARIABLE](const mavlink_message_t& message) {
            mavlink_heartbeat_t heartbeat;
            mavlink_msg_heartbeat_decode(&message, &heartbeat);
            LogInfo() << "Received heartbeat from system " << static_cast<int>(message.sysid)
                      << " with type " << static_cast<int>(heartbeat.type);
            ASSERT_EQ(heartbeat.type, TEST_VARIABLE);
            promise.set_value();
        });

    // Wait for the message or timeout
    if (future.wait_for(std::chrono::seconds(5)) == std::future_status::timeout) {
        LogErr() << "Timeout: No heartbeat message received within 10 seconds";
        FAIL();
    }
}
