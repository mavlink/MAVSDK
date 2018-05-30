#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <vector>

#include "core/core_service_impl.h"
#include "core/mocks/dronecore_mock.h"

namespace {

using testing::_;
using testing::NiceMock;

using MockDroneCore = NiceMock<dronecore::testing::MockDroneCore>;
using CoreServiceImpl = dronecore::backend::CoreServiceImpl<MockDroneCore>;
using CoreService = dronecore::rpc::core::CoreService;

using DiscoverResponse = dronecore::rpc::core::DiscoverResponse;
using TimeoutResponse = dronecore::rpc::core::TimeoutResponse;

static constexpr auto DEFAULT_BACKEND_PORT = 50051;
static constexpr auto DEFAULT_BACKEND_ADDRESS = "localhost";

static constexpr uint64_t ARBITRARY_UINT64_T = 42;
static constexpr uint64_t ARBITRARY_SMALL_INT = 11;

class CoreServiceImplTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        _dc = std::unique_ptr<MockDroneCore>(new MockDroneCore());
        _core_service = std::unique_ptr<CoreServiceImpl>(new CoreServiceImpl(*_dc));

        grpc::ServerBuilder builder;
        builder.RegisterService(_core_service.get());
        _server = builder.BuildAndStart();

        grpc::ChannelArguments channel_args;
        auto channel = _server->InProcessChannel(channel_args);
        _stub = CoreService::NewStub(channel);
    }

    virtual void TearDown() { _server->Shutdown(); }

    void checkPluginIsRunning(const std::string plugin_name);
    std::future<void> subscribeDiscoverAsync(std::vector<uint64_t> &uuids);
    std::future<void> subscribeTimeoutAsync(int *stream_count);

    std::unique_ptr<CoreServiceImpl> _core_service;
    std::unique_ptr<MockDroneCore> _dc;
    std::unique_ptr<grpc::Server> _server;
    std::unique_ptr<CoreService::Stub> _stub;
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
    dronecore::rpc::core::ListRunningPluginsResponse response;

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
    dronecore::rpc::core::ListRunningPluginsResponse response;

    _core_service->ListRunningPlugins(nullptr, nullptr, &response);

    for (int i = 0; i < response.plugin_info_size(); i++) {
        EXPECT_EQ(response.plugin_info(i).address(), DEFAULT_BACKEND_ADDRESS);
    }
}

TEST_F(CoreServiceImplTest, portIsDefaultInPluginInfos)
{
    dronecore::rpc::core::ListRunningPluginsResponse response;

    _core_service->ListRunningPlugins(nullptr, nullptr, &response);

    for (int i = 0; i < response.plugin_info_size(); i++) {
        EXPECT_EQ(response.plugin_info(i).port(), DEFAULT_BACKEND_PORT);
    }
}

TEST_F(CoreServiceImplTest, subscribeDiscoverActuallySubscribes)
{
    EXPECT_CALL(*_dc, register_on_discover(_)).Times(1);
    grpc::ClientContext context;
    dronecore::rpc::core::SubscribeDiscoverRequest request;

    _stub->SubscribeDiscover(&context, request);
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, discoverStreamEmptyIfCallbackNotCalled)
{
    std::vector<uint64_t> uuids;
    auto uuids_stream_future = subscribeDiscoverAsync(uuids);

    _core_service->stop();
    uuids_stream_future.wait();

    EXPECT_EQ(0, uuids.size());
}

std::future<void> CoreServiceImplTest::subscribeDiscoverAsync(std::vector<uint64_t> &uuids)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        dronecore::rpc::core::SubscribeDiscoverRequest request;
        auto response_reader = _stub->SubscribeDiscover(&context, request);

        dronecore::rpc::core::DiscoverResponse response;
        while (response_reader->Read(&response)) {
            uuids.push_back(response.uuid());
        }
    });
}

