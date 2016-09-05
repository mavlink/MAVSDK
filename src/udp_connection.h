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
    explicit UdpConnection(DroneLinkImpl *parent, const std::string &ip, int port_number);
    ~UdpConnection();
    bool is_ok() const;
    Result start();
    Result stop();

    Result send_message(const mavlink_message_t &message);

    // Non-copyable
    UdpConnection(const UdpConnection &) = delete;
    const UdpConnection &operator=(const UdpConnection &) = delete;

private:
    Result setup_port();
    Result start_recv_thread();

    static void receive(UdpConnection *parent);

    std::string _ip;
    int _port_number;
    std::mutex _mutex;
    int _socket_fd;
    std::thread *_recv_thread;
    std::atomic_bool _should_exit;
};

} // namespace dronelink
