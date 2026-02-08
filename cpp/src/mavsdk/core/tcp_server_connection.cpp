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
        LogErr() << "Error: Winsock failed, error: " << get_socket_error_string(WSAGetLastError());
        return ConnectionResult::SocketError;
    }
#endif

    std::lock_guard<std::mutex> lock(_mutex);

    _server_socket_fd.reset(socket(AF_INET, SOCK_STREAM, 0));
    if (_server_socket_fd.empty()) {
        LogErr() << "socket error: " << strerror(errno);
        return ConnectionResult::SocketError;
    }

    // Allow reuse of address to avoid "Address already in use" errors
    int yes = 1;
#ifdef WINDOWS
    setsockopt(_server_socket_fd.get(), SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
#else
    setsockopt(_server_socket_fd.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#endif

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_local_port);

    if (bind(
            _server_socket_fd.get(),
            reinterpret_cast<sockaddr*>(&server_addr),
            sizeof(server_addr)) < 0) {
        LogErr() << "bind error: " << strerror(errno);
        return ConnectionResult::SocketError;
    }

    if (listen(_server_socket_fd.get(), 3) < 0) {
        LogErr() << "listen error: " << strerror(errno);
        return ConnectionResult::SocketError;
    }

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

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _client_socket_fd.close();
        _server_socket_fd.close();
    }

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
    // Get server socket fd with mutex protection for setup
    SocketHolder::DescriptorType server_socket_fd;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        server_socket_fd = _server_socket_fd.get();
    }

#ifdef WINDOWS
    // Set server socket to non-blocking
    u_long iMode = 1;
    int iResult = ioctlsocket(server_socket_fd, FIONBIO, &iMode);
    if (iResult != 0) {
        LogErr() << "ioctlsocket failed with error: " << get_socket_error_string(WSAGetLastError());
    }
#else
    // Set server socket to non-blocking
    int flags = fcntl(server_socket_fd, F_GETFL, 0);
    fcntl(server_socket_fd, F_SETFL, flags | O_NONBLOCK);
#endif

    while (!_should_exit) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_socket_fd, &readfds);

        // Set timeout to 1 second
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        const int activity = select(server_socket_fd + 1, &readfds, nullptr, nullptr, &timeout);

        if (activity < 0 && errno != EINTR) {
            LogErr() << "select error: " << strerror(errno);
            continue;
        }

        if (activity == 0) {
            // Timeout, no incoming connection
            continue;
        }

        if (FD_ISSET(server_socket_fd, &readfds)) {
            sockaddr_in client_addr{};
            socklen_t client_addr_len = sizeof(client_addr);

            int new_fd = accept(
                server_socket_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);

            if (new_fd < 0) {
                if (_should_exit) {
                    return;
                }
                LogErr() << "accept error: " << strerror(errno);
                continue;
            }

            // Set receive timeout on client socket
            const unsigned timeout_ms = 500;
#if defined(WINDOWS)
            setsockopt(
                new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
#else
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = timeout_ms * 1000;
            setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(tv));
#endif

            // Now store the new client socket with mutex protection
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _client_socket_fd.reset(new_fd);
            }

            receive();
        }
    }
}

void TcpServerConnection::receive()
{
    std::array<char, 2048> buffer{};

    // Get client socket fd with mutex protection, then release mutex before blocking recv
    SocketHolder::DescriptorType client_socket_fd;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_client_socket_fd.empty()) {
            return;
        }
        client_socket_fd = _client_socket_fd.get();
    }

    bool dataReceived = false;
    while (!dataReceived && !_should_exit) {
        const auto recv_len = recv(client_socket_fd, buffer.data(), buffer.size(), 0);

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

            // Connection reset - if shutting down, exit quietly; otherwise log and exit
            if (errno == ECONNRESET) {
                if (!_should_exit) {
                    LogErr() << "recv failed: " << strerror(errno);
                }
                return;
            }

            LogErr() << "recv failed: " << strerror(errno);
            return;
        }
#endif

        if (recv_len == 0) {
            // Client disconnected, close the socket and go back to accept new connections
            LogInfo() << "TCP client disconnected, waiting for new connection...";
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _client_socket_fd.close();
            }
            return;
        }

        _mavlink_receiver->set_new_datagram(buffer.data(), static_cast<int>(recv_len));

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit loop.
        auto parse_result = _mavlink_receiver->parse_message();
        while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
            receive_message(parse_result, _mavlink_receiver->get_last_message(), this);
            parse_result = _mavlink_receiver->parse_message();
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

    // Get client socket fd with mutex protection, then release mutex before blocking send
    SocketHolder::DescriptorType client_socket_fd;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_client_socket_fd.empty()) {
            result.first = false;
            result.second = "Not connected";
            return result;
        }
        client_socket_fd = _client_socket_fd.get();
    }

#if !defined(MSG_NOSIGNAL)
    auto flags = 0;
#else
    auto flags = MSG_NOSIGNAL;
#endif

    const auto send_len = send(client_socket_fd, bytes, length, flags);

    if (send_len != static_cast<std::remove_cv_t<decltype(send_len)>>(length)) {
        // Broken pipe is expected during shutdown, don't log it
        std::stringstream ss;
#ifdef WINDOWS
        int err = WSAGetLastError();
        ss << "Send failure: " << get_socket_error_string(err);
        if (err != WSAECONNRESET || !_should_exit) {
            LogErr() << ss.str();
        }
#else
        ss << "Send failure: " << strerror(errno);
        if (errno != EPIPE || !_should_exit) {
            LogErr() << ss.str();
        }
#endif
        result.first = false;
        result.second = ss.str();
        return result;
    }

    result.first = true;
    return result;
}

} // namespace mavsdk
