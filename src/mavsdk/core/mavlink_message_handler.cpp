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
    std::lock_guard<std::mutex> lock(_mutex);

    Entry entry = {msg_id, {}, callback, cookie};
    _table.push_back(entry);
}

void MavlinkMessageHandler::register_one_with_component_id(
    uint16_t msg_id, uint8_t component_id, const Callback& callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    Entry entry = {msg_id, component_id, callback, cookie};
    _table.push_back(entry);
}

void MavlinkMessageHandler::unregister_one(uint16_t msg_id, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto it = _table.begin(); it != _table.end();
         /* no ++it */) {
        if (it->msg_id == msg_id && it->cookie == cookie) {
            it = _table.erase(it);
        } else {
            ++it;
        }
    }
}

void MavlinkMessageHandler::unregister_all(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto it = _table.begin(); it != _table.end();
         /* no ++it */) {
        if (it->cookie == cookie) {
            it = _table.erase(it);
        } else {
            ++it;
        }
    }
}

void MavlinkMessageHandler::process_message(const mavlink_message_t& message)
{
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
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto& entry : _table) {
        if (entry.msg_id == msg_id && entry.cookie == cookie) {
            entry.component_id = component_id;
        }
    }
}

} // namespace mavsdk
