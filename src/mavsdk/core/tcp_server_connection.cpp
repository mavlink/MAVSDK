#include "tcp_server_connection.h"
#include "log.h"

#include <cassert>
#include <fcntl.h>

#ifdef WINDOWS
#ifndef MINGW
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif
#else
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h> // for close()
#endif

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace mavsdk {
TcpServerConnection::TcpServerConnection(
    Connection::ReceiverCallback receiver_callback,
    std::string local_ip,
    int local_port,
    ForwardingOption forwarding_option) :
    Connection(std::move(receiver_callback), forwarding_option),
    _local_ip(std::move(local_ip)),
    _local_port(local_port)
{}

TcpServerConnection::~TcpServerConnection()
{
    stop();
}

ConnectionResult TcpServerConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: " << WSAGetLastError();
        return ConnectionResult::SocketError;
    }
#endif

    _server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket_fd < 0) {
        LogErr() << "socket error: " << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_local_port);

    if (bind(_server_socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) <
        0) {
        LogErr() << "bind error: " << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    if (listen(_server_socket_fd, 3) < 0) {
        LogErr() << "listen error: " << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    _accept_receive_thread =
        std::make_unique<std::thread>(&TcpServerConnection::accept_client, this);

    return ConnectionResult::Success;
}

ConnectionResult TcpServerConnection::stop()
{
    _should_exit = true;

#ifndef WINDOWS
    shutdown(_client_socket_fd, SHUT_RDWR);
    close(_client_socket_fd);
    close(_server_socket_fd);
#else
    shutdown(_client_socket_fd, SD_BOTH);
    closesocket(_client_socket_fd);
    closesocket(_server_socket_fd);
    WSACleanup();
#endif

    if (_accept_receive_thread && _accept_receive_thread->joinable()) {
        _accept_receive_thread->join();
        _accept_receive_thread.reset();
    }

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

bool TcpServerConnection::send_message(const mavlink_message_t& message)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    assert(buffer_len <= MAVLINK_MAX_PACKET_LEN);

#if !defined(MSG_NOSIGNAL)
    auto flags = 0;
#else
    auto flags = MSG_NOSIGNAL;
#endif

    const auto send_len = send(_client_socket_fd, buffer, buffer_len, flags);

    if (send_len != buffer_len) {
        LogErr() << "send failure: " << GET_ERROR(errno);
        return false;
    }
    return true;
}

void TcpServerConnection::accept_client()
{
    // Set server socket to non-blocking
    int flags = fcntl(_server_socket_fd, F_GETFL, 0);
    fcntl(_server_socket_fd, F_SETFL, flags | O_NONBLOCK);

    while (!_should_exit) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(_server_socket_fd, &readfds);

        // Set timeout to 1 second
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        const int activity = select(_server_socket_fd + 1, &readfds, nullptr, nullptr, &timeout);

        if (activity < 0 && errno != EINTR) {
            LogErr() << "select error: " << GET_ERROR(errno);
            continue;
        }

        if (activity == 0) {
            // Timeout, no incoming connection
            continue;
        }

        if (FD_ISSET(_server_socket_fd, &readfds)) {
            sockaddr_in client_addr{};
            socklen_t client_addr_len = sizeof(client_addr);

            _client_socket_fd = accept(
                _server_socket_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
            if (_client_socket_fd < 0) {
                if (_should_exit) {
                    return;
                }
                LogErr() << "accept error: " << GET_ERROR(errno);
                continue;
            }

            receive();
        }
    }
}

void TcpServerConnection::receive()
{
    std::array<char, 2048> buffer{};

    while (!_should_exit) {
        const auto recv_len = recv(_client_socket_fd, buffer.data(), buffer.size(), 0);

        if (recv_len == 0) {
            continue;
        }

        if (recv_len < 0) {
            LogErr() << "recv failed: " << GET_ERROR(errno);
            return;
        }

        _mavlink_receiver->set_new_datagram(buffer.data(), static_cast<int>(recv_len));

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (_mavlink_receiver->parse_message()) {
            receive_message(_mavlink_receiver->get_last_message(), this);
        }
    }
}

} // namespace mavsdk
