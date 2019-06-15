#include <iostream>
#include <functional>
#include <memory>
#include <atomic>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/geofence/geofence.h"

using namespace mavsdk;
using namespace std::placeholders; // for `_1`

static void receive_send_geofence_result(Geofence::Result result);

static Geofence::Polygon::Point add_point(double latitude_deg, double longitude_deg);
static std::atomic<bool> _got_result{false};

TEST_F(SitlTest, GeofenceInclusion)
{
    Mavsdk dl;

    ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dl.system();
    ASSERT_TRUE(system.has_autopilot());
    auto telemetry = std::make_shared<Telemetry>(system);
    auto geofence = std::make_shared<Geofence>(system);

    while (!telemetry->health_all_ok()) {
        LogInfo() << "waiting for system to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "System ready, let's start";

    std::vector<Geofence::Polygon::Point> points;
    points.push_back(add_point(47.39929240, 8.54296524));
    points.push_back(add_point(47.39696482, 8.54161340));
    points.push_back(add_point(47.39626761, 8.54527193));
    points.push_back(add_point(47.39980072, 8.54736050));

    std::vector<std::shared_ptr<Geofence::Polygon>> polygons;
    std::shared_ptr<Geofence::Polygon> new_polygon(new Geofence::Polygon());
    new_polygon->type = Geofence::Polygon::Type::INCLUSION;
    new_polygon->points = points;

    polygons.push_back(new_polygon);

    geofence->send_geofence_async(polygons, std::bind(&receive_send_geofence_result, _1));

    for (unsigned i = 0; i < 5; ++i) {
        if (_got_result) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    EXPECT_TRUE(_got_result);
}

void receive_send_geofence_result(Geofence::Result result)
{
    EXPECT_EQ(result, Geofence::Result::SUCCESS);

    _got_result = true;
}

Geofence::Polygon::Point add_point(double latitude_deg, double longitude_deg)
{
    Geofence::Polygon::Point new_point;
    new_point.latitude_deg = latitude_deg;
    new_point.longitude_deg = longitude_deg;
    return new_point;
}
