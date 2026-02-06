#include "tcp_client_connection.h"
#include "log.h"

#include <cassert>
#include <sstream>
#include <utility>
#include <thread>
#include <sstream>

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
#endif

namespace mavsdk {

/* change to remote_ip and remote_port */
TcpClientConnection::TcpClientConnection(
    Connection::ReceiverCallback receiver_callback,
    Connection::LibmavReceiverCallback libmav_receiver_callback,
    MavsdkImpl& mavsdk_impl,
    std::string remote_ip,
    int remote_port,
    ForwardingOption forwarding_option) :
    Connection(
        std::move(receiver_callback),
        std::move(libmav_receiver_callback),
        mavsdk_impl,
        forwarding_option),
    _remote_ip(std::move(remote_ip)),
    _remote_port_number(remote_port),
    _should_exit(false)
{}

TcpClientConnection::~TcpClientConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}

ConnectionResult TcpClientConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

    if (!start_libmav_receiver()) {
        return ConnectionResult::ConnectionsExhausted;
    }

    ConnectionResult ret = setup_port();
    if (ret != ConnectionResult::Success) {
        return ret;
    }

    start_recv_thread();

    return ConnectionResult::Success;
}

ConnectionResult TcpClientConnection::setup_port()
{
#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: " << get_socket_error_string(WSAGetLastError());
        return ConnectionResult::SocketError;
    }
#endif

    std::lock_guard<std::mutex> lock(_mutex);

    int new_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (new_fd < 0) {
        LogErr() << "socket error" << strerror(errno);
        return ConnectionResult::SocketError;
    }

    struct sockaddr_in remote_addr{};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(_remote_port_number);

    struct hostent* hp;
    hp = gethostbyname(_remote_ip.c_str());
    if (hp == nullptr) {
        LogErr() << "Could not get host by name";
        return ConnectionResult::SocketConnectionError;
    }

    memcpy(&remote_addr.sin_addr, hp->h_addr, hp->h_length);

    if (connect(new_fd, reinterpret_cast<sockaddr*>(&remote_addr), sizeof(struct sockaddr_in)) <
        0) {
        LogErr() << "Connect error: " << strerror(errno);
        return ConnectionResult::SocketConnectionError;
    }

    // Set receive timeout cross-platform
    const unsigned timeout_ms = 500;

#if defined(WINDOWS)
    setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(tv));
#endif

    _socket_fd.reset(new_fd);

    return ConnectionResult::Success;
}

void TcpClientConnection::start_recv_thread()
{
    _recv_thread = std::make_unique<std::thread>(&TcpClientConnection::receive, this);
}

ConnectionResult TcpClientConnection::stop()
{
    _should_exit = true;

    if (_recv_thread) {
        _recv_thread->join();
        _recv_thread.reset();
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _socket_fd.close();
    }

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

std::pair<bool, std::string> TcpClientConnection::send_message(const mavlink_message_t& message)
{
    // Convert message to raw bytes and use common send path
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

std::pair<bool, std::string> TcpClientConnection::send_raw_bytes(const char* bytes, size_t length)
{
    std::pair<bool, std::string> result;

    if (_remote_ip.empty()) {
        result.first = false;
        result.second = "Remote IP unknown";
        LogErr() << result.second;
        return result;
    }

    if (_remote_port_number == 0) {
        result.first = false;
        result.second = "Remote port unknown";
        LogErr() << result.second;
        return result;
    }

    // Hold mutex during send to prevent socket from being closed mid-send
    std::lock_guard<std::mutex> lock(_mutex);

    if (_socket_fd.empty()) {
        result.first = false;
        result.second = "Not connected";
        return result;
    }

#if !defined(MSG_NOSIGNAL)
    auto flags = 0;
#else
    auto flags = MSG_NOSIGNAL;
#endif

    const auto send_len = send(_socket_fd.get(), bytes, length, flags);

    if (send_len != static_cast<std::remove_cv_t<decltype(send_len)>>(length)) {
        std::stringstream ss;
        ss << "Send failure: " << strerror(errno);
        LogErr() << ss.str();
        result.first = false;
        result.second = ss.str();
        return result;
    }

    result.first = true;
    return result;
}

void TcpClientConnection::receive()
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!_should_exit) {
        // Get socket fd with mutex protection, then release mutex before blocking recv
        SocketHolder::DescriptorType socket_fd;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_socket_fd.empty()) {
                // Socket not connected, need to reconnect
                socket_fd = SocketHolder::invalid_socket_fd;
            } else {
                socket_fd = _socket_fd.get();
            }
        }

        if (socket_fd == SocketHolder::invalid_socket_fd) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            setup_port();
            continue;
        }

        const auto recv_len = recv(socket_fd, buffer, sizeof(buffer), 0);

        if (recv_len == 0) {
            // Connection closed, just try again.
            LogInfo() << "TCP connection closed, trying to reconnect...";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            setup_port();
            continue;
        }

        if (recv_len < 0) {
#ifdef WINDOWS
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT) {
                // Timeout, just try again.
                continue;
            }
            LogErr() << "TCP receive error: " << get_socket_error_string(err)
                     << ", trying to reconnect...";
#else
            if (errno == EAGAIN || errno == ETIMEDOUT) {
                // Timeout, just try again.
                continue;
            }
            LogErr() << "TCP receive error: " << strerror(errno) << ", trying to reconnect...";
#endif
            std::this_thread::sleep_for(std::chrono::seconds(1));
            setup_port();
            continue;
        }

        _mavlink_receiver->set_new_datagram(buffer, static_cast<int>(recv_len));

        auto parse_result = _mavlink_receiver->parse_message();
        while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
            receive_message(parse_result, _mavlink_receiver->get_last_message(), this);
            parse_result = _mavlink_receiver->parse_message();
        }

        // Also parse with libmav if available
        if (_libmav_receiver) {
            _libmav_receiver->set_new_datagram(buffer, static_cast<int>(recv_len));

            while (_libmav_receiver->parse_message()) {
                receive_libmav_message(_libmav_receiver->get_last_message(), this);
            }
        }
    }
}

} // namespace mavsdk
