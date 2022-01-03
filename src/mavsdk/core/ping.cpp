#include "log.h"
#include "ping.h"
#include "system_impl.h"

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
    _system_impl.unregister_all_mavlink_message_handlers(this);
}

void Ping::run_once()
{
    mavlink_message_t message;

    mavlink_msg_ping_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &message,
        _system_impl.get_time().elapsed_us(),
        _ping_sequence,
        0,
        0); // to all

    _system_impl.send_message(message);
}

void Ping::process_ping(const mavlink_message_t& message)
{
    mavlink_ping_t ping;
    mavlink_msg_ping_decode(&message, &ping);

    if (ping.target_system == 0 && ping.target_component == 0) {
        // Response to ping request.
        mavlink_message_t response_message;
        mavlink_msg_ping_pack(
            _system_impl.get_own_system_id(),
            _system_impl.get_own_component_id(),
            &response_message,
            ping.time_usec,
            ping.seq,
            message.sysid,
            message.compid);

        _system_impl.send_message(response_message);

    } else {
        // Answer from ping request.
        if (ping.seq != _ping_sequence) {
            LogWarn() << "Ignoring unknown ping sequence";
            return;
        }

        if (message.compid != MAV_COMP_ID_AUTOPILOT1) {
            // We're currently only interested in the ping of the autopilot.
            return;
        }

        _last_ping_time_us = _system_impl.get_time().elapsed_us() - ping.time_usec;
    }
}

} // namespace mavsdk
