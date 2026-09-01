#include "tcp_server_connection.hpp"
#include "mavsdk_impl.hpp"
#include "log.hpp"

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/ip/address.hpp>
#include <asio/dispatch.hpp>
#include <asio/post.hpp>
#include <asio/socket_base.hpp>
#include <asio/write.hpp>

#include <future>
#include <memory>
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
        LogErr("Invalid local IP '{}': {}", _local_ip, ec.message());
        return ConnectionResult::SocketError;
    }

    asio::ip::tcp::endpoint local_endpoint(local_addr, static_cast<unsigned short>(_local_port));

    _acceptor.open(asio::ip::tcp::v4(), ec);
    if (ec) {
        LogErr("Socket open error: {}", ec.message());
        return ConnectionResult::SocketError;
    }

    _acceptor.set_option(asio::socket_base::reuse_address(true), ec);

    _acceptor.bind(local_endpoint, ec);
    if (ec) {
        LogErr("Bind error: {}", ec.message());
        return ConnectionResult::SocketError;
    }

    _acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        LogErr("Listen error: {}", ec.message());
        return ConnectionResult::SocketError;
    }

    // Kick off the first async accept — subsequent ones follow after each client disconnects.
    do_accept();

    return ConnectionResult::Success;
}

ConnectionResult TcpServerConnection::stop()
{
    // Signal handlers to stop re-arming.  Must be set before closing sockets so
    // that any handler seeing operation_aborted (or a stale non-error code) does
    // not re-arm a new async_accept / async_read_some.
    _stopping = true;
    _client_connected = false;

    auto& io_ctx = static_cast<asio::io_context&>(_acceptor.get_executor().context());
    if (!io_ctx.stopped()) {
        // Close the acceptor and client socket ON the io_context thread.  This
        // serialises the close with do_accept() / do_receive() — both of which
        // also run on the io_context thread — and eliminates the data race on the
        // epoll reactor's descriptor state (TSan warning: cleanup_descriptor_data
        // vs start_op / do_start_accept_op).
        //
        // A double round-trip is used:
        //   Round 1 — outer lambda: closes sockets (queues cancellation callbacks).
        //   Round 2 — inner lambda: waits until those callbacks have been drained.
        std::promise<void> done;
        auto done_future = done.get_future();
        asio::post(io_ctx, [this, &io_ctx, p = std::move(done)]() mutable {
            {
                asio::error_code ec;
                _acceptor.close(ec);
            }
            _tx_queue.clear();
            if (_client_socket.is_open()) {
                asio::error_code ec;
                _client_socket.close(ec);
            }
            // Post a second fence so that the cancellation callbacks queued
            // by close() above are dispatched before stop() returns. That also means
            // the in-flight write (if any) has completed and released its buffer.
            asio::post(io_ctx, [p = std::move(p)]() mutable { p.set_value(); });
        });
        done_future.wait();
    } else {
        // io_context is already stopped — no handlers are running, so it is safe
        // to close from the calling thread.
        asio::error_code ec;
        _acceptor.close(ec);
        _tx_queue.clear();
        if (_client_socket.is_open()) {
            asio::error_code ec2;
            _client_socket.close(ec2);
        }
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
    // Queue the bytes and let the async write chain drain them on the io thread. This
    // never blocks the caller and never occupies the io thread with a synchronous write,
    // so a client that has stopped reading can no longer stall the rest of the SDK.
    //
    // The flip side is that a write failure is only known later; it reaches the user
    // through report_send_error() instead of this return value.
    if (!_client_connected) {
        return {false, "Not connected"};
    }

    std::vector<char> buf(bytes, bytes + length);

    // dispatch() rather than post(): callers are normally already on the io thread (the
    // send path runs there), and running inline keeps the enqueue ordered with respect
    // to the writes already queued.
    asio::dispatch(_acceptor.get_executor(), [this, buf = std::move(buf)]() mutable {
        if (_stopping || !_client_socket.is_open()) {
            return;
        }

        const auto dropped = _tx_queue.push(std::move(buf));
        if (dropped > 0) {
            LogErr("TCP send queue full, dropped {} message(s)", dropped);
            report_send_error("Send queue full, dropped oldest message(s)");
        }

        start_write();
    });

    return {true, {}};
}

void TcpServerConnection::start_write()
{
    // Runs on the io thread. One write is in flight at a time; its completion handler
    // starts the next.
    if (_tx_queue.busy() || _tx_queue.empty()) {
        return;
    }

    if (_stopping || !_client_socket.is_open()) {
        _tx_queue.clear();
        return;
    }

    const auto& item = _tx_queue.start();
    asio::async_write(
        _client_socket, asio::buffer(item), [this](const asio::error_code& ec, std::size_t) {
            _tx_queue.finish();

            if (ec) {
                // Broken pipe / connection reset during shutdown are expected.
                if (ec != asio::error::operation_aborted && ec != asio::error::broken_pipe &&
                    ec != asio::error::connection_reset && !_stopping) {
                    const std::string msg = "Send failure: " + ec.message();
                    LogErr("{}", msg);
                    report_send_error(msg);
                }
                // do_receive() sees the same disconnect and goes back to accepting. Drop
                // what is queued so we don't replay stale traffic onto the next client.
                _tx_queue.clear();
                return;
            }

            start_write();
        });
}

void TcpServerConnection::do_accept()
{
    // Use the overload that delivers the new socket in the callback rather than
    // writing directly into _client_socket from asio internals (which would race
    // with send_raw_bytes reading the socket's state on the work thread).
    _acceptor.async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket peer) {
        if (ec == asio::error::operation_aborted || _stopping) {
            // stop() closed the acceptor — do not re-arm.
            return;
        }
        if (ec) {
            LogErr("Accept error: {}", ec.message());
            // Re-arm so the server keeps listening (unless we're shutting down).
            if (!_stopping) {
                do_accept();
            }
            return;
        }

        // Everything that touches _client_socket runs on this thread, so the assignment
        // needs no lock. Anything left over from the previous client is stale.
        _tx_queue.clear();
        _client_socket = std::move(peer);
        _client_connected = true;

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
                    LogInfo("TCP client disconnected, waiting for new connection...");
                } else {
                    LogErr("TCP receive error: {}", ec.message());
                }
                _client_connected = false;
                _tx_queue.clear();
                {
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
