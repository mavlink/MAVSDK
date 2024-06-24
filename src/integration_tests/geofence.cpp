#include <iostream>
#include <memory>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/geofence/geofence.h"

using namespace mavsdk;

static Geofence::Point add_point(double latitude_deg, double longitude_deg);

TEST(SitlTest, PX4GeofenceInclusion)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
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

    std::vector<Geofence::Polygon> polygons;
    std::vector<Geofence::Circle> circles;

    // Polygon inclusion
    {
        std::vector<Geofence::Point> points;
        points.emplace_back(add_point(47.40029181357694, 8.540208324993841));
        points.emplace_back(add_point(47.400173434578114, 8.547689686153717));
        points.emplace_back(add_point(47.39677781722575, 8.547444838745832));
        points.emplace_back(add_point(47.39709351430107, 8.539753608416305));
        Geofence::Polygon new_polygon{};
        new_polygon.fence_type = Geofence::FenceType::Inclusion;
        new_polygon.points = points;
        polygons.push_back(new_polygon);
    }

    // Polygon exclusion
    {
        std::vector<Geofence::Point> points;
        points.emplace_back(add_point(47.39845295869903, 8.543682820794851));
        points.emplace_back(add_point(47.39837403681116, 8.545066315854541));
        points.emplace_back(add_point(47.39759073790796, 8.544926413920791));
        points.emplace_back(add_point(47.39762230688655, 8.543531259263398));
        Geofence::Polygon new_polygon{};
        new_polygon.fence_type = Geofence::FenceType::Exclusion;
        new_polygon.points = points;
        polygons.push_back(new_polygon);
    }

    // Circle inclusion
    {
        Geofence::Point center = add_point(47.39867310982157, 8.54379160368353);
        Geofence::Circle new_circle{};
        new_circle.point = center;
        new_circle.fence_type = Geofence::FenceType::Inclusion;
        new_circle.radius = 319.54F;
        circles.push_back(new_circle);
    }

    // Circle exclusion
    {
        Geofence::Point center = add_point(47.39928080314044, 8.54540060087578);
        Geofence::Circle new_circle{};
        new_circle.point = center;
        new_circle.fence_type = Geofence::FenceType::Exclusion;
        new_circle.radius = 49.52F;
        circles.push_back(new_circle);
    }

    Geofence::GeofenceData geofence_data{};
    geofence_data.polygons = polygons;
    geofence_data.circles = circles;

    EXPECT_EQ(Geofence::Result::Success, geofence->upload_geofence(geofence_data));

    EXPECT_EQ(Geofence::Result::Success, geofence->clear_geofence());
}

Geofence::Point add_point(double latitude_deg, double longitude_deg)
{
    Geofence::Point new_point;
    new_point.latitude_deg = latitude_deg;
    new_point.longitude_deg = longitude_deg;
    return new_point;
}
