#include "rtk_impl.h"
#include "base64.h"

namespace mavsdk {

RtkImpl::RtkImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

RtkImpl::RtkImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

RtkImpl::~RtkImpl()
{
    _system_impl->unregister_plugin(this);
}

void RtkImpl::init() {}

void RtkImpl::deinit() {}

void RtkImpl::enable() {}

void RtkImpl::disable() {}

Rtk::Result RtkImpl::send_rtcm_data(Rtk::RtcmData rtcm_data)
{
    constexpr size_t field_len = MAVLINK_MSG_GPS_RTCM_DATA_FIELD_DATA_LEN;

    std::vector<uint8_t> decoded = base64_decode(rtcm_data.data_base64);

    const size_t num_packets_required =
        decoded.size() / field_len + (decoded.size() % field_len == 0 ? 0 : 1);

    // The maximum is 4 times the 180 bytes because we only have two bits to
    // denote the fragment ID.
    if (num_packets_required > 4) {
        return Rtk::Result::TooLong;
    }

    // Copy length before we change it.
    size_t bytes_to_send = decoded.size();

    // The mavlink helpers memcpy, so we need to make sure we're not
    // copying from where we shouldn't.
    decoded.resize(num_packets_required * field_len);

    for (size_t i = 0; i < num_packets_required; ++i) {
        const uint8_t flags =
            (num_packets_required > 1 ? 0x1 : 0x0) | ((i & 0x3) << 1) | ((_sequence & 0x1F) << 3);

        if (!_system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
                mavlink_message_t message;
                mavlink_msg_gps_rtcm_data_pack_chan(
                    mavlink_address.system_id,
                    mavlink_address.component_id,
                    channel,
                    &message,
                    flags,
                    static_cast<uint8_t>(std::min(field_len, bytes_to_send)),
                    (decoded.data() + (i * field_len)));
                return message;
            })) {
            ++_sequence;
            return Rtk::Result::ConnectionError;
        }

        bytes_to_send -= field_len;
    }

    ++_sequence;
    return Rtk::Result::Success;
}

} // namespace mavsdk
