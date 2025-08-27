#include "tcp_server_connection.h"
#include "log.h"

#include <cassert>
#include <fcntl.h>
#include <sstream>

#ifdef WINDOWS
#ifndef MINGW
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif
#include <Windows.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#endif

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace mavsdk {
TcpServerConnection::TcpServerConnection(
    Connection::ReceiverCallback receiver_callback,
    Connection::LibmavReceiverCallback libmav_receiver_callback,
    MavsdkImpl& mavsdk_impl,
    std::string local_ip,
    int local_port,
    ForwardingOption forwarding_option) :
    Connection(
        std::move(receiver_callback),
        std::move(libmav_receiver_callback),
        mavsdk_impl,
        forwarding_option),
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

    if (!start_libmav_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: " << WSAGetLastError();
        return ConnectionResult::SocketError;
    }
#endif

    _server_socket_fd.reset(socket(AF_INET, SOCK_STREAM, 0));
    if (_server_socket_fd.empty()) {
        LogErr() << "socket error: " << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_local_port);

    if (bind(
            _server_socket_fd.get(),
            reinterpret_cast<sockaddr*>(&server_addr),
            sizeof(server_addr)) < 0) {
        LogErr() << "bind error: " << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    if (listen(_server_socket_fd.get(), 3) < 0) {
        LogErr() << "listen error: " << GET_ERROR(errno);
        return ConnectionResult::SocketError;
    }

    // Set receive timeout cross-platform
    const unsigned timeout_ms = 500;

#if defined(WINDOWS)
    setsockopt(
        _server_socket_fd.get(),
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char*)&timeout_ms,
        sizeof(timeout_ms));
    setsockopt(
        _client_socket_fd.get(),
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char*)&timeout_ms,
        sizeof(timeout_ms));
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    setsockopt(_server_socket_fd.get(), SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(tv));
    setsockopt(_client_socket_fd.get(), SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(tv));
#endif

    _accept_receive_thread =
        std::make_unique<std::thread>(&TcpServerConnection::accept_client, this);

    return ConnectionResult::Success;
}

ConnectionResult TcpServerConnection::stop()
{
    _should_exit = true;

    if (_accept_receive_thread && _accept_receive_thread->joinable()) {
        _accept_receive_thread->join();
        _accept_receive_thread.reset();
    }

    _client_socket_fd.close();
    _server_socket_fd.close();

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

std::pair<bool, std::string> TcpServerConnection::send_message(const mavlink_message_t& message)
{
    // Convert message to raw bytes and use common send path
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    assert(buffer_len <= MAVLINK_MAX_PACKET_LEN);

    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

void TcpServerConnection::accept_client()
{
#ifdef WINDOWS
    // Set server socket to non-blocking
    u_long iMode = 1;
    int iResult = ioctlsocket(_server_socket_fd.get(), FIONBIO, &iMode);
    if (iResult != 0) {
        LogErr() << "ioctlsocket failed with error: " << WSAGetLastError();
    }
#else
    // Set server socket to non-blocking
    int flags = fcntl(_server_socket_fd.get(), F_GETFL, 0);
    fcntl(_server_socket_fd.get(), F_SETFL, flags | O_NONBLOCK);
#endif

    while (!_should_exit) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(_server_socket_fd.get(), &readfds);

        // Set timeout to 1 second
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        const int activity =
            select(_server_socket_fd.get() + 1, &readfds, nullptr, nullptr, &timeout);

        if (activity < 0 && errno != EINTR) {
            LogErr() << "select error: " << GET_ERROR(errno);
            continue;
        }

        if (activity == 0) {
            // Timeout, no incoming connection
            continue;
        }

        if (FD_ISSET(_server_socket_fd.get(), &readfds)) {
            sockaddr_in client_addr{};
            socklen_t client_addr_len = sizeof(client_addr);

            _client_socket_fd.reset(accept(
                _server_socket_fd.get(),
                reinterpret_cast<sockaddr*>(&client_addr),
                &client_addr_len));
            if (_client_socket_fd.empty()) {
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

    bool dataReceived = false;
    while (!dataReceived && !_should_exit) {
        const auto recv_len = recv(_client_socket_fd.get(), buffer.data(), buffer.size(), 0);

#ifdef WINDOWS
        if (recv_len == SOCKET_ERROR) {
            // On Windows, on the first try, select says there is something
            // but recv doesn't succeed yet, and we just need to try again.
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            // And at the end, we get an abort that we can silently ignore.
            if (WSAGetLastError() == WSAECONNABORTED) {
                return;
            }
        }
#else
        if (recv_len < 0) {
            // On macOS we presumably see the same thing, and have to try again.
            if (errno == EAGAIN) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            LogErr() << "recv failed: " << GET_ERROR(errno);
            return;
        }
#endif

        if (recv_len == 0) {
            continue;
        }

        _mavlink_receiver->set_new_datagram(buffer.data(), static_cast<int>(recv_len));

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (_mavlink_receiver->parse_message()) {
            receive_message(_mavlink_receiver->get_last_message(), this);
        }

        // Also parse with libmav if available
        if (_libmav_receiver) {
            _libmav_receiver->set_new_datagram(buffer.data(), static_cast<int>(recv_len));

            while (_libmav_receiver->parse_message()) {
                receive_libmav_message(_libmav_receiver->get_last_message(), this);
            }
        }
    }
}

std::pair<bool, std::string> TcpServerConnection::send_raw_bytes(const char* bytes, size_t length)
{
    // Basic implementation for TCP server connections
    std::pair<bool, std::string> result;

#if !defined(MSG_NOSIGNAL)
    auto flags = 0;
#else
    auto flags = MSG_NOSIGNAL;
#endif

    const auto send_len = send(_client_socket_fd.get(), bytes, length, flags);

    if (send_len != static_cast<std::remove_cv_t<decltype(send_len)>>(length)) {
        std::stringstream ss;
        ss << "Send failure: " << GET_ERROR(errno);
        LogErr() << ss.str();
        result.first = false;
        result.second = ss.str();
        return result;
    }

    result.first = true;
    return result;
}

} // namespace mavsdk
