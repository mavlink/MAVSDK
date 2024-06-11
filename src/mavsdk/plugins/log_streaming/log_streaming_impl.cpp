#include <future>
#include <limits>

#include "log_streaming_impl.h"
#include "plugins/log_streaming/log_streaming.h"
#include "callback_list.tpp"
#include "base64.h"

// Note: the implementation below is mostly inspired by the pymavlink
//       implementation written by Beat Küng:
//       https://github.com/PX4/PX4-Autopilot/blob/main/Tools/mavlink_ulog_streaming.py
//
// For more information about the ulog protocol, visit:
// https://docs.px4.io/main/en/dev_log/ulog_file_format.html

namespace mavsdk {

template class CallbackList<LogStreaming::LogStreamingRaw>;

LogStreamingImpl::LogStreamingImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

LogStreamingImpl::LogStreamingImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

LogStreamingImpl::~LogStreamingImpl()
{
    _system_impl->unregister_plugin(this);
}

void LogStreamingImpl::init()
{
    if (const char* env_p = std::getenv("MAVSDK_LOG_STREAMING_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Log streaming debugging is on.";
            _debugging = true;
        }
    }

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOGGING_DATA,
        [this](const mavlink_message_t& message) { process_logging_data(message); },
        this);
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOGGING_DATA_ACKED,
        [this](const mavlink_message_t& message) { process_logging_data_acked(message); },
        this);
}

void LogStreamingImpl::deinit()
{
    auto is_active = [this]() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _active;
    }();

    if (is_active) {
        // We try to stop log streaming before we leave but without waiting for a result.
        stop_log_streaming_async(nullptr);
    }
}

void LogStreamingImpl::enable() {}

void LogStreamingImpl::disable() {}

void LogStreamingImpl::start_log_streaming_async(const LogStreaming::ResultCallback& callback)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        reset();
        _active = true;
    }

    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_LOGGING_START;
    command.params.maybe_param1 = 0.0f; // ulog

    _system_impl->send_command_async(command, [=](MavlinkCommandSender::Result result, float) {
        if (result != MavlinkCommandSender::Result::InProgress) {
            if (callback) {
                _system_impl->call_user_callback(
                    [=]() { callback(log_streaming_result_from_command_result(result)); });
            }
        }
    });
}

LogStreaming::Result LogStreamingImpl::start_log_streaming()
{
    auto prom = std::promise<LogStreaming::Result>{};
    auto fut = prom.get_future();

    start_log_streaming_async([&](LogStreaming::Result result) { prom.set_value(result); });

    return fut.get();
}

void LogStreamingImpl::stop_log_streaming_async(const LogStreaming::ResultCallback& callback)
{
    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_LOGGING_STOP;

    _system_impl->send_command_async(command, [=](MavlinkCommandSender::Result result, float) {
        if (result != MavlinkCommandSender::Result::InProgress) {
            if (callback) {
                _system_impl->call_user_callback(
                    [=]() { callback(log_streaming_result_from_command_result(result)); });
            }
        }
    });

    std::lock_guard<std::mutex> lock(_mutex);
    reset();
    _active = false;
}

LogStreaming::Result LogStreamingImpl::stop_log_streaming()
{
    auto prom = std::promise<LogStreaming::Result>{};
    auto fut = prom.get_future();

    stop_log_streaming_async([&](LogStreaming::Result result) { prom.set_value(result); });

    return fut.get();
}

LogStreaming::LogStreamingRawHandle
LogStreamingImpl::subscribe_log_streaming_raw(const LogStreaming::LogStreamingRawCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto handle = _subscription_callbacks.subscribe(callback);

    return handle;
}

void LogStreamingImpl::unsubscribe_log_streaming_raw(LogStreaming::LogStreamingRawHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _subscription_callbacks.unsubscribe(handle);
}

void LogStreamingImpl::reset()
{
    // Assume we have lock

    _current_sequence = 0;
    _drop_state = DropState::Unknown;
}

