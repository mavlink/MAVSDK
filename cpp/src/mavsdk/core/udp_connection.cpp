#include "udp_connection.hpp"
#include "mavsdk_impl.hpp"
#include "log.hpp"

#include <asio/buffer.hpp>
#include <asio/dispatch.hpp>
#include <asio/error.hpp>
#include <asio/ip/address.hpp>
#include <asio/post.hpp>
#include <asio/socket_base.hpp>

#include <algorithm>
#include <future>
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
    _local_port_number(local_port_number),
    _socket(mavsdk_impl.io_context())
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

    // Kick off the first async receive — subsequent ones are re-posted from the handler.
    do_receive();

    return ConnectionResult::Success;
}

ConnectionResult UdpConnection::setup_port()
{
    asio::error_code ec;

    // Resolve the local address
    asio::ip::address local_addr = asio::ip::make_address(_local_ip, ec);
    if (ec) {
        LogErr("Invalid local IP '{}': {}", _local_ip, ec.message());
        return ConnectionResult::SocketError;
    }

    asio::ip::udp::endpoint local_endpoint(
        local_addr, static_cast<unsigned short>(_local_port_number));

    _socket.open(asio::ip::udp::v4(), ec);
    if (ec) {
        LogErr("Socket open error: {}", ec.message());
        return ConnectionResult::SocketError;
    }

    _socket.set_option(asio::socket_base::reuse_address(true), ec);

    _socket.bind(local_endpoint, ec);
    if (ec) {
        LogErr("Bind error: {}", ec.message());
        return ConnectionResult::BindError;
    }

    return ConnectionResult::Success;
}

ConnectionResult UdpConnection::stop()
{
    if (_socket.is_open()) {
        auto& io_ctx = static_cast<asio::io_context&>(_socket.get_executor().context());

        if (!io_ctx.stopped()) {
            // Close the socket from the io_context thread to avoid a data race
            // with a concurrent async_receive_from() / async_send_to() call. Because
            // io_ctx is driven by a single thread, posting the close here serialises it
            // with any in-flight socket access — the close can only run between
            // handler invocations, never while async_receive_from() is reading
            // the socket's internal state.
            std::promise<void> close_done;
            asio::post(io_ctx, [this, &close_done]() {
                _tx_queue.clear();
                asio::error_code ec;
                _socket.close(ec);
                close_done.set_value();
            });
            close_done.get_future().wait();

            // close() cancelled any pending async_receive_from / async_send_to, so the
            // operation_aborted completion handlers are now queued. Wait for them
            // to run (they will not re-arm) before returning, so that callers can
            // safely destroy UdpConnection's members.
            std::promise<void> fence;
            asio::post(io_ctx, [&fence]() { fence.set_value(); });
            fence.get_future().wait();
        } else {
            // io_context has already been stopped (io_thread joined) — no
            // concurrent async operation can be running, and send_raw_bytes() no
            // longer touches the socket from a caller thread.
            _tx_queue.clear();
            asio::error_code ec;
            _socket.close(ec);
        }
    }

    // We need to stop this after stopping the socket, otherwise
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

    // Work out the destinations synchronously (so "no remotes" is still an accurate,
    // immediate answer), then hand the actual sending to the async write chain on the io
    // thread. Nothing below holds _remote_mutex across I/O any more.
    std::vector<asio::ip::udp::endpoint> destinations;
    {
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

                    const bool should_remove =
                        inactive && remote.remote_option == RemoteOption::Found;

                    if (should_remove) {
                        LogInfo("Removing inactive remote: {}:{}", remote.ip, remote.port_number);
                    }

                    return should_remove;
                }),
            _remotes.end());

        if (_remotes.empty()) {
            result.first = false;
            result.second = "no remotes";
            return result;
        }

        destinations.reserve(_remotes.size());

        // As before, a remote we cannot even address counts as a failure for the caller,
        // even if the other remotes are fine.
        result.first = true;

        for (const auto& remote : _remotes) {
            asio::error_code ec;
            asio::ip::address dest_addr = asio::ip::make_address(remote.ip, ec);
            if (ec) {
                std::stringstream ss;
                ss << "make_address failure for: " << remote.ip << ":" << remote.port_number << ": "
                   << ec.message();
                LogErr("{}", ss.str());
                result.first = false;
                if (!result.second.empty()) {
                    result.second += ", ";
                }
                result.second += ss.str();
                continue;
            }

            destinations.emplace_back(dest_addr, static_cast<unsigned short>(remote.port_number));
        }
    }

    if (destinations.empty()) {
        // Every remote failed to parse; result already says so and describes why.
        return result;
    }

    // One shared payload, one queue entry per destination.
    auto payload = std::make_shared<const std::vector<char>>(bytes, bytes + length);

    // dispatch() rather than post(): callers are normally already on the io thread (the
    // send path runs there), and running inline keeps the enqueue ordered with respect
    // to the datagrams already queued.
    asio::dispatch(
        _socket.get_executor(),
        [this, payload = std::move(payload), destinations = std::move(destinations)]() {
            if (!_socket.is_open()) {
                return;
            }

            std::size_t dropped = 0;
            for (const auto& endpoint : destinations) {
                dropped += _tx_queue.push(TxItem{payload, endpoint});
            }
            if (dropped > 0) {
                LogErr("UDP send queue full, dropped {} datagram(s)", dropped);
                report_send_error("Send queue full, dropped oldest datagram(s)");
            }

            start_write();
        });

    return result;
}

