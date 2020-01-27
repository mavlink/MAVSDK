#include <algorithm>
#include "mavlink_mission_transfer.h"
#include "log.h"

namespace mavsdk {

MAVLinkMissionTransfer::MAVLinkMissionTransfer(
    Config config,
    Sender& sender,
    MAVLinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler) :
    _config(config),
    _sender(sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler)
{
    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_REQUEST_INT,
        [this](const mavlink_message_t& message) { process_mission_request_int(message); },
        this);
}

MAVLinkMissionTransfer::~MAVLinkMissionTransfer()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MAVLinkMissionTransfer::upload_items_async(
    const std::vector<ItemInt>& items, ResultCallback callback)
{
    if (items.size() == 0) {
        if (callback) {
            callback(Result::NoMissionAvailable);
        }
        return;
    }

    int count = 0;
    for (const auto& item : items) {
        if (count++ != item.seq) {
            if (callback) {
                callback(Result::WrongSequence);
            }
            return;
        }
    }

    int num_currents = 0;
    std::for_each(items.cbegin(), items.cend(), [&num_currents](const ItemInt& item) {
        num_currents += item.current;
    });
    if (num_currents != 1) {
        if (callback) {
            callback(Result::CurrentInvalid);
        }
        return;
    }

    auto first_mission_type = items[0].mission_type;

    if (std::any_of(items.cbegin(), items.cend(), [first_mission_type](const ItemInt& item) {
            return item.mission_type != first_mission_type;
        })) {
        if (callback) {
            callback(Result::MissionTypeNotConsistent);
        }
        return;
    }

    _callback = callback;
    _items = items;

    _timeout_handler.add([this]() { process_timeout(); }, timeout_s, &_cookie);

    _next_sequence_expected = 0;

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
        _timeout_handler.remove(_cookie);
        if (callback) {
            callback(Result::ConnectionError);
        }
        return;
    }
}

void MAVLinkMissionTransfer::process_mission_request_int(const mavlink_message_t& message)
{
    mavlink_mission_request_int_t request_int;
    mavlink_msg_mission_request_int_decode(&message, &request_int);

    if (_next_sequence_expected != request_int.seq) {
        LogWarn() << "mission_request_int: sequence incorrect";
        return;
    }

    mavlink_message_t new_message;
    mavlink_msg_mission_item_int_pack(
        _config.own_system_id,
        _config.own_component_id,
        &new_message,
        _config.target_system_id,
        _config.target_component_id,
        request_int.seq,
        _items[request_int.seq].frame,
        _items[request_int.seq].command,
        _items[request_int.seq].current,
        _items[request_int.seq].autocontinue,
        1.0f,
        NAN,
        -1.0f,
        0.0f,
        0,
        0,
        NAN,
        MAV_MISSION_TYPE_MISSION);
    LogWarn() << "sending: " << new_message.msgid;

    if (!_sender.send_message(new_message)) {
        _timeout_handler.remove(_cookie);
        if (_callback) {
            _callback(Result::ConnectionError);
        }
        return;
    }
}

void MAVLinkMissionTransfer::process_timeout()
{
    if (!_callback) {
        return;
    }
    _callback(Result::Timeout);
}

} // namespace mavsdk
