#include <iostream>
#include <unistd.h>
#include "dronelink.h"

#define UNUSED(x) (void)(x)

using namespace std::placeholders; // for _1

bool _discovered_device = false;
bool _timeouted_device = false;

void on_discover(uint64_t uuid);
void on_timeout(uint64_t uuid);

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    dronelink::DroneLink dl;

    dronelink::DroneLink::ConnectionResult ret = dl.add_udp_connection();
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
        return -1;
    }

    dl.register_on_discover(std::bind(&on_discover, _1));
    dl.register_on_timeout(std::bind(&on_timeout, _1));

    while (!_discovered_device) {
        std::cout << "waiting for device to appear..." << std::endl;
        usleep(1000000);
    }

    while (!_timeouted_device) {
        std::cout << "waiting for device to disappear..." << std::endl;
        usleep(1000000);
    }

    return 0;
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found device with UUID: " << uuid << std::endl;
    _discovered_device = true;
}

void on_timeout(uint64_t uuid)
{
    std::cout << "Lost device with UUID: " << uuid << std::endl;
    _timeouted_device = true;
}
