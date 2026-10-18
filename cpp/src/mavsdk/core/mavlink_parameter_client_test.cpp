#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <gtest/gtest.h>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>

#include "mavlink_message_handler.hpp"
#include "mavlink_parameter_client.hpp"
#include "sender.hpp"
#include "timeout_handler.hpp"
#include "mavsdk_time.hpp"
#include "mocks/sender_mock.hpp"

using namespace mavsdk;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using MockSender = NiceMock<mavsdk::testing::MockSender>;

using Result = MavlinkParameterClient::Result;

// The blocking getters and setters wait for their work item's callback. Teardown stops the
// io thread before it destroys the client, so neither a reply nor the timeout can call it
// anymore. Tearing down during a blocking call is not supported, but it should fail the
// call instead of leaving the caller stuck forever.
TEST(MavlinkParameterClientTeardown, DestroyEndsBlockingCall)
{
    asio::io_context io_context;
    auto work_guard = asio::make_work_guard(io_context);

    MockSender sender;
    ON_CALL(sender, io_context()).WillByDefault(ReturnRef(io_context));
    ON_CALL(sender, get_own_system_id()).WillByDefault(Return(245));
    ON_CALL(sender, get_own_component_id()).WillByDefault(Return(MAV_COMP_ID_MISSIONPLANNER));
    ON_CALL(sender, compatibility_mode()).WillByDefault(Return(CompatibilityMode::Auto));

    // Nobody answers, but the requests do go out.
    auto first_request_sent = std::make_shared<std::promise<void>>();
    auto first = std::make_shared<std::once_flag>();
    ON_CALL(sender, queue_message(_)).WillByDefault([first_request_sent, first](auto&&) {
        std::call_once(*first, [first_request_sent]() { first_request_sent->set_value(); });
        return true;
    });

    MavlinkMessageHandler message_handler{io_context};
    FakeTime time;
    TimeoutHandler timeout_handler{time};

    auto client = std::make_unique<MavlinkParameterClient>(
        sender,
        message_handler,
        timeout_handler,
        []() { return 0.5; },
        []() { return Autopilot::Px4; },
        1);

    std::thread io_thread([&io_context]() { io_context.run(); });

    // A plugin's own request, with its own cookie. Its callback typically captures the
    // plugin, which is gone by the time of teardown, so this must not be called back.
    int plugin_cookie = 0;
    auto plugin_called = std::make_shared<std::atomic<bool>>(false);
    client->get_param_int_async(
        "SYS_AUTOSTART",
        [plugin_called](Result, int32_t) { *plugin_called = true; },
        &plugin_cookie);

    // Detached rather than joined, so a regression fails this test instead of hanging it.
    auto blocked_result = std::make_shared<std::promise<Result>>();
    auto blocked_future = blocked_result->get_future();
    auto* raw_client = client.get();
    std::thread([raw_client, blocked_result]() {
        blocked_result->set_value(raw_client->get_all_params().first);
    }).detach();

    ASSERT_EQ(
        first_request_sent->get_future().wait_for(std::chrono::seconds(2)),
        std::future_status::ready);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_NE(blocked_future.wait_for(std::chrono::seconds(0)), std::future_status::ready);

    // Teardown the way ~MavsdkImpl does it: stop the io thread, then destroy the client.
    work_guard.reset();
    io_context.stop();
    io_thread.join();
    client.reset();

    ASSERT_EQ(blocked_future.wait_for(std::chrono::seconds(2)), std::future_status::ready)
        << "get_all_params() is still blocked after its client was destroyed";
    EXPECT_TRUE(blocked_future.get() == Result::ConnectionError);
    EXPECT_FALSE(*plugin_called);

    // Drain what is left on the io_context while the message handler is still alive.
    io_context.restart();
    while (io_context.poll()) {
    }
}

// Here the io thread is stopped before the call comes in, so its request never even reaches
// the work queue. Destroying the client still has to end the wait.
TEST(MavlinkParameterClientTeardown, DestroyEndsBlockingCallNotYetQueued)
{
    asio::io_context io_context;

    MockSender sender;
    ON_CALL(sender, io_context()).WillByDefault(ReturnRef(io_context));
    ON_CALL(sender, get_own_system_id()).WillByDefault(Return(245));
    ON_CALL(sender, get_own_component_id()).WillByDefault(Return(MAV_COMP_ID_MISSIONPLANNER));
    ON_CALL(sender, compatibility_mode()).WillByDefault(Return(CompatibilityMode::Auto));
    ON_CALL(sender, queue_message(_)).WillByDefault(Return(true));

    MavlinkMessageHandler message_handler{io_context};
    FakeTime time;
    TimeoutHandler timeout_handler{time};

    auto client = std::make_unique<MavlinkParameterClient>(
        sender,
        message_handler,
        timeout_handler,
        []() { return 0.5; },
        []() { return Autopilot::Px4; },
        1);

    // Teardown has already stopped the io thread when the call comes in.
    io_context.stop();

    auto blocked_result = std::make_shared<std::promise<Result>>();
    auto blocked_future = blocked_result->get_future();
    auto* raw_client = client.get();
    std::thread([raw_client, blocked_result]() {
        blocked_result->set_value(raw_client->get_param_int("SYS_AUTOSTART").first);
    }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    client.reset();

    ASSERT_EQ(blocked_future.wait_for(std::chrono::seconds(2)), std::future_status::ready)
        << "get_param_int() is still blocked after its client was destroyed";
}
