#include "mavsdk.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>
#include <vector>

using namespace mavsdk;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
TEST(Action, SetModeNonCustomSingleAck)
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

    auto mavlink_direct = MavlinkDirect{system};

    std::mutex mutex;
    std::vector<ActionServer::Result> callback_results;
    std::vector<nlohmann::json> command_acks;

    auto mode_handle = action_server.subscribe_flight_mode_change(
        [&](ActionServer::Result result, ActionServer::FlightMode) {
            std::lock_guard<std::mutex> lock(mutex);
            callback_results.push_back(result);
        });

    auto ack_handle =
        mavsdk_groundstation.subscribe_incoming_messages_json([&](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "COMMAND_ACK") {
                auto json = nlohmann::json::parse(message.fields_json, nullptr, false);
                if (!json.is_discarded() && json.value("command", 0) == 11) {
                    std::lock_guard<std::mutex> lock(mutex);
                    command_acks.push_back(json);
                }
            }
            return true;
        });

    MavlinkDirect::MavlinkMessage set_mode_msg{};
    set_mode_msg.message_name = "SET_MODE";
    set_mode_msg.fields_json =
        nlohmann::json{{"target_system", 1}, {"base_mode", 0x40}, {"custom_mode", 0}}.dump();

    EXPECT_EQ(mavlink_direct.send_message(set_mode_msg), MavlinkDirect::Result::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    {
        std::lock_guard<std::mutex> lock(mutex);
        EXPECT_EQ(callback_results.size(), 1u);
        if (!callback_results.empty()) {
            EXPECT_EQ(callback_results[0], ActionServer::Result::ParameterError);
        }

        EXPECT_EQ(command_acks.size(), 1u);
        if (!command_acks.empty()) {
            EXPECT_EQ(command_acks[0]["result"].get<int>(), 3); // MAV_RESULT_UNSUPPORTED
        }
    }

    mavsdk_groundstation.unsubscribe_incoming_messages_json(ack_handle);
    action_server.unsubscribe_flight_mode_change(mode_handle);
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
