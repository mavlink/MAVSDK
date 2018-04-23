#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <random>
#include <vector>

#include "telemetry/mocks/telemetry_mock.h"
#include "telemetry/telemetry_service_impl.h"

namespace {

using testing::_;
using testing::NiceMock;

using MockTelemetry = NiceMock<dronecore::testing::MockTelemetry>;
using TelemetryServiceImpl = dronecore::backend::TelemetryServiceImpl<MockTelemetry>;
using TelemetryService = dronecore::rpc::telemetry::TelemetryService;

using PositionResponse = dronecore::rpc::telemetry::PositionResponse;
using Position = dronecore::Telemetry::Position;

using HealthResponse = dronecore::rpc::telemetry::HealthResponse;
using Health = dronecore::Telemetry::Health;

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

        initRandomGenerator();
    }

    virtual void TearDown()
    {
        _server->Shutdown();
    }

    std::future<void> subscribePositionAsync(std::vector<Position> &positions);
    Position createPosition(const double lat, const double lng, const float abs_alt,
                            const float rel_alt) const;
    void checkSendsPositions(const std::vector<Position> &positions);

    std::future<void> subscribeHealthAsync(std::vector<Health> &healths);
    void checkSendsHealths(const std::vector<Health> &healths);
    Health createRandomHealth();
    std::vector<Health> generateRandomHealthsVector(const int size);
    bool generateRandomBool();

    void checkSendsHomePositions(const std::vector<Position> &home_positions);
    std::future<void> subscribeHomeAsync(std::vector<Position> &home_positions);

    std::unique_ptr<grpc::Server> _server;
    std::unique_ptr<TelemetryService::Stub> _stub;
    std::unique_ptr<MockTelemetry> _telemetry;
    std::unique_ptr<TelemetryServiceImpl> _telemetry_service;

private:
    void initRandomGenerator();

    std::random_device _random_device;
    std::mt19937 _generator;
    std::uniform_int_distribution<> _uniform_int_distribution;
};

void TelemetryServiceImplTest::initRandomGenerator()
{
    _generator = std::mt19937(_random_device());
    _uniform_int_distribution = std::uniform_int_distribution<>(0, 1);
}

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

std::future<void> TelemetryServiceImplTest::subscribePositionAsync(std::vector<Position> &positions)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        dronecore::rpc::telemetry::SubscribePositionRequest request;
        auto response_reader = _stub->SubscribePosition(&context, request);

        dronecore::rpc::telemetry::PositionResponse response;
        while (response_reader->Read(&response)) {
            auto position_rpc = response.position();

            Position position;
            position.latitude_deg = position_rpc.latitude_deg();
            position.longitude_deg = position_rpc.longitude_deg();
            position.absolute_altitude_m = position_rpc.absolute_altitude_m();
            position.relative_altitude_m = position_rpc.relative_altitude_m();

            positions.push_back(position);
        }

        response_reader->Finish();
    });
}

TEST_F(TelemetryServiceImplTest, doesNotSendPositionIfCallbackNotCalled)
{
    std::vector<Position> positions;
    auto position_stream_future = subscribePositionAsync(positions);

    _telemetry_service->stop();
    position_stream_future.wait();

    EXPECT_EQ(0, positions.size());
}

TEST_F(TelemetryServiceImplTest, sendsOnePosition)
{
    std::vector<Position> positions;
    positions.push_back(createPosition(41.848695, 75.132751, 3002.1f, 50.3f));

    checkSendsPositions(positions);
}

void TelemetryServiceImplTest::checkSendsPositions(const std::vector<Position> &positions)
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Telemetry::position_callback_t position_callback;
    EXPECT_CALL(*_telemetry, position_async(_))
    .WillOnce(SaveCallback(&position_callback, &subscription_promise));

    std::vector<Position> received_positions;
    auto position_stream_future = subscribePositionAsync(received_positions);
    subscription_future.wait();
    for (const auto position : positions) {
        position_callback(position);
    }
    _telemetry_service->stop();
    position_stream_future.wait();

    ASSERT_EQ(positions.size(), received_positions.size());
    for (size_t i = 0; i < positions.size(); i++) {
        EXPECT_EQ(positions.at(i), received_positions.at(i));
    }
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
    std::vector<Position> positions;
    positions.push_back(createPosition(41.848695, 75.132751, 3002.1f, 50.3f));
    positions.push_back(createPosition(46.522626, 6.635356, 542.2f, 79.8f));
    positions.push_back(createPosition(-50.995944711358824, -72.99892046835936, 1217.12f, 2.52f));

    checkSendsPositions(positions);
}

TEST_F(TelemetryServiceImplTest, registersToTelemetryHealthAsync)
{
    EXPECT_CALL(*_telemetry, health_async(_))
    .Times(1);

    std::vector<Health> healths;
    auto health_stream_future = subscribeHealthAsync(healths);

    _telemetry_service->stop();
    health_stream_future.wait();
}

std::future<void> TelemetryServiceImplTest::subscribeHealthAsync(std::vector<Health> &healths)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        dronecore::rpc::telemetry::SubscribeHealthRequest request;
        auto response_reader = _stub->SubscribeHealth(&context, request);

        dronecore::rpc::telemetry::HealthResponse response;
        while (response_reader->Read(&response)) {
            auto health_rpc = response.health();

            Health health;
            health.gyrometer_calibration_ok = health_rpc.is_gyrometer_calibration_ok();
            health.accelerometer_calibration_ok = health_rpc.is_accelerometer_calibration_ok();
            health.magnetometer_calibration_ok = health_rpc.is_magnetometer_calibration_ok();
            health.level_calibration_ok = health_rpc.is_level_calibration_ok();
            health.local_position_ok = health_rpc.is_local_position_ok();
            health.global_position_ok = health_rpc.is_global_position_ok();
            health.home_position_ok = health_rpc.is_home_position_ok();

            healths.push_back(health);
        }

        response_reader->Finish();
    });
}

