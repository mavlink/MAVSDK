#pragma once

#include "device.h"

#include <string>
#include <vector>

namespace dronelink {

class DroneLinkImpl;

class DroneLink
{
public:
    DroneLink();
    ~DroneLink();

    enum class ConnectionResult {
        SUCCESS = 0,
        TIMEOUT,
        SOCKET_ERROR,
        BIND_ERROR,
        CONNECTION_ERROR,
        NOT_IMPLEMENTED,
        DEVICE_NOT_CONNECTED,
        DEVICE_BUSY,
        COMMAND_DENIED,
        DESTINATION_IP_UNKNOWN,
        CONNECTIONS_EXHAUSTED
    };

    ConnectionResult add_udp_connection();
    ConnectionResult add_udp_connection(int port_number);
    ConnectionResult add_udp_connection(const std::string &ip);
    ConnectionResult add_udp_connection(const std::string &ip, int port_number);

    const std::vector<uint64_t> &device_uuids() const;
    Device &device(uint64_t uuid) const;

private:
    DroneLinkImpl *_impl;

    // Non-copyable
    DroneLink(const DroneLink &) = delete;
    const DroneLink &operator=(const DroneLink &) = delete;
};

} // namespace dronelink
