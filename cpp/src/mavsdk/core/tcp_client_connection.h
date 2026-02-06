#pragma once

#include "socket_holder.h"

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

class TcpClientConnection : public Connection {
public:
    TcpClientConnection(
        Connection::ReceiverCallback receiver_callback,
        Connection::LibmavReceiverCallback libmav_receiver_callback,
        MavsdkImpl& mavsdk_impl,
        std::string remote_ip,
        int remote_port,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    ~TcpClientConnection() override;
    ConnectionResult start() override;
    ConnectionResult stop() override;

    std::pair<bool, std::string> send_message(const mavlink_message_t& message) override;
    std::pair<bool, std::string> send_raw_bytes(const char* bytes, size_t length) override;

    // Non-copyable
    TcpClientConnection(const TcpClientConnection&) = delete;
    const TcpClientConnection& operator=(const TcpClientConnection&) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();
    void receive();

    std::string _remote_ip = {};
    int _remote_port_number;

    std::mutex _mutex = {};
    SocketHolder _socket_fd;

    std::unique_ptr<std::thread> _recv_thread{};
    std::atomic_bool _should_exit;
};

} // namespace mavsdk
