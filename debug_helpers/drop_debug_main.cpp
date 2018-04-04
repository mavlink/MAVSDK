#include <iostream>
#include <unistd.h>
#include "dronecore.h"
#include <thread>

#if DROP_DEBUG != 1
#error DROB_DEBUG needs to be set for this test to work
#endif

#define UNUSED(x) (void)(x)

bool _discovered_system = false;
bool _timeouted_system = false;

void on_discover(uint64_t uuid);
void on_timeout(uint64_t uuid);

int main(int argc, const char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    dronecore::DroneCore dc;

    dronecore::ConnectionResult ret = dc.add_udp_connection();
    if (ret != dronecore::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
        return -1;
    }

    dc.register_on_discover(std::bind(&on_discover, std::placeholders::_1));
    dc.register_on_timeout(std::bind(&on_timeout, std::placeholders::_1));

    while (true) {
        if (!_discovered_system) {
            std::cout << "waiting for system to appear..." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found system with UUID: " << uuid << std::endl;
    _discovered_system = true;
}

void on_timeout(uint64_t uuid)
{
    std::cout << "Lost system with UUID: " << uuid << std::endl;
    _timeouted_system = true;
}

