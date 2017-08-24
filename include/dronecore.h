#pragma once

#include "device.h"

#include <string>
#include <vector>
#include <functional>

namespace dronecore {

class DroneCoreImpl;

class DroneCore
{
public:
    DroneCore();
    ~DroneCore();

    enum class ConnectionResult {
        SUCCESS = 0,
        TIMEOUT,
        SOCKET_ERROR,
        BIND_ERROR,
        SOCKET_CONNECTION_ERROR,
        CONNECTION_ERROR,
        NOT_IMPLEMENTED,
        DEVICE_NOT_CONNECTED,
        DEVICE_BUSY,
        COMMAND_DENIED,
        DESTINATION_IP_UNKNOWN,
        CONNECTIONS_EXHAUSTED
    };

    static const char *connection_result_str(ConnectionResult);

    ConnectionResult add_udp_connection();
    ConnectionResult add_udp_connection(int local_port_number);
    ConnectionResult add_tcp_connection();
    ConnectionResult add_tcp_connection(std::string ipAddress,int local_port_number);

    const std::vector<uint64_t> &device_uuids() const;
    Device &device() const;
    Device &device(uint64_t uuid) const;

    typedef std::function<void(uint64_t uuid)> event_callback_t;
    void register_on_discover(event_callback_t callback);
    void register_on_timeout(event_callback_t callback);

private:
    DroneCoreImpl *_impl;

    // Non-copyable
    DroneCore(const DroneCore &) = delete;
    const DroneCore &operator=(const DroneCore &) = delete;
};

} // namespace dronecore
