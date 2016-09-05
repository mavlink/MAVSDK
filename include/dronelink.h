#pragma once

#include "error_handling.h"
#include "device.h"
#include "info.h"

#include <string>
#include <vector>

namespace dronelink {

class DroneLinkImpl;

class DroneLink {
public:
    DroneLink();
    ~DroneLink();

    Result add_serial_connection(const std::string &path, int baudrate);

    Result add_udp_connection();
    Result add_udp_connection(int port_number);
    Result add_udp_connection(const std::string &ip);
    Result add_udp_connection(const std::string &ip, int port_number);

    Result add_tcp_connection(const std::string &ip, int port_number);

    const std::vector<uint64_t> &device_uuids() const;
    Device &device(uint64_t uuid) const;

    //Result list_connections(std::vector<Connection&> connections);
    //Result remove_connection(Connection&);
    //const std::vector<Vehicle *> &get_vehicles() const;

private:
    DroneLinkImpl *_impl;

    // Non-copyable
    DroneLink(const DroneLink &) = delete;
    const DroneLink &operator=(const DroneLink &) = delete;
};

} // namespace dronelink
