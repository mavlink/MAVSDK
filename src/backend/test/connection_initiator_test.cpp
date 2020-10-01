#include <future>
#include <gmock/gmock.h>

#include "connection_initiator.h"
#include "mocks/mavsdk_mock.h"

namespace {

using testing::_;
using testing::NiceMock;

using ChangeCallback = mavsdk::testing::ChangeCallback;
using MockMavsdk = NiceMock<mavsdk::testing::MockMavsdk>;
using ConnectionInitiator = mavsdk::backend::ConnectionInitiator<MockMavsdk>;

static constexpr auto ARBITRARY_CONNECTION_URL = "udp://1291";

ACTION_P(SaveCallback, change_callback)
{
    *change_callback = arg0;
}

TEST(ConnectionInitiator, subscribeChangeIsCalledExactlyOnce)
{
    ConnectionInitiator initiator;
    MockMavsdk mavsdk;
    EXPECT_CALL(mavsdk, subscribe_on_change(_)).Times(1);

    initiator.start(mavsdk, ARBITRARY_CONNECTION_URL);
}

TEST(ConnectionInitiator, startAddsUDPConnection)
{
    ConnectionInitiator initiator;
    MockMavsdk mavsdk;

    EXPECT_CALL(mavsdk, add_any_connection(_));

    initiator.start(mavsdk, ARBITRARY_CONNECTION_URL);
}

TEST(ConnectionInitiator, startHangsUntilSystemDiscovered)
{
    ConnectionInitiator initiator;
    MockMavsdk mavsdk;
    ChangeCallback change_callback;
    EXPECT_CALL(mavsdk, subscribe_on_change(_)).WillOnce(SaveCallback(&change_callback));

    auto async_future = std::async(std::launch::async, [&initiator, &mavsdk]() {
        initiator.start(mavsdk, ARBITRARY_CONNECTION_URL);
        initiator.wait();
    });

    EXPECT_TRUE(
        async_future.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout)
        << "Call to 'start(...)' should hang until 'change_callback(...)' is actually called!";
    change_callback();
}

TEST(ConnectionInitiator, connectionDetectedIfDiscoverCallbackCalledBeforeWait)
{
    ConnectionInitiator initiator;
    MockMavsdk mavsdk;
    ChangeCallback change_callback;
    EXPECT_CALL(mavsdk, subscribe_on_change(_)).WillOnce(SaveCallback(&change_callback));

    initiator.start(mavsdk, ARBITRARY_CONNECTION_URL);
    change_callback();
    initiator.wait();
}

TEST(ConnectionInitiator, doesNotCrashIfDiscoverCallbackCalledMoreThanOnce)
{
    ConnectionInitiator initiator;
    MockMavsdk mavsdk;
    ChangeCallback change_callback;
    EXPECT_CALL(mavsdk, subscribe_on_change(_)).WillOnce(SaveCallback(&change_callback));

    initiator.start(mavsdk, ARBITRARY_CONNECTION_URL);
    change_callback();
    change_callback();
}

} // namespace
