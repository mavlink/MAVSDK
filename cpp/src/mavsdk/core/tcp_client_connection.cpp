#include "tcp_client_connection.hpp"
#include "mavsdk_impl.hpp"
#include "log.hpp"

#include <cassert>

#include <asio/buffer.hpp>
#include <asio/connect.hpp>
#include <asio/error.hpp>
#include <asio/dispatch.hpp>
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
    _resolver(mavsdk_impl.io_context()),
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
    // Posted rather than called inline so that every access to _socket happens on the
    // io_context thread, which is what lets the send path run without a lock.
    asio::post(_socket.get_executor(), [this]() {
        if (_stopping) {
            return;
        }
        do_connect();
    });

    return ConnectionResult::Success;
}

ConnectionResult TcpClientConnection::stop()
{
    // This posts onto the io_context and waits, so it must not run on the io thread.
    assert(!_mavsdk_impl.on_io_thread());

    // Signal handlers to stop re-arming BEFORE cancelling/closing.  An EOF
    // handler that was already queued (ec == eof, not operation_aborted) would
    // otherwise call start_reconnect() after we've cancelled the timer and
    // closed the socket, leaving a dangling async_wait handler after destruction.
    _stopping = true;
    _connected = false;

    auto& io_ctx = io_context();
    if (!io_ctx.stopped()) {
        // Cancel and close from the io_context thread to avoid a data race with
        // a concurrent async_read_some() / async_connect() / async_write() /
        // async_resolve() touching the same object. Because io_ctx is driven by a
        // single thread, posting here serialises all of it with the in-flight work.
        // (The timer cancel used to happen on the caller's thread, which was the same
        // kind of cross-thread access this avoids.)
        std::promise<void> close_done;
        asio::post(io_ctx, [this, &close_done]() {
            _reconnect_timer.cancel();
            _resolver.cancel();
            _tx_queue.clear();
            if (_socket.is_open()) {
                asio::error_code ec;
                _socket.close(ec);
            }
            close_done.set_value();
        });
        close_done.get_future().wait();

        // Drain any operation_aborted handlers queued by the close. This also means the
        // in-flight write (if any) has completed, so its buffer is no longer referenced.
        std::promise<void> fence;
        asio::post(io_ctx, [&fence]() { fence.set_value(); });
        fence.get_future().wait();
    } else {
        // io_context already stopped — no concurrent async operations are running.
        _reconnect_timer.cancel();
        _resolver.cancel();
        _tx_queue.clear();
        if (_socket.is_open()) {
            asio::error_code ec;
            _socket.close(ec);
        }
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
    // Queue the bytes and let the async write chain drain them on the io thread. This
    // never blocks the caller and never occupies the io thread with a synchronous write,
    // so a wedged peer can no longer stall every other connection, timer and plugin.
    //
    // The flip side is that a write failure is only known later; it reaches the user
    // through report_send_error() instead of this return value.
    if (!_connected) {
        return {false, "Not connected"};
    }

    std::vector<char> buf(bytes, bytes + length);

    // dispatch() rather than post(): callers are normally already on the io thread (the
    // send path runs there), and running inline keeps the enqueue ordered with respect
    // to the writes already queued.
    asio::dispatch(_socket.get_executor(), [this, buf = std::move(buf)]() mutable {
        if (_stopping || !_socket.is_open()) {
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

void TcpClientConnection::start_write()
{
    // Runs on the io thread. One write is in flight at a time; its completion handler
    // starts the next.
    if (_tx_queue.busy() || _tx_queue.empty()) {
        return;
    }

    if (_stopping || !_socket.is_open()) {
        _tx_queue.clear();
        return;
    }

    const auto& item = _tx_queue.start();
    asio::async_write(_socket, asio::buffer(item), [this](const asio::error_code& ec, std::size_t) {
        _tx_queue.finish();

        if (ec) {
            if (ec != asio::error::operation_aborted && !_stopping) {
                const std::string msg = "Send failure: " + ec.message();
                LogErr("{}", msg);
                report_send_error(msg);
            }
            // do_receive() sees the same disconnect and drives the reconnect. Drop
            // what is queued so we don't replay stale traffic onto a new connection.
            _tx_queue.clear();
            return;
        }

        start_write();
    });
}

void TcpClientConnection::do_connect()
{
    // Resolve asynchronously. A synchronous resolve() would run getaddrinfo() on the io
    // thread, and this is not a one-off: the reconnect path calls it again every second
    // for as long as the peer is unreachable. A DNS server that is itself unreachable
    // then freezes the whole SDK -- message dispatch, timers, every other connection --
    // for the resolver timeout, over and over. Asio runs the lookup on its own internal
    // thread and delivers the result back here on the io thread.
    _resolver.async_resolve(
        _remote_ip,
        std::to_string(_remote_port_number),
        [this](const asio::error_code& ec, const asio::ip::tcp::resolver::results_type& endpoints) {
            // The operation_aborted check must stay FIRST, and the || must short-circuit
            // before `_stopping` dereferences `this`. resolver::cancel() cannot interrupt
            // a getaddrinfo() already running on Asio's internal resolver thread: it only
            // expires the cancellation token, so this handler is still invoked afterwards
            // -- possibly after stop() has returned and the connection has been
            // destroyed. Reading `ec` is safe then; touching a member would not be.
            if (ec == asio::error::operation_aborted || _stopping) {
                // stop() cancelled the resolve — do not reconnect.
                return;
            }

            if (ec) {
                LogErr(
                    "Resolve error for {}:{}: {}, trying to reconnect...",
                    _remote_ip,
                    _remote_port_number,
                    ec.message());
                start_reconnect();
                return;
            }

            // Ensure the socket is in a clean state before connecting.
            if (_socket.is_open()) {
                asio::error_code close_ec;
                _socket.close(close_ec);
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
                        LogErr("Connect error: {}", connect_ec.message());
                        if (!_stopping) {
                            start_reconnect();
                        }
                        return;
                    }
                    _connected = true;
                    do_receive();
                    // Anything queued while we were disconnected is stale by now, but a
                    // send that raced the connect completing is not, so drain whatever
                    // is waiting.
                    start_write();
                });
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
                    LogInfo("TCP connection closed, trying to reconnect...");
                } else {
                    LogErr("TCP receive error: {}, trying to reconnect...", ec.message());
                }
                _connected = false;
                _tx_queue.clear();
                {
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
