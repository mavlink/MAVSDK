#include "mavsdk.hpp"
#include "plugins/mission_raw/mission_raw.hpp"
#include "plugins/mission_raw_server/mission_raw_server.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;

namespace {

std::vector<MissionRaw::MissionItem> make_mission_items(uint16_t count, float altitude_m)
{
    std::vector<MissionRaw::MissionItem> items;
    items.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        MissionRaw::MissionItem item{};
        item.seq = i;
        item.frame = 3; // MAV_FRAME_GLOBAL_RELATIVE_ALT
        item.command = 16; // MAV_CMD_NAV_WAYPOINT
        item.current = (i == 0) ? 1 : 0;
        item.autocontinue = 1;
        item.x = 473981700 + static_cast<int32_t>(i) * 100;
        item.y = 85456490 + static_cast<int32_t>(i) * 100;
        item.z = altitude_m;
        item.mission_type = 0; // MAV_MISSION_TYPE_MISSION
        items.push_back(item);
    }
    return items;
}

} // namespace

TEST(MissionRaw, FailedUploadKeepsPreviousPlan)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    mavsdk_groundstation.set_timeout_s(0.5);
    mavsdk_autopilot.set_timeout_s(0.5);

    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission_raw = MissionRaw{system};

    const auto plan_a = make_mission_items(3, 10.0f);
    ASSERT_EQ(mission_raw.upload_mission(plan_a), MissionRaw::Result::Success);

    auto server_timeout_prom = std::make_shared<std::promise<MissionRawServer::Result>>();
    auto server_timeout_fut = server_timeout_prom->get_future();
    auto server_result_called = std::make_shared<std::atomic<bool>>(false);

    auto incoming_handle = mission_raw_server.subscribe_incoming_mission(
        [server_timeout_prom, server_result_called](
            MissionRawServer::Result result, MissionRawServer::MissionPlan /*plan*/) {
            if (!server_result_called->exchange(true)) {
                server_timeout_prom->set_value(result);
            }
        });

    auto items_sent = std::make_shared<std::atomic<unsigned>>(0);
    auto drop_handle = mavsdk_groundstation.subscribe_outgoing_messages_json(
        [items_sent](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "MISSION_ITEM_INT") {
                if (items_sent->fetch_add(1) >= 2) {
                    return false;
                }
            }
            return true;
        });

    const auto plan_b = make_mission_items(5, 50.0f);
    EXPECT_EQ(mission_raw.upload_mission(plan_b), MissionRaw::Result::Timeout);

    ASSERT_EQ(server_timeout_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(server_timeout_fut.get(), MissionRawServer::Result::Timeout);

    mission_raw_server.unsubscribe_incoming_mission(incoming_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(drop_handle);

    const auto [download_result, downloaded_items] = mission_raw.download_mission();
    ASSERT_EQ(download_result, MissionRaw::Result::Success);
    EXPECT_EQ(downloaded_items.size(), plan_a.size());
    EXPECT_EQ(downloaded_items, plan_a);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
