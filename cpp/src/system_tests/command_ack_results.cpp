#include "mavsdk.hpp"
#include "plugins/action/action.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

namespace {

static constexpr int cmd_component_arm_disarm = 400;
static constexpr double reduced_timeout_s = 0.2;
static constexpr auto retry_exhaustion_wait = std::chrono::milliseconds(1000);
static constexpr int mav_result_accepted = 0;
static constexpr int mav_result_in_progress = 5;
static constexpr int mav_result_command_long_only = 7;
static constexpr int mav_result_command_int_only = 8;
static constexpr int mav_result_command_unsupported_mav_frame = 9;
static constexpr int mav_result_not_in_control = 10;
static constexpr int mav_result_unknown_future = 42;

MavlinkDirectServer::MavlinkMessage
make_ack(const MavlinkDirectServer::MavlinkMessage& command, int result, int progress = 0)
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

} // namespace

// Every non-IN_PROGRESS COMMAND_ACK must complete the command once without
// retransmitting or invoking the completion callback again on timeout.
TEST(CommandAckResults, TerminalAndUnknownResultsCompleteOnceWithoutRetransmit)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17031"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17031"), ConnectionResult::Success);

    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    auto mavlink_direct_server = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    struct TestCase {
        int mav_result;
        Action::Result expected_result;
    };

    const std::vector<TestCase> cases{
        {mav_result_command_long_only, Action::Result::Unsupported},
        {mav_result_command_int_only, Action::Result::Unsupported},
        {mav_result_command_unsupported_mav_frame, Action::Result::Unsupported},
        {mav_result_not_in_control, Action::Result::CommandDenied},
        {mav_result_unknown_future, Action::Result::Unknown},
    };

    for (const auto& test_case : cases) {
        auto received_count = std::make_shared<std::atomic<int>>(0);
        auto reply_once = std::make_shared<std::once_flag>();

        auto handle = mavlink_direct_server.subscribe_message(
            "COMMAND_LONG",
            [&mavlink_direct_server, received_count, reply_once, result = test_case.mav_result](
                MavlinkDirectServer::MavlinkMessage message) {
                if (nlohmann::json::parse(message.fields_json)["command"] !=
                    cmd_component_arm_disarm) {
                    return;
                }
                ++(*received_count);
                std::call_once(*reply_once, [&]() {
                    EXPECT_EQ(
                        mavlink_direct_server.send_message(make_ack(message, result)),
                        MavlinkDirectServer::Result::Success);
                });
            });

        std::mutex results_mutex;
        std::vector<Action::Result> results;
        auto first_cb_prom = std::make_shared<std::promise<void>>();
        auto first_cb_fut = first_cb_prom->get_future();
        auto cb_once = std::make_shared<std::once_flag>();

        action.arm_async([&results_mutex, &results, first_cb_prom, cb_once](Action::Result res) {
            {
                std::lock_guard<std::mutex> lock(results_mutex);
                results.push_back(res);
            }
            std::call_once(*cb_once, [&]() { first_cb_prom->set_value(); });
        });

        ASSERT_EQ(first_cb_fut.wait_for(std::chrono::seconds(2)), std::future_status::ready)
            << "for mav_result=" << test_case.mav_result;

        // Wait well past the retry timeout (0.2 s * (1 + DEFAULT_RETRIES=3) = 0.8 s)
        // so any leaked timeout would retransmit and fire the callback a second time.
        std::this_thread::sleep_for(retry_exhaustion_wait);

        {
            std::lock_guard<std::mutex> lock(results_mutex);
            ASSERT_EQ(results.size(), 1u) << "for mav_result=" << test_case.mav_result;
            EXPECT_EQ(results[0], test_case.expected_result)
                << "for mav_result=" << test_case.mav_result;
        }
        EXPECT_EQ(received_count->load(), 1) << "for mav_result=" << test_case.mav_result;

        mavlink_direct_server.unsubscribe_message(handle);
    }
}

