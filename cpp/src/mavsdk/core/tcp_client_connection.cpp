#include "tcp_client_connection.h"
#include "mavsdk_impl.h"
#include "log.h"

#include <asio/buffer.hpp>
#include <asio/connect.hpp>
#include <asio/error.hpp>
#include <asio/post.hpp>
#include <asio/write.hpp>

#include <future>
#include <memory>
#include <sstream>
#include <utility>

namespace mavsdk {

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
    _socket(mavsdk_impl.io_context()),
    _reconnect_timer(mavsdk_impl.io_context())
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

    // Kick off the first async connect — subsequent ones follow from do_receive() errors.
    do_connect();

    return ConnectionResult::Success;
}

ConnectionResult TcpClientConnection::stop()
{
    // Signal handlers to stop re-arming BEFORE cancelling/closing.  An EOF
    // handler that was already queued (ec == eof, not operation_aborted) would
    // otherwise call start_reconnect() after we've cancelled the timer and
    // closed the socket, leaving a dangling async_wait handler after destruction.
    _stopping = true;

    _reconnect_timer.cancel();

    {
        std::lock_guard<std::mutex> lock(_send_mutex);
        if (_socket.is_open()) {
            asio::error_code ec;
            _socket.close(ec);
        }
    }

    // Fence: wait for any in-flight handler to finish before we allow member destruction.
    // Because _stopping is set, no handler will post a new async op, so one fence suffices.
    // Skip when the io_context is already stopped (e.g. called from ~MavsdkImpl after
    // the io_thread has been joined — no handler can be running in that case).
    auto& io_ctx = static_cast<asio::io_context&>(_socket.get_executor().context());
    if (!io_ctx.stopped()) {
        std::promise<void> fence;
        asio::post(io_ctx, [&fence]() { fence.set_value(); });
        fence.get_future().wait();
    }

    // Stop this after stopping the socket so we don't interfere with message parsing.
    stop_mavlink_receiver();

    return ConnectionResult::Success;
}

std::pair<bool, std::string> TcpClientConnection::send_message(const mavlink_message_t& message)
{
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);
    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

std::pair<bool, std::string> TcpClientConnection::send_raw_bytes(const char* bytes, size_t length)
{
    // Copy bytes into a heap buffer that outlives this stack frame while the
    // posted work runs on the io_context thread.
    auto buf = std::make_shared<std::vector<char>>(bytes, bytes + length);

    std::promise<std::pair<bool, std::string>> promise;
    auto future = promise.get_future();

    // Post to the io_context so the synchronous write runs on the same thread as
    // async_connect / async_read_some — eliminating data races on socket internals.
    asio::post(
        _socket.get_executor(), [this, buf = std::move(buf), p = std::move(promise)]() mutable {
            std::lock_guard<std::mutex> lock(_send_mutex);
            if (!_socket.is_open()) {
                p.set_value({false, "Not connected"});
                return;
            }
            asio::error_code ec;
            asio::write(_socket, asio::buffer(*buf), ec);
            if (ec) {
                std::string msg = "Send failure: " + ec.message();
                LogErr() << msg;
                p.set_value({false, std::move(msg)});
            } else {
                p.set_value({true, {}});
            }
        });

    return future.get();
}

void TcpClientConnection::do_connect()
{
    // Resolve the remote hostname/IP synchronously. This is called rarely (once at startup,
    // then on each reconnect) so blocking briefly here is acceptable.
    asio::error_code ec;
    asio::ip::tcp::resolver resolver(_socket.get_executor());
    auto endpoints = resolver.resolve(_remote_ip, std::to_string(_remote_port_number), ec);

    if (ec) {
        LogErr() << "Resolve error for " << _remote_ip << ":" << _remote_port_number << ": "
                 << ec.message() << ", trying to reconnect...";
        start_reconnect();
        return;
    }

    // Ensure the socket is in a clean state before connecting.
    // Hold _send_mutex so this close() is serialised with send_raw_bytes.
    {
        std::lock_guard<std::mutex> lock(_send_mutex);
        if (_socket.is_open()) {
            asio::error_code close_ec;
            _socket.close(close_ec);
        }
    }

    asio::async_connect(
        _socket,
        endpoints,
        [this](const asio::error_code& connect_ec, const asio::ip::tcp::endpoint&) {
            if (connect_ec == asio::error::operation_aborted || _stopping) {
                // stop() was called — do not reconnect.
                return;
            }
            if (connect_ec) {
                LogErr() << "Connect error: " << connect_ec.message();
                if (!_stopping) {
                    start_reconnect();
                }
                return;
            }
            do_receive();
        });
}

void TcpClientConnection::do_receive()
{
    _socket.async_read_some(
        asio::buffer(_recv_buffer), [this](const asio::error_code& ec, std::size_t recv_len) {
            if (ec == asio::error::operation_aborted || _stopping) {
                // stop() was called — do not reconnect.
                return;
            }

            if (ec) {
                if (ec == asio::error::eof || ec == asio::error::connection_reset) {
                    LogInfo() << "TCP connection closed, trying to reconnect...";
                } else {
                    LogErr() << "TCP receive error: " << ec.message() << ", trying to reconnect...";
                }
                {
                    std::lock_guard<std::mutex> lock(_send_mutex);
                    asio::error_code close_ec;
                    _socket.close(close_ec);
                }
                if (!_stopping) {
                    start_reconnect();
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

void TcpClientConnection::start_reconnect()
{
    if (_stopping) {
        return;
    }
    _reconnect_timer.expires_after(std::chrono::seconds(1));
    _reconnect_timer.async_wait([this](const asio::error_code& ec) {
        if (ec == asio::error::operation_aborted || _stopping) {
            // stop() cancelled the timer — do not reconnect.
            return;
        }
        do_connect();
    });
}

} // namespace mavsdk