void UdpConnection::start_write()
{
    // Runs on the io thread. One datagram is in flight at a time; its completion handler
    // sends the next.
    if (_tx_queue.busy() || _tx_queue.empty()) {
        return;
    }

    if (!_socket.is_open()) {
        _tx_queue.clear();
        return;
    }

    const auto& item = _tx_queue.start();
    _socket.async_send_to(
        asio::buffer(*item.bytes),
        item.endpoint,
        [this, endpoint = item.endpoint](const asio::error_code& ec, std::size_t) {
            _tx_queue.finish();

            if (ec && ec != asio::error::operation_aborted) {
                std::stringstream ss;
                ss << "send_to failure: " << ec.message() << " for: " << endpoint.address() << ":"
                   << endpoint.port();
                LogErr("{}", ss.str());
                report_send_error(ss.str());
            }

            // Unlike a stream, one failed datagram says nothing about the next, so keep
            // draining the queue either way.
            start_write();
        });
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
        if (static_cast<int>(remote_sysid) != 0) {
            LogInfo(
                "New system on: {}:{} (system ID: {})",
                new_remote.ip,
                new_remote.port_number,
                static_cast<int>(remote_sysid));
        }
        _remotes.push_back(new_remote);
    } else {
        existing_remote->last_activity = std::chrono::steady_clock::now();
    }
}

void UdpConnection::do_receive()
{
    // Post an async receive. The handler runs on the dedicated io_context thread (_io_thread).
    _socket.async_receive_from(
        asio::buffer(_recv_buffer),
        _sender_endpoint,
        [this](const asio::error_code& ec, std::size_t recv_len) {
            if (ec) {
                // operation_aborted happens when the socket is closed (stop()), which is normal.
                if (ec != asio::error::operation_aborted) {
                    LogErr("Error from async_receive_from: {}", ec.message());
                }
                // Do NOT re-post — the connection is being torn down.
                return;
            }

            if (recv_len == 0) {
                // Empty datagram — re-post and continue.
                do_receive();
                return;
            }

            char* buffer = reinterpret_cast<char*>(_recv_buffer.data());

            _mavlink_receiver->set_new_datagram(buffer, static_cast<int>(recv_len));

            // Parse all mavlink messages in one datagram.
            auto parse_result = _mavlink_receiver->parse_message();
            while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
                if (parse_result == MavlinkReceiver::ParseResult::MessageParsed) {
                    const uint8_t sysid = _mavlink_receiver->get_last_message().sysid;
                    if (sysid != 0) {
                        add_remote_impl(
                            _sender_endpoint.address().to_string(),
                            static_cast<int>(_sender_endpoint.port()),
                            sysid,
                            RemoteOption::Found);
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

            // Re-post for the next datagram.
            do_receive();
        });
}

} // namespace mavsdk
