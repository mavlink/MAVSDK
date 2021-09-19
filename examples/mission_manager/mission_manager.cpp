//
// Try to implement mission logic on companion computer.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission_raw_server/mission_raw_server.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <gcs connection> <autopilot connection>\n";
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    mavsdk.set_configuration(Mavsdk::Configuration{1, 34, true}); // MAV_COMP_ID_USER10

    ConnectionResult connection_result_gcs = mavsdk.add_any_connection(argv[1], mavsdk::ForwardingOption::ForwardingOn);
    if (connection_result_gcs!= ConnectionResult::Success) {
        std::cerr << "Error setting up connection to Mission Manager: " << connection_result_gcs << '\n';
        return 1;
    }

    ConnectionResult connection_result_autopilot = mavsdk.add_any_connection(argv[2], mavsdk::ForwardingOption::ForwardingOn);
    if (connection_result_autopilot != ConnectionResult::Success) {
        std::cerr << "Error setting up connection to Autopilot: " << connection_result_autopilot << '\n';
        return 1;
    }

    std::cout << "MissionRawServer running, waiting for ground station and autopilot to connect...\n";
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

    auto mission_raw_server = MissionRawServer{gcs};

    mission_raw_server.subscribe_incoming_mission([](MissionRawServer::Result result, MissionRawServer::MissionPlan plan) {
        std::cout << "Received mission with result: " << result << '\n';
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
