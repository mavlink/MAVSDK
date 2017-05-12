#pragma once

#include "dronelink.h"
#include "connection.h"
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

namespace dronelink {

class UdpConnection : public Connection
{
public:
    explicit UdpConnection(DroneLinkImpl *parent, int local_port_number);
    ~UdpConnection();
    bool is_ok() const;
    DroneLink::ConnectionResult start();
    DroneLink::ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

    // Non-copyable
    UdpConnection(const UdpConnection &) = delete;
    const UdpConnection &operator=(const UdpConnection &) = delete;

private:
    DroneLink::ConnectionResult setup_port();
    void start_recv_thread();

    static void receive(UdpConnection *parent);

    // This port is shared with mavros, so either one, this SDK or mavros can be used.
    static constexpr int DEFAULT_UDP_LOCAL_PORT = 14540;

    int _local_port_number;
    std::string _remote_ip = {};
    int _remote_port_number = 0;
    std::mutex _mutex = {};
    int _socket_fd = -1;
    std::thread *_recv_thread = nullptr;
    std::atomic_bool _should_exit;
};

} // namespace dronelink
