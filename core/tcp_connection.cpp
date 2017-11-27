#include "tcp_connection.h"
#include "global_include.h"

#ifndef WINDOWS
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h> // for close()
#else
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif

#include <cassert>

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace dronecore {

/* change to remote_ip and remote_port */
TcpConnection::TcpConnection(DroneCoreImpl *parent, const std::string &remote_ip, int remote_port) :
    Connection(parent),
    _remote_ip(remote_ip),
    _remote_port_number(remote_port),
    _should_exit(false)
{
    if (_remote_port_number == 0) {
        _remote_port_number = DEFAULT_TCP_REMOTE_PORT;
    }
    if (_remote_ip == "") {
        _remote_ip = DEFAULT_TCP_REMOTE_IP;
    }
}

TcpConnection::~TcpConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}


bool TcpConnection::is_ok() const
{
    return true;
}

DroneCore::ConnectionResult TcpConnection::start()
{
    if (!start_mavlink_receiver()) {
        return DroneCore::ConnectionResult::CONNECTIONS_EXHAUSTED;
    }

    DroneCore::ConnectionResult ret = setup_port();
    if (ret != DroneCore::ConnectionResult::SUCCESS) {
        return ret;
    }

    start_recv_thread();

    return DroneCore::ConnectionResult::SUCCESS;
}

DroneCore::ConnectionResult TcpConnection::setup_port()
{

#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: %d", WSAGetLastError();
        return DroneCore::ConnectionResult::SOCKET_ERROR;
    }
#endif

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_socket_fd < 0) {
        LogErr() << "socket error" << GET_ERROR(errno);
        return DroneCore::ConnectionResult::SOCKET_ERROR;
    }

    struct sockaddr_in remote_addr {};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(_remote_port_number);
    remote_addr.sin_addr.s_addr = inet_addr(_remote_ip.c_str());

    if (connect(_socket_fd, reinterpret_cast<sockaddr *>(&remote_addr),
                sizeof(struct sockaddr_in)) < 0) {
        LogErr() << "connect error: " << GET_ERROR(errno);
        return DroneCore::ConnectionResult::SOCKET_CONNECTION_ERROR;
    }

    return DroneCore::ConnectionResult::SUCCESS;
}

void TcpConnection::start_recv_thread()
{
    _recv_thread = new std::thread(receive, this);
}

DroneCore::ConnectionResult TcpConnection::stop()
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

    return DroneCore::ConnectionResult::SUCCESS;
}

bool TcpConnection::send_message(const mavlink_message_t &message)
{
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

    int send_len = sendto(_socket_fd, reinterpret_cast<char *>(buffer), buffer_len, 0,
                          reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));

    if (send_len != buffer_len) {
        LogErr() << "sendto failure: " << GET_ERROR(errno);
        return false;
    }
    return true;
}

void TcpConnection::receive(TcpConnection *parent)
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!parent->_should_exit) {

        int recv_len = recv(parent->_socket_fd, buffer, sizeof(buffer), 0);

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

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (parent->_mavlink_receiver->parse_message()) {
            parent->receive_message(parent->_mavlink_receiver->get_last_message());
        }
    }
}

} // namespace dronecore
