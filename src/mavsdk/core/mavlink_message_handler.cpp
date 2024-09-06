#include <algorithm>
#include <mutex>
#include "mavlink_message_handler.h"
#include "log.h"

namespace mavsdk {

MavlinkMessageHandler::MavlinkMessageHandler()
{
    if (const char* env_p = std::getenv("MAVSDK_MESSAGE_HANDLER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Mavlink message handler debugging is on.";
            _debugging = true;
        }
    }
}

void MavlinkMessageHandler::register_one(
    uint16_t msg_id, const Callback& callback, const void* cookie)
{
    register_one_impl(msg_id, {}, callback, cookie);
}

void MavlinkMessageHandler::register_one_with_component_id(
    uint16_t msg_id, uint8_t component_id, const Callback& callback, const void* cookie)
{
    register_one_impl(msg_id, {component_id}, callback, cookie);
}

void MavlinkMessageHandler::register_one_impl(
    uint16_t msg_id,
    std::optional<uint8_t> maybe_component_id,
    const Callback& callback,
    const void* cookie)
{
    Entry entry = {msg_id, maybe_component_id, callback, cookie};

    std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
    if (lock.owns_lock()) {
        _table.push_back(entry);
    } else {
        std::lock_guard<std::mutex> register_later_lock(_register_later_mutex);
        _register_later_table.push_back(entry);
    }
}

void MavlinkMessageHandler::unregister_one(uint16_t msg_id, const void* cookie)
{
    unregister_impl({msg_id}, cookie);
}

void MavlinkMessageHandler::unregister_all(const void* cookie)
{
    unregister_impl({}, cookie);
}

void MavlinkMessageHandler::unregister_impl(
    std::optional<uint16_t> maybe_msg_id, const void* cookie)
{
    std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
    if (lock.owns_lock()) {
        _table.erase(
            std::remove_if(
                _table.begin(),
                _table.end(),
                [&](auto& entry) {
                    if (maybe_msg_id) {
                        return (entry.msg_id == maybe_msg_id.value() && entry.cookie == cookie);
                    } else {
                        return (entry.cookie == cookie);
                    }
                }),
            _table.end());
    } else {
        std::lock_guard<std::mutex> unregister_later_lock(_unregister_later_mutex);
        _unregister_later_table.push_back(UnregisterEntry{maybe_msg_id, cookie});
    }
}

void MavlinkMessageHandler::check_register_later()
{
    std::lock_guard<std::mutex> _register_later_lock(_register_later_mutex);

    // We could probably just grab the lock here, but it's safer not to
    // acquire both locks to avoid deadlocks.
    std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        // Try again later.
        return;
    }

    for (const auto& entry : _register_later_table) {
        _table.push_back(entry);
    }

    _register_later_table.clear();
}

void MavlinkMessageHandler::check_unregister_later()
{
    std::lock_guard<std::mutex> _unregister_later_lock(_unregister_later_mutex);

    // We could probably just grab the lock here, but it's safer not to
    // acquire both locks to avoid deadlocks.
    std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        // Try again later.
        return;
    }

    for (const auto& unregister_entry : _unregister_later_table) {
        _table.erase(
            std::remove_if(
                _table.begin(),
                _table.end(),
                [&](auto& entry) {
                    if (unregister_entry.maybe_msg_id) {
                        return (
                            entry.msg_id == unregister_entry.maybe_msg_id.value() &&
                            entry.cookie == unregister_entry.cookie);
                    } else {
                        return (entry.cookie == unregister_entry.cookie);
                    }
                }),
            _table.end());
    }

    _unregister_later_table.clear();
}

void MavlinkMessageHandler::process_message(const mavlink_message_t& message)
{
    check_register_later();
    check_unregister_later();

    std::lock_guard<std::mutex> lock(_mutex);

    bool forwarded = false;

    if (_debugging) {
        LogDebug() << "Table entries: ";
    }

    for (auto& entry : _table) {
        if (_debugging) {
            LogDebug() << "Msg id: " << entry.msg_id << ", component id: "
                       << (entry.component_id.has_value() ?
                               std::to_string(entry.component_id.value()) :
                               "none");
        }

        if (entry.msg_id == message.msgid &&
            (!entry.component_id.has_value() || entry.component_id.value() == message.compid)) {
            if (_debugging) {
                LogDebug() << "Using msg " << int(message.msgid) << " to " << size_t(entry.cookie);
            }

            forwarded = true;
            entry.callback(message);
        }
    }

    if (_debugging && !forwarded) {
        LogDebug() << "Ignoring msg " << int(message.msgid);
    }
}

void MavlinkMessageHandler::update_component_id(
    uint16_t msg_id, uint8_t component_id, const void* cookie)
{
    check_register_later();
    check_unregister_later();

    std::lock_guard<std::mutex> lock(_mutex);

    for (auto& entry : _table) {
        if (entry.msg_id == msg_id && entry.cookie == cookie) {
            entry.component_id = component_id;
        }
    }
}

} // namespace mavsdk
