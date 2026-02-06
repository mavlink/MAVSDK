#include "log.h"
#include "mavsdk.h"
#include "plugins/mission/mission.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include <random>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

static std::vector<Mission::MissionItem> create_mission_items();
static bool should_keep_message(const mavlink_message_t& message);

static std::default_random_engine generator;
static std::uniform_real_distribution<double> distribution(0.0, 1.0);

TEST(SystemTest, MissionTransferLossy)
{
    // Create two MAVSDK instances: groundstation and autopilot
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    // Set up the autopilot side with MissionRawServer
    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    // Wait for groundstation to discover autopilot
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};

    // Set up lossy message interception on both directions
    // Drop ~5% of mission-related messages to simulate packet loss
    mavsdk_groundstation.intercept_outgoing_messages_async(
        [](const mavlink_message_t& message) { return should_keep_message(message); });

    mavsdk_groundstation.intercept_incoming_messages_async(
        [](const mavlink_message_t& message) { return should_keep_message(message); });

    // Create mission plan
    Mission::MissionPlan mission_plan;
    mission_plan.mission_items = create_mission_items();

    LogInfo() << "Uploading mission with simulated packet loss...";
    ASSERT_EQ(mission.upload_mission(mission_plan), Mission::Result::Success);
    LogInfo() << "Mission upload succeeded despite packet loss.";

    LogInfo() << "Downloading mission with simulated packet loss...";
    auto result = mission.download_mission();
    ASSERT_EQ(result.first, Mission::Result::Success);
    LogInfo() << "Mission download succeeded despite packet loss.";

    // Verify downloaded mission matches uploaded mission
    EXPECT_EQ(mission_plan, result.second);

    // Cleanup
    mavsdk_groundstation.intercept_outgoing_messages_async(nullptr);
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

bool should_keep_message(const mavlink_message_t& message)
{
    bool should_keep = true;
    if (message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST ||
        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_LIST ||
        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_INT ||
        // Note: We don't drop MISSION_ACK as the protocol relies on it
        message.msgid == MAVLINK_MSG_ID_MISSION_COUNT ||
        message.msgid == MAVLINK_MSG_ID_MISSION_ITEM_INT) {
        // Keep 95% of messages (drop 5%)
        should_keep = distribution(generator) < 0.95;
        if (!should_keep) {
            LogInfo() << "Dropping message ID " << message.msgid << " to simulate packet loss";
        }
    }
    return should_keep;
}

std::vector<Mission::MissionItem> create_mission_items()
{
    std::vector<Mission::MissionItem> mission_items;

    for (unsigned i = 0; i < 20; ++i) {
        Mission::MissionItem new_item{};
        new_item.latitude_deg = 47.398170327054473 + (i * 1e-6);
        new_item.longitude_deg = 8.5456490218639658 + (i * 1e-6);
        new_item.relative_altitude_m = 10.0f + (i * 0.2f);
        new_item.speed_m_s = 5.0f + (i * 0.1f);
        new_item.acceptance_radius_m = 2.0f;
        mission_items.push_back(new_item);
    }
    return mission_items;
}
