#include <atomic>
#include <future>
#include <iostream>
#include <random>

#include "integration_test_helper.h"
#include "log.h"
#include "mavsdk.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

static std::vector<Mission::MissionItem> create_mission_items();
static bool should_keep_message(const mavlink_message_t& message);

static std::default_random_engine generator;
static std::uniform_real_distribution<double> distribution(0.0, 1.0);

TEST(SitlTest, PX4MissionTransferLossy)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("udpin://0.0.0.0:14540"), ConnectionResult::Success);

    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                prom.set_value();
            }
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);
    auto mission = std::make_shared<Mission>(system);

    mavsdk.intercept_outgoing_messages_async(
        [](const mavlink_message_t& message) { return should_keep_message(message); });

    mavsdk.intercept_incoming_messages_async(
        [](const mavlink_message_t& message) { return should_keep_message(message); });

    Mission::MissionPlan mission_plan;
    mission_plan.mission_items = create_mission_items();

    ASSERT_EQ(mission->upload_mission(mission_plan), Mission::Result::Success);

    auto result = mission->download_mission();
    ASSERT_EQ(result.first, Mission::Result::Success);

    EXPECT_EQ(mission_plan, result.second);
}

bool should_keep_message(const mavlink_message_t& message)
{
    bool should_keep = true;
    if (message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST ||
        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_LIST ||
        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_INT ||
        // message.msgid == MAVLINK_MSG_ID_MISSION_ACK || FIXME: we rely on ack
        message.msgid == MAVLINK_MSG_ID_MISSION_COUNT ||
        message.msgid == MAVLINK_MSG_ID_MISSION_ITEM_INT) {
        should_keep = distribution(generator) < 0.95;
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
