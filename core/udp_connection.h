#pragma once

#include "dronecore.h"
#include "connection.h"
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

namespace dronecore {

class UdpConnection : public Connection
{
public:
    explicit UdpConnection(DroneCoreImpl &parent, int local_port_number, size_t no_of_clients = 1);
    ~UdpConnection();
    bool is_ok() const;
    ConnectionResult start();
    ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

    // Non-copyable
    UdpConnection(const UdpConnection &) = delete;
    const UdpConnection &operator=(const UdpConnection &) = delete;

private:
    ConnectionResult setup_port();
    void start_recv_thread();

    static void receive(UdpConnection *parent);

    int _local_port_number;

    struct RemoteClient {
        std::string ip = {};
        int port = 0;
    };
    std::mutex _remote_mutex = {};
    struct Client {
        uint8_t sysid = 0;
        uint8_t compid = 0;
        std::string ip = {};
        int port = 0;
    };
    std::vector<Client> _clients = {};

    bool is_new(const Client &client) const;
    bool is_new(const std::string &ip) const;
    bool is_new(int port) const;
    bool is_valid(const Client &client) const;
    int find_client(uint8_t sysid, uint8_t compid) const;

    int _socket_fd = -1;
    std::thread *_recv_thread = nullptr;
    std::atomic_bool _should_exit {false};
};

} // namespace dronecore
