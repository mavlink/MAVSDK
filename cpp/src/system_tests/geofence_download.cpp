#include "log.h"
#include "mavsdk.h"
#include "plugins/geofence/geofence.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include <future>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Geofence, UploadDownloadRoundTrip)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17012"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17012"), ConnectionResult::Success);

    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    auto geofence = Geofence{system};

    // Build test geofence data: 1 inclusion polygon + 1 exclusion circle
    Geofence::GeofenceData upload_data;

    Geofence::Polygon polygon;
    polygon.fence_type = Geofence::FenceType::Inclusion;
    polygon.points.push_back({47.3977, 8.5456});
    polygon.points.push_back({47.3980, 8.5460});
    polygon.points.push_back({47.3975, 8.5465});
    polygon.points.push_back({47.3972, 8.5458});
    upload_data.polygons.push_back(polygon);

    Geofence::Circle circle;
    circle.fence_type = Geofence::FenceType::Exclusion;
    circle.point = {47.3978, 8.5459};
    circle.radius = 50.0f;
    upload_data.circles.push_back(circle);

    // Upload
    auto upload_result = geofence.upload_geofence(upload_data);
    ASSERT_EQ(upload_result, Geofence::Result::Success);

    // Download
    auto [download_result, download_data] = geofence.download_geofence();
    ASSERT_EQ(download_result, Geofence::Result::Success);

    // Verify polygon
    ASSERT_EQ(download_data.polygons.size(), 1u);
    EXPECT_EQ(download_data.polygons[0].fence_type, Geofence::FenceType::Inclusion);
    ASSERT_EQ(download_data.polygons[0].points.size(), 4u);
    EXPECT_NEAR(download_data.polygons[0].points[0].latitude_deg, 47.3977, 1e-5);
    EXPECT_NEAR(download_data.polygons[0].points[0].longitude_deg, 8.5456, 1e-5);
    EXPECT_NEAR(download_data.polygons[0].points[3].latitude_deg, 47.3972, 1e-5);
    EXPECT_NEAR(download_data.polygons[0].points[3].longitude_deg, 8.5458, 1e-5);

    // Verify circle
    ASSERT_EQ(download_data.circles.size(), 1u);
    EXPECT_EQ(download_data.circles[0].fence_type, Geofence::FenceType::Exclusion);
    EXPECT_NEAR(download_data.circles[0].point.latitude_deg, 47.3978, 1e-5);
    EXPECT_NEAR(download_data.circles[0].point.longitude_deg, 8.5459, 1e-5);
    EXPECT_FLOAT_EQ(download_data.circles[0].radius, 50.0f);

    // Clear
    auto clear_result = geofence.clear_geofence();
    EXPECT_EQ(clear_result, Geofence::Result::Success);
}
