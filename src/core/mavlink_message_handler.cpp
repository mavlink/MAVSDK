#include <mutex>
#include "mavlink_message_handler.h"

namespace mavsdk {

void MAVLinkMessageHandler::register_one(
    uint16_t msg_id, Callback callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    Entry entry = {msg_id, callback, cookie};
    _table.push_back(entry);
}

void MAVLinkMessageHandler::unregister_one(uint16_t msg_id, const void* cookie)
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

void MAVLinkMessageHandler::unregister_all(const void* cookie)
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

void MAVLinkMessageHandler::process_message(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

#if MESSAGE_DEBUGGING == 1
    bool forwarded = false;
#endif
    for (auto it = _table.begin(); it != _table.end(); ++it) {
        if (it->msg_id == message.msgid) {
#if MESSAGE_DEBUGGING == 1
            LogDebug() << "Forwarding msg " << int(message.msgid) << " to " << size_t(it->cookie);
            forwarded = true;
#endif
            it->callback(message);
        }
    }

#if MESSAGE_DEBUGGING == 1
    if (!forwarded) {
        LogDebug() << "Ignoring msg " << int(message.msgid);
    }
#endif
}

} // namespace mavsdk
