#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <vector>

#include "core_service_impl.hpp"
#include "mocks/mavsdk_mock.hpp"

namespace {

using testing::_;
using testing::NiceMock;

using MockMavsdk = NiceMock<mavsdk::testing::MockMavsdk>;
using CoreServiceImpl = mavsdk::mavsdk_server::CoreServiceImpl<MockMavsdk>;
using CoreService = mavsdk::rpc::core::CoreService;

using ConnectionStateResponse = mavsdk::rpc::core::ConnectionStateResponse;

class CoreServiceImplTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        _mavsdk = std::make_unique<MockMavsdk>();
        _core_service = std::make_unique<CoreServiceImpl>(*_mavsdk);

        grpc::ServerBuilder builder;
        builder.RegisterService(_core_service.get());
        _server = builder.BuildAndStart();

        grpc::ChannelArguments channel_args;
        auto channel = _server->InProcessChannel(channel_args);
        _stub = CoreService::NewStub(channel);
    }

    virtual void TearDown() { _server->Shutdown(); }

    std::future<void> subscribeConnectionStateAsync(std::vector<bool>& events);

    std::unique_ptr<CoreServiceImpl> _core_service{};
    std::unique_ptr<MockMavsdk> _mavsdk{};
    std::unique_ptr<grpc::Server> _server{};
    std::unique_ptr<CoreService::Stub> _stub{};
};

ACTION_P2(SaveCallback, callback, callback_promise)
{
    *callback = arg0;
    callback_promise->set_value();
}

TEST_F(CoreServiceImplTest, subscribeConnectionStateSubscribesToChange)
{
    EXPECT_CALL(*_mavsdk, subscribe_on_new_system(_)).Times(1);
    grpc::ClientContext context;
    mavsdk::rpc::core::SubscribeConnectionStateRequest request;

    _stub->SubscribeConnectionState(&context, request);
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, feedHeartbeatWatchdogFeeds)
{
    EXPECT_CALL(*_mavsdk, feed_heartbeat_watchdog()).Times(1);

    grpc::ClientContext context;
    mavsdk::rpc::core::FeedHeartbeatWatchdogRequest request;
    mavsdk::rpc::core::FeedHeartbeatWatchdogResponse response;

    const auto status = _stub->FeedHeartbeatWatchdog(&context, request, &response);

    EXPECT_TRUE(status.ok());
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, setHeartbeatWatchdogTimeoutSetsTimeout)
{
    EXPECT_CALL(*_mavsdk, set_heartbeat_watchdog_timeout_s(2.5)).Times(1);

    grpc::ClientContext context;
    mavsdk::rpc::core::SetHeartbeatWatchdogTimeoutRequest request;
    request.set_timeout_s(2.5);
    mavsdk::rpc::core::SetHeartbeatWatchdogTimeoutResponse response;

    const auto status = _stub->SetHeartbeatWatchdogTimeout(&context, request, &response);

    EXPECT_TRUE(status.ok());
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, setHeartbeatWatchdogTimeoutDisable)
{
    EXPECT_CALL(*_mavsdk, set_heartbeat_watchdog_timeout_s(0.0)).Times(1);

    grpc::ClientContext context;
    mavsdk::rpc::core::SetHeartbeatWatchdogTimeoutRequest request;
    request.set_timeout_s(0.0);
    mavsdk::rpc::core::SetHeartbeatWatchdogTimeoutResponse response;

    const auto status = _stub->SetHeartbeatWatchdogTimeout(&context, request, &response);

    EXPECT_TRUE(status.ok());
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, setHeartbeatWatchdogTimeoutRejectsSubSecond)
{
    EXPECT_CALL(*_mavsdk, set_heartbeat_watchdog_timeout_s(_)).Times(0);

    grpc::ClientContext context;
    mavsdk::rpc::core::SetHeartbeatWatchdogTimeoutRequest request;
    request.set_timeout_s(0.5);
    mavsdk::rpc::core::SetHeartbeatWatchdogTimeoutResponse response;

    const auto status = _stub->SetHeartbeatWatchdogTimeout(&context, request, &response);

    EXPECT_EQ(grpc::StatusCode::INVALID_ARGUMENT, status.error_code());
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, connectionStateStreamEmptyIfCallbackNotCalled)
{
    std::vector<bool> events;
    auto events_stream_future = subscribeConnectionStateAsync(events);

    _core_service->stop();
    events_stream_future.wait();

    EXPECT_EQ(0, events.size());
}

std::future<void> CoreServiceImplTest::subscribeConnectionStateAsync(std::vector<bool>& events)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        mavsdk::rpc::core::SubscribeConnectionStateRequest request;
        auto response_reader = _stub->SubscribeConnectionState(&context, request);

        mavsdk::rpc::core::ConnectionStateResponse response;
        while (response_reader->Read(&response)) {
            events.push_back(response.connection_state().is_connected());
        }
    });
}

} // namespace
