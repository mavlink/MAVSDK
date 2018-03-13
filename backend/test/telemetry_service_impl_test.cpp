#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <vector>

#include "telemetry/mocks/telemetry_mock.h"
#include "telemetry/telemetryrpc_impl.h"

namespace {

using testing::_;
using testing::NiceMock;

using MockTelemetry = NiceMock<dronecore::testing::MockTelemetry>;
using TelemetryServiceImpl = dronecore::backend::TelemetryServiceImpl<MockTelemetry>;
using TelemetryService = dronecore::rpc::telemetry::TelemetryService;

using PositionResponse = dronecore::rpc::telemetry::PositionResponse;
using Position = dronecore::Telemetry::Position;

class TelemetryServiceImplTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        _telemetry = std::unique_ptr<MockTelemetry>(new MockTelemetry());
        _telemetry_service = std::unique_ptr<TelemetryServiceImpl>(new TelemetryServiceImpl(*_telemetry));

        grpc::ServerBuilder builder;
        builder.RegisterService(_telemetry_service.get());
        _server = builder.BuildAndStart();

        grpc::ChannelArguments channel_args;
        auto channel = _server->InProcessChannel(channel_args);
        _stub = TelemetryService::NewStub(channel);
    }

    virtual void TearDown()
    {
        _server->Shutdown();
    }

    std::future<void> subscribePositionAsync(std::vector<Position> &positions);
    Position createPosition(const double lat, const double lng, const float abs_alt,
                            const float rel_alt) const;

    std::unique_ptr<grpc::Server> _server;
    std::unique_ptr<TelemetryService::Stub> _stub;
    std::unique_ptr<MockTelemetry> _telemetry;
    std::unique_ptr<TelemetryServiceImpl> _telemetry_service;
};

ACTION_P2(SaveCallback, callback, callback_promise)
{
    *callback = arg0;
    callback_promise->set_value();
}

TEST_F(TelemetryServiceImplTest, registersToTelemetryPositionAsync)
{
    EXPECT_CALL(*_telemetry, position_async(_))
    .Times(1);

    std::vector<Position> positions;
    auto position_stream_future = subscribePositionAsync(positions);

    _telemetry_service->stop();
    position_stream_future.wait();
}

TEST_F(TelemetryServiceImplTest, doesNotSendPositionIfCallbackNotCalled)
{
    std::vector<Position> positions;
    auto position_stream_future = subscribePositionAsync(positions);

    _telemetry_service->stop();
    position_stream_future.wait();

    EXPECT_EQ(0, positions.size());
}

std::future<void> TelemetryServiceImplTest::subscribePositionAsync(std::vector<Position> &positions)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        dronecore::rpc::telemetry::SubscribePositionRequest request;
        auto responseReader = _stub->SubscribePosition(&context, request);

        dronecore::rpc::telemetry::PositionResponse response;
        while (responseReader->Read(&response)) {
            auto position_rpc = response.position();

            Position position;
            position.latitude_deg = position_rpc.latitude_deg();
            position.longitude_deg = position_rpc.longitude_deg();
            position.absolute_altitude_m = position_rpc.absolute_altitude_m();
            position.relative_altitude_m = position_rpc.relative_altitude_m();

            positions.push_back(position);
        }

        responseReader->Finish();
    });
}

TEST_F(TelemetryServiceImplTest, sendsOnePosition)
{
    auto expected_position = createPosition(41.848695, 75.132751, 3002.1f, 50.3f);
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::testing::position_callback_t position_callback;
    EXPECT_CALL(*_telemetry, position_async(_))
    .WillOnce(SaveCallback(&position_callback, &subscription_promise));

    std::vector<Position> positions;
    auto position_stream_future = subscribePositionAsync(positions);
    subscription_future.wait();
    position_callback(expected_position);
    _telemetry_service->stop();
    position_stream_future.wait();

    EXPECT_EQ(1, positions.size());
    EXPECT_EQ(expected_position, positions.at(0));
}

Position TelemetryServiceImplTest::createPosition(const double lat, const double lng,
                                                  const float abs_alt, const float rel_alt) const
{
    dronecore::Telemetry::Position expected_position;

    expected_position.latitude_deg = lat;
    expected_position.longitude_deg = lng;
    expected_position.absolute_altitude_m = abs_alt;
    expected_position.relative_altitude_m = rel_alt;

    return expected_position;
}

TEST_F(TelemetryServiceImplTest, sendsMultiplePositions)
{
    auto position0 = createPosition(41.848695, 75.132751, 3002.1f, 50.3f);
    auto position1 = createPosition(46.522626, 6.635356, 542.2f, 79.8f);
    auto position2 = createPosition(-50.995944711358824, -72.99892046835936, 1217.12f, 2.52f);
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::testing::position_callback_t position_callback;
    EXPECT_CALL(*_telemetry, position_async(_))
    .WillOnce(SaveCallback(&position_callback, &subscription_promise));

    std::vector<Position> positions;
    auto position_stream_future = subscribePositionAsync(positions);
    subscription_future.wait();
    position_callback(position0);
    position_callback(position1);
    position_callback(position2);
    _telemetry_service->stop();
    position_stream_future.wait();

    EXPECT_EQ(3, positions.size());
    EXPECT_EQ(position0, positions.at(0));
    EXPECT_EQ(position1, positions.at(1));
    EXPECT_EQ(position2, positions.at(2));
}

} // namespace
