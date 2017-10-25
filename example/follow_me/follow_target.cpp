#include <iostream>
#include <cmath>
#include <dronecore/dronecore.h>
#include <thread>
#include <chrono>

using namespace dronecore;

int main(int, char **)
{
    DroneCore dc;

    (void) dc.add_udp_connection();

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    (void) device.action().arm();

    (void) device.action().takeoff();

    std::cout << "[Takeoff] Waiting for 5 secs..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Done." << std::endl;

    (void) device.followme().start();

    std::cout << "[FollowMe] Waiting for 20 secs :-) ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::cout << "Done." << std::endl;
	return 0;
}
