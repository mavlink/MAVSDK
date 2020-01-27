#include <algorithm>
#include "mavlink_mission_transfer.h"

namespace mavsdk {

void MAVLinkMissionTransfer::upload_items_async(
        const std::vector<ItemInt> &items, ResultCallback callback)
{
    if (items.size() == 0) {
        if (callback) {
            callback(Result::NoMissionAvailable);
        }
        return;
    }

    auto first_mission_type = items[0].mission_type;

    if (std::any_of(items.cbegin(), items.cend(), [first_mission_type](const ItemInt& item) {
            return item.mission_type != first_mission_type; })) {
        if (callback) {
            callback(Result::MissionTypeNotConsistent);
        }
        return;
    }

    void* cookie = nullptr;

    _timeout_handler.add(
        [this, callback]() {
            if (callback) {
                callback(Result::Timeout);
            }
        }, timeout_s, &cookie);

    mavlink_message_t message;
    mavlink_msg_mission_count_pack(
        _config.own_system_id,
        _config.own_component_id,
        &message,
        _config.target_system_id,
        _config.target_component_id,
        items.size(),
        first_mission_type);

    if (!_sender.send_message(message)) {
        _timeout_handler.remove(cookie);
        if (callback) {
            callback(Result::ConnectionError);
        }
        return;
    }
}

} // namespace mavsdk
