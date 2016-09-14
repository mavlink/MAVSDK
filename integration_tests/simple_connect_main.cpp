#include <iostream>
#include <unistd.h>
#include "dronelink.h"

#define UNUSED(x) (void)(x)

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    dronelink::DroneLink *dl;
    dl = new dronelink::DroneLink();
    std::cout << "started" << std::endl;

    dronelink::DroneLink::ConnectionResult ret = dl->add_udp_connection("127.0.0.1", 14540);
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }

    ret = dl->add_udp_connection("127.0.0.1", 14550);
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }

    usleep(2000000);

    std::cout << "deleting it" << std::endl;
    delete dl;

    usleep(1000000);

    dl = new dronelink::DroneLink();

    ret = dl->add_udp_connection("127.0.0.1", 14540);
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }
    std::cout << "created and started yet again" << std::endl;
    usleep(1000000);
    delete dl;
    std::cout << "exiting" << std::endl;
    return 0;
}
