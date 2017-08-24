#pragma once

#include "dronecore.h"
#include "dronecore_impl.h"
#include "connection.h"
#include<sys/types.h>
#include<netdb.h>

namespace dronecore {

class TcpConnection : public Connection
{
public:
    explicit TcpConnection(DroneCoreImpl *parent, const std::string &ip, int port_number);
    ~TcpConnection();
    bool is_ok() const;
    DroneCore::ConnectionResult start();
    DroneCore::ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

    // Non-copyable
    TcpConnection(const TcpConnection &) = delete;
    const TcpConnection &operator=(const TcpConnection &) = delete;

private:
    DroneCore::ConnectionResult setup_port();
    void start_recv_thread();
    void resolve_address(const std::string& ipAddr, int port, sockaddr_in* addr);
    static void receive(TcpConnection *parent);

    static constexpr int DEFAULT_TCP_LOCAL_PORT = 5760;
    int _local_port_number = 14580; //unused port
    std::string _remote_ip = {};
    int _remote_port_number;
    
    std::mutex _mutex = {};
    int _socket_fd = -1;
    std::thread *_recv_thread = nullptr;
    std::atomic_bool _should_exit;
};

} // namespace dronecore
