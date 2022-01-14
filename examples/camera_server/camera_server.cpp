#include <future>

#include <iostream>
#include <map>
#include <thread>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>
#include <mavsdk/plugins/camera_server/camera_server.h>

/*
 This example runs a MAVLink "camera" utilizing the MAVSDK server plugins
 on a separate thread. This uses two MAVSDK instances, one GCS, one camera.

 The main thread acts as a GCS and reads telemetry, parameters, transmits across
 a mission, clears the mission, arms the vehicle and then triggers a vehicle takeoff.

 The camera thread handles all the servers and triggers callbacks, publishes telemetry,
 handles and stores parameters, prints received missions and sets the vehicle height to 10m on
 successful takeoff request.
*/

using namespace mavsdk;

using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::this_thread::sleep_for;

// map of system id to camera server instance
static std::map<uint8_t, std::shared_ptr<mavsdk::CameraServer>> all_camera_servers{};

void handle_per_system_camera(std::shared_ptr<mavsdk::System> system)
{
    if (all_camera_servers.find(system->get_system_id()) != all_camera_servers.end()) {
        std::cerr << "Unexpected duplicate system ID " << +system->get_system_id() << ", ignoring."
                  << std::endl;
        return;
    }

    // Create server plugin
    auto camera_server = std::make_shared<mavsdk::CameraServer>(system);

    all_camera_servers.insert({system->get_system_id(), camera_server});

    camera_server->set_information({
        .vendor_name = "MAVSDK",
        .model_name = "Example Camera Server",
        .focal_length_mm = 3.0,
        .horizontal_sensor_size_mm = 3.68,
        .vertical_sensor_size_mm = 2.76,
        .horizontal_resolution_px = 3280,
        .vertical_resolution_px = 2464,
    });

    camera_server->set_in_progress(false);

    camera_server->subscribe_take_photo(
        [&camera_server](CameraServer::Result result, int32_t index) {
            camera_server->set_in_progress(true);

            std::cout << "taking a picture..." << std::endl;

            // TODO : actually capture image here
            // simulating with delay
            sleep_for(seconds(1));

            // TODO: populate with telemetry data
            auto position = CameraServer::Position{};
            auto attitude = CameraServer::Quaternion{};

            auto timestamp =
                duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            auto success = true;

            camera_server->set_in_progress(false);

            camera_server->publish_photo({
                .position = position,
                .attitude_quaternion = attitude,
                .time_utc_us = static_cast<uint64_t>(timestamp),
                .is_success = success,
                .index = index,
                .file_url = {},
            });
        });

    std::cout << "Connected to " << (system->is_standalone() ? "GCS" : "autopilot") << " system ID " << +system->get_system_id() << std::endl;
}

int main(int argc, char** argv)
{
    mavsdk::Mavsdk mavsdk;
    mavsdk::Mavsdk::Configuration configuration(mavsdk::Mavsdk::Configuration::UsageType::Camera);
    mavsdk.set_configuration(configuration);

    // 14030 is the default camera port for PX4 SITL
    auto result = mavsdk.add_any_connection("udp://:14030");
    if (result == mavsdk::ConnectionResult::Success) {
        std::cout << "Created camera server connection" << std::endl;
    }

    mavsdk.subscribe_on_new_system([&mavsdk]() {
        // REVISIT: is it safe to assume that this will not miss any systems,
        // e.g. two discovered at the same time?
        auto system = mavsdk.systems().back();

        handle_per_system_camera(system);
    });

    for (auto&& system : mavsdk.systems()) {
        handle_per_system_camera(system);
    }

    while (true) {
        // TODO: better way to wait forever?
        sleep_for(seconds(1));
    }

    return 0;
}