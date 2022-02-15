#include <iostream>
#include <memory>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/geofence/geofence.h"

using namespace mavsdk;

static Geofence::Point add_point(double latitude_deg, double longitude_deg);

TEST_F(SitlTest, PX4GeofenceInclusion)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto telemetry = std::make_shared<Telemetry>(system);
    auto geofence = std::make_shared<Geofence>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    LogInfo() << "System ready, let's start";

    std::vector<Geofence::Point> points;
    points.push_back(add_point(47.39929240, 8.54296524));
    points.push_back(add_point(47.39696482, 8.54161340));
    points.push_back(add_point(47.39626761, 8.54527193));
    points.push_back(add_point(47.39980072, 8.54736050));

    std::vector<Geofence::Polygon> polygons;
    Geofence::Polygon new_polygon{};
    new_polygon.fence_type = Geofence::Polygon::FenceType::Inclusion;
    new_polygon.points = points;

    polygons.push_back(new_polygon);

    EXPECT_EQ(Geofence::Result::Success, geofence->upload_geofence(polygons));

    EXPECT_EQ(Geofence::Result::Success, geofence->clear_geofence());
}

Geofence::Point add_point(double latitude_deg, double longitude_deg)
{
    Geofence::Point new_point;
    new_point.latitude_deg = latitude_deg;
    new_point.longitude_deg = longitude_deg;
    return new_point;
}
