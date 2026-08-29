#pragma once

#include <array>
#include <atomic>
#include <string>
#include <vector>

#include <asio/ip/tcp.hpp>

#include "connection.hpp"
#include "tx_queue.hpp"

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
    void start_write();

    std::string _local_ip;
    int _local_port;

    // Set to true by stop() before closing sockets; prevents handlers from re-arming.
    std::atomic<bool> _stopping{false};

    // Whether a client is currently attached. Maintained by the io thread, read by
    // send_raw_bytes() from any thread so it can reject a send up front rather than
    // silently queueing for a client that isn't there.
    std::atomic<bool> _client_connected{false};

    // Asio objects and the send queue — all only touched on MavsdkImpl::_io_context's
    // thread, which is why none of them need a lock.
    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _client_socket;
    std::array<char, 2048> _recv_buffer{};
    TxQueue<std::vector<char>> _tx_queue{MAX_TX_QUEUE_ITEMS};
};

} // namespace mavsdk
