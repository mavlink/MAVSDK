#include "log.hpp"
#include "mavlink_address.hpp"
#include "ping.hpp"
#include "system_impl.hpp"

namespace mavsdk {

Ping::Ping(SystemImpl& system_impl) : _system_impl(system_impl)
{
    _system_impl.register_mavlink_message_handler(
        MAVLINK_MSG_ID_PING,
        [this](const mavlink_message_t& message) { Ping::process_ping(message); },
        this);
}

Ping::~Ping()
{
    // Blocking, so that no message can be dispatched into us while we are destroyed.
    _system_impl.unregister_all_mavlink_message_handlers_blocking(this);
}

void Ping::run_once()
{
    _system_impl.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_ping_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl.get_time().elapsed_us(),
            _ping_sequence,
            0,
            0); // to all
        return message;
    });
}

void Ping::process_ping(const mavlink_message_t& message)
{
    mavlink_ping_t ping;
    mavlink_msg_ping_decode(&message, &ping);

    if (ping.target_system == 0 && ping.target_component == 0) {
        // Response to ping request.

        _system_impl.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t response_message;
            mavlink_msg_ping_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &response_message,
                ping.time_usec,
                ping.seq,
                message.sysid,
                message.compid);
            return response_message;
        });

    } else {
        // Answer from ping request.
        if (ping.seq != _ping_sequence) {
            // Ignoring unknown ping sequence.
            return;
        }

        if (message.compid != MAV_COMP_ID_AUTOPILOT1) {
            // We're currently only interested in the ping of the autopilot.
            return;
        }

        if (ping.target_system != _system_impl.get_own_system_id() ||
            ping.target_component != _system_impl.get_own_component_id()) {
            // An answer to someone else's ping, e.g. a ground station on the same link.
            // Its timestamp is in that sender's time base, not ours.
            return;
        }

        const uint64_t now_us = _system_impl.get_time().elapsed_us();
        if (ping.time_usec > now_us) {
            return;
        }

        _last_ping_time_us = now_us - ping.time_usec;
        _rtt_estimator.add_sample_s(static_cast<double>(_last_ping_time_us) * 1e-6);
    }
}

} // namespace mavsdk
