#include "timesync.hpp"
#include "log.hpp"
#include "mavlink_address.hpp"
#include "system_impl.hpp"
#include "callback_list.tpp"

#include <algorithm>

// Partially based on: https://github.com/mavlink/mavros/blob/master/mavros/src/plugins/sys_time.cpp

namespace mavsdk {

template class MAVSDK_TEMPL_INST CallbackList<System::TimesyncStatus>;

Timesync::Timesync(SystemImpl& parent) :
    _system_impl(parent),
    _status_callbacks(parent.io_context())
{
    _system_impl.register_mavlink_message_handler(
        MAVLINK_MSG_ID_TIMESYNC,
        [this](const mavlink_message_t& message) { process_timesync(message); },
        this);
}

Timesync::~Timesync()
{
    // Blocking, so that no message can be dispatched into us while we are being destroyed.
    _system_impl.unregister_all_mavlink_message_handlers_blocking(this);
}

void Timesync::enable()
{
    // System::enable_timesync() is public API and idempotent.
    _is_enabled = true;
}

void Timesync::reset_rtt()
{
    _rtt_estimator.reset();
}

System::TimesyncStatus Timesync::status() const
{
    std::lock_guard<std::mutex> lock(_status_mutex);
    return _status;
}

System::TimesyncStatusHandle
Timesync::subscribe_status(const System::TimesyncStatusCallback& callback)
{
    return _status_callbacks.subscribe(callback);
}

void Timesync::unsubscribe_status(System::TimesyncStatusHandle handle)
{
    _status_callbacks.unsubscribe(handle);
}

void Timesync::update_status(bool synced, int64_t rtt_ns)
{
    System::TimesyncStatus status;
    {
        std::lock_guard<std::mutex> lock(_status_mutex);
        _status.synced = synced;
        _status.offset_ns = _system_impl.get_autopilot_time().offset().count();
        _status.rtt_ns = rtt_ns;
        status = _status;
    }

    _status_callbacks.queue(
        status, [this](const auto& func) { _system_impl.call_user_callback(func); });
}

void Timesync::send_tick()
{
    if (_system_impl.is_connected()) {
        int64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             _system_impl.get_autopilot_time().now().time_since_epoch())
                             .count();
        remember_request(now_ns);
        send_timesync(0, static_cast<uint64_t>(now_ns));
    } else if (_autopilot_timesync_acquired) {
        _autopilot_timesync_acquired = false;
        update_status(false, status().rtt_ns);
    }
}

void Timesync::process_timesync(const mavlink_message_t& message)
{
    mavlink_timesync_t timesync{};

    mavlink_msg_timesync_decode(&message, &timesync);

    // MavsdkImpl already drops messages for other systems, but not for other components of
    // ours, and replies are broadcast. Older senders leave the target at 0.
    using TargetComponent = decltype(timesync.target_component);
    if (timesync.target_component != 0 &&
        timesync.target_component !=
            static_cast<TargetComponent>(_system_impl.get_own_component_id())) {
        return;
    }

    int64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                         _system_impl.get_autopilot_time().now().time_since_epoch())
                         .count();

    if (timesync.tc1 == 0 && _autopilot_timesync_acquired) {
        // Send synced time to remote system
        send_timesync(now_ns, timesync.ts1, message.sysid, message.compid);
    } else if (timesync.tc1 > 0) {
        if (message.compid == MAV_COMP_ID_AUTOPILOT1) {
            add_rtt_sample(timesync.ts1);
        }

        if (!_is_enabled) {
            return;
        }

        // Time offset between this system and the remote system is calculated assuming RTT for
        // the timesync packet is roughly equal both ways.
        set_timesync_offset((timesync.tc1 * 2 - (timesync.ts1 + now_ns)) / 2, timesync.ts1);
    }
}

void Timesync::remember_request(int64_t ts1)
{
    std::lock_guard<std::mutex> lock(_requests_mutex);
    _requests[_next_request] = Request{ts1, std::chrono::steady_clock::now()};
    _next_request = (_next_request + 1) % _requests.size();
}

void Timesync::add_rtt_sample(int64_t ts1)
{
    std::chrono::steady_clock::duration rtt{};
    {
        std::lock_guard<std::mutex> lock(_requests_mutex);
        auto it = std::find_if(_requests.begin(), _requests.end(), [ts1](const Request& request) {
            return request.ts1 != 0 && request.ts1 == ts1;
        });
        if (it == _requests.end()) {
            // Not one of ours, e.g. the answer to another ground station on the same link
            // that does not set the target, or a duplicate.
            return;
        }
        rtt = std::chrono::steady_clock::now() - it->sent;
        *it = Request{};
    }

    _rtt_estimator.add_sample_s(std::chrono::duration<double>(rtt).count());
}

void Timesync::send_timesync(
    uint64_t tc1, uint64_t ts1, uint32_t target_system_id, uint8_t target_component_id)
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
            target_system_id,
            target_component_id);
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

        update_status(true, static_cast<int64_t>(rtt_ns));
    } else {
        // Increment counter if round trip time is too high for accurate timesync
        _high_rtt_count++;

        if (_high_rtt_count > MAX_CONS_HIGH_RTT) {
            // Issue a warning to the user if the RTT is constantly high
            LogWarn("RTT too high for timesync: {} ms.", rtt_ns / 1000000ULL);

            // Reset counter
            _high_rtt_count = 0;
        }

        update_status(_autopilot_timesync_acquired, static_cast<int64_t>(rtt_ns));
    }
}

} // namespace mavsdk
