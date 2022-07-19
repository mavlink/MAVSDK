//
// Try to implement mission logic on companion computer.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/geometry.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission_raw_server/mission_raw_server.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "rounded_corners/rounded_corners.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

using namespace mavsdk;
using namespace rounded_corners;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <gcs connection> <autopilot connection>\n";
}

struct LocalMission {
    std::vector<Point> points;
};

LocalMission
convert_mission_plan(Telemetry::GpsGlobalOrigin origin, const MissionRawServer::MissionPlan& plan)
{
    LocalMission local_mission;

    using CT = geometry::CoordinateTransformation;

    auto ct = CT{CT::GlobalCoordinate{origin.latitude_deg, origin.longitude_deg}};

    for (const auto& item : plan.mission_items) {
        switch (item.command) {
            case MAV_CMD_NAV_TAKEOFF:
            case MAV_CMD_NAV_WAYPOINT: {
                std::cout << "Using command: " << item.command << " -> "
                          << static_cast<double>(item.x) / 1e7 << ", "
                          << static_cast<double>(item.y) / 1e7 << ", seq: " << item.seq << '\n';
                const auto local_coord = ct.local_from_global(CT::GlobalCoordinate{
                    static_cast<double>(item.x) / 1e7, static_cast<double>(item.y) / 1e7});
                local_mission.points.emplace_back(
                    Point{local_coord.north_m, local_coord.east_m, -item.z});
                break;
            }

            default:
                std::cout << "Ignoring command: " << item.command << '\n';
                break;
        }
    }

    return local_mission;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    mavsdk.set_configuration(Mavsdk::Configuration{1, 34, true}); // MAV_COMP_ID_USER10

    ConnectionResult connection_result_gcs =
        mavsdk.add_any_connection(argv[1], mavsdk::ForwardingOption::ForwardingOn);
    if (connection_result_gcs != ConnectionResult::Success) {
        std::cerr << "Error setting up connection to Mission Manager: " << connection_result_gcs
                  << '\n';
        return 1;
    }

    ConnectionResult connection_result_autopilot =
        mavsdk.add_any_connection(argv[2], mavsdk::ForwardingOption::ForwardingOn);
    if (connection_result_autopilot != ConnectionResult::Success) {
        std::cerr << "Error setting up connection to Autopilot: " << connection_result_autopilot
                  << '\n';
        return 1;
    }

    std::cout
        << "MissionRawServer running, waiting for ground station and autopilot to connect...\n";
    while (mavsdk.systems().size() < 2) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    auto maybe_autopilot = [&]() {
        for (auto system : mavsdk.systems()) {
            if (system->has_autopilot()) {
                return std::make_optional<std::shared_ptr<System>>(system);
            }
        }
        return std::optional<std::shared_ptr<System>>();
    }();

    auto maybe_gcs = [&]() {
        for (auto system : mavsdk.systems()) {
            for (auto component_id : system->component_ids()) {
                // FIXME: we assume the component ID of 190 for QGC
                if (component_id == 190) {
                    return std::make_optional<std::shared_ptr<System>>(system);
                }
            }
        }
        return std::optional<std::shared_ptr<System>>();
    }();

    if (!maybe_autopilot) {
        std::cerr << "Error: no autopilot discovered\n";
        return 1;
    }

    if (!maybe_gcs) {
        std::cerr << "Error: no ground station discovered\n";
        return 1;
    }

    auto autopilot = maybe_autopilot.value();
    auto gcs = maybe_gcs.value();

    if (autopilot == gcs) {
        std::cerr << "Error: autopilot and ground station should not be the same system!\n";
        return 1;
    }

    std::cout << "Ground station and autopilot have connected\n";

    auto telemetry = Telemetry{autopilot};

    std::cout << "Waiting for estimator origin...\n";
    const auto origin = [&]() {
        while (true) {
            const auto estimator_origin = telemetry.get_gps_global_origin();
            if (estimator_origin.first == Telemetry::Result::Success) {
                return estimator_origin.second;
            }
        }
    }();
    std::cout << "Got origin: " << origin << '\n';

    auto prom = std::promise<LocalMission>{};
    auto fut = prom.get_future();

    auto mission_raw_server = MissionRawServer{gcs};
    mission_raw_server.subscribe_incoming_mission(
        [&prom, &origin, &mission_raw_server](
            MissionRawServer::Result result, MissionRawServer::MissionPlan plan) {
            if (result == MissionRawServer::Result::Success && !plan.mission_items.empty() &&
                    std::cout << "Received mission with result: " << result << '\n';
                plan.mission_items.front().mission_type == MAV_MISSION_TYPE_MISSION) {
                mission_raw_server.subscribe_incoming_mission(nullptr);
                prom.set_value(convert_mission_plan(origin, plan));
            }
        });

    std::cout << "Waiting for mission to be uploaded...\n";
    auto local_mission = fut.get();

    std::cout << "Got local mission\n";
    for (const auto& point : local_mission.points) {
        std::cout << point << '\n';
    }

    auto action = Action(autopilot);
    auto offboard = Offboard(autopilot);

    if (!telemetry.armed()) {
        auto arm_result = action.arm();
        if (arm_result != Action::Result::Success) {
            std::cerr << "Error: Could not arm: " << arm_result << '\n';
            return 1;
        }
    }

    if (!telemetry.in_air()) {
        auto takeoff_result = action.takeoff();
        if (takeoff_result != Action::Result::Success) {
            std::cerr << "Error: Could not take off: " << takeoff_result << '\n';
            return 1;
        }
    }
    auto current_position = telemetry.position_velocity_ned().position;
    local_mission.points.insert(
        local_mission.points.begin(),
        Point{current_position.north_m, current_position.east_m, current_position.down_m});

    auto rc = rounded_corners::RoundedCorners{5.0};
    rc.set_points(local_mission.points);
    const auto sampled = rc.sample(0.1);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    offboard.set_position_ned(Offboard::PositionNedYaw{0, 0, -10});
    offboard.start();

    std::cout << "Rounded local mission\n";
    for (const auto& sample : sampled) {
        std::cout << sample << '\n';
        offboard.set_position_ned(Offboard::PositionNedYaw{
            static_cast<float>(sample.x),
            static_cast<float>(sample.y),
            static_cast<float>(sample.z)});
        const auto current_position = telemetry.position_velocity_ned().position;
        // std::cout << "dist: " << std::sqrt(
        //    (sample.x-current_position.north_m)*(sample.x-current_position.north_m) +
        //    (sample.y-current_position.east_m)*(sample.y-current_position.east_m) +
        //    (sample.z-current_position.down_m)*(sample.z-current_position.down_m)) << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
