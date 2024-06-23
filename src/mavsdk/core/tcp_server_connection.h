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
    ~TcpServerConnection();

    ConnectionResult start();
    ConnectionResult stop();
    bool send_message(const mavlink_message_t& message);

private:
    Connection::ReceiverCallback _receiver_callback;
    std::string _local_ip;
    int _local_port;
    int _server_socket_fd;
    int _client_socket_fd;
    std::unique_ptr<std::thread> _accept_thread;
    std::unique_ptr<std::thread> _recv_thread;
    std::atomic<bool> _should_exit;
    std::atomic<bool> _is_ok;

    void accept_client();
    void receive();
};

} // namespace mavsdk
