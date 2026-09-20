#include "geofence_impl.hpp"

#include <gtest/gtest.h>

#include <limits>

namespace mavsdk {
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

TEST(GeofenceValidation, RejectsTooFewVerticesAndItemCountOverflow)
{
    auto data = valid_polygon();
    data.polygons[0].points.resize(2);
    EXPECT_FALSE(is_valid_geofence(data));

    data.polygons[0].points.resize(65536, {47.0, 8.0});
    EXPECT_FALSE(is_valid_geofence(data));
}

TEST(GeofenceValidation, RejectsInvalidCoordinates)
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
        EXPECT_FALSE(is_valid_geofence(data));
    }
}

TEST(GeofenceValidation, RejectsInvalidCircleRadiusAndCenter)
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
        EXPECT_FALSE(is_valid_geofence(data));
    }

    circle.radius = 10.0F;
    circle.point.latitude_deg = -91.0;
    Geofence::GeofenceData data{};
    data.circles.push_back(circle);
    EXPECT_FALSE(is_valid_geofence(data));
}

TEST(GeofenceValidation, AcceptsCoordinateBoundaries)
{
    auto data = valid_polygon();
    data.polygons[0].points = {{90.0, 180.0}, {-90.0, -180.0}, {0.0, 0.0}};

    Geofence::Circle circle{};
    circle.fence_type = Geofence::FenceType::Exclusion;
    circle.point = {0.0, 0.0};
    circle.radius = 10.0F;
    data.circles.push_back(circle);

    EXPECT_TRUE(is_valid_geofence(data));
}

} // namespace
} // namespace mavsdk
