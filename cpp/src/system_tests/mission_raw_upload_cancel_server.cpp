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

MissionRaw::MissionPlan make_mission_plan(uint16_t count)
{
    MissionRaw::MissionPlan plan{};
    plan.mission_items.reserve(count);
    for (uint16_t i = 0; i < count; ++i) {
        MissionRaw::MissionItem item{};
        item.seq = i;
        item.frame = 3; // MAV_FRAME_GLOBAL_RELATIVE_ALT
        item.command = 16; // MAV_CMD_NAV_WAYPOINT
        item.current = (i == 0) ? 1 : 0;
        item.autocontinue = 1;
        item.x = 473981700 + static_cast<int32_t>(i) * 100;
        item.y = 85456490 + static_cast<int32_t>(i) * 100;
        item.z = 20.0f;
        item.mission_type = 0; // MAV_MISSION_TYPE_MISSION
        plan.mission_items.push_back(item);
    }
    return plan;
}

} // namespace

TEST(MissionRaw, UploadCancellationStopsServer)
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

    auto mission_raw = std::make_shared<MissionRaw>(system);
    std::weak_ptr<MissionRaw> mission_raw_weak = mission_raw;

    auto server_result_prom = std::make_shared<std::promise<MissionRawServer::Result>>();
    auto server_result_fut = server_result_prom->get_future();
    auto server_result_called = std::make_shared<std::atomic<bool>>(false);

    auto incoming_mission_handle = mission_raw_server.subscribe_incoming_mission(
        [server_result_prom, server_result_called](
            MissionRawServer::Result result, MissionRawServer::MissionPlan /*plan*/) {
            if (!server_result_called->exchange(true)) {
                server_result_prom->set_value(result);
            }
        });

    auto cancel_ack_received = std::make_shared<std::atomic<bool>>(false);
    auto requests_after_cancel = std::make_shared<std::atomic<unsigned>>(0);

    auto server_in_handle = mavsdk_autopilot.subscribe_incoming_messages_json(
        [cancel_ack_received](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "MISSION_ACK") {
                cancel_ack_received->store(true);
            }
            return true;
        });

    auto server_out_handle = mavsdk_autopilot.subscribe_outgoing_messages_json(
        [cancel_ack_received, requests_after_cancel](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "MISSION_REQUEST_INT" && cancel_ack_received->load()) {
                requests_after_cancel->fetch_add(1);
            }
            return true;
        });

    // Allow the first MISSION_ITEM_INT through so the transfer is mid-flight, then
    // hold subsequent items while the client cancels the upload.
    auto items_sent = std::make_shared<std::atomic<unsigned>>(0);
    auto client_out_handle = mavsdk_groundstation.subscribe_outgoing_messages_json(
        [items_sent](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "MISSION_ITEM_INT") {
                if (items_sent->fetch_add(1) >= 1) {
                    return false;
                }
            }
            return true;
        });

    auto client_result_prom = std::make_shared<std::promise<MissionRaw::Result>>();
    auto client_result_fut = client_result_prom->get_future();
    auto cancel_triggered = std::make_shared<std::atomic<bool>>(false);

    mission_raw->upload_mission_with_progress_async(
        make_mission_plan(10),
        [client_result_prom, cancel_triggered, mission_raw_weak](
            MissionRaw::Result result, MissionRaw::ProgressData progress_data) {
            if (result == MissionRaw::Result::Next) {
                if (progress_data.progress > 0.0f && !cancel_triggered->exchange(true)) {
                    if (auto locked = mission_raw_weak.lock()) {
                        locked->cancel_mission_upload();
                    }
                }
                return;
            }
            client_result_prom->set_value(result);
        });

    ASSERT_EQ(client_result_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(client_result_fut.get(), MissionRaw::Result::TransferCancelled);

    ASSERT_EQ(server_result_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(server_result_fut.get(), MissionRawServer::Result::TransferCancelled);
    EXPECT_TRUE(cancel_ack_received->load());
    EXPECT_EQ(requests_after_cancel->load(), 0u);

    mission_raw_server.unsubscribe_incoming_mission(incoming_mission_handle);
    mavsdk_autopilot.unsubscribe_incoming_messages_json(server_in_handle);
    mavsdk_autopilot.unsubscribe_outgoing_messages_json(server_out_handle);
    mavsdk_groundstation.unsubscribe_outgoing_messages_json(client_out_handle);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
