#pragma once

#include "dronecore.h"
#include "connection.h"
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

namespace dronecore {

class UdpConnection : public Connection
{
public:
    explicit UdpConnection(DroneCoreImpl &parent, int local_port_number);
    ~UdpConnection();
    bool is_ok() const;
    ConnectionResult start();
    ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

    // Non-copyable
    UdpConnection(const UdpConnection &) = delete;
    const UdpConnection &operator=(const UdpConnection &) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();

    static void receive(UdpConnection *parent);

    int _local_port_number;

    std::mutex _remote_mutex = {};
    std::string _remote_ip = {};
    int _remote_port_number = 0;

    int _socket_fd = -1;
    std::thread *_recv_thread = nullptr;
    std::atomic_bool _should_exit {false};
};

} // namespace dronecore
