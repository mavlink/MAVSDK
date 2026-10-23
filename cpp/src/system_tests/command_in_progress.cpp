#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/action/action.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

static constexpr int cmd_component_arm_disarm = 400;
static constexpr int mav_result_accepted = 0;
static constexpr int mav_result_in_progress = 5;

static MavlinkDirectServer::MavlinkMessage
make_ack(const MavlinkDirectServer::MavlinkMessage& command, int result, int progress)
{
    MavlinkDirectServer::MavlinkMessage ack{};
    ack.message_name = "COMMAND_ACK";
    ack.target_system_id = command.system_id;
    ack.target_component_id = command.component_id;
    ack.fields_json =
        nlohmann::json{
            {"command", cmd_component_arm_disarm},
            {"result", result},
            {"progress", progress},
            {"result_param2", 0},
            {"target_system", command.system_id},
            {"target_component", command.component_id}}
            .dump();
    return ack;
}

// After an IN_PROGRESS ack the command must not be retransmitted, even when the final
// ack takes longer than the normal retransmit timeout.
TEST(Command, InProgressIsNotRetransmitted)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17030"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17030"), ConnectionResult::Success);

    auto mavlink_direct_server = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    auto received_count = std::make_shared<std::atomic<int>>(0);
    auto first_command_prom = std::make_shared<std::promise<MavlinkDirectServer::MavlinkMessage>>();
    auto first_command_fut = first_command_prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto handle = mavlink_direct_server.subscribe_message(
        "COMMAND_LONG",
        [&mavlink_direct_server, received_count, first_command_prom, flag](
            MavlinkDirectServer::MavlinkMessage message) {
            if (nlohmann::json::parse(message.fields_json)["command"] != cmd_component_arm_disarm) {
                return;
            }
            ++(*received_count);
            LogInfo("Received arm command: {}", message.fields_json);
            std::call_once(*flag, [&]() {
                EXPECT_EQ(
                    mavlink_direct_server.send_message(
                        make_ack(message, mav_result_in_progress, 255)),
                    MavlinkDirectServer::Result::Success);
                first_command_prom->set_value(message);
            });
        });

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    auto arm_fut = std::async(std::launch::async, [&action]() { return action.arm(); });

    ASSERT_EQ(first_command_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    const auto command = first_command_fut.get();

    // Longer than the previous 3 s timeout after IN_PROGRESS, which triggered a retransmit.
    std::this_thread::sleep_for(std::chrono::milliseconds(4500));

    EXPECT_EQ(
        mavlink_direct_server.send_message(make_ack(command, mav_result_accepted, 100)),
        MavlinkDirectServer::Result::Success);

    ASSERT_EQ(arm_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(arm_fut.get(), Action::Result::Success);
    EXPECT_EQ(received_count->load(), 1);

    mavlink_direct_server.unsubscribe_message(handle);
}
