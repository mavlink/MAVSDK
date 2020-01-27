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

    mavlink_message_t message;
    mavlink_msg_mission_count_pack(
        0,
        0,
        &message,
        0,
        0,
        items.size(),
        MAV_MISSION_TYPE_MISSION);

    if (!_sender.send_message(message)) {
        if (callback) {
            callback(Result::ConnectionError);
        }
        return;
    }
}

} // namespace mavsdk
