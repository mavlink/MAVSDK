#include "mavsdk.hpp"
#include "plugins/geofence/geofence.hpp"
#include "plugins/mission_raw_server/mission_raw_server.hpp"

#include <atomic>
#include <chrono>
#include <limits>
#include <memory>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;
using namespace std::chrono_literals;

namespace {

Geofence::GeofenceData valid_polygon()
{
    Geofence::Polygon polygon{};
    polygon.fence_type = Geofence::FenceType::Inclusion;
    polygon.points = {{47.0, 8.0}, {47.001, 8.0}, {47.0, 8.001}};
    Geofence::GeofenceData data{};
    data.polygons.push_back(polygon);
    return data;
}

class GeofenceValidation : public ::testing::Test {
protected:
    void SetUp() override
    {
        server = std::make_unique<MissionRawServer>(peer.server_component());
        ASSERT_EQ(client.add_any_connection("udpin://0.0.0.0:17935"), ConnectionResult::Success);
        ASSERT_EQ(peer.add_any_connection("udpout://127.0.0.1:17935"), ConnectionResult::Success);
        auto system = client.first_autopilot(5.0).value_or(nullptr);
        ASSERT_NE(system, nullptr);
        geofence = std::make_unique<Geofence>(system);
        client.subscribe_outgoing_messages_json(
            [count = mission_counts](Mavsdk::MavlinkMessage message) {
                if (message.message_name == "MISSION_COUNT") {
                    ++*count;
                }
                return true;
            });
    }

    void expect_rejected(const Geofence::GeofenceData& data)
    {
        EXPECT_EQ(geofence->upload_geofence(data), Geofence::Result::InvalidArgument);
        std::this_thread::sleep_for(20ms);
        EXPECT_EQ(mission_counts->load(), 0U) << "invalid input must not start a wire transfer";
    }

    Mavsdk client{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk peer{Mavsdk::Configuration{ComponentType::Autopilot}};
    std::unique_ptr<MissionRawServer> server;
    std::unique_ptr<Geofence> geofence;
    std::shared_ptr<std::atomic<unsigned>> mission_counts =
        std::make_shared<std::atomic<unsigned>>(0);
};

TEST_F(GeofenceValidation, RejectsTooFewVerticesAndWireCountOverflow)
{
    auto data = valid_polygon();
    data.polygons[0].points.resize(2);
    expect_rejected(data);
    data.polygons[0].points.resize(65536, {47.0, 8.0});
    expect_rejected(data);
}

TEST_F(GeofenceValidation, RejectsInvalidCoordinates)
{
    for (const auto point :
         {Geofence::Point{91.0, 8.0},
          Geofence::Point{-91.0, 8.0},
          Geofence::Point{47.0, 181.0},
          Geofence::Point{47.0, -181.0},
          Geofence::Point{std::numeric_limits<double>::quiet_NaN(), 8.0},
          Geofence::Point{47.0, std::numeric_limits<double>::infinity()}}) {
        auto data = valid_polygon();
        data.polygons[0].points[0] = point;
        expect_rejected(data);
    }
}

TEST_F(GeofenceValidation, RejectsInvalidCircleRadiusAndCenter)
{
    Geofence::Circle circle{};
    circle.fence_type = Geofence::FenceType::Exclusion;
    circle.point = {47.0, 8.0};
    for (const auto radius :
         {0.0F,
          -1.0F,
          std::numeric_limits<float>::quiet_NaN(),
          std::numeric_limits<float>::infinity()}) {
        circle.radius = radius;
        Geofence::GeofenceData data{};
        data.circles.push_back(circle);
        expect_rejected(data);
    }
    circle.radius = 10.0F;
    circle.point.latitude_deg = -91.0;
    Geofence::GeofenceData data{};
    data.circles.push_back(circle);
    expect_rejected(data);
}

TEST_F(GeofenceValidation, AcceptsCoordinateBoundariesAfterRejectedInput)
{
    auto data = valid_polygon();
    data.polygons[0].points.resize(2);
    expect_rejected(data);
    data.polygons[0].points = {{90.0, 180.0}, {-90.0, -180.0}, {0.0, 0.0}};
    EXPECT_EQ(geofence->upload_geofence(data), Geofence::Result::Success);
    EXPECT_GT(mission_counts->load(), 0U) << "valid boundary values must reach mission transfer";
}

} // namespace
