#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/action/action.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

using namespace mavsdk;

TEST(Action, ArmForce)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto action = Action{system};
    auto telemetry = Telemetry{system};

    // Only allow force arming and force disarming so a forced disarm vs forced arm
    // produces opposite vehicle armed states.
    EXPECT_EQ(action_server.set_armable(false, true), ActionServer::Result::Success);
    EXPECT_EQ(action_server.set_disarmable(false, true), ActionServer::Result::Success);

    // Normal arming is denied when only force_armable is true.
    EXPECT_EQ(action.arm(), Action::Result::CommandDenied);

    auto cmd_prom = std::make_shared<std::promise<nlohmann::json>>();
    auto cmd_fut = cmd_prom->get_future();
    auto cmd_flag = std::make_shared<std::once_flag>();

    auto msg_handle = mavsdk_autopilot.subscribe_incoming_messages_json(
        [cmd_prom, cmd_flag](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "COMMAND_LONG") {
                auto json = nlohmann::json::parse(message.fields_json, nullptr, false);
                if (!json.is_discarded() && json.value("command", 0) == 400) {
                    std::call_once(*cmd_flag, [&]() { cmd_prom->set_value(json); });
                }
            }
            return true;
        });

    auto arm_disarm_prom = std::make_shared<std::promise<ActionServer::ArmDisarm>>();
    auto arm_disarm_fut = arm_disarm_prom->get_future();
    auto arm_disarm_flag = std::make_shared<std::once_flag>();

    auto arm_disarm_handle = action_server.subscribe_arm_disarm(
        [arm_disarm_prom,
         arm_disarm_flag](ActionServer::Result, ActionServer::ArmDisarm arm_disarm) {
            std::call_once(*arm_disarm_flag, [&]() { arm_disarm_prom->set_value(arm_disarm); });
        });

    EXPECT_EQ(action.arm_force(), Action::Result::Success);

    ASSERT_EQ(cmd_fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    const auto cmd_json = cmd_fut.get();
    EXPECT_FLOAT_EQ(cmd_json["param1"].get<float>(), 1.0f);
    EXPECT_FLOAT_EQ(cmd_json["param2"].get<float>(), 21196.0f);

    ASSERT_EQ(arm_disarm_fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    const auto received_arm_disarm = arm_disarm_fut.get();
    EXPECT_TRUE(received_arm_disarm.arm);
    EXPECT_TRUE(received_arm_disarm.force);

    // Wait for the next autopilot heartbeat to propagate the updated armed state.
    for (int i = 0; i < 20 && !telemetry.armed(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_TRUE(telemetry.armed());

    action_server.unsubscribe_arm_disarm(arm_disarm_handle);
    mavsdk_autopilot.unsubscribe_incoming_messages_json(msg_handle);
}
