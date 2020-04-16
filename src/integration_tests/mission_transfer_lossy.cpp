#include <iostream>
#include <future>
#include <atomic>
#include "log.h"
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/mission/mission.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"

using namespace mavsdk;

static void set_link_lossy(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough);
static std::vector<Mission::MissionItem> create_mission_items();
static bool should_keep_message(const mavlink_message_t& message);

static std::atomic<unsigned> _lossy_counter{0};

TEST_F(SitlTest, MissionTransferLossy)
{
    Mavsdk dc;
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        dc.register_on_discover([&prom](uint64_t uuid) {
            prom.set_value();
            UNUSED(uuid);
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    System& system = dc.system();
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);
    auto mission = std::make_shared<Mission>(system);

    set_link_lossy(mavlink_passthrough);

    Mission::MissionPlan mission_plan;
    mission_plan.mission_items = create_mission_items();

    ASSERT_EQ(mission->upload_mission(mission_plan), Mission::Result::Success);

    auto result = mission->download_mission();
    ASSERT_EQ(result.first, Mission::Result::Success);

    EXPECT_EQ(mission_plan, result.second);
}

void set_link_lossy(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough)
{
    mavlink_passthrough->intercept_outgoing_messages_async(
        [](mavlink_message_t& message) { return should_keep_message(message); });

    mavlink_passthrough->intercept_incoming_messages_async(
        [](mavlink_message_t& message) { return should_keep_message(message); });
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
        should_keep = (_lossy_counter++ % 10 != 0);
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
        mission_items.push_back(new_item);
    }
    return mission_items;
}
