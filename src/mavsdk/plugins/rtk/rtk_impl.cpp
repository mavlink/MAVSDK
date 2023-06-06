#include "rtk_impl.h"
#include "unused.h"

namespace mavsdk {

RtkImpl::RtkImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

RtkImpl::RtkImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _parent->register_plugin(this);
}

RtkImpl::~RtkImpl()
{
    _parent->unregister_plugin(this);
}

void RtkImpl::init() {}

void RtkImpl::deinit() {}

void RtkImpl::enable() {}

void RtkImpl::disable() {}

Rtk::Result RtkImpl::send_rtcm_data(Rtk::RtcmData rtcm_data)
{
    constexpr size_t field_len = MAVLINK_MSG_GPS_RTCM_DATA_FIELD_DATA_LEN;

    const size_t num_packets_required =
        rtcm_data.data.size() / field_len + (rtcm_data.data.size() % field_len == 0 ? 0 : 1);

    // The maximum is 4 times the 180 bytes because we only have two bits to
    // denote the fragment ID.
    if (num_packets_required > 4) {
        return Rtk::Result::TooLong;
    }

    // From Python, we get the bytes in a Python str such as:
    // b'\xd3\x00\x13>\xd0\x00\x034\x14|\t\xe0\x81\x17\xca\xde[7=s\xf0?\x1b\t\x8a'

    // Instead of changing the interface at this point for MAVSDK v1.4, we just
    // try to detect Python strings instead and convert it back.

    const std::string original_bytes = [&]() {
        // Some use single inverted commas.
        if (rtcm_data.data.size() >= 2 && char(rtcm_data.data[0]) == 'b' &&
            char(rtcm_data.data[1]) == '\'' &&
            char(rtcm_data.data[rtcm_data.data.size() - 1]) == '\'') {
            return convert_from_python_string(rtcm_data.data);
        }

        // Others use double inverted commas.
        else if (
            rtcm_data.data.size() >= 2 && char(rtcm_data.data[0]) == 'b' &&
            char(rtcm_data.data[1]) == '"' &&
            char(rtcm_data.data[rtcm_data.data.size() - 1]) == '"') {
            return convert_from_python_string(rtcm_data.data);

        } else {
            return rtcm_data.data;
        }
    }();

    // The mavlink helpers memcpy, so we need to make sure we're not
    // copying from where we shouldn't.
    rtcm_data.data.resize(num_packets_required * field_len);

    size_t bytes_to_send = original_bytes.size();

    for (size_t i = 0; i < num_packets_required; ++i) {
        const uint8_t flags =
            (num_packets_required > 1 ? 0x1 : 0x0) | ((i & 0x3) << 1) | ((_sequence & 0x1F) << 3);

        mavlink_message_t message;
        mavlink_msg_gps_rtcm_data_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &message,
            flags,
            static_cast<uint8_t>(std::min(field_len, bytes_to_send)),
            reinterpret_cast<const uint8_t*>(original_bytes.c_str() + (i * field_len)));

        if (!_parent->send_message(message)) {
            ++_sequence;
            return Rtk::Result::ConnectionError;
        }

        bytes_to_send -= field_len;
    }

    ++_sequence;
    return Rtk::Result::Success;
}

std::string RtkImpl::convert_from_python_string(const std::string& pstr)
{
    std::string bytes;

    // Skip first two bytes and last.
    const size_t end = pstr.size() - 1;
    size_t i = 2;
    while (i < end) {
        // According to:
        // https://www.w3schools.com/python/gloss_python_escape_characters.asp

        // \' -> '.
        if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == '\'') {
            bytes += '\'';
            i += 2;

            // \\ -> \.
        } else if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == '\\') {
            bytes += '\\';
            i += 2;

            // \n -> 0x0A
        } else if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == 'n') {
            bytes += 0x0A;
            i += 2;

            // \r -> 0x0D
        } else if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == 'r') {
            bytes += 0x0D;
            i += 2;

            // \t -> 0x09
        } else if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == 't') {
            bytes += 0x09;
            i += 2;

            // \b -> 0x08
        } else if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == 'b') {
            bytes += 0x08;
            i += 2;

            // \f -> 0x0C
        } else if (i + 1 < end && pstr[i] == '\\' && pstr[i + 1] == 'f') {
            bytes += 0x0C;
            i += 2;

            // \x00 -> hex number
        } else if (i + 3 < end && pstr[i] == '\\' && pstr[i + 1] == 'x') {
            std::string hex = {pstr[i + 2], pstr[i + 3]};
            const uint8_t byte = std::stoul(hex, nullptr, 16);
            bytes += byte;
            i += 4;

            // some valid char that we can use as is
        } else {
            bytes += pstr[i];
            i += 1;
        }
    }

    return bytes;
}

} // namespace mavsdk
