#include "raw_connection.hpp"
#include "mavsdk_impl.hpp"
#include "log.hpp"
#include <asio/post.hpp>
#include <vector>

namespace mavsdk {

RawConnection::RawConnection(
    ReceiverCallback receiver_callback,
    LibmavReceiverCallback libmav_receiver_callback,
    MavsdkImpl& mavsdk_impl,
    ForwardingOption forwarding_option) :
    Connection(receiver_callback, libmav_receiver_callback, mavsdk_impl, forwarding_option)
{}

RawConnection::~RawConnection() = default;

ConnectionResult RawConnection::start()
{
    if (!start_mavlink_receiver()) {
        return ConnectionResult::ConnectionError;
    }

    if (!start_libmav_receiver()) {
        return ConnectionResult::ConnectionError;
    }

    return ConnectionResult::Success;
}

ConnectionResult RawConnection::stop()
{
    // Wait for anything receive() posted, so that no handler still references us (or the
    // receivers below) once this returns.
    drain_io_context();

    stop_mavlink_receiver();
    stop_libmav_receiver();
    return ConnectionResult::Success;
}

std::pair<bool, std::string> RawConnection::send_message(const mavlink_message_t& message)
{
    // Convert message to raw bytes and use common send path
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t buffer_len = mavlink_msg_to_send_buffer(buffer, &message);
    return send_raw_bytes(reinterpret_cast<const char*>(buffer), buffer_len);
}

std::pair<bool, std::string> RawConnection::send_raw_bytes(const char* bytes, size_t length)
{
    if (_mavsdk_impl.notify_raw_bytes_sent(bytes, length)) {
        return {true, ""};
    } else {
        return {false, "no raw_bytes sender"};
    }
}

void RawConnection::receive(const char* bytes, size_t length)
{
    // Callers hand us bytes from whatever thread they like. The receivers below hold parser
    // state that is not synchronized -- two threads feeding bytes concurrently would corrupt
    // it -- and every other connection parses on the io_context thread, so hop over there
    // instead of parsing here. stop() drains the io_context, so the posted handler cannot
    // outlive us.
    auto& io_ctx = io_context();
    if (io_ctx.stopped()) {
        return;
    }

    asio::post(io_ctx, [this, buffer = std::vector<char>(bytes, bytes + length)]() mutable {
        parse(buffer.data(), buffer.size());
    });
}

void RawConnection::parse(char* bytes, size_t length)
{
    _mavlink_receiver->set_new_datagram(bytes, static_cast<int>(length));

    // Parse all mavlink messages in one datagram. Once exhausted, we'll exit loop.
    auto parse_result = _mavlink_receiver->parse_message();
    while (parse_result != MavlinkReceiver::ParseResult::NoneAvailable) {
        receive_message(parse_result, _mavlink_receiver->get_last_message(), this);
        parse_result = _mavlink_receiver->parse_message();
    }

    // Also parse with libmav if available
    if (_libmav_receiver) {
        _libmav_receiver->set_new_datagram(bytes, static_cast<int>(length));

        while (_libmav_receiver->parse_message()) {
            receive_libmav_message(_libmav_receiver->get_last_message(), this);
        }
    }
}

} // namespace mavsdk
