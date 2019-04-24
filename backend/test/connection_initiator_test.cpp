#include <future>
#include <gmock/gmock.h>

#include "connection_initiator.h"
#include "mocks/dronecode_sdk_mock.h"

namespace {

using testing::_;
using testing::NiceMock;

using event_callback_t = dronecode_sdk::testing::event_callback_t;
using MockDronecodeSDK = NiceMock<dronecode_sdk::testing::MockDronecodeSDK>;
using ConnectionInitiator = dronecode_sdk::backend::ConnectionInitiator<MockDronecodeSDK>;

static constexpr auto ARBITRARY_CONNECTION_URL = "udp://1291";
static constexpr auto ARBITRARY_UUID = 1492;

ACTION_P(SaveCallback, event_callback)
{
    *event_callback = arg0;
}

TEST(ConnectionInitiator, registerDiscoverIsCalledExactlyOnce)
{
    ConnectionInitiator initiator;
    MockDronecodeSDK dc;
    EXPECT_CALL(dc, register_on_discover(_)).Times(1);

    initiator.start(dc, ARBITRARY_CONNECTION_URL);
}

TEST(ConnectionInitiator, startAddsUDPConnection)
{
    ConnectionInitiator initiator;
    MockDronecodeSDK dc;

    EXPECT_CALL(dc, add_any_connection(_));

    initiator.start(dc, ARBITRARY_CONNECTION_URL);
}

TEST(ConnectionInitiator, startHangsUntilSystemDiscovered)
{
    ConnectionInitiator initiator;
    MockDronecodeSDK dc;
    event_callback_t discover_callback;
    EXPECT_CALL(dc, register_on_discover(_)).WillOnce(SaveCallback(&discover_callback));

    auto async_future = std::async(std::launch::async, [&initiator, &dc]() {
        initiator.start(dc, ARBITRARY_CONNECTION_URL);
        initiator.wait();
    });

    EXPECT_TRUE(async_future.wait_for(std::chrono::milliseconds(100)) ==
                std::future_status::timeout)
        << "Call to 'start(...)' should hang until 'discover_callback(...)' is actually called!";
    discover_callback(ARBITRARY_UUID);
}

TEST(ConnectionInitiator, connectionDetectedIfDiscoverCallbackCalledBeforeWait)
{
    ConnectionInitiator initiator;
    MockDronecodeSDK dc;
    event_callback_t discover_callback;
    EXPECT_CALL(dc, register_on_discover(_)).WillOnce(SaveCallback(&discover_callback));

    initiator.start(dc, ARBITRARY_CONNECTION_URL);
    discover_callback(ARBITRARY_UUID);
    initiator.wait();
}

TEST(ConnectionInitiator, doesNotCrashIfDiscoverCallbackCalledMoreThanOnce)
{
    ConnectionInitiator initiator;
    MockDronecodeSDK dc;
    event_callback_t discover_callback;
    EXPECT_CALL(dc, register_on_discover(_)).WillOnce(SaveCallback(&discover_callback));

    initiator.start(dc, ARBITRARY_CONNECTION_URL);
    discover_callback(ARBITRARY_UUID);
    discover_callback(ARBITRARY_UUID);
}

} // namespace