TEST_F(CoreServiceImplTest, discoverSendsOneUUID)
{
    const int expected_uuid = 42;
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::testing::event_callback_t event_callback;
    EXPECT_CALL(*_dc, register_on_discover(_))
        .WillOnce(SaveCallback(&event_callback, &subscription_promise));

    std::vector<uint64_t> uuids;
    auto uuids_stream_future = subscribeDiscoverAsync(uuids);
    subscription_future.wait();
    event_callback(expected_uuid);
    _core_service->stop();
    uuids_stream_future.wait();

    ASSERT_EQ(1, uuids.size());
    EXPECT_EQ(expected_uuid, uuids.at(0));
}

TEST_F(CoreServiceImplTest, discoverSendsMultipleUUIDs)
{
    const int uuid0 = 234132413;
    const int uuid1 = 948789299;
    const int uuid2 = 861987343;
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::testing::event_callback_t event_callback;
    EXPECT_CALL(*_dc, register_on_discover(_))
        .WillOnce(SaveCallback(&event_callback, &subscription_promise));

    std::vector<uint64_t> uuids;
    auto uuids_stream_future = subscribeDiscoverAsync(uuids);
    subscription_future.wait();
    event_callback(uuid0);
    event_callback(uuid1);
    event_callback(uuid2);
    _core_service->stop();
    uuids_stream_future.wait();

    ASSERT_EQ(3, uuids.size());
    EXPECT_EQ(uuid0, uuids.at(0));
    EXPECT_EQ(uuid1, uuids.at(1));
    EXPECT_EQ(uuid2, uuids.at(2));
}

TEST_F(CoreServiceImplTest, subscribeTimeoutActuallySubscribes)
{
    EXPECT_CALL(*_dc, register_on_timeout(_)).Times(1);
    grpc::ClientContext context;
    dronecore::rpc::core::SubscribeTimeoutRequest request;

    _stub->SubscribeTimeout(&context, request);
    _core_service->stop();
}

TEST_F(CoreServiceImplTest, timeoutStreamEmptyIfCallbackNotCalled)
{
    int count = 0;
    auto timeout_stream_future = subscribeTimeoutAsync(&count);

    _core_service->stop();
    timeout_stream_future.wait();

    EXPECT_EQ(0, count);
}

std::future<void> CoreServiceImplTest::subscribeTimeoutAsync(int *stream_count)
{
    return std::async(std::launch::async, [this, stream_count]() {
        grpc::ClientContext context;
        dronecore::rpc::core::SubscribeTimeoutRequest request;
        auto response_reader = _stub->SubscribeTimeout(&context, request);

        dronecore::rpc::core::TimeoutResponse response;
        while (response_reader->Read(&response)) {
            (*stream_count)++;
        }
    });
}

TEST_F(CoreServiceImplTest, timeoutIsCalledOnce)
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::testing::event_callback_t event_callback;
    EXPECT_CALL(*_dc, register_on_timeout(_))
        .WillOnce(SaveCallback(&event_callback, &subscription_promise));

    int count = 0;
    auto timeout_stream_future = subscribeTimeoutAsync(&count);
    subscription_future.wait();
    event_callback(ARBITRARY_UINT64_T);
    _core_service->stop();
    timeout_stream_future.wait();

    EXPECT_EQ(1, count);
}

TEST_F(CoreServiceImplTest, timeoutIsCalledMultipleTimes)
{
    const int expected_count = ARBITRARY_SMALL_INT;
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::testing::event_callback_t event_callback;
    EXPECT_CALL(*_dc, register_on_timeout(_))
        .WillOnce(SaveCallback(&event_callback, &subscription_promise));

    int count = 0;
    auto timeout_stream_future = subscribeTimeoutAsync(&count);
    subscription_future.wait();
    for (int i = 0; i < expected_count; i++) {
        event_callback(ARBITRARY_UINT64_T);
    }
    _core_service->stop();
    timeout_stream_future.wait();

    EXPECT_EQ(expected_count, count);
}

} // namespace
