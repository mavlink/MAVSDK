#include "timesync.hpp"
#include "log.hpp"
#include "mavlink_address.hpp"
#include "system_impl.hpp"

// Partially based on: https://github.com/mavlink/mavros/blob/master/mavros/src/plugins/sys_time.cpp

namespace mavsdk {

Timesync::Timesync(SystemImpl& parent) : _system_impl(parent) {}

Timesync::~Timesync()
{
    // Blocking variants, so that neither the tick nor a message can be dispatched into us
    // while we are being destroyed.
    _system_impl.remove_call_every_blocking(_send_cookie);
    _system_impl.unregister_all_mavlink_message_handlers_blocking(this);
}

void Timesync::enable()
{
    if (_is_enabled) {
        // System::enable_timesync() is public API and used to be idempotent, so keep it that
        // way: registering the tick twice would double the timesync rate and leak a cookie.
        return;
    }

    _is_enabled = true;
    _system_impl.register_mavlink_message_handler(
        MAVLINK_MSG_ID_TIMESYNC,
        [this](const mavlink_message_t& message) { process_timesync(message); },
        this);

    // This used to be a do_work() that compared elapsed time on every pass of the system
    // work poll. It is just a fixed interval, so let CallEveryHandler keep the time.
    _send_cookie = _system_impl.add_call_every(
        [this]() { send_timesync_tick(); }, static_cast<float>(TIMESYNC_SEND_INTERVAL_S));
}

void Timesync::send_timesync_tick()
{
    if (!_is_enabled) {
        return;
    }

    if (_system_impl.is_connected()) {
        uint64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                              _system_impl.get_autopilot_time().now().time_since_epoch())
                              .count();
        send_timesync(0, now_ns);
    } else {
        _autopilot_timesync_acquired = false;
    }
}

void Timesync::process_timesync(const mavlink_message_t& message)
{
    mavlink_timesync_t timesync{};

    mavlink_msg_timesync_decode(&message, &timesync);

    int64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                         _system_impl.get_autopilot_time().now().time_since_epoch())
                         .count();

    if (timesync.tc1 == 0 && _autopilot_timesync_acquired) {
        // Send synced time to remote system
        send_timesync(now_ns, timesync.ts1);
    } else if (timesync.tc1 > 0) {
        // Time offset between this system and the remote system is calculated assuming RTT for
        // the timesync packet is roughly equal both ways.
        set_timesync_offset((timesync.tc1 * 2 - (timesync.ts1 + now_ns)) / 2, timesync.ts1);
    }
}

void Timesync::send_timesync(uint64_t tc1, uint64_t ts1)
{
    _system_impl.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_timesync_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<int64_t>(tc1),
            static_cast<int64_t>(ts1),
            0,
            0);
        return message;
    });
}

void Timesync::set_timesync_offset(int64_t offset_ns, uint64_t start_transfer_local_time_ns)
{
    uint64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          _system_impl.get_autopilot_time().now().time_since_epoch())
                          .count();

    // Calculate the round trip time (RTT) it took the timesync packet to bounce back to us from
    // remote system
    uint64_t rtt_ns = now_ns - start_transfer_local_time_ns;

    if (rtt_ns < MAX_RTT_SAMPLE_MS * 1000000ULL) { // Only use samples with low RTT

        // Save time offset for other components to use
        _system_impl.get_autopilot_time().shift_time_by(std::chrono::nanoseconds(offset_ns));
        _autopilot_timesync_acquired = true;

        // Reset high RTT count after filter update
        _high_rtt_count = 0;
    } else {
        // Increment counter if round trip time is too high for accurate timesync
        _high_rtt_count++;

        if (_high_rtt_count > MAX_CONS_HIGH_RTT) {
            // Issue a warning to the user if the RTT is constantly high
            LogWarn("RTT too high for timesync: {} ms.", rtt_ns / 1000000ULL);

            // Reset counter
            _high_rtt_count = 0;
        }
    }
}

} // namespace mavsdk
