#include "udp_connection.h"
#include "log.h"

#ifdef WINDOWS
#include <winsock2.h>
#include <Ws2tcpip.h> // For InetPton
#undef SOCKET_ERROR // conflicts with ConnectionResult::SocketError
#ifndef MINGW
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

#include <algorithm>
#include <utility>

#ifdef WINDOWS
#define GET_ERROR(_x) WSAGetLastError()
#else
#define GET_ERROR(_x) strerror(_x)
#endif

namespace mavsdk {

UdpConnection::UdpConnection(
    Connection::ReceiverCallback receiver_callback,
    std::string local_ip,
    int local_port_number,
    ForwardingOption forwarding_option) :
    Connection(std::move(receiver_callback), forwarding_option),
    _local_ip(std::move(local_ip)),
    _local_port_number(local_port_number)
{}

UdpConnection::~UdpConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}

ConnectionResult UdpConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

    ConnectionResult ret = setup_port();
    if (ret != ConnectionResult::Success) {
        return ret;
    }

    start_recv_thread();

    return ConnectionResult::Success;
}

ConnectionResult UdpConnection::setup_port()
{
#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: %d", WSAGetLastError();
        return ConnectionResult::SocketError;
    }
#endif

    _socket_fd.reset(socket(AF_INET, SOCK_DGRAM, 0));

    if (_socket_fd.empty()) {
        LogErr() << "socket error" << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, _local_ip.c_str(), &(addr.sin_addr));
    addr.sin_port = htons(_local_port_number);

    if (bind(_socket_fd.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        LogErr() << "bind error: " << GET_ERROR(errno);
        return ConnectionResult::BindError;
    }

    return ConnectionResult::Success;
}

void UdpConnection::start_recv_thread()
{
    _recv_thread = std::make_unique<std::thread>(&UdpConnection::receive, this);
}

ConnectionResult UdpConnection::stop()
{
    _should_exit = true;

    _socket_fd.close();

    if (_recv_thread) {
        _recv_thread->join();
        _recv_thread.reset();
    }

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

bool UdpConnection::send_message(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_remote_mutex);

    if (_remotes.size() == 0) {
        LogErr() << "No known remotes";
        return false;
    }

    // Send the message to all the remotes. A remote is a UDP endpoint
    // identified by its <ip, port>. This means that if we have two
    // systems on two different endpoints, then messages directed towards
    // only one system will be sent to both remotes. The systems are
    // then expected to ignore messages that are not directed to them.
    bool send_successful = true;
    for (auto& remote : _remotes) {
        struct sockaddr_in dest_addr {};
        dest_addr.sin_family = AF_INET;

        inet_pton(AF_INET, remote.ip.c_str(), &dest_addr.sin_addr.s_addr);
        dest_addr.sin_port = htons(remote.port_number);

        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

        const auto send_len = sendto(
            _socket_fd.get(),
            reinterpret_cast<char*>(buffer),
            buffer_len,
            0,
            reinterpret_cast<const sockaddr*>(&dest_addr),
            sizeof(dest_addr));

        if (send_len != buffer_len) {
            LogErr() << "sendto failure: " << GET_ERROR(errno);
            send_successful = false;
            continue;
        }
    }

    return send_successful;
}

void UdpConnection::add_remote(const std::string& remote_ip, const int remote_port)
{
    add_remote_with_remote_sysid(remote_ip, remote_port, 0);
}

void UdpConnection::add_remote_with_remote_sysid(
    const std::string& remote_ip, const int remote_port, const uint8_t remote_sysid)
{
    std::lock_guard<std::mutex> lock(_remote_mutex);
    Remote new_remote;
    new_remote.ip = remote_ip;
    new_remote.port_number = remote_port;

    auto existing_remote =
        std::find_if(_remotes.begin(), _remotes.end(), [&new_remote](Remote& remote) {
            return remote == new_remote;
        });

    if (existing_remote == _remotes.end()) {
        // System with sysid 0 is a bit special: it is a placeholder for a connection initiated
        // by MAVSDK. As such, it should not be advertised as a newly discovered system.
        if (static_cast<int>(remote_sysid) != 0) {
            LogInfo() << "New system on: " << new_remote.ip << ":" << new_remote.port_number
                      << " (with system ID: " << static_cast<int>(remote_sysid) << ")";
        }
        _remotes.push_back(new_remote);
    }
}

void UdpConnection::receive()
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!_should_exit) {
        struct sockaddr_in src_addr = {};
        socklen_t src_addr_len = sizeof(src_addr);
        const auto recv_len = recvfrom(
            _socket_fd.get(),
            buffer,
            sizeof(buffer),
            0,
            reinterpret_cast<struct sockaddr*>(&src_addr),
            &src_addr_len);

        if (recv_len == 0) {
            // This can happen when shutdown is called on the socket,
            // therefore we check _should_exit again.
            continue;
        }

        if (recv_len < 0) {
            // This happens on destruction when _socket_fd.close() is called,
            // therefore be quiet.
            // LogErr() << "recvfrom error: " << GET_ERROR(errno);
            continue;
        }

        _mavlink_receiver->set_new_datagram(buffer, static_cast<int>(recv_len));

        // Parse all mavlink messages in one datagram. Once exhausted, we'll exit while.
        while (_mavlink_receiver->parse_message()) {
            const uint8_t sysid = _mavlink_receiver->get_last_message().sysid;

            if (sysid != 0) {
                add_remote_with_remote_sysid(
                    inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port), sysid);
            }

            receive_message(_mavlink_receiver->get_last_message(), this);
        }
    }
}

} // namespace mavsdk
