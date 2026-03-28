#include "tcp_server_connection.h"
#include "mavsdk_impl.h"
#include "log.h"

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/ip/address.hpp>
#include <asio/post.hpp>
#include <asio/socket_base.hpp>
#include <asio/write.hpp>

#include <future>
#include <sstream>
#include <utility>

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
    _local_port(local_port),
    _acceptor(mavsdk_impl.io_context()),
    _client_socket(mavsdk_impl.io_context())
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

    asio::error_code ec;

    asio::ip::address local_addr = asio::ip::make_address(_local_ip, ec);
    if (ec) {
        LogErr() << "Invalid local IP '" << _local_ip << "': " << ec.message();
        return ConnectionResult::SocketError;
    }

    asio::ip::tcp::endpoint local_endpoint(local_addr, static_cast<unsigned short>(_local_port));

    _acceptor.open(asio::ip::tcp::v4(), ec);
    if (ec) {
        LogErr() << "socket open error: " << ec.message();
        return ConnectionResult::SocketError;
    }

    _acceptor.set_option(asio::socket_base::reuse_address(true), ec);

    _acceptor.bind(local_endpoint, ec);
    if (ec) {
        LogErr() << "bind error: " << ec.message();
        return ConnectionResult::SocketError;
    }

    _acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        LogErr() << "listen error: " << ec.message();
        return ConnectionResult::SocketError;
    }

    // Kick off the first async accept — subsequent ones follow after each client disconnects.
    do_accept();

    return ConnectionResult::Success;
}

ConnectionResult TcpServerConnection::stop()
{
    // Signal handlers to stop re-arming BEFORE closing sockets.  An EOF handler
    // that already started running (with ec == eof, not operation_aborted) would
    // otherwise call do_accept() on the now-closed acceptor, producing a
    // bad_descriptor error that re-arms again — an infinite loop.
    _stopping = true;

    // Close acceptor and client socket — cancels outstanding async operations.
    {
        asio::error_code ec;
        _acceptor.close(ec);
    }
    {
        std::lock_guard<std::mutex> lock(_send_mutex);
        if (_client_socket.is_open()) {
            asio::error_code ec;
            _client_socket.close(ec);
        }
    }

    // Fence: wait for any in-flight handler to finish before allowing member destruction.
    // Because _stopping is set, no handler will post a new async op, so one fence suffices.
    auto& io_ctx = static_cast<asio::io_context&>(_acceptor.get_executor().context());
    if (!io_ctx.stopped()) {
        std::promise<void> fence;
        asio::post(io_ctx, [&fence]() { fence.set_value(); });
        fence.get_future().wait();
    }

    // Stop after stopping the socket so we don't interfere with message parsing.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

std::pair<bool, std::string> TcpServerConnection::send_message(const mavlink_message_t& message)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);
    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

std::pair<bool, std::string> TcpServerConnection::send_raw_bytes(const char* bytes, size_t length)
{
    std::lock_guard<std::mutex> lock(_send_mutex);

    if (!_client_socket.is_open()) {
        return {false, "Not connected"};
    }

    asio::error_code ec;
    asio::write(_client_socket, asio::buffer(bytes, length), ec);

    if (ec) {
        // Broken pipe / connection reset during shutdown are expected — log only if relevant.
        if (ec != asio::error::broken_pipe && ec != asio::error::connection_reset) {
            LogErr() << "Send failure: " << ec.message();
        }
        return {false, "Send failure: " + ec.message()};
    }

    return {true, {}};
}

void TcpServerConnection::do_accept()
{
    _acceptor.async_accept(_client_socket, [this](const asio::error_code& ec) {
        if (ec == asio::error::operation_aborted || _stopping) {
            // stop() closed the acceptor — do not re-arm.
            return;
        }
        if (ec) {
            LogErr() << "accept error: " << ec.message();
            // Re-arm so the server keeps listening (unless we're shutting down).
            if (!_stopping) {
                do_accept();
            }
            return;
        }

        // Start receiving from the newly accepted client.
        do_receive();
    });
}

void TcpServerConnection::do_receive()
{
    _client_socket.async_read_some(
        asio::buffer(_recv_buffer), [this](const asio::error_code& ec, std::size_t recv_len) {
            if (ec == asio::error::operation_aborted || _stopping) {
                // stop() was called — do not reconnect.
                return;
            }

            if (ec) {
                if (ec == asio::error::eof || ec == asio::error::connection_reset) {
                    LogInfo() << "TCP client disconnected, waiting for new connection...";
                } else {
                    LogErr() << "TCP receive error: " << ec.message();
                }
                {
                    std::lock_guard<std::mutex> lock(_send_mutex);
                    asio::error_code close_ec;
                    _client_socket.close(close_ec);
                }
                // Go back to waiting for a new client (unless stop() is in progress).
                if (!_stopping) {
                    do_accept();
                }
                return;
            }

            _mavlink_receiver->set_new_datagram(_recv_buffer.data(), static_cast<int>(recv_len));

            auto parse_result = _mavlink_receiver->parse_message();
            while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
                receive_message(parse_result, _mavlink_receiver->get_last_message(), this);
                parse_result = _mavlink_receiver->parse_message();
            }

            if (_libmav_receiver) {
                _libmav_receiver->set_new_datagram(_recv_buffer.data(), static_cast<int>(recv_len));
                while (_libmav_receiver->parse_message()) {
                    receive_libmav_message(_libmav_receiver->get_last_message(), this);
                }
            }

            // Re-arm for the next chunk.
            do_receive();
        });
}

} // namespace mavsdk
