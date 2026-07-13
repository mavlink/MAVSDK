#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/mission/mission.hpp"
#include "plugins/mission_raw/mission_raw.hpp"
#include "plugins/mission_raw_server/mission_raw_server.hpp"
#include <random>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

static std::vector<Mission::MissionItem> create_mission_items();

static std::default_random_engine generator;
static std::uniform_real_distribution<double> distribution(0.0, 1.0);

static bool should_keep_mission_message(const std::string& message_name)
{
    static const std::vector<std::string> lossy_messages = {
        "MISSION_REQUEST",
        "MISSION_REQUEST_LIST",
        "MISSION_REQUEST_INT",
        "MISSION_COUNT",
        "MISSION_ITEM_INT",
    };
    for (const auto& name : lossy_messages) {
        if (message_name == name) {
            // Keep 95% of messages (drop 5%)
            bool keep = distribution(generator) < 0.95;
            if (!keep) {
                LogInfo("Dropping {} to simulate packet loss", message_name);
            }
            return keep;
        }
    }
    return true;
}

TEST(Mission, TransferLossy)
{
    // Create two MAVSDK instances: groundstation and autopilot
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    // Set up the autopilot side with MissionRawServer before connections
    // so capabilities (MISSION_INT) are available before AUTOPILOT_VERSION exchange.
    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    // Wait for groundstation to discover autopilot
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};

    // Drop ~5% of mission-related messages to simulate packet loss
    auto out_handle =
        mavsdk_groundstation.subscribe_outgoing_messages_json([](Mavsdk::MavlinkMessage message) {
            return should_keep_mission_message(message.message_name);
        });
    auto in_handle =
        mavsdk_groundstation.subscribe_incoming_messages_json([](Mavsdk::MavlinkMessage message) {
            return should_keep_mission_message(message.message_name);
        });

    // Create mission plan
    Mission::MissionPlan mission_plan;
    mission_plan.mission_items = create_mission_items();

    LogInfo("Uploading mission with simulated packet loss...");
    ASSERT_EQ(mission.upload_mission(mission_plan), Mission::Result::Success);
    LogInfo("Mission upload succeeded despite packet loss.");

    LogInfo("Downloading mission with simulated packet loss...");
    auto result = mission.download_mission();
    ASSERT_EQ(result.first, Mission::Result::Success);
    LogInfo("Mission download succeeded despite packet loss.");

    // Verify downloaded mission matches uploaded mission
    EXPECT_EQ(mission_plan, result.second);

    mavsdk_groundstation.unsubscribe_outgoing_messages_json(out_handle);
    mavsdk_groundstation.unsubscribe_incoming_messages_json(in_handle);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
