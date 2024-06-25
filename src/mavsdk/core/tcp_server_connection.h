#pragma once

#include "connection.h"

#include <atomic>
#include <string>
#include <thread>

namespace mavsdk {

class TcpServerConnection : public Connection {
public:
    TcpServerConnection(
        Connection::ReceiverCallback receiver_callback,
        std::string local_ip,
        int local_port,
        ForwardingOption forwarding_option);
    ~TcpServerConnection() override;

    ConnectionResult start() override;
    ConnectionResult stop() override;
    bool send_message(const mavlink_message_t& message) override;

private:
    void accept_client();
    void receive();

    Connection::ReceiverCallback _receiver_callback;
    std::string _local_ip;
    int _local_port;
    int _server_socket_fd{-1};
    int _client_socket_fd{-1};
    std::unique_ptr<std::thread> _accept_receive_thread;
    std::atomic<bool> _should_exit{false};
};

} // namespace mavsdk
