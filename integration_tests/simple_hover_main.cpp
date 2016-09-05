#include <iostream>
#include <unistd.h>
#include <dronelink/dronelink.h>

#define UNUSED(x) (void)(x)

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    dronelink::DroneLink dl;

    dronelink::Result ret = dl.add_udp_connection();
    if (ret != dronelink::Result::SUCCESS) {
        std::cout << "failed to add connection: " << result_str(ret) << std::endl;
    }

    // Wait for device to connect via heartbeat.
    usleep(1000000);

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

    ret = dl.device(uuid).arm();
    if (ret != dronelink::Result::SUCCESS) {
        std::cout << "failed to arm: " << result_str(ret) << std::endl;
    }

    usleep(500000);

    ret = dl.device(uuid).takeoff();
    if (ret != dronelink::Result::SUCCESS) {
        std::cout << "failed to takeoff: " << result_str(ret) << std::endl;
    }

    usleep(5000000);

    ret = dl.device(uuid).land();
    if (ret != dronelink::Result::SUCCESS) {
        std::cout << "failed to land: " << result_str(ret) << std::endl;
    }

    // TODO: we need a check for is_landed() before calling disarm.
    //usleep(1000000);
    //ret = dl.device(1).disarm();
    //if (ret != dronelink::Result::SUCCESS) {
    //    std::cout << "failed to disarm: " << result_str(ret) << std::endl;
    //}

    return 0;
}
