#pragma once

#include <array>
#include <atomic>
#include <string>
#include <vector>

#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>

#include "connection.hpp"
#include "tx_queue.hpp"

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
    void start_write();

    std::string _remote_ip;
    int _remote_port_number;

    // Set to true by stop() before cancelling/closing; prevents handlers from re-arming.
    std::atomic<bool> _stopping{false};

    // Whether the socket is currently connected. Maintained by the io thread, read by
    // send_raw_bytes() from any thread so that it can reject a send up front rather than
    // silently queueing for a peer that isn't there.
    std::atomic<bool> _connected{false};

    // Asio objects and the send queue — all only touched on MavsdkImpl::_io_context's
    // thread, which is why none of them need a lock.
    asio::ip::tcp::socket _socket;
    asio::steady_timer _reconnect_timer;
    std::array<char, 2048> _recv_buffer{};
    TxQueue<std::vector<char>> _tx_queue{MAX_TX_QUEUE_ITEMS};
};

} // namespace mavsdk
