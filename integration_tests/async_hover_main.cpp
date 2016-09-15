#include <iostream>
#include <unistd.h>
#include "dronelink.h"
#include "action.h"

#define UNUSED(x) (void)(x)


#define MAGIC_NUMBER 42

void receive_result(dronelink::Action::Result result, void *user);


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

    // Wait for device to connect via heartbeat.
    usleep(1500000);

    std::vector<uint64_t> uuids = dl.device_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found device with UUID: " << *it << std::endl;
    }

    if (uuids.size() > 1) {
        std::cout << "found more than one device, not sure which one to use." << std::endl;
        return -1;
    } else if (uuids.size() == 0) {
        std::cout << "no device found." << std::endl;
        return -1;
    }

    uint64_t uuid = uuids.at(0);

    unsigned magic = MAGIC_NUMBER;
    dronelink::Action::CallbackData arm_receive = {&receive_result, &magic};
    dl.device(uuid).action().arm_async(arm_receive);

    usleep(500000);

    dronelink::Action::CallbackData takeoff_receive = {&receive_result, &magic};
    dl.device(uuid).action().takeoff_async(takeoff_receive);

    usleep(5000000);

    dronelink::Action::CallbackData land_receive = {&receive_result, &magic};
    dl.device(uuid).action().land_async(land_receive);

    return 0;
}


void receive_result(dronelink::Action::Result result, void *user)
{
    std::cout << "got result: " << unsigned(result) << std::endl;
    if (user != nullptr) {
        unsigned *magic = reinterpret_cast<unsigned *>(user);
        std::cout << "still have user: " << *magic << std::endl;

        if (*magic != MAGIC_NUMBER) {
            std::cerr << "Error: our number got lost!" << std::endl;
            exit(1);
        }
    }
}