TEST_F(TelemetryServiceImplTest, doesNotSendHealthIfCallbackNotCalled)
{
    std::vector<Health> healths;
    auto health_stream_future = subscribeHealthAsync(healths);

    _telemetry_service->stop();
    health_stream_future.wait();

    EXPECT_EQ(0, healths.size());
}

TEST_F(TelemetryServiceImplTest, sendsOneHealth)
{
    const auto health = generateRandomHealthsVector(1);
    checkSendsHealths(health);
}

std::vector<Health> TelemetryServiceImplTest::generateRandomHealthsVector(const int size)
{
    std::vector<Health> healths;
    for (int i = 0; i < size; i++) {
        healths.push_back(createRandomHealth());
    }

    return healths;
}

void TelemetryServiceImplTest::checkSendsHealths(const std::vector<Health> &healths)
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Telemetry::health_callback_t health_callback;
    EXPECT_CALL(*_telemetry, health_async(_))
    .WillOnce(SaveCallback(&health_callback, &subscription_promise));

    std::vector<Health> received_healths;
    auto health_stream_future = subscribeHealthAsync(received_healths);
    subscription_future.wait();
    for (const auto health : healths) {
        health_callback(health);
    }
    _telemetry_service->stop();
    health_stream_future.wait();

    ASSERT_EQ(healths.size(), received_healths.size());
    for (size_t i = 0; i < healths.size(); i++) {
        EXPECT_EQ(healths.at(i), received_healths.at(i));
    }
}

Health TelemetryServiceImplTest::createRandomHealth()
{
    dronecore::Telemetry::Health health;

    health.gyrometer_calibration_ok = generateRandomBool();
    health.accelerometer_calibration_ok = generateRandomBool();
    health.magnetometer_calibration_ok = generateRandomBool();
    health.level_calibration_ok = generateRandomBool();
    health.local_position_ok = generateRandomBool();
    health.global_position_ok = generateRandomBool();
    health.home_position_ok = generateRandomBool();

    return health;
}

bool TelemetryServiceImplTest::generateRandomBool()
{
    return _uniform_int_distribution(_generator) == 0;
}

TEST_F(TelemetryServiceImplTest, sendsMultipleHealths)
{
    const auto health = generateRandomHealthsVector(10);
    checkSendsHealths(health);
}

TEST_F(TelemetryServiceImplTest, registersToTelemetryHomeAsync)
{
    EXPECT_CALL(*_telemetry, home_position_async(_))
    .Times(1);

    std::vector<Position> home_positions;
    auto home_stream_future = subscribeHomeAsync(home_positions);

    _telemetry_service->stop();
    home_stream_future.wait();
}

std::future<void> TelemetryServiceImplTest::subscribeHomeAsync(std::vector<Position>
                                                               &home_positions)
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        dronecore::rpc::telemetry::SubscribeHomeRequest request;
        auto response_reader = _stub->SubscribeHome(&context, request);

        dronecore::rpc::telemetry::HomeResponse response;
        while (response_reader->Read(&response)) {
            auto home_rpc = response.home();

            Position home;
            home.latitude_deg = home_rpc.latitude_deg();
            home.longitude_deg = home_rpc.longitude_deg();
            home.absolute_altitude_m = home_rpc.absolute_altitude_m();
            home.relative_altitude_m = home_rpc.relative_altitude_m();

            home_positions.push_back(home);
        }

        response_reader->Finish();
    });
}

TEST_F(TelemetryServiceImplTest, doesNotSendHomeIfCallbackNotCalled)
{
    std::vector<Position> home_positions;
    auto home_stream_future = subscribeHomeAsync(home_positions);

    _telemetry_service->stop();
    home_stream_future.wait();

    EXPECT_EQ(0, home_positions.size());
}

TEST_F(TelemetryServiceImplTest, sendsOneHome)
{
    std::vector<Position> home_positions;
    home_positions.push_back(createPosition(41.848695, 75.132751, 3002.1f, 50.3f));

    checkSendsHomePositions(home_positions);
}

void TelemetryServiceImplTest::checkSendsHomePositions(const std::vector<Position> &home_positions)
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Telemetry::position_callback_t home_callback;
    EXPECT_CALL(*_telemetry, home_position_async(_))
    .WillOnce(SaveCallback(&home_callback, &subscription_promise));

    std::vector<Position> received_home_positions;
    auto home_stream_future = subscribeHomeAsync(received_home_positions);
    subscription_future.wait();
    for (const auto home_position : home_positions) {
        home_callback(home_position);
    }
    _telemetry_service->stop();
    home_stream_future.wait();

    ASSERT_EQ(home_positions.size(), received_home_positions.size());
    for (size_t i = 0; i < home_positions.size(); i++) {
        EXPECT_EQ(home_positions.at(i), received_home_positions.at(i));
    }
}

TEST_F(TelemetryServiceImplTest, sendsMultipleHomePositions)
{
    std::vector<Position> home_positions;
    home_positions.push_back(createPosition(41.848695, 75.132751, 3002.1f, 50.3f));
    home_positions.push_back(createPosition(46.522626, 6.635356, 542.2f, 79.8f));
    home_positions.push_back(createPosition(-50.995944711358824, -72.99892046835936, 1217.12f, 2.52f));

    checkSendsHomePositions(home_positions);
}

} // namespace
