#include "mavsdk.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

static constexpr uint32_t cmd_user_1 = 31000; // MAV_CMD_USER_1
static constexpr int mav_result_accepted = 0;
static constexpr int mav_result_denied = 2;
static constexpr int mav_result_in_progress = 5;

namespace {

// A ground station sending commands with MavlinkDirect to an autopilot that handles them
// with MavlinkDirectServer.
struct CommandSetup {
    explicit CommandSetup(int port)
    {
        EXPECT_EQ(
            groundstation.add_any_connection("udpin://0.0.0.0:" + std::to_string(port)),
            ConnectionResult::Success);
        EXPECT_EQ(
            autopilot.add_any_connection("udpout://127.0.0.1:" + std::to_string(port)),
            ConnectionResult::Success);
        server = std::make_unique<MavlinkDirectServer>(autopilot.server_component());

        auto maybe_system = groundstation.first_autopilot(10.0);
        EXPECT_TRUE(maybe_system);
        if (maybe_system) {
            client = std::make_unique<MavlinkDirect>(maybe_system.value());
        }
    }

    Mavsdk groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    std::unique_ptr<MavlinkDirectServer> server;
    std::unique_ptr<MavlinkDirect> client;
};

MavlinkDirect::MavlinkMessage make_command_long(const nlohmann::json& params)
{
    nlohmann::json fields{{"target_system", 1}, {"target_component", 1}, {"command", cmd_user_1}};
    fields.update(params);

    MavlinkDirect::MavlinkMessage command;
    command.message_name = "COMMAND_LONG";
    command.fields_json = fields.dump();
    return command;
}

MavlinkDirectServer::MavlinkMessage make_ack(
    const MavlinkDirectServer::MavlinkMessage& command,
    int result,
    int progress = 0,
    int result_param2 = 0)
{
    MavlinkDirectServer::MavlinkMessage ack;
    ack.message_name = "COMMAND_ACK";
    ack.target_system_id = command.system_id;
    ack.target_component_id = command.component_id;
    ack.fields_json =
        nlohmann::json{
            {"command", cmd_user_1},
            {"result", result},
            {"progress", progress},
            {"result_param2", result_param2},
            {"target_system", command.system_id},
            {"target_component", command.component_id}}
            .dump();
    return ack;
}

} // namespace

