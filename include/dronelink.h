#pragma once

#include "error_handling.h"
#include "device.h"

#include <string>
#include <vector>

namespace dronelink {

class DroneLinkImpl;

class DroneLink {
public:
    DroneLink();
    ~DroneLink();

    Result add_udp_connection();
    Result add_udp_connection(int port_number);
    Result add_udp_connection(const std::string &ip);
    Result add_udp_connection(const std::string &ip, int port_number);

    const std::vector<uint64_t> &device_uuids() const;
    Device &device(uint64_t uuid) const;

private:
    DroneLinkImpl *_impl;

    // Non-copyable
    DroneLink(const DroneLink &) = delete;
    const DroneLink &operator=(const DroneLink &) = delete;
};

} // namespace dronelink
