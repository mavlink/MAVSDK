#include "log_streaming_backend_ardupilot.h"

// ArduPilot uses REMOTE_LOG_DATA_BLOCK (ID 184) for real-time log streaming.
// This is different from PX4's LOGGING_DATA protocol.
//
// Protocol:
// - GCS sends REMOTE_LOG_BLOCK_STATUS with seqno = MAV_REMOTE_LOG_DATA_BLOCK_START to start
// - GCS sends REMOTE_LOG_BLOCK_STATUS with seqno = MAV_REMOTE_LOG_DATA_BLOCK_STOP to stop
// - Autopilot streams REMOTE_LOG_DATA_BLOCK messages with 200 bytes of DataFlash data
// - GCS should ACK each block with REMOTE_LOG_BLOCK_STATUS

namespace mavsdk {

void LogStreamingBackendArdupilot::init(SystemImpl* system_impl)
{
    _system_impl = system_impl;

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK,
        [this](const mavlink_message_t& message) { process_remote_log_data_block(message); },
        this);
}

void LogStreamingBackendArdupilot::deinit()
{
    auto is_active = [this]() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _active;
    }();

    if (is_active) {
        // We try to stop log streaming before we leave but without waiting for a result.
        stop_log_streaming_async(nullptr);
    }

    if (_system_impl) {
        _system_impl->unregister_all_mavlink_message_handlers(this);
    }
}

void LogStreamingBackendArdupilot::set_data_callback(DataCallback callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _data_callback = std::move(callback);
}

void LogStreamingBackendArdupilot::set_debugging(bool debugging)
{
    _debugging = debugging;
}

void LogStreamingBackendArdupilot::start_log_streaming_async(
    const LogStreaming::ResultCallback& callback)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _active = true;
        _first_block_received = false;
        _last_seqno = 0;
    }

    // Send START command via REMOTE_LOG_BLOCK_STATUS
    send_remote_log_block_status(MAV_REMOTE_LOG_DATA_BLOCK_START, MAV_REMOTE_LOG_DATA_BLOCK_ACK);

    if (_debugging) {
        LogDebug() << "Sent REMOTE_LOG_BLOCK_STATUS with seqno=START to "
                   << static_cast<int>(_system_impl->get_system_id()) << "/"
                   << static_cast<int>(_system_impl->get_autopilot_id());
    }

    // ArduPilot doesn't acknowledge the start command,
    // so we assume success immediately
    if (callback) {
        _system_impl->call_user_callback([callback]() { callback(LogStreaming::Result::Success); });
    }
}

void LogStreamingBackendArdupilot::stop_log_streaming_async(
    const LogStreaming::ResultCallback& callback)
{
    // Send STOP command via REMOTE_LOG_BLOCK_STATUS
    send_remote_log_block_status(MAV_REMOTE_LOG_DATA_BLOCK_STOP, MAV_REMOTE_LOG_DATA_BLOCK_ACK);

    if (_debugging) {
        LogDebug() << "Sent REMOTE_LOG_DATA_BLOCK_STOP";
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _active = false;
    }

    // ArduPilot doesn't acknowledge the stop command,
    // so we assume success immediately
    if (callback) {
        _system_impl->call_user_callback([callback]() { callback(LogStreaming::Result::Success); });
    }
}

void LogStreamingBackendArdupilot::send_remote_log_block_status(uint32_t seqno, uint8_t status)
{
    auto target_sysid = _system_impl->get_system_id();
    auto target_compid = _system_impl->get_autopilot_id();

    if (_debugging) {
        LogDebug() << "Sending REMOTE_LOG_BLOCK_STATUS: seqno=" << seqno
                   << " status=" << static_cast<int>(status)
                   << " target=" << static_cast<int>(target_sysid) << "/"
                   << static_cast<int>(target_compid);
    }

    _system_impl->queue_message([target_sysid, target_compid, seqno, status](
                                    MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_remote_log_block_status_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            target_sysid,
            target_compid,
            seqno,
            status);
        return message;
    });
}

void LogStreamingBackendArdupilot::process_remote_log_data_block(const mavlink_message_t& message)
{
    if (_debugging) {
        LogDebug() << "Received REMOTE_LOG_DATA_BLOCK from " << static_cast<int>(message.sysid)
                   << "/" << static_cast<int>(message.compid);
    }

    std::lock_guard<std::mutex> lock(_mutex);

    if (!_active) {
        if (_debugging) {
            LogDebug() << "Ignoring remote log data block because we're not active";
        }
        return;
    }

    mavlink_remote_log_data_block_t block;
    mavlink_msg_remote_log_data_block_decode(&message, &block);

    // Check if this message is targeted at us
    if (block.target_system != _system_impl->get_own_system_id() ||
        block.target_component != _system_impl->get_own_component_id()) {
        if (_debugging) {
            LogDebug() << "Remote log data block with wrong target "
                       << std::to_string(block.target_system) << '/'
                       << std::to_string(block.target_component) << " instead of "
                       << std::to_string(_system_impl->get_own_system_id()) << '/'
                       << std::to_string(_system_impl->get_own_component_id());
        }
        return;
    }

    if (_debugging) {
        LogDebug() << "Received remote log data block with seqno: " << block.seqno;
    }

    // Check for dropped blocks
    if (_first_block_received) {
        uint32_t expected_seqno = _last_seqno + 1;
        if (block.seqno != expected_seqno) {
            uint32_t dropped = block.seqno - expected_seqno;
            if (_debugging) {
                LogDebug() << "Dropped " << dropped << " blocks (expected " << expected_seqno
                           << ", got " << block.seqno << ")";
            }
            // We could potentially NACK the missing blocks to request retransmission,
            // but for now we just note the drop and continue
        }
    } else {
        _first_block_received = true;
    }
    _last_seqno = block.seqno;

    // Send ACK for this block
    send_remote_log_block_status(block.seqno, MAV_REMOTE_LOG_DATA_BLOCK_ACK);

    // The data block is always 200 bytes (fixed size in the message definition)
    std::vector<uint8_t> data(block.data, block.data + sizeof(block.data));

    // Pass data to the user via the callback
    if (_data_callback) {
        _system_impl->call_user_callback([this, data]() { _data_callback(data); });
    }
}

} // namespace mavsdk
