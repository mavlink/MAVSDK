#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <cstdint>

#include "connection.h"
#include "socket_holder.h"

namespace mavsdk {

class UdpConnection : public Connection {
public:
    explicit UdpConnection(
        Connection::ReceiverCallback receiver_callback,
        std::string local_ip,
        int local_port,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    ~UdpConnection() override;
    ConnectionResult start() override;
    ConnectionResult stop() override;

    bool send_message(const mavlink_message_t& message) override;

    void add_remote(const std::string& remote_ip, int remote_port);

    // Non-copyable
    UdpConnection(const UdpConnection&) = delete;
    const UdpConnection& operator=(const UdpConnection&) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();

    void receive();

    void add_remote_with_remote_sysid(
        const std::string& remote_ip, int remote_port, uint8_t remote_sysid);

    std::string _local_ip;
    int _local_port_number;

    std::mutex _remote_mutex{};
    struct Remote {
        std::string ip{};
        int port_number{0};

        bool operator==(const UdpConnection::Remote& other) const
        {
            return ip == other.ip && port_number == other.port_number;
        }
    };
    std::vector<Remote> _remotes{};

    SocketHolder _socket_fd;
    std::unique_ptr<std::thread> _recv_thread{};
    std::atomic_bool _should_exit{false};
};

} // namespace mavsdk
