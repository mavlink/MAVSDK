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

    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_ACK,
        [this](const mavlink_message_t& message) { process_mission_ack(message); },
        this);
}

MAVLinkMissionTransfer::~MAVLinkMissionTransfer()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MAVLinkMissionTransfer::upload_items_async(
    uint8_t type, const std::vector<ItemInt>& items, ResultCallback callback)
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
                callback(Result::InvalidSequence);
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

    if (std::any_of(items.cbegin(), items.cend(), [type](const ItemInt& item) {
            return item.mission_type != type;
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
        type);

    if (!_sender.send_message(message)) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }
}

void MAVLinkMissionTransfer::download_items_async(uint8_t type, ResultAndItemsCallback callback)
{
    _callback_download = callback;

    mavlink_message_t message;
    mavlink_msg_mission_request_list_pack(
        _config.own_system_id,
        _config.own_component_id,
        &message,
        _config.target_system_id,
        _config.target_component_id,
        type);

    if (!_sender.send_message(message)) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }
}

void MAVLinkMissionTransfer::process_mission_request_int(const mavlink_message_t& message)
{
    mavlink_mission_request_int_t request_int;
    mavlink_msg_mission_request_int_decode(&message, &request_int);

    _timeout_handler.refresh(_cookie);

    if (_next_sequence_expected < request_int.seq) {
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
        _items[request_int.seq].param1,
        _items[request_int.seq].param2,
        _items[request_int.seq].param3,
        _items[request_int.seq].param4,
        _items[request_int.seq].x,
        _items[request_int.seq].y,
        _items[request_int.seq].z,
        MAV_MISSION_TYPE_MISSION);

    _next_sequence_expected = request_int.seq + 1;

    if (!_sender.send_message(new_message)) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }
}

void MAVLinkMissionTransfer::process_mission_ack(const mavlink_message_t& message)
{
    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    _timeout_handler.remove(_cookie);

    switch (mission_ack.type) {
        case MAV_MISSION_ERROR:
            callback_and_reset(Result::ProtocolError);
            return;
        case MAV_MISSION_UNSUPPORTED_FRAME:
            callback_and_reset(Result::UnsupportedFrame);
            return;
        case MAV_MISSION_UNSUPPORTED:
            callback_and_reset(Result::Unsupported);
            return;
        case MAV_MISSION_NO_SPACE:
            callback_and_reset(Result::TooManyMissionItems);
            return;
        case MAV_MISSION_INVALID:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM1:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM2:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM3:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM4:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM5_X:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM6_Y:
            // FALLTHROUGH
        case MAV_MISSION_INVALID_PARAM7:
            callback_and_reset(Result::InvalidParam);
            return;
        case MAV_MISSION_INVALID_SEQUENCE:
            callback_and_reset(Result::InvalidSequence);
            return;
        case MAV_MISSION_DENIED:
            callback_and_reset(Result::Denied);
            return;
        case MAV_MISSION_OPERATION_CANCELLED:
            callback_and_reset(Result::Cancelled);
            return;
    }

    if (_next_sequence_expected == static_cast<int>(_items.size())) {
        callback_and_reset(Result::Success);
    } else {
        callback_and_reset(Result::ProtocolError);
    }
}

void MAVLinkMissionTransfer::process_timeout()
{
    callback_and_reset(Result::Timeout);
}

void MAVLinkMissionTransfer::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result);
    }
    _callback = nullptr;
    _items.clear();
}

} // namespace mavsdk
