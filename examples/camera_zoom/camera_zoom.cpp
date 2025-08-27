//
// Example to demonstrate how to switch to photo mode and take a picture.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>
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

        // TODO: filter for camera
        // if (system->has_camera()) {
        std::cout << "Discovered camera\n";

        // Unsubscribe again as we only want to find one system.
        mavsdk.unsubscribe_on_new_system(handle);
        prom.set_value(system);
        //}
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(seconds(5)) == std::future_status::timeout) {
        std::cerr << "No camera found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
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

    // Zoom in
    auto result = camera.zoom_range(component_id, 2.0f);
    if (result != Camera::Result::Success) {
        std::cerr << "Zooming failed: " << result << '\n';
        return 1;
    }

    // Wait a bit
    sleep_for(seconds(4));

    // Now try continuous zooming in.
    result = camera.zoom_in_start(component_id);
    if (result != Camera::Result::Success) {
        std::cerr << "Zooming in failed: " << result << '\n';
        return 1;
    }

    sleep_for(seconds(1));

    // Now try stopping.
    result = camera.zoom_stop(component_id);
    if (result != Camera::Result::Success) {
        std::cerr << "Stop zooming failed: " << result << '\n';
        return 1;
    }

    sleep_for(seconds(1));

    // And back out.
    result = camera.zoom_out_start(component_id);
    if (result != Camera::Result::Success) {
        std::cerr << "Zooming out failed: " << result << '\n';
        return 1;
    }

    sleep_for(seconds(1));

    // Stop again.
    result = camera.zoom_stop(component_id);
    if (result != Camera::Result::Success) {
        std::cerr << "Stop zooming failed: " << result << '\n';
        return 1;
    }

    sleep_for(seconds(1));

    // Zoom back out
    result = camera.zoom_range(component_id, 1.0f);
    if (result != Camera::Result::Success) {
        std::cerr << "Zooming failed: " << result << '\n';
        return 1;
    }

    return 0;
}
