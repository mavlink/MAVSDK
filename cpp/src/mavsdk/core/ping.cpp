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
    }
}

} // namespace mavsdk
