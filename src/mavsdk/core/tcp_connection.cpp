#include "tcp_connection.h"
#include "log.h"

#ifdef WINDOWS
#ifndef MINGW
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h> // for close()
#endif

#include <cassert>
#include <utility>

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace mavsdk {

/* change to remote_ip and remote_port */
TcpConnection::TcpConnection(
    Connection::ReceiverCallback receiver_callback,
    std::string remote_ip,
    int remote_port,
    ForwardingOption forwarding_option) :
    Connection(std::move(receiver_callback), forwarding_option),
    _remote_ip(std::move(remote_ip)),
    _remote_port_number(remote_port),
    _should_exit(false)
{}

TcpConnection::~TcpConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}

ConnectionResult TcpConnection::start()
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

ConnectionResult TcpConnection::setup_port()
{
#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: %d", WSAGetLastError();
        _is_ok = false;
        return ConnectionResult::SocketError;
    }
#endif

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_socket_fd < 0) {
        LogErr() << "socket error" << GET_ERROR(errno);
        _is_ok = false;
        return ConnectionResult::SocketError;
    }

    struct sockaddr_in remote_addr {};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(_remote_port_number);

    struct hostent* hp;
    hp = gethostbyname(_remote_ip.c_str());
    if (hp == nullptr) {
        LogErr() << "Could not get host by name";
        _is_ok = false;
        return ConnectionResult::SocketConnectionError;
    }

    memcpy(&remote_addr.sin_addr, hp->h_addr, hp->h_length);

    if (connect(_socket_fd, reinterpret_cast<sockaddr*>(&remote_addr), sizeof(struct sockaddr_in)) <
        0) {
        LogErr() << "connect error: " << GET_ERROR(errno);
        _is_ok = false;
        return ConnectionResult::SocketConnectionError;
    }

    _is_ok = true;
    return ConnectionResult::Success;
}

void TcpConnection::start_recv_thread()
{
    _recv_thread = std::make_unique<std::thread>(&TcpConnection::receive, this);
}

ConnectionResult TcpConnection::stop()
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
        _recv_thread.reset();
    }

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

bool TcpConnection::send_message(const mavlink_message_t& message)
{
    if (!_is_ok) {
        return false;
    }

    if (_remote_ip.empty()) {
        LogErr() << "Remote IP unknown";
        return false;
    }

    if (_remote_port_number == 0) {
        LogErr() << "Remote port unknown";
        return false;
    }

    struct sockaddr_in dest_addr {};
    dest_addr.sin_family = AF_INET;

    inet_pton(AF_INET, _remote_ip.c_str(), &dest_addr.sin_addr.s_addr);

    dest_addr.sin_port = htons(_remote_port_number);

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    // TODO: remove this assert again
    assert(buffer_len <= MAVLINK_MAX_PACKET_LEN);

#if !defined(MSG_NOSIGNAL)
    auto flags = 0;
#else
    auto flags = MSG_NOSIGNAL;
#endif

    const auto send_len = sendto(
        _socket_fd,
        reinterpret_cast<char*>(buffer),
        buffer_len,
        flags,
        reinterpret_cast<const sockaddr*>(&dest_addr),
        sizeof(dest_addr));

    if (send_len != buffer_len) {
        LogErr() << "sendto failure: " << GET_ERROR(errno);
        _is_ok = false;
        return false;
    }
    return true;
}

void TcpConnection::receive()
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!_should_exit) {
        if (!_is_ok) {
            LogErr() << "TCP receive error, trying to reconnect...";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            setup_port();
        }

        const auto recv_len = recv(_socket_fd, buffer, sizeof(buffer), 0);

        if (recv_len == 0) {
            // This can happen when shutdown is called on the socket,
            // therefore we check _should_exit again.
            _is_ok = false;
            continue;
        }

        if (recv_len < 0) {
            // This happens on desctruction when close(_socket_fd) is called,
            // therefore be quiet.
            // LogErr() << "recvfrom error: " << GET_ERROR(errno);
            // Something went wrong, we should try to re-connect in next iteration.
            _is_ok = false;
            continue;
        }

        _mavlink_receiver->set_new_datagram(buffer, static_cast<int>(recv_len));

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (_mavlink_receiver->parse_message()) {
            receive_message(_mavlink_receiver->get_last_message(), this);
        }
    }
}

} // namespace mavsdk
