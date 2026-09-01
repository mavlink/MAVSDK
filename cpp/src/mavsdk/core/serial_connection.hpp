#pragma once

#include <array>
#include <atomic>
#include <string>
#include <vector>

#include <asio/serial_port.hpp>

#include "connection.hpp"
#include "tx_queue.hpp"

namespace mavsdk {

class SerialConnection : public Connection {
public:
    explicit SerialConnection(
        Connection::ReceiverCallback receiver_callback,
        Connection::LibmavReceiverCallback libmav_receiver_callback,
        MavsdkImpl& mavsdk_impl,
        std::string path,
        int baudrate,
        bool flow_control,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    ConnectionResult start() override;
    ConnectionResult stop() override;
    ~SerialConnection() override;

    std::pair<bool, std::string> send_message(const mavlink_message_t& message) override;
    std::pair<bool, std::string> send_raw_bytes(const char* bytes, size_t length) override;

    // Non-copyable
    SerialConnection(const SerialConnection&) = delete;
    const SerialConnection& operator=(const SerialConnection&) = delete;

private:
    ConnectionResult setup_port();
    void do_receive();
    void start_write();

#if defined(LINUX)
    static int define_from_baudrate(int baudrate);
#endif

    const std::string _serial_node;
    const int _baudrate;
    const bool _flow_control;

    // Set to true by stop() before closing; prevents handlers from re-arming.
    std::atomic<bool> _stopping{false};

    // Whether the port is currently open. Maintained alongside the port itself, read by
    // send_raw_bytes() from any thread so it can reject a send up front.
    std::atomic<bool> _port_open{false};

    // Asio serial port and the send queue — all only touched on MavsdkImpl::_io_context's
    // thread, which is why neither needs a lock.
    asio::serial_port _serial_port;
    std::array<char, 2048> _recv_buffer{};
    TxQueue<std::vector<char>> _tx_queue{MAX_TX_QUEUE_ITEMS};
};

} // namespace mavsdk
