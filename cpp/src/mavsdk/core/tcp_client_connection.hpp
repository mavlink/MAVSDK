#pragma once

#include <array>
#include <atomic>
#include <mutex>
#include <string>

#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>

#include "connection.hpp"

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
    void do_connect();
    void do_receive();
    void start_reconnect();

    std::string _remote_ip;
    int _remote_port_number;

    // Set to true by stop() before cancelling/closing; prevents handlers from re-arming.
    std::atomic<bool> _stopping{false};

    // Protects synchronous sends against concurrent close/reconnect on the io_thread.
    std::mutex _send_mutex{};

    // Asio objects — driven by MavsdkImpl::_io_context.
    asio::ip::tcp::socket _socket;
    asio::steady_timer _reconnect_timer;
    std::array<char, 2048> _recv_buffer{};
};

} // namespace mavsdk
