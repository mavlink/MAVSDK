#include "udp_connection.h"
#include "global_include.h"
#include "log.h"

#ifndef WINDOWS
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h> // for close()
#else
#include <winsock2.h>
#include <Ws2tcpip.h> // For InetPton
#undef SOCKET_ERROR // conflicts with ConnectionResult::SOCKET_ERROR
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif

#include <cassert>

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace dronecore {

UdpConnection::UdpConnection(DroneCoreImpl &parent,
                             int local_port_number):
    Connection(parent),
    _local_port_number(local_port_number) {}

UdpConnection::~UdpConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}

bool UdpConnection::is_ok() const
{
    return true;
}

ConnectionResult UdpConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::CONNECTIONS_EXHAUSTED;
    }

    ConnectionResult ret = setup_port();
    if (ret != ConnectionResult::SUCCESS) {
        return ret;
    }

    start_recv_thread();

    return ConnectionResult::SUCCESS;
}

ConnectionResult UdpConnection::setup_port()
{

#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: %d", WSAGetLastError();
        return ConnectionResult::SOCKET_ERROR;
    }
#endif

    _socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_socket_fd < 0) {
        LogErr() << "socket error" << GET_ERROR(errno);
        return ConnectionResult::SOCKET_ERROR;
    }

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_local_port_number);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(_socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) {
        LogErr() << "bind error: " << GET_ERROR(errno);
        return ConnectionResult::BIND_ERROR;
    }

    return ConnectionResult::SUCCESS;
}

void UdpConnection::start_recv_thread()
{
    _recv_thread = new std::thread(receive, this);
}

ConnectionResult UdpConnection::stop()
{
    _should_exit = true;

#ifndef WINDOWS
    // This should interrupt a recv/recvfrom call.
    shutdown(_socket_fd, SHUT_RDWR);

    // But on Mac, closing is also needed to stop blocking recv/recvfrom.
    close(_socket_fd);
#else
    shutdown(_socket_fd, SD_BOTH);

    closesocket(_socket_fd);

    WSACleanup();
#endif

    if (_recv_thread) {
        _recv_thread->join();
        delete _recv_thread;
        _recv_thread = nullptr;
    }

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::SUCCESS;
}

int UdpConnection::find_client(uint8_t sysid, uint8_t compid) const
{
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i].sysid == sysid
            && _clients[i].compid == compid) {
            return i;
        }
    }
    return -1;
}

bool UdpConnection::is_valid(const Client &client) const
{
    if (client.ip.empty()
        || client.port == 0) {
        LogErr() << "Remote IP/Port Unknown! IP: "
                 << client.ip << " Port: " << client.port;
        return false;
    }
    return true;
}

bool UdpConnection::send_message(const mavlink_message_t &message,
                                 uint8_t target_sysid, uint8_t target_compid)
{
    std::vector<struct sockaddr_in> destinations {};
    // In point-to-point MAVLink message, target system ID and component ID exists.
    bool point_to_point = target_sysid && target_compid;

    {
        std::lock_guard<std::mutex> lock(_remote_mutex);

        if (point_to_point) {
            // send to a specific component
            auto cli_idx = find_client(target_sysid, target_compid);
            if (cli_idx < 0) {
                LogErr() << "No such component (SysID: " << target_sysid
                         << ", CompID: " << target_compid << ")";
                return false;
            }
            struct sockaddr_in addr {};
            addr.sin_family = AF_INET;
            inet_pton(AF_INET, _clients[cli_idx].ip.c_str(), &addr.sin_addr.s_addr);
            addr.sin_port = htons(_clients[cli_idx].port);
            destinations.push_back(addr);
        } else {
            // broadcast
            for (auto client : _clients) {
                struct sockaddr_in addr {};
                addr.sin_family = AF_INET;
                inet_pton(AF_INET, client.ip.c_str(), &addr.sin_addr.s_addr);
                addr.sin_port = htons(client.port);
                destinations.push_back(addr);
            }
        }
    }

    if (destinations.size()) {
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

        // TODO: remove this assert again
        assert(buffer_len <= MAVLINK_MAX_PACKET_LEN);

        for (auto dest : destinations) {
            int send_len = sendto(_socket_fd, reinterpret_cast<char *>(buffer), buffer_len, 0,
                                  reinterpret_cast<const sockaddr *>(&dest), sizeof(dest));

            if (send_len != buffer_len) {
                LogErr() << "sendto failure: " << GET_ERROR(errno);
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}


bool UdpConnection::is_new(const Client &client) const
{
    return (_clients.size() == 0		// No clients yet!
            || is_new(client.ip) 		// New System!
            || is_new(client.port));	// New component!
}

bool UdpConnection::is_new(const std::string &ip) const
{
    bool found = false;
    for (auto client : _clients)
        if (client.ip == ip) {
            found = true;
        }

    return !found;
}

bool UdpConnection::is_new(int port) const
{
    bool found = false;
    for (auto client : _clients)
        if (client.port == port) {
            found = true;
        }

    return !found;
}

void UdpConnection::receive(UdpConnection *parent)
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!parent->_should_exit) {

        struct sockaddr_in src_addr = {};
        socklen_t src_addr_len = sizeof(src_addr);
        int recv_len = recvfrom(parent->_socket_fd, buffer, sizeof(buffer), 0,
                                reinterpret_cast<struct sockaddr *>(&src_addr), &src_addr_len);

        if (recv_len == 0) {
            // This can happen when shutdown is called on the socket,
            // therefore we check _should_exit again.
            continue;
        }

        if (recv_len < 0) {
            // This happens on desctruction when close(_socket_fd) is called,
            // therefore be quiet.
            //LogErr() << "recvfrom error: " << GET_ERROR(errno);
            continue;
        }

        parent->_mavlink_receiver->set_new_datagram(buffer, recv_len);
        // Parse a message to get sysid, compid of the sender.
        if (parent->_mavlink_receiver->parse_message()) {
            auto mavlink_msg = parent->_mavlink_receiver->get_last_message();

            std::lock_guard<std::mutex> lock(parent->_remote_mutex);

            Client client;
            client.port = ntohs(src_addr.sin_port);
            client.ip = inet_ntoa(src_addr.sin_addr);
            client.sysid = mavlink_msg.sysid;
            client.compid = mavlink_msg.compid;

            if (parent->is_new(client)) {
                LogInfo() << "New system on: " << client.ip
                          << ":" << client.port;
                // Set IP if we don't know it yet.
                parent->_clients.push_back(client);
            }
        }

        // Parse all mavlink messages in one datagram. Once exhausted, we'll exit while.
        // We've already parsed a message above. So, lets process it before parsing next one.
        do {
            parent->receive_message(parent->_mavlink_receiver->get_last_message());
        } while (parent->_mavlink_receiver->parse_message());
    }
}


} // namespace dronecore
