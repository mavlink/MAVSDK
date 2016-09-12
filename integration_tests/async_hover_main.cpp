#include <iostream>
#include <unistd.h>
#include "dronelink.h"

#define UNUSED(x) (void)(x)


void receive_result(dronelink::Result result, void *user);


int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    dronelink::DroneLink dl;

    dronelink::Result ret = dl.add_udp_connection();
    if (ret != dronelink::Result::SUCCESS) {
        std::cout << "failed to add connection: " << result_str(ret) << std::endl;
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

    dl.device(uuid).control().arm_async(&receive_result);

    usleep(500000);

    dl.device(uuid).control().takeoff_async(&receive_result);

    usleep(5000000);

    dl.device(uuid).control().land_async(&receive_result);

    return 0;
}


void receive_result(dronelink::Result result, void *user)
{
    UNUSED(user);
    std::cout << "got result: " << result_str(result) << std::endl;
}
