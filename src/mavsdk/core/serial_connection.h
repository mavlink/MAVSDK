#pragma once

#if defined(WINDOWS)
#include <windows.h>
#endif

#include <mutex>
#include <memory>
#include <atomic>
#include <thread>
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
    void start_recv_thread();
    void receive();

#if defined(LINUX)
    static int define_from_baudrate(int baudrate);
#endif

    const std::string _serial_node;
    const int _baudrate;
    const bool _flow_control;

    std::mutex _mutex = {};
#if !defined(WINDOWS)
    int _fd = -1;
#else
    HANDLE _handle;
#endif

    std::unique_ptr<std::thread> _recv_thread{};
    std::atomic_bool _should_exit{false};
};

} // namespace mavsdk
