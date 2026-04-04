#include "udp_connection.h"
#include "mavsdk_impl.h"
#include "log.h"

#include <asio/buffer.hpp>
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
        LogErr("socket open error: {}", ec.message());
        return ConnectionResult::SocketError;
    }

    _socket.set_option(asio::socket_base::reuse_address(true), ec);

    _socket.bind(local_endpoint, ec);
    if (ec) {
        LogErr("bind error: {}", ec.message());
        return ConnectionResult::BindError;
    }

    return ConnectionResult::Success;
}

ConnectionResult UdpConnection::stop()
{
    // Close the socket — this cancels any outstanding async_receive_from so the
    // io_context handler fires with operation_aborted and does NOT re-post itself.
    if (_socket.is_open()) {
        asio::error_code ec;
        _socket.close(ec);

        // Synchronise with the io_thread: a do_receive() completion that was
        // already dispatched may still be running its handler body (accessing
        // _recv_buffer, _sender_endpoint, _mavlink_receiver, …).  We post a
        // no-op fence onto the same io_context; because completions are
        // processed in FIFO order the fence can only execute after the
        // operation_aborted handler has returned, making it safe to destroy
        // UdpConnection members once we return from stop().
        //
        // Skip the fence when the io_context has already been stopped
        // (e.g. called via _connections.clear() in ~MavsdkImpl after
        // _io_thread has been joined — no handler is running in that path).
        auto& io_ctx = static_cast<asio::io_context&>(_socket.get_executor().context());
        if (!io_ctx.stopped()) {
            std::promise<void> fence;
            asio::post(io_ctx, [&fence]() { fence.set_value(); });
            fence.get_future().wait();
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

                const bool should_remove = inactive && remote.remote_option == RemoteOption::Found;

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

    // Send the raw bytes to all the remotes synchronously.
    result.first = true;

    for (auto& remote : _remotes) {
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

        asio::ip::udp::endpoint dest_endpoint(
            dest_addr, static_cast<unsigned short>(remote.port_number));

        const auto send_len = _socket.send_to(asio::buffer(bytes, length), dest_endpoint, 0, ec);

        if (ec || send_len != length) {
            std::stringstream ss;
            ss << "send_to failure";
            if (ec) {
                ss << ": " << ec.message();
            }
            ss << " for: " << remote.ip << ":" << remote.port_number;
            LogErr("{}", ss.str());
            result.first = false;
            if (!result.second.empty()) {
                result.second += ", ";
            }
            result.second += ss.str();
            continue;
        }
    }

    return result;
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
                    LogErr("async_receive_from error: {}", ec.message());
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
