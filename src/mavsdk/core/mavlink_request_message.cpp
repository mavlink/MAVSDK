
#include "log.h"
#include "mavlink_request_message.h"
#include "system_impl.h"
#include <cstdint>
#include <string>

namespace mavsdk {

MavlinkRequestMessage::MavlinkRequestMessage(
    SystemImpl& system_impl,
    MavlinkCommandSender& command_sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler) :
    _system_impl(system_impl),
    _command_sender(command_sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler)
{}

void MavlinkRequestMessage::request(
    uint32_t message_id,
    uint8_t target_component,
    MavlinkRequestMessageCallback callback,
    uint32_t param2)
{
    std::unique_lock<std::mutex> lock(_mutex);

    // Cleanup previous requests.
    for (const auto id : _deferred_message_cleanup) {
        _message_handler.unregister_one(id, this);
    }
    _deferred_message_cleanup.clear();

    // Respond with 'Busy' if already in progress.
    for (auto& item : _work_items) {
        if (item.message_id == message_id && item.param2 == param2) {
            lock.unlock();
            if (callback) {
                callback(MavlinkCommandSender::Result::Busy, {});
            }
            return;
        }
    }

    // Otherwise, schedule it.
    _work_items.emplace_back(WorkItem{message_id, target_component, callback, param2});

    // Register for message
    _message_handler.register_one(
        message_id,
        [this](const mavlink_message_t& message) { handle_any_message(message); },
        this);

    // And send off command
    send_request(_work_items.back());
}

void MavlinkRequestMessage::send_request(WorkItem& item)
{
    // Every second time, starting with first retry, we try the old request commands.
    // If no old commands exist, we of course just send the new one again.
    if (item.retries % 2 != 0) {
        if (!try_sending_request_using_old_command(item)) {
            send_request_using_new_command(item);
        }
    } else {
        send_request_using_new_command(item);
    }
}

void MavlinkRequestMessage::send_request_using_new_command(WorkItem& item)
{
    if (item.retries > 0) {
        LogWarn() << "Request message " << item.message_id
                  << " again using REQUEST_MESSAGE (retries: " << item.retries << ")";
    } else {
        LogDebug() << "Request message " << item.message_id << " using REQUEST_MESSAGE";
    }

    MavlinkCommandSender::CommandLong command_request_message{};
    command_request_message.command = MAV_CMD_REQUEST_MESSAGE;
    command_request_message.target_system_id = _system_impl.get_system_id();
    command_request_message.target_component_id = item.target_component;
    command_request_message.params.maybe_param1 = {static_cast<float>(item.message_id)};
    _command_sender.queue_command_async(
        command_request_message,
        [this, message_id = item.message_id](MavlinkCommandSender::Result result, float) {
            if (result != MavlinkCommandSender::Result::InProgress) {
                handle_command_result(message_id, result);
            }
        });
}

bool MavlinkRequestMessage::try_sending_request_using_old_command(WorkItem& item)
{
    MavlinkCommandSender::CommandLong command_request_message{};
    command_request_message.target_system_id = _system_impl.get_system_id();
    command_request_message.target_component_id = item.target_component;

    std::string command_name;

    switch (item.message_id) {
        case MAVLINK_MSG_ID_AUTOPILOT_VERSION:
            command_request_message.command = MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES;
            command_request_message.params.maybe_param1 = {1.f};
            command_name = "REQUEST_AUTOPILOT_CAPABILITIES";
            break;

        case MAVLINK_MSG_ID_CAMERA_INFORMATION:
            command_request_message.command = MAV_CMD_REQUEST_CAMERA_INFORMATION;
            command_request_message.params.maybe_param1 = {1.0f};
            command_name = "REQUEST_CAMERA_INFORMATION";
            break;

        case MAVLINK_MSG_ID_CAMERA_SETTINGS:
            command_request_message.command = MAV_CMD_REQUEST_CAMERA_SETTINGS;
            command_request_message.params.maybe_param1 = {1.0f};
            command_name = "REQUEST_CAMERA_SETTINGS";

            break;

        case MAVLINK_MSG_ID_STORAGE_INFORMATION:
            command_request_message.command = MAV_CMD_REQUEST_STORAGE_INFORMATION;
            command_request_message.params.maybe_param1 = {
                static_cast<float>(item.param2)}; // storage ID
            command_request_message.params.maybe_param2 = {1.0f};
            command_name = "REQUEST_STORAGE_INFORMATION";
            break;

        case MAVLINK_MSG_ID_CAMERA_CAPTURE_STATUS:
            command_request_message.command = MAV_CMD_REQUEST_CAMERA_CAPTURE_STATUS;
            command_request_message.params.maybe_param1 = {1.0f};
            command_name = "REQUEST_CAMERA_CAPTURE_STATUS";
            break;

        case MAVLINK_MSG_ID_FLIGHT_INFORMATION:
            command_request_message.command = MAV_CMD_REQUEST_FLIGHT_INFORMATION;
            command_request_message.params.maybe_param1 = {1.0f};
            command_name = "REQUEST_FLIGHT_INFORMATION";
            break;

        case MAVLINK_MSG_ID_VIDEO_STREAM_STATUS:
            command_request_message.command = MAV_CMD_REQUEST_VIDEO_STREAM_STATUS;
            command_request_message.params.maybe_param1 = {
                static_cast<float>(item.param2)}; // stream ID
            command_name = "REQUEST_VIDEO_STREAM_STATUS";
            break;

        case MAVLINK_MSG_ID_VIDEO_STREAM_INFORMATION:
            command_request_message.command = MAV_CMD_REQUEST_VIDEO_STREAM_INFORMATION;
            command_request_message.params.maybe_param1 = {
                static_cast<float>(item.param2)}; // stream ID
            command_name = "REQUEST_VIDEO_STREAM_INFORMATION";
            break;

        case MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED:
            command_request_message.command = MAV_CMD_REQUEST_CAMERA_IMAGE_CAPTURE;
            command_request_message.params.maybe_param1 = {
                static_cast<float>(item.param2)}; // sequence number
            command_name = "REQUEST_CAMERA_IMAGE_CAPTURE";
            break;

        default:
            // No alternative command for this message.
            return false;
    }

    LogWarn() << "Request message " << item.message_id << " again using " << command_name
              << " (retries: " << item.retries << ")";

    _command_sender.queue_command_async(
        command_request_message,
        [this, message_id = item.message_id](MavlinkCommandSender::Result result, float) {
            if (result != MavlinkCommandSender::Result::InProgress) {
                handle_command_result(message_id, result);
            }
        });

    return true;
}

void MavlinkRequestMessage::handle_any_message(const mavlink_message_t& message)
{
    std::unique_lock<std::mutex> lock(_mutex);

    for (auto it = _work_items.begin(); it != _work_items.end(); ++it) {
        // Check if we're waiting for this message.
        // TODO: check if params are correct.
        if (it->message_id != message.msgid && it->target_component == message.compid) {
            continue;
        }

        _timeout_handler.remove(it->timeout_cookie);
        // We have at least the message which is what we care about most, so we
        // tell the user about it and call it a day. If we receive the result
        // of the command later, we ignore it, and we fake the result to be successful
        // anyway.
        auto temp_callback = it->callback;
        // We can now get rid of the entry now.
        _work_items.erase(it);
        // We have to clean up later outside this callback, otherwise we lock ourselves out.
        _deferred_message_cleanup.push_back(message.msgid);
        lock.unlock();

        if (temp_callback) {
            temp_callback(MavlinkCommandSender::Result::Success, message);
        }
        return;
    }
}

void MavlinkRequestMessage::handle_command_result(
    uint32_t message_id, MavlinkCommandSender::Result result)
{
    std::unique_lock<std::mutex> lock(_mutex);

    for (auto it = _work_items.begin(); it != _work_items.end(); ++it) {
        // Check if we're waiting for this result
        if (it->message_id != message_id) {
            continue;
        }

        switch (result) {
            case MavlinkCommandSender::Result::Success:
                // This is promising, let's hope the message will actually arrive.
                // We'll set a timeout in case we need to retry.
                it->timeout_cookie = _timeout_handler.add(
                    [this, message_id, target_component = it->target_component]() {
                        handle_timeout(message_id, target_component);
                    },
                    1.0);
                return;

            case MavlinkCommandSender::Result::NoSystem:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::ConnectionError:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::Busy:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::Denied:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::Unsupported:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::Timeout:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::TemporarilyRejected:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::Failed:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::Cancelled:
                // FALLTHROUGH
            case MavlinkCommandSender::Result::UnknownError: {
                // It looks like this did not work, and we can report the error
                // No need to try again.
                auto temp_callback = it->callback;
                _message_handler.unregister_one(it->message_id, this);
                _work_items.erase(it);
                lock.unlock();
                if (temp_callback) {
                    temp_callback(result, {});
                }
                return;
            }

            case MavlinkCommandSender::Result::InProgress:
                // Should not happen, as it is already filtered out.
                return;
        }
    }
}

void MavlinkRequestMessage::handle_timeout(uint32_t message_id, uint8_t target_component)
{
    std::unique_lock<std::mutex> lock(_mutex);

    for (auto it = _work_items.begin(); it != _work_items.end(); ++it) {
        // Check if we're waiting for this result
        if (it->message_id != message_id || it->target_component != target_component) {
            continue;
        }

        if (++it->retries > 3) {
            // We have already retried, let's give up.
            auto temp_callback = it->callback;
            _message_handler.unregister_one(it->message_id, this);
            _work_items.erase(it);
            lock.unlock();
            if (temp_callback) {
                temp_callback(MavlinkCommandSender::Result::Timeout, {});
            }
            return;
        }

        send_request(*it);
    }
}

} // namespace mavsdk
