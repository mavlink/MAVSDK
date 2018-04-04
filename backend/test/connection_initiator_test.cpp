#include <future>
#include <gmock/gmock.h>

#include "connection_initiator.h"
#include "mocks/dronecore_mock.h"

namespace {

using testing::_;
using testing::NiceMock;

using event_callback_t = dronecore::testing::event_callback_t;
using MockDroneCore = NiceMock<dronecore::testing::MockDroneCore>;
using ConnectionInitiator = dronecore::backend::ConnectionInitiator<MockDroneCore>;

static constexpr auto ARBITRARY_PORT = 1291;
static constexpr auto ARBITRARY_UUID = 1492;

ACTION_P(SaveCallback, event_callback)
{
    *event_callback = arg0;
}

TEST(ConnectionInitiator, registerDiscoverIsCalledExactlyOnce)
{
    ConnectionInitiator initiator;
    MockDroneCore dc;
    EXPECT_CALL(dc, register_on_discover(_))
    .Times(1);

    initiator.start(dc, ARBITRARY_PORT);
}

TEST(ConnectionInitiator, startAddsUDPConnection)
{
    ConnectionInitiator initiator;
    MockDroneCore dc;

    EXPECT_CALL(dc, add_udp_connection(_));

    initiator.start(dc, ARBITRARY_PORT);
}

TEST(ConnectionInitiator, startHangsUntilSystemDiscovered)
{
    ConnectionInitiator initiator;
    MockDroneCore dc;
    event_callback_t discover_callback;
    EXPECT_CALL(dc, register_on_discover(_))
    .WillOnce(SaveCallback(&discover_callback));

    auto async_future = std::async(std::launch::async, [&initiator, &dc]() {
        initiator.start(dc, ARBITRARY_PORT);
        initiator.wait();
    });

    EXPECT_TRUE(async_future.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout) <<
            "Call to 'start(...)' should hang until 'discover_callback(...)' is actually called!";
    discover_callback(ARBITRARY_UUID);
}

TEST(ConnectionInitiator, connectionDetectedIfDiscoverCallbackCalledBeforeWait)
{
    ConnectionInitiator initiator;
    MockDroneCore dc;
    event_callback_t discover_callback;
    EXPECT_CALL(dc, register_on_discover(_))
    .WillOnce(SaveCallback(&discover_callback));

    initiator.start(dc, ARBITRARY_PORT);
    discover_callback(ARBITRARY_UUID);
    initiator.wait();
}

TEST(ConnectionInitiator, doesNotCrashIfDiscoverCallbackCalledMoreThanOnce)
{
    ConnectionInitiator initiator;
    MockDroneCore dc;
    event_callback_t discover_callback;
    EXPECT_CALL(dc, register_on_discover(_))
    .WillOnce(SaveCallback(&discover_callback));

    initiator.start(dc, ARBITRARY_PORT);
    discover_callback(ARBITRARY_UUID);
    discover_callback(ARBITRARY_UUID);
}

} // namespace
