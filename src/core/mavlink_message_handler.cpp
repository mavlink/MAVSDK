#include <mutex>
#include "mavlink_message_handler.h"

namespace mavsdk {

void MAVLinkMessageHandler::register_one(
    uint16_t msg_id, const Callback& callback, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_map.count(msg_id) == 0) {
        _map.emplace(std::make_pair(msg_id, std::vector<MAVLinkMessageHandler::Entry>()));
    }

    Entry entry{callback, cookie};
    _map[msg_id].push_back(entry);
}

void MAVLinkMessageHandler::unregister_one(uint16_t msg_id, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_map.count(msg_id) == 0) return;

    auto& _table = _map[msg_id];
    for (auto it = _table.begin(); it != _table.end();
         /* no ++it */) {
        if (it->cookie == cookie) {
            it = _table.erase(it);
        } else {
            ++it;
        }
    }

    if (_table.size() == 0) {
        _map.erase(msg_id);
    }
}

void MAVLinkMessageHandler::unregister_all(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto map_it = _map.begin(); map_it != _map.end(); /* no ++map_it */) {
	auto& _table = map_it->second;

        for (auto it = _table.begin(); it != _table.end();
             /* no ++it */) {
            if (it->cookie == cookie) {
                it = _table.erase(it);
            } else {
                ++it;
            }
        }

        if (_table.size() == 0) {
            map_it = _map.erase(map_it);
        } else {
            ++map_it;
	}
    }
}

void MAVLinkMessageHandler::process_message(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_map.count(message.msgid) == 0) return;
#if MESSAGE_DEBUGGING == 1
    bool processed = false;
#endif

    auto& _table = _map[message.msgid];
    for (auto& entry : _table) {
#if MESSAGE_DEBUGGING == 1
        LogDebug() << "Forwarding msg " << int(message.msgid) << " to "
                   << size_t(entry->cookie);
        processed = true;
#endif
        entry.callback(message);
    }

#if MESSAGE_DEBUGGING == 1
    if (!processed) {
        LogDebug() << "Ignoring msg " << int(message.msgid);
    }
#endif
}

} // namespace mavsdk