void LogStreamingImpl::process_logging_data(const mavlink_message_t& message)
{
    if (!_active) {
        if (_debugging) {
            LogDebug() << "Ignoring logging data because we're not active";
        }
        return;
    }

    mavlink_logging_data_t logging_data;
    mavlink_msg_logging_data_decode(&message, &logging_data);

    if (logging_data.target_system != _system_impl->get_own_system_id() ||
        logging_data.target_component != _system_impl->get_own_component_id()) {
        LogWarn() << "Logging data with wrong target " << std::to_string(logging_data.target_system)
                  << '/' << std::to_string(logging_data.target_component) << " instead of "
                  << std::to_string(_system_impl->get_own_system_id()) << '/'
                  << std::to_string(_system_impl->get_own_component_id());
        return;
    }

    if (_debugging) {
        LogDebug() << "Received logging data with len: " << std::to_string(logging_data.length)

                   << ", first message: " << std::to_string(logging_data.first_message_offset)
                   << ", sequence: " << logging_data.sequence;
    }

    if (logging_data.length > sizeof(logging_data.data)) {
        LogWarn() << "Invalid length";
        return;
    }

    if (logging_data.first_message_offset != 255 &&
        logging_data.first_message_offset > sizeof(logging_data.data)) {
        LogWarn() << "Invalid first_message_offset";
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    if (is_duplicate(logging_data.sequence)) {
        return;
    }
    check_drop_state(logging_data.sequence, logging_data.first_message_offset);

    switch (_drop_state) {
        case DropState::Ok:
            // All data can be used.
            if (logging_data.first_message_offset == 255) {
                // This is just part that we add for now.
                _ulog_data.insert(
                    _ulog_data.end(), logging_data.data, logging_data.data + logging_data.length);

            } else {
                // Finish the previous message.
                _ulog_data.insert(
                    _ulog_data.end(),
                    logging_data.data,
                    logging_data.data + logging_data.first_message_offset);
                process_message();

                _ulog_data.clear();
                // Then start the next one.
                _ulog_data.insert(
                    _ulog_data.end(),
                    logging_data.data + logging_data.first_message_offset,
                    logging_data.data + logging_data.length);
            }
            break;

        case DropState::Dropped:
            // Nothing to do with the partial message.
            _ulog_data.clear();
            break;

        case DropState::RecoveringFromDropped:
            // Nothing to do with any partial message.
            _ulog_data.clear();
            // Now start fresh.
            _ulog_data.insert(
                _ulog_data.end(),
                logging_data.data + logging_data.first_message_offset,
                logging_data.data + logging_data.length);
            break;
        case DropState::Unknown:
            LogErr() << "Logical error";
            break;
    }
}

void LogStreamingImpl::process_logging_data_acked(const mavlink_message_t& message)
{
    if (!_active) {
        if (_debugging) {
            LogDebug() << "Ignoring logging data acked because we're not active";
        }
        return;
    }

    mavlink_logging_data_acked_t logging_data_acked;
    mavlink_msg_logging_data_acked_decode(&message, &logging_data_acked);

    if (logging_data_acked.target_system != _system_impl->get_own_system_id() ||
        logging_data_acked.target_component != _system_impl->get_own_component_id()) {
        LogWarn() << "Logging data acked with wrong target "
                  << std::to_string(logging_data_acked.target_system) << '/'
                  << std::to_string(logging_data_acked.target_component) << " instead of "
                  << std::to_string(_system_impl->get_own_system_id()) << '/'
                  << std::to_string(_system_impl->get_own_component_id());
        return;
    }

    _system_impl->queue_message(
        [target_system = message.sysid,
         target_component = message.compid,
         sequence = logging_data_acked.sequence](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t response_message;
            mavlink_msg_logging_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &response_message,
                target_system,
                target_component,
                sequence);
            return response_message;
        });

    if (_debugging) {
        LogInfo() << "Received logging data acked with len: "
                  << std::to_string(logging_data_acked.length)
                  << ", first message: " << std::to_string(logging_data_acked.first_message_offset)
                  << ", sequence: " << logging_data_acked.sequence;
    }

    if (logging_data_acked.length > sizeof(logging_data_acked.data)) {
        LogWarn() << "Invalid length";
        return;
    }

    if (logging_data_acked.first_message_offset != 255 &&
        logging_data_acked.first_message_offset > sizeof(logging_data_acked.data)) {
        LogWarn() << "Invalid first_message_offset";
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    if (is_duplicate(logging_data_acked.sequence)) {
        return;
    }
    check_drop_state(logging_data_acked.sequence, logging_data_acked.first_message_offset);

    switch (_drop_state) {
        case DropState::Ok:
            // All data can be used.
            if (logging_data_acked.first_message_offset == 255) {
                // This is just part that we add for now.
                _ulog_data.insert(
                    _ulog_data.end(),
                    logging_data_acked.data,
                    logging_data_acked.data + logging_data_acked.length);

            } else {
                // Finish the previous message.
                _ulog_data.insert(
                    _ulog_data.end(),
                    logging_data_acked.data,
                    logging_data_acked.data + logging_data_acked.first_message_offset);
                process_message();

                _ulog_data.clear();
                // Then start the next one.
                _ulog_data.insert(
                    _ulog_data.end(),
                    logging_data_acked.data + logging_data_acked.first_message_offset,
                    logging_data_acked.data + logging_data_acked.length);
            }
            break;

        case DropState::Dropped:
            // Nothing to do with the partial message.
            _ulog_data.clear();
            break;

        case DropState::RecoveringFromDropped:
            // Nothing to do with any partial message.
            _ulog_data.clear();
            // Now start fresh.
            _ulog_data.insert(
                _ulog_data.end(),
                logging_data_acked.data + logging_data_acked.first_message_offset,
                logging_data_acked.data + logging_data_acked.length);
            break;
        case DropState::Unknown:
            LogErr() << "Logical error";
            break;
    }
}

bool LogStreamingImpl::is_duplicate(uint16_t sequence) const
{
    // Assume we have lock
    return _drop_state != DropState::Unknown && sequence == _current_sequence;
}

void LogStreamingImpl::check_drop_state(uint16_t sequence, uint8_t first_message_offset)
{
    // Assume we have lock.

    switch (_drop_state) {
        case DropState::Dropped:
            if (first_message_offset != 255) {
                // This is the first time we use the sequence.
                _current_sequence = sequence;
                _drop_state = DropState::RecoveringFromDropped;
            } else {
                _drop_state = DropState::Dropped;
            }
            break;

        case DropState::Unknown:
            _drop_state = DropState::Ok;
            _current_sequence = sequence;
            break;
        case DropState::Ok:
        case DropState::RecoveringFromDropped:
            uint16_t drop;
            if (sequence > _current_sequence) {
                // No wrap around.
                drop = (sequence - 1 - _current_sequence);
                _drops += drop;
                if (drop > 0 && _debugging) {
                    LogDebug() << "Dropped: " << drop << " (no wrap around), overall: " << _drops;
                }

            } else {
                // Wrap around!
                drop = (sequence + std::numeric_limits<uint16_t>::max() - 1 - _current_sequence);
                _drops += drop;
                if (drop > 0 && _debugging) {
                    LogDebug() << "Dropped: " << drop << " (with wrap around), overall: " << _drops;
                }
            }

            _current_sequence = sequence;

            if (drop > 0) {
                if (first_message_offset == 255) {
                    _drop_state = DropState::Dropped;
                } else {
                    _drop_state = DropState::RecoveringFromDropped;
                }
            } else {
                _drop_state = DropState::Ok;
            }
            break;
    }
}

void LogStreamingImpl::process_message()
{
    // Assumes lock.

    if (_debugging) {
        LogDebug() << "Processing ulog message with size " << _ulog_data.size();
    }

    // We don't check the magic and version. That's up to the log viewer to parse.

    // Convert to base64
    LogStreaming::LogStreamingRaw part;
    part.data_base64 = base64_encode(_ulog_data);

    // Let's pass it to the user.
    if (!_subscription_callbacks.empty()) {
        _subscription_callbacks.queue(
            part, [this](const auto& func) { _system_impl->call_user_callback(func); });
    }
}

LogStreaming::Result
LogStreamingImpl::log_streaming_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return LogStreaming::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return LogStreaming::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return LogStreaming::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return LogStreaming::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            return LogStreaming::Result::CommandDenied;
        case MavlinkCommandSender::Result::Unsupported:
            return LogStreaming::Result::Unsupported;
        case MavlinkCommandSender::Result::Timeout:
            return LogStreaming::Result::Timeout;
        case MavlinkCommandSender::Result::InProgress:
        case MavlinkCommandSender::Result::TemporarilyRejected:
        case MavlinkCommandSender::Result::Failed:
        case MavlinkCommandSender::Result::Cancelled:
        case MavlinkCommandSender::Result::UnknownError:
        default:
            return LogStreaming::Result::Unknown;
    }
}

} // namespace mavsdk
