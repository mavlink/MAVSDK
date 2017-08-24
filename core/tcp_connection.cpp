#include "tcp_connection.h"
#include "global_include.h"

#ifndef WINDOWS
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h> // for close()
#else
#include <winsock2.h>
#include <Ws2tcpip.h> // For InetPton
#undef SOCKET_ERROR // conflicts with ConnectionResult::SOCKET_ERROR
#endif

#include <cassert>

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace dronecore {

TcpConnection::TcpConnection(DroneCoreImpl *parent, const std::string &ip, int port_number) :
    Connection(parent),
    _remote_ip(ip),
    _remote_port_number(port_number),
    _should_exit(false)
{
    if (_local_port_number == 0) {
        _local_port_number = DEFAULT_TCP_LOCAL_PORT;
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
        Debug() << "Error: Winsock failed, error: %d", WSAGetLastError();
        return DroneCore::ConnectionResult::SOCKET_ERROR;
    }
#endif

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (_socket_fd < 0) {
        Debug() << "socket error" << GET_ERROR(errno);
        return DroneCore::ConnectionResult::SOCKET_ERROR;
    }

    sockaddr_in local_addr; 
    sockaddr_in remote_addr;
    memset(&local_addr, 0, sizeof(sockaddr_in));
    memset(&remote_addr, 0, sizeof(sockaddr_in)); 
     
    std::string local_ip="127.0.0.1";
    resolve_address(local_ip, _local_port_number, &local_addr);
    resolve_address(_remote_ip,_remote_port_number, &remote_addr);
    std::cout << _remote_ip<<::std::endl;

    if (bind(_socket_fd, (sockaddr *)&local_addr, sizeof(sockaddr_in)) < 0) {
        Debug() << "bind error: " << GET_ERROR(errno);
        return DroneCore::ConnectionResult::BIND_ERROR;
    }
    if (connect(_socket_fd, (sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
        Debug() << "connect error: " << GET_ERROR(errno);
        return DroneCore::ConnectionResult::SOCKET_CONNECTION_ERROR;
    }

    return DroneCore::ConnectionResult::SUCCESS;
}

void TcpConnection::resolve_address(const std::string& ipAddress, int port, sockaddr_in* addr)
{
        struct addrinfo hints;
		bool ip_found = false;
		struct addrinfo *result = NULL;

		std::string portName = std::to_string(port);
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

        int rc = getaddrinfo(ipAddress.c_str(),portName.c_str(), &hints, &result);
		if (rc != 0) {
			throw std::runtime_error("TcpClientPort getaddrinfo failed with error:\n");
		}
		addr->sin_family = AF_INET;
		addr->sin_port = htons(port);
		for (struct addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			if (ptr->ai_family == AF_INET && ptr->ai_socktype == SOCK_STREAM && ptr->ai_protocol == IPPROTO_TCP)
			{
				// found it!
				sockaddr_in* temp_ptr = reinterpret_cast<sockaddr_in*>(ptr->ai_addr);
				addr->sin_family = temp_ptr->sin_family;
				addr->sin_addr.s_addr = temp_ptr->sin_addr.s_addr;
				addr->sin_port = temp_ptr->sin_port;
				ip_found = true;
				break;
			}
		}
		freeaddrinfo(result);
		if (!ip_found)
			throw std::runtime_error("TcpClientPort could not resolve ip address for:\n");
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

    int send_len = sendto(_socket_fd, (const char *)buffer, buffer_len, 0,
                          (const sockaddr *)&dest_addr, sizeof(dest_addr));

    if (send_len != buffer_len) {
        Debug() << "sendto failure: " << GET_ERROR(errno);
        return false;
    }
    return true;
}

void TcpConnection::receive(TcpConnection *parent)
{
    // Enough for MTU 1500 bytes.
    char buffer[2048];

    while (!parent->_should_exit) {

        struct sockaddr_in src_addr = {};
        //socklen_t src_addr_len = sizeof(src_addr);
        int recv_len = recv(parent->_socket_fd, buffer, sizeof(buffer), 0);

        if (recv_len == 0) {
            // This can happen when shutdown is called on the socket,
            // therefore we check _should_exit again.
            continue;
        }

        if (recv_len < 0) {
            // This happens on desctruction when close(_socket_fd) is called,
            // therefore be quiet.
            //Debug() << "recvfrom error: " << GET_ERROR(errno);
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

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (parent->_mavlink_receiver->parse_message()) {
            parent->receive_message(parent->_mavlink_receiver->get_last_message());
        }
    }
}

} // namespace dronecore
