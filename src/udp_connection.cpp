#include "udp_connection.h"
#include "global_include.h"
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cassert>

namespace dronelink {

UdpConnection::UdpConnection(DroneLinkImpl *parent, const std::string &ip, int port_number) :
    Connection(parent),
    _ip(ip),
    _port_number(port_number),
    _mutex(),
    _socket_fd(-1),
    _recv_thread(),
    _should_exit(false)
{
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

Result UdpConnection::start()
{
    if (!start_mavlink_receiver()) {
        return Result::CONNECTION_ERROR; // TODO: add better error
    }

    Result ret = setup_port();
    if (ret != Result::SUCCESS) {
        return ret;
    }

    ret = start_recv_thread();
    if (ret != Result::SUCCESS) {
        return ret;
    }

    return Result::SUCCESS;
}

Result UdpConnection::setup_port()
{
    _socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_socket_fd < 0) {
        Debug() << "socket error" << strerror(errno);
        return Result::CONNECTION_ERROR;
    }

    struct sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port_number);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(_socket_fd, (sockaddr *)&addr, sizeof(addr)) != 0 ) {
        Debug() << "bind error: " << strerror(errno);
        return Result::CONNECTION_ERROR;
    }

    return Result::SUCCESS;
}

Result UdpConnection::start_recv_thread()
{
    _recv_thread = new std::thread(receive, this);

    return Result::SUCCESS;
}

Result UdpConnection::stop()
{
    stop_mavlink_receiver();

    _should_exit = true;
    shutdown(_socket_fd, SHUT_RDWR);

    _recv_thread->join();
    delete _recv_thread;

    close(_socket_fd);

    return Result::SUCCESS;
}

Result UdpConnection::send_message(const mavlink_message_t &message)
{
    if (_ip.empty()) {
        return Result::DESTINATION_IP_UNKNOWN;
    }

    struct sockaddr_in dest_addr;
    memset((char *)&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, _ip.c_str(), &dest_addr.sin_addr.s_addr);
    // TODO: remove this magic number
    dest_addr.sin_port = htons(14557);

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);

    // TODO: remove this assert again
    assert(buffer_len <= MAVLINK_MAX_PACKET_LEN);

    int send_len = sendto(_socket_fd, buffer, buffer_len, 0,
                          (const sockaddr *)&dest_addr, sizeof(dest_addr));

    if (send_len != buffer_len) {
        Debug() << "send failure: " << strerror(errno);
        return Result::CONNECTION_ERROR;
    }

    return Result::SUCCESS;
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

        // TODO make _ip threadsafe.

        if (parent->_ip.empty()) {
            // Set IP if we don't know it yet.
            Debug() << "partner IP: " << inet_ntoa(src_addr.sin_addr);
            parent->_ip = inet_ntoa(src_addr.sin_addr);
        } else {
            // Otherwise, only accept packets from the set IP.
            if (strcmp(parent->_ip.c_str(), inet_ntoa(src_addr.sin_addr)) != 0) {

                Debug() << "Ignoring message from: " << inet_ntoa(src_addr.sin_addr);
                continue;
            }
        }

        if (parent->_mavlink_receiver->parse_datagram(buffer, recv_len)) {
            parent->receive_message(parent->_mavlink_receiver->get_message());
        }
    }
}


} // namespace dronelink
