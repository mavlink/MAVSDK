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
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

#include <algorithm>
#include <utility>
#include <sstream>

namespace mavsdk {

UdpConnection::UdpConnection(
    Connection::ReceiverCallback receiver_callback,
    Connection::LibmavReceiverCallback libmav_receiver_callback,
    MavsdkImpl& mavsdk_impl,
    std::string local_ip,
    int local_port_number,
    ForwardingOption forwarding_option) :
    Connection(
        std::move(receiver_callback),
        std::move(libmav_receiver_callback),
        mavsdk_impl,
        forwarding_option),
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

ConnectionResult UdpConnection::setup_port()
{
#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: " << get_socket_error_string(WSAGetLastError());
        return ConnectionResult::SocketError;
    }
#endif

    _socket_fd.reset(socket(AF_INET, SOCK_DGRAM, 0));

    if (_socket_fd.empty()) {
        LogErr() << "socket error" << strerror(errno);
        return ConnectionResult::SocketError;
    }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, _local_ip.c_str(), &(addr.sin_addr)) != 1) {
        LogErr() << "inet_pton failure for address: " << _local_ip;
        return ConnectionResult::SocketError;
    }
    addr.sin_port = htons(_local_port_number);

    if (bind(_socket_fd.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        LogErr() << "bind error: " << strerror(errno);
        return ConnectionResult::BindError;
    }

    // Set receive timeout cross-platform
    const unsigned timeout_ms = 500;

#if defined(WINDOWS)
    setsockopt(
        _socket_fd.get(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(timeout_ms));
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    setsockopt(_socket_fd.get(), SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(tv));
#endif

    return ConnectionResult::Success;
}

void UdpConnection::start_recv_thread()
{
    _recv_thread = std::make_unique<std::thread>(&UdpConnection::receive, this);
}

ConnectionResult UdpConnection::stop()
{
    _should_exit = true;

    if (_recv_thread) {
        _recv_thread->join();
        _recv_thread.reset();
    }

    _socket_fd.close();

    // We need to stop this after stopping the receive thread, otherwise
    // it can happen that we interfere with the parsing of a message.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

std::pair<bool, std::string> UdpConnection::send_message(const mavlink_message_t& message)
{
    // Convert message to raw bytes and use common send path
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);
    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

std::pair<bool, std::string> UdpConnection::send_raw_bytes(const char* bytes, size_t length)
{
    std::pair<bool, std::string> result;

    std::lock_guard<std::mutex> lock(_remote_mutex);

    // Remove inactive remotes before sending messages
    auto now = std::chrono::steady_clock::now();

    _remotes.erase(
        std::remove_if(
            _remotes.begin(),
            _remotes.end(),
            [&now, this](const Remote& remote) {
                const auto elapsed = now - remote.last_activity;
                const bool inactive = elapsed > REMOTE_TIMEOUT;

                const bool should_remove = inactive && remote.remote_option == RemoteOption::Found;

                // We can cleanup old/previous remotes if we have
                if (should_remove) {
                    LogInfo() << "Removing inactive remote: " << remote.ip << ":"
                              << remote.port_number;
                }

                return should_remove;
            }),
        _remotes.end());

    if (_remotes.size() == 0) {
        result.first = false;
        result.second = "no remotes";
        return result;
    }

    // Send the raw bytes to all the remotes. A remote is a UDP endpoint
    // identified by its <ip, port>. This means that if we have two
    // systems on two different endpoints, then messages directed towards
    // only one system will be sent to both remotes. The systems are
    // then expected to ignore messages that are not directed to them.

    // For multiple remotes, we ignore errors, for just one, we bubble it up.
    result.first = true;

    for (auto& remote : _remotes) {
        struct sockaddr_in dest_addr{};
        dest_addr.sin_family = AF_INET;

        if (inet_pton(AF_INET, remote.ip.c_str(), &dest_addr.sin_addr.s_addr) != 1) {
            std::stringstream ss;
            ss << "inet_pton failure for: " << remote.ip << ":" << remote.port_number;
            LogErr() << ss.str();
            result.first = false;
            if (!result.second.empty()) {
                result.second += ", ";
            }
            result.second += ss.str();
            continue;
        }
        dest_addr.sin_port = htons(remote.port_number);

        const auto send_len = sendto(
            _socket_fd.get(),
            bytes,
            length,
            0,
            reinterpret_cast<const sockaddr*>(&dest_addr),
            sizeof(dest_addr));

        if (send_len != static_cast<std::remove_cv_t<decltype(send_len)>>(length)) {
            std::stringstream ss;
#ifdef WINDOWS
            int err = WSAGetLastError();
            ss << "sendto failure: " << get_socket_error_string(err) << " for: " << remote.ip << ":"
               << remote.port_number;
#else
            ss << "sendto failure: " << strerror(errno) << " for: " << remote.ip << ":"
               << remote.port_number;
#endif
            LogErr() << ss.str();
            result.first = false;
            if (!result.second.empty()) {
                result.second += ", ";
            }
            result.second += ss.str();
            continue;
        }
    }

    return result;
}

void UdpConnection::add_remote_to_keep(const std::string& remote_ip, const int remote_port)
{
    add_remote_impl(remote_ip, remote_port, 0, RemoteOption::Fixed);
}

void UdpConnection::add_remote_impl(
    const std::string& remote_ip,
    const int remote_port,
    const uint8_t remote_sysid,
    RemoteOption remote_option)
{
    std::lock_guard<std::mutex> lock(_remote_mutex);
    Remote new_remote;
    new_remote.ip = remote_ip;
    new_remote.port_number = remote_port;
    new_remote.last_activity = std::chrono::steady_clock::now();
    new_remote.remote_option = remote_option;

    auto existing_remote =
        std::find_if(_remotes.begin(), _remotes.end(), [&new_remote](Remote& remote) {
            return remote == new_remote;
        });

    if (existing_remote == _remotes.end()) {
        // System with sysid 0 is a bit special: it is a placeholder for a connection initiated
        // by MAVSDK. As such, it should not be advertised as a newly discovered system.
        if (static_cast<int>(remote_sysid) != 0) {
            LogInfo() << "New system on: " << new_remote.ip << ":" << new_remote.port_number
                      << " (system ID: " << static_cast<int>(remote_sysid) << ")";
        }
        _remotes.push_back(new_remote);
    } else {
        // Update the timestamp for the existing remote
        existing_remote->last_activity = std::chrono::steady_clock::now();
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

        // Parse all mavlink messages in one datagram. Once exhausted, we'll exit loop.
        auto parse_result = _mavlink_receiver->parse_message();
        while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
            // Track remote endpoint for valid messages
            if (parse_result == MavlinkReceiver::ParseResult::MessageParsed) {
                const uint8_t sysid = _mavlink_receiver->get_last_message().sysid;
                if (sysid != 0) {
                    char ip_str[INET_ADDRSTRLEN];
                    if (inet_ntop(AF_INET, &src_addr.sin_addr, ip_str, INET_ADDRSTRLEN) !=
                        nullptr) {
                        add_remote_impl(
                            ip_str, ntohs(src_addr.sin_port), sysid, RemoteOption::Found);
                    } else {
                        LogErr() << "inet_ntop failure for: " << strerror(errno);
                    }
                }
            }
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
