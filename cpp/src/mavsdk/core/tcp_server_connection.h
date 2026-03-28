#pragma once

#include <array>
#include <mutex>
#include <string>

#include <asio/ip/tcp.hpp>

#include "connection.h"

namespace mavsdk {

class TcpServerConnection : public Connection {
public:
    TcpServerConnection(
        Connection::ReceiverCallback receiver_callback,
        Connection::LibmavReceiverCallback libmav_receiver_callback,
        MavsdkImpl& mavsdk_impl,
        std::string local_ip,
        int local_port,
        ForwardingOption forwarding_option);
    ~TcpServerConnection() override;

    ConnectionResult start() override;
    ConnectionResult stop() override;
    std::pair<bool, std::string> send_message(const mavlink_message_t& message) override;
    std::pair<bool, std::string> send_raw_bytes(const char* bytes, size_t length) override;

private:
    void do_accept();
    void do_receive();

    std::string _local_ip;
    int _local_port;

    // Protects synchronous sends against concurrent close on the io_thread.
    std::mutex _send_mutex{};

    // Asio objects — driven by MavsdkImpl::_io_context.
    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _client_socket;
    std::array<char, 2048> _recv_buffer{};
};

} // namespace mavsdk
