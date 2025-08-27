//
// Example to demonstrate how to switch to photo mode and take a picture.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(std::string bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udp://<our_ip>:<port>\n"
              << " For UDP client: udp://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has a
    // camera, we decide to use it.
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        if (system->has_camera()) {
            std::cout << "Discovered camera\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No camera found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
    auto telemetry = Telemetry{system};
    auto camera = Camera{system};

    // Wait for camera to be discovered.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // We expect to find one camera.
    if (camera.camera_list().cameras.size() == 0) {
        std::cerr << "No camera found, exiting.\n";
        return 1;
    }

    if (camera.camera_list().cameras.size() > 1) {
        std::cout << "More than one camera found, using first one discovered.\n";
    }
    const auto component_id = camera.camera_list().cameras[0].component_id;

    // First, make sure camera is in photo mode.
    const auto mode_result = camera.set_mode(component_id, Camera::Mode::Photo);
    if (mode_result != Camera::Result::Success) {
        std::cerr << "Could not switch to Photo mode: " << mode_result;
        return 1;
    }

    // We want to subscribe to information about pictures that are taken.
    camera.subscribe_capture_info([](Camera::CaptureInfo capture_info) {
        std::cout << "Image captured, stored at: " << capture_info.file_url << '\n';
    });

    const auto photo_result = camera.take_photo(component_id);
    if (photo_result != Camera::Result::Success) {
        std::cerr << "Taking Photo failed: " << mode_result;
        return 1;
    }

    // Wait a bit to make sure we see capture information.
    sleep_for(seconds(2));

    return 0;
}
