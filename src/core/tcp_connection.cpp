#include "tcp_connection.h"
#include "global_include.h"
#include "log.h"

#include <openssl/err.h>
#include <openssl/ssl.h>

#ifdef WINDOWS
#ifndef MINGW
#pragma comment(lib, "Ws2_32.lib") // Without this, Ws2_32.lib is not included in static library.
#endif
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h> // for close()
#endif

#include <cassert>

#ifndef WINDOWS
#define GET_ERROR(_x) strerror(_x)
#else
#define GET_ERROR(_x) WSAGetLastError()
#endif

namespace mavsdk {

/* change to remote_ip and remote_port */
TcpConnection::TcpConnection(
    Connection::receiver_callback_t receiver_callback,
    const std::string& remote_ip,
    int remote_port) :
    Connection(receiver_callback),
    _remote_ip(remote_ip),
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
    bool is_server = false;

    if (_remote_ip == "0.0.0.0") {
        LogErr() << "Sparta - running as server on port " << _remote_port_number << "!";
        is_server = true;
    }

    if (!start_mavlink_receiver()) {
        return ConnectionResult::CONNECTIONS_EXHAUSTED;
    }

    // ------------- TLS proto
    const SSL_METHOD* method = TLS_method(); // Create generic TLS method
    SSL_CTX* ssl_context = SSL_CTX_new(method); // Create context
    if (ssl_context == nullptr) {
        ERR_print_errors_fp(stderr);
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    if (SSL_CTX_set_min_proto_version(ssl_context, TLS1_2_VERSION) != 1) {
        // TODO: not sure if this specific error appears in the error stack
        ERR_print_errors_fp(stderr);
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    // Generate self-signed certificate with:
    // $ openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365
    if (SSL_CTX_use_certificate_chain_file(ssl_context, "/tmp/cert/cert.pem") != 1) {
        ERR_print_errors_fp(stderr);
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    // TODO: This asks for a passphrase. Maybe the key can be loaded differently than with _file, to avoid that?
    // NOTE: can use $ openssl req -nodes ... above to not encrypt the output key (and therefore not have a passphrase)
    if (SSL_CTX_use_PrivateKey_file(ssl_context, "/tmp/cert/key.pem", SSL_FILETYPE_PEM) != 1) {
        ERR_print_errors_fp(stderr);
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    if (SSL_CTX_check_private_key(ssl_context) != 1) {
        ERR_print_errors_fp(stderr);
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    BIO* bio = BIO_new_ssl_connect(ssl_context);
    if (bio == nullptr) {
        ERR_print_errors_fp(stderr); // Does that work for bio?
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    if (!is_server) {
        std::string hostname = _remote_ip + ":" + std::to_string(_remote_port_number);
        BIO_set_conn_hostname(bio, hostname.c_str());
    }

    BIO_get_ssl(bio, &_ssl);
    if (_ssl == nullptr) {
        ERR_print_errors_fp(stderr); // Does that work for bio?
        return ConnectionResult::SOCKET_ERROR; // TODO appropriate error
    }

    if (is_server) {
        SSL_set_accept_state(_ssl);
    } else {
        SSL_set_connect_state(_ssl);
        SSL_set_mode(_ssl, SSL_MODE_AUTO_RETRY);
    }

    // ------------- /TLS proto

    const sockaddr_in remote_addr = create_remote_addr();

    if (is_server) {
        _server_socket = create_socket();
        if (_server_socket < 0) {
            return ConnectionResult::SOCKET_ERROR;
        }

        if (bind(_server_socket, reinterpret_cast<const sockaddr*>(&remote_addr), sizeof(struct sockaddr_in)) == 0) {

            if (listen(_server_socket, 1) == 0) { // Allow only one connection
                socklen_t client_addr_size = sizeof(_client_addr);

                _socket_fd = accept(_server_socket, reinterpret_cast<sockaddr*>(&_client_addr), &client_addr_size);
                if (_socket_fd != -1) {
                    LogErr() << "Sparta - accepted!";

                    SSL_set_fd(_ssl, _socket_fd);
                    if (SSL_accept(_ssl) <= 0) {
                        ERR_print_errors_fp(stderr);
                    } else {
                        LogErr() << "Sparta - handshake completed!";
                    }

                    _is_ok = true;
                    //return ConnectionResult::SUCCESS;
                } else {
                    return ConnectionResult::SOCKET_CONNECTION_ERROR; // TODO more appropriate error?
                }
            }
        } else {
            return ConnectionResult::SOCKET_CONNECTION_ERROR; // TODO BIND_ERROR?
        }
    } else {
        if (SSL_connect(_ssl) != 1) {
            ERR_print_errors_fp(stderr);
            return ConnectionResult::SOCKET_CONNECTION_ERROR; // TODO more appropriate error?
        }

        /*
        _socket_fd = create_socket();
        if (_socket_fd < 0) {
            return ConnectionResult::SOCKET_ERROR;
        }

        ConnectionResult ret = connect(_socket_fd, remote_addr);
        if (ret != ConnectionResult::SUCCESS) {
            return ret;
        }
        */

        _is_ok = true;
    }

    start_recv_thread();

    return ConnectionResult::SUCCESS;
}

int TcpConnection::create_socket()
{
#ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        LogErr() << "Error: Winsock failed, error: %d", WSAGetLastError();
        _is_ok = false;
        return -1;
    }
#endif

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        LogErr() << "socket error " << GET_ERROR(errno);
        _is_ok = false;
        return -1;
    }

    return socket_fd;
}

sockaddr_in TcpConnection::create_remote_addr() {
    struct sockaddr_in remote_addr {};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(_remote_port_number);
    remote_addr.sin_addr.s_addr = inet_addr(_remote_ip.c_str());

    return remote_addr;
}

ConnectionResult TcpConnection::connect(const int socket_fd, const sockaddr_in remote_addr)
{
    if (::connect(socket_fd, reinterpret_cast<const sockaddr*>(&remote_addr), sizeof(struct sockaddr_in)) <
        0) {
        LogErr() << "connect error: " << GET_ERROR(errno);
        _is_ok = false;
        return ConnectionResult::SOCKET_CONNECTION_ERROR;
    }

    LogErr() << "Sparta - client connected!";
    _is_ok = true;
    return ConnectionResult::SUCCESS;
}

void TcpConnection::start_recv_thread()
{
    _recv_thread = new std::thread(&TcpConnection::receive, this);
}

ConnectionResult TcpConnection::stop()
{
    _should_exit = true;

#ifndef WINDOWS
    // This should interrupt a recv/recvfrom call.
    shutdown(_socket_fd, SHUT_RDWR);

    // But on Mac, closing is also needed to stop blocking recv/recvfrom.
    close(_socket_fd);
    // TODO _server_socket?
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

    return ConnectionResult::SUCCESS;
}

bool TcpConnection::send_message(const mavlink_message_t& message)
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

    const auto send_len = SSL_write(_ssl, buffer, buffer_len);
    /*
    const auto send_len = sendto(
        _socket_fd,
        reinterpret_cast<char*>(buffer),
        buffer_len,
        0,
        reinterpret_cast<const sockaddr*>(&dest_addr),
        sizeof(dest_addr));
        */

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
            //TODO _socket_fd = create_socket();
            //TODO connect(_socket_fd);
        }

        const auto recv_len = SSL_read(_ssl, buffer, sizeof(buffer));
        //const auto recv_len = recv(_socket_fd, buffer, sizeof(buffer), 0);

        if (recv_len == 0) {
            // This can happen when shutdown is called on the socket,
            // therefore we check _should_exit again.
            LogErr() << "Sparta - disconnecting because recv_len == 0";
            _is_ok = false;
            continue;
        }

        if (recv_len < 0) {
            // This happens on destruction when close(_socket_fd) is called,
            // therefore be quiet.
            LogErr() << "recvfrom error: " << GET_ERROR(errno);
            // Something went wrong, we should try to re-connect in next iteration.
            _is_ok = false;
            continue;
        }

        _mavlink_receiver->set_new_datagram(buffer, static_cast<int>(recv_len));

        // Parse all mavlink messages in one data packet. Once exhausted, we'll exit while.
        while (_mavlink_receiver->parse_message()) {
            receive_message(_mavlink_receiver->get_last_message());
        }
    }
}

} // namespace mavsdk
