#pragma once

#include <array>
#include <mutex>
#include <string>

#include <asio/serial_port.hpp>

#include "connection.h"

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

#if defined(LINUX)
    static int define_from_baudrate(int baudrate);
#endif

    const std::string _serial_node;
    const int _baudrate;
    const bool _flow_control;

    // Protects synchronous writes against concurrent close on the io_thread.
    std::mutex _send_mutex{};

    // Asio serial port — driven by MavsdkImpl::_io_context.
    asio::serial_port _serial_port;
    std::array<char, 2048> _recv_buffer{};
};

} // namespace mavsdk