// Synchronous commands must not abort (double promise satisfaction) after
// receiving standard or unknown terminal MAV_RESULT codes.
TEST(CommandAckResults, SyncCommandSurvivesTerminalAndUnknownResults)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17032"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17032"), ConnectionResult::Success);

    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    auto mavlink_direct_server = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    struct TestCase {
        int mav_result;
        Action::Result expected_result;
    };

    const std::vector<TestCase> cases{
        {mav_result_command_long_only, Action::Result::Unsupported},
        {mav_result_command_int_only, Action::Result::Unsupported},
        {mav_result_command_unsupported_mav_frame, Action::Result::Unsupported},
        {mav_result_not_in_control, Action::Result::CommandDenied},
        {mav_result_unknown_future, Action::Result::Unknown},
    };

    for (const auto& test_case : cases) {
        auto received_count = std::make_shared<std::atomic<int>>(0);
        auto reply_once = std::make_shared<std::once_flag>();

        auto handle = mavlink_direct_server.subscribe_message(
            "COMMAND_LONG",
            [&mavlink_direct_server, received_count, reply_once, result = test_case.mav_result](
                MavlinkDirectServer::MavlinkMessage message) {
                if (nlohmann::json::parse(message.fields_json)["command"] !=
                    cmd_component_arm_disarm) {
                    return;
                }
                ++(*received_count);
                std::call_once(*reply_once, [&]() {
                    EXPECT_EQ(
                        mavlink_direct_server.send_message(make_ack(message, result)),
                        MavlinkDirectServer::Result::Success);
                });
            });

        const auto result = action.arm();
        EXPECT_EQ(result, test_case.expected_result) << "for mav_result=" << test_case.mav_result;

        // Wait past retry timeout: without the fix, the leaked timeout fires
        // the callback again after action.arm() has already returned.
        std::this_thread::sleep_for(retry_exhaustion_wait);

        EXPECT_EQ(received_count->load(), 1) << "for mav_result=" << test_case.mav_result;

        mavlink_direct_server.unsubscribe_message(handle);
    }
}

// An IN_PROGRESS command that ends with NOT_IN_CONTROL or an unknown MAV_RESULT
// must also terminate cleanly once.
TEST(CommandAckResults, InProgressThenTerminalOrUnknownResult)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17033"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17033"), ConnectionResult::Success);

    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    auto mavlink_direct_server = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    struct TestCase {
        int final_mav_result;
        Action::Result expected_result;
    };

    const std::vector<TestCase> cases{
        {mav_result_not_in_control, Action::Result::CommandDenied},
        {mav_result_unknown_future, Action::Result::Unknown},
        {mav_result_accepted, Action::Result::Success},
    };

    for (const auto& test_case : cases) {
        auto received_count = std::make_shared<std::atomic<int>>(0);
        auto first_command_prom =
            std::make_shared<std::promise<MavlinkDirectServer::MavlinkMessage>>();
        auto first_command_fut = first_command_prom->get_future();
        auto reply_once = std::make_shared<std::once_flag>();

        auto handle = mavlink_direct_server.subscribe_message(
            "COMMAND_LONG",
            [&mavlink_direct_server, received_count, first_command_prom, reply_once](
                MavlinkDirectServer::MavlinkMessage message) {
                if (nlohmann::json::parse(message.fields_json)["command"] !=
                    cmd_component_arm_disarm) {
                    return;
                }
                ++(*received_count);
                std::call_once(*reply_once, [&]() {
                    EXPECT_EQ(
                        mavlink_direct_server.send_message(
                            make_ack(message, mav_result_in_progress, 255)),
                        MavlinkDirectServer::Result::Success);
                    first_command_prom->set_value(message);
                });
            });

        auto arm_fut = std::async(std::launch::async, [&action]() { return action.arm(); });

        ASSERT_EQ(first_command_fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
        const auto command = first_command_fut.get();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        EXPECT_EQ(
            mavlink_direct_server.send_message(make_ack(command, test_case.final_mav_result)),
            MavlinkDirectServer::Result::Success);

        ASSERT_EQ(arm_fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
        EXPECT_EQ(arm_fut.get(), test_case.expected_result)
            << "for final_mav_result=" << test_case.final_mav_result;
        EXPECT_EQ(received_count->load(), 1);

        mavlink_direct_server.unsubscribe_message(handle);
    }
}
