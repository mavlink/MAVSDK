#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <vector>

#include "core/core_service_impl.h"
#include "core/mocks/mavsdk_mock.h"

namespace {

using testing::_;
using testing::NiceMock;

using MockMavsdk = NiceMock<mavsdk::testing::MockMavsdk>;
using CoreServiceImpl = mavsdk::backend::CoreServiceImpl<MockMavsdk>;
using CoreService = mavsdk::rpc::core::CoreService;

using ConnectionStateResponse = mavsdk::rpc::core::ConnectionStateResponse;

static constexpr auto DEFAULT_BACKEND_PORT = 50051;
static constexpr auto DEFAULT_BACKEND_ADDRESS = "localhost";

class CoreServiceImplTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        _dc = std::make_unique<MockMavsdk>();
        _core_service = std::make_unique<CoreServiceImpl>(*_dc);

        grpc::ServerBuilder builder;
        builder.RegisterService(_core_service.get());
        _server = builder.BuildAndStart();

        grpc::ChannelArguments channel_args;
        auto channel = _server->InProcessChannel(channel_args);
        _stub = CoreService::NewStub(channel);
    }

    virtual void TearDown() { _server->Shutdown(); }

    void checkPluginIsRunning(const std::string plugin_name);
    std::future<void> subscribeConnectionStateAsync(std::vector<std::pair<uint64_t, bool>>& events);

    std::unique_ptr<CoreServiceImpl> _core_service{};
    std::unique_ptr<MockMavsdk> _dc{};
    std::unique_ptr<grpc::Server> _server{};
    std::unique_ptr<CoreService::Stub> _stub{};
};

ACTION_P2(SaveCallback, callback, callback_promise)
{
    *callback = arg0;
    callback_promise->set_value();
}

TEST_F(CoreServiceImplTest, actionPluginIsRunning)
{
    checkPluginIsRunning("action");
}

void CoreServiceImplTest::checkPluginIsRunning(const std::string plugin_name)
{
    mavsdk::rpc::core::ListRunningPluginsResponse response;

    _core_service->ListRunningPlugins(nullptr, nullptr, &response);

    bool exists = false;
    for (int i = 0; i < response.plugin_info_size(); i++) {
        if (response.plugin_info(i).name() == plugin_name) {
            exists = true;
        }
    }

    EXPECT_TRUE(exists);
}

TEST_F(CoreServiceImplTest, missionPluginIsRunning)
{
    checkPluginIsRunning("mission");
}

TEST_F(CoreServiceImplTest, telemetryPluginIsRunning)
{
    checkPluginIsRunning("telemetry");
}

TEST_F(CoreServiceImplTest, addressIsLocalhostInPluginInfos)
{
    mavsdk::rpc::core::ListRunningPluginsResponse response;

    _core_service->ListRunningPlugins(nullptr, nullptr, &response);

    for (int i = 0; i < response.plugin_info_size(); i++) {
        EXPECT_EQ(response.plugin_info(i).address(), DEFAULT_BACKEND_ADDRESS);
    }
}

TEST_F(CoreServiceImplTest, portIsDefaultInPluginInfos)
{
    mavsdk::rpc::core::ListRunningPluginsResponse response;

    _core_service->ListRunningPlugins(nullptr, nullptr, &response);

    for (int i = 0; i < response.plugin_info_size(); i++) {
        EXPECT_EQ(response.plugin_info(i).port(), DEFAULT_BACKEND_PORT);
    }
}

TEST_F(CoreServiceImplTest, subscribeConnectionStateSubscribesToChange)
{
    EXPECT_CALL(*_dc, subscribe_on_new_system(_)).Times(1);
    grpc::ClientContext context;
    mavsdk::rpc::core::SubscribeConnectionStateRequest request;

    _stub->SubscribeConnectionState(&context, request);
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, connectionStateStreamEmptyIfCallbackNotCalled)
{
    std::vector<std::pair<uint64_t, bool>> events;
    auto events_stream_future = subscribeConnectionStateAsync(events);

    _core_service->stop();
    events_stream_future.wait();

    EXPECT_EQ(0, events.size());
}

std::future<void>
CoreServiceImplTest::subscribeConnectionStateAsync(std::vector<std::pair<uint64_t, bool>>& events)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        mavsdk::rpc::core::SubscribeConnectionStateRequest request;
        auto response_reader = _stub->SubscribeConnectionState(&context, request);

        mavsdk::rpc::core::ConnectionStateResponse response;
        while (response_reader->Read(&response)) {
            events.push_back(std::make_pair(
                response.connection_state().uuid(), response.connection_state().is_connected()));
        }
    });
}

} // namespace
