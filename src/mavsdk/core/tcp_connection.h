#pragma once

#include <atomic>
#include <mutex>
#include <memory>
#include <thread>
#include "connection.h"
#include <sys/types.h>
#ifndef WINDOWS
#include <netdb.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#include <Ws2tcpip.h> // For InetPton
#undef SOCKET_ERROR
#endif

namespace mavsdk {

class TcpConnection : public Connection {
public:
    explicit TcpConnection(
        Connection::ReceiverCallback receiver_callback,
        std::string remote_ip,
        int remote_port,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    ~TcpConnection() override;
    ConnectionResult start() override;
    ConnectionResult stop() override;

    bool send_message(const mavlink_message_t& message) override;

    // Non-copyable
    TcpConnection(const TcpConnection&) = delete;
    const TcpConnection& operator=(const TcpConnection&) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();
    void receive();

    std::string _remote_ip = {};
    int _remote_port_number;

    std::mutex _mutex = {};
    int _socket_fd = -1;

    std::unique_ptr<std::thread> _recv_thread{};
    std::atomic_bool _should_exit;
    std::atomic_bool _is_ok{false};
};

} // namespace mavsdk
