#include "udp_connection.h"
#include "global_include.h"
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cassert>

namespace dronelink {

UdpConnection::UdpConnection(DroneLinkImpl *parent,
                             int local_port_number,
                             int remote_port_number) :
    Connection(parent),
    _local_port_number(local_port_number),
    _remote_ip(),
    _remote_port_number(remote_port_number),
    _mutex(),
    _socket_fd(-1),
    _recv_thread(nullptr),
    _should_exit(false)
{
    if (_local_port_number == 0) {
        _local_port_number = DEFAULT_UDP_LOCAL_PORT;
    }
}

UdpConnection::~UdpConnection()
{
    // If no one explicitly called stop before, we should at least do it.
    stop();
}

bool UdpConnection::is_ok() const
{
    return true;
}

DroneLink::ConnectionResult UdpConnection::start()
{
    if (!start_mavlink_receiver()) {
        return DroneLink::ConnectionResult::CONNECTIONS_EXHAUSTED;
    }

    DroneLink::ConnectionResult ret = setup_port();
    if (ret != DroneLink::ConnectionResult::SUCCESS) {
        return ret;
    }

    start_recv_thread();

    return DroneLink::ConnectionResult::SUCCESS;
}

DroneLink::ConnectionResult UdpConnection::setup_port()
{
    _socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_socket_fd < 0) {
        Debug() << "socket error" << strerror(errno);
        return DroneLink::ConnectionResult::SOCKET_ERROR;
    }

    struct sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_local_port_number);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(_socket_fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
        Debug() << "bind error: " << strerror(errno);
        return DroneLink::ConnectionResult::BIND_ERROR;
    }

    return DroneLink::ConnectionResult::SUCCESS;
}

void UdpConnection::start_recv_thread()
{
    _recv_thread = new std::thread(receive, this);
}

DroneLink::ConnectionResult UdpConnection::stop()
{
    stop_mavlink_receiver();

    _should_exit = true;
    shutdown(_socket_fd, SHUT_RDWR);

    if (_recv_thread) {
        _recv_thread->join();
        delete _recv_thread;
        _recv_thread = nullptr;
    }

    close(_socket_fd);

    return DroneLink::ConnectionResult::SUCCESS;
}

bool UdpConnection::send_message(const mavlink_message_t &message)
{
    if (_remote_ip.empty()) {
        Debug() << "Remote IP unknown";
        return false;
    }

    if (_remote_port_number == 0) {
        Debug() << "Remote port unknown";
        return false;
    }

    struct sockaddr_in dest_addr;
    memset((char *)&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, _remote_ip.c_str(), &dest_addr.sin_addr.s_addr);

    dest_addr.sin_port = htons(_remote_port_number);

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    // TODO: remove this assert again
    assert(buffer_len <= MAVLINK_MAX_PACKET_LEN);

    int send_len = sendto(_socket_fd, buffer, buffer_len, 0,
                          (const sockaddr *)&dest_addr, sizeof(dest_addr));

    if (send_len != buffer_len) {
        Debug() << "sendto failure: " << strerror(errno);
        return false;
    }

    return true;
}

void UdpConnection::receive(UdpConnection *parent)
{
    char buffer[1024];

    while (!parent->_should_exit) {

        struct sockaddr_in src_addr = {};
        socklen_t src_addr_len = sizeof(src_addr);
        int recv_len = recvfrom(parent->_socket_fd, buffer, sizeof(buffer), 0,
                                (struct sockaddr *)&src_addr, &src_addr_len);

        if (recv_len == 0) {
            // This can happen when shutdown is called on the socket,
            // therefore we check _should_exit again.
            continue;
        }

        int new_remote_port_number = ntohs(src_addr.sin_port);
        std::string new_remote_ip(inet_ntoa(src_addr.sin_addr));

        // TODO make calls to remote threadsafe.

        if (parent->_remote_ip.empty()) {

            if (parent->_remote_port_number == 0 ||
                parent->_remote_port_number == new_remote_port_number) {
                // Set IP if we don't know it yet.
                parent->_remote_ip = new_remote_ip;
                parent->_remote_port_number = new_remote_port_number;

                Debug() << "Partner IP: " << parent->_remote_ip
                        << ":" << parent->_remote_port_number;

            } else {

                Debug() << "Ignoring message from remote port " << new_remote_port_number
                        << " instead of " << parent->_remote_port_number;
                continue;
            }

        } else if (parent->_remote_ip.compare(new_remote_ip) != 0) {
            Debug() << "Ignoring message from IP: " << new_remote_ip;
            continue;

        } else {
            if (parent->_remote_port_number != new_remote_port_number) {
                Debug() << "Ignoring message from remote port " << new_remote_port_number
                        << " instead of " << parent->_remote_port_number;
                continue;
            }
        }

        parent->_mavlink_receiver->set_new_datagram(buffer, recv_len);

        // Parse all mavlink messages in one datagram. Once exhausted, we'll exit while.
        while (parent->_mavlink_receiver->parse_message()) {
            parent->receive_message(parent->_mavlink_receiver->get_last_message());
        }
    }
}


} // namespace dronelink