TEST(MavlinkDirectCommand, CommandLongAccepted)
{
    CommandSetup setup{17040};
    ASSERT_TRUE(setup.client);

    auto received = std::make_shared<std::promise<MavlinkDirectServer::MavlinkMessage>>();
    auto received_fut = received->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto* server = setup.server.get();
    auto handle = server->subscribe_command(
        cmd_user_1, [server, received, flag](MavlinkDirectServer::MavlinkMessage command) {
            EXPECT_EQ(
                server->send_message(make_ack(command, mav_result_accepted, 0, 42)),
                MavlinkDirectServer::Result::Success);
            std::call_once(*flag, [&]() { received->set_value(command); });
        });

    auto [result, ack] =
        setup.client->send_command(make_command_long({{"param1", 1.5}, {"param4", nullptr}}));
    EXPECT_EQ(result, MavlinkDirect::Result::Success);
    EXPECT_EQ(ack.message_name, "COMMAND_ACK");

    const auto ack_json = nlohmann::json::parse(ack.fields_json);
    EXPECT_EQ(ack_json["command"], cmd_user_1);
    EXPECT_EQ(ack_json["result"], mav_result_accepted);
    EXPECT_EQ(ack_json["result_param2"], 42);

    ASSERT_EQ(received_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    const auto command = received_fut.get();
    EXPECT_EQ(command.message_name, "COMMAND_LONG");
    const auto command_json = nlohmann::json::parse(command.fields_json);
    EXPECT_EQ(command_json["command"], cmd_user_1);
    EXPECT_FLOAT_EQ(command_json["param1"].get<float>(), 1.5f);
    // NaN is represented as null.
    EXPECT_TRUE(command_json["param4"].is_null());

    server->unsubscribe_command(handle);
}

TEST(MavlinkDirectCommand, CommandIntDenied)
{
    CommandSetup setup{17041};
    ASSERT_TRUE(setup.client);

    auto received = std::make_shared<std::promise<MavlinkDirectServer::MavlinkMessage>>();
    auto received_fut = received->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto* server = setup.server.get();
    auto handle = server->subscribe_command(
        cmd_user_1, [server, received, flag](MavlinkDirectServer::MavlinkMessage command) {
            EXPECT_EQ(
                server->send_message(make_ack(command, mav_result_denied)),
                MavlinkDirectServer::Result::Success);
            std::call_once(*flag, [&]() { received->set_value(command); });
        });

    MavlinkDirect::MavlinkMessage command;
    command.message_name = "COMMAND_INT";
    command.fields_json =
        nlohmann::json{
            {"target_system", 1},
            {"target_component", 1},
            {"command", cmd_user_1},
            {"frame", 11},
            {"x", 473977418},
            {"y", 85455939},
            {"z", 5.0}}
            .dump();

    // A denied command still means an ack was received, the outcome is in the ack.
    auto [result, ack] = setup.client->send_command(command);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);
    EXPECT_EQ(nlohmann::json::parse(ack.fields_json)["result"], mav_result_denied);

    ASSERT_EQ(received_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    const auto received_command = received_fut.get();
    EXPECT_EQ(received_command.message_name, "COMMAND_INT");
    const auto command_json = nlohmann::json::parse(received_command.fields_json);
    EXPECT_EQ(command_json["frame"], 11);
    EXPECT_EQ(command_json["x"], 473977418);
    EXPECT_EQ(command_json["y"], 85455939);
    EXPECT_FLOAT_EQ(command_json["z"].get<float>(), 5.0f);

    server->unsubscribe_command(handle);
}

TEST(MavlinkDirectCommand, InProgressThenAccepted)
{
    CommandSetup setup{17042};
    ASSERT_TRUE(setup.client);

    auto received_count = std::make_shared<std::atomic<int>>(0);
    auto received = std::make_shared<std::promise<MavlinkDirectServer::MavlinkMessage>>();
    auto received_fut = received->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto* server = setup.server.get();
    auto handle = server->subscribe_command(
        cmd_user_1, [received_count, received, flag](MavlinkDirectServer::MavlinkMessage command) {
            ++(*received_count);
            std::call_once(*flag, [&]() { received->set_value(command); });
        });

    auto* client = setup.client.get();
    auto send_fut = std::async(std::launch::async, [client]() {
        return client->send_command(make_command_long({{"param1", 1}}));
    });

    // Act like a long-running command: report progress, then the final result.
    ASSERT_EQ(received_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    const auto command = received_fut.get();
    for (int progress : {10, 50, 90}) {
        EXPECT_EQ(
            server->send_message(make_ack(command, mav_result_in_progress, progress)),
            MavlinkDirectServer::Result::Success);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    EXPECT_EQ(
        server->send_message(make_ack(command, mav_result_accepted, 100)),
        MavlinkDirectServer::Result::Success);

    ASSERT_EQ(send_fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    auto [result, ack] = send_fut.get();
    EXPECT_EQ(result, MavlinkDirect::Result::Success);
    EXPECT_EQ(nlohmann::json::parse(ack.fields_json)["result"], mav_result_accepted);
    EXPECT_EQ(received_count->load(), 1);

    server->unsubscribe_command(handle);
}

TEST(MavlinkDirectCommand, NoSubscriberTimesOut)
{
    CommandSetup setup{17043};
    ASSERT_TRUE(setup.client);

    auto [result, ack] = setup.client->send_command(make_command_long({{"param1", 1}}));
    EXPECT_EQ(result, MavlinkDirect::Result::Timeout);
    EXPECT_TRUE(ack.fields_json.empty());
}

TEST(MavlinkDirectCommand, OnlyCommandMessagesAccepted)
{
    CommandSetup setup{17044};
    ASSERT_TRUE(setup.client);

    MavlinkDirect::MavlinkMessage heartbeat;
    heartbeat.message_name = "HEARTBEAT";
    heartbeat.fields_json = "{}";

    auto [result, ack] = setup.client->send_command(heartbeat);
    EXPECT_EQ(result, MavlinkDirect::Result::InvalidMessage);
}

TEST(MavlinkDirectCommand, SubscriptionsForSameCommand)
{
    CommandSetup setup{17045};
    ASSERT_TRUE(setup.client);

    auto* server = setup.server.get();
    auto first_count = std::make_shared<std::atomic<int>>(0);
    auto second_count = std::make_shared<std::atomic<int>>(0);

    auto first_handle = server->subscribe_command(
        cmd_user_1, [first_count](MavlinkDirectServer::MavlinkMessage) { ++(*first_count); });
    auto second_handle = server->subscribe_command(
        cmd_user_1, [server, second_count](MavlinkDirectServer::MavlinkMessage command) {
            ++(*second_count);
            EXPECT_EQ(
                server->send_message(make_ack(command, mav_result_accepted)),
                MavlinkDirectServer::Result::Success);
        });

    EXPECT_EQ(
        setup.client->send_command(make_command_long({{"param1", 1}})).first,
        MavlinkDirect::Result::Success);
    EXPECT_EQ(first_count->load(), 1);
    EXPECT_EQ(second_count->load(), 1);

    // With one subscriber left, the command is still handled.
    server->unsubscribe_command(first_handle);
    EXPECT_EQ(
        setup.client->send_command(make_command_long({{"param1", 2}})).first,
        MavlinkDirect::Result::Success);
    EXPECT_EQ(first_count->load(), 1);
    EXPECT_EQ(second_count->load(), 2);

    // With no subscriber left, nobody answers.
    server->unsubscribe_command(second_handle);
    EXPECT_EQ(
        setup.client->send_command(make_command_long({{"param1", 3}})).first,
        MavlinkDirect::Result::Timeout);
    EXPECT_EQ(second_count->load(), 2);
}
