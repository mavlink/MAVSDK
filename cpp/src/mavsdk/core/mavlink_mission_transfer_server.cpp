#include <algorithm>
#include "mavlink_mission_transfer_server.h"
#include "log.h"
#include "unused.h"

namespace mavsdk {

MavlinkMissionTransferServer::MavlinkMissionTransferServer(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    TimeoutSCallback timeout_s_callback) :
    _sender(sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler),
    _timeout_s_callback(std::move(timeout_s_callback))
{
    if (const char* env_p = std::getenv("MAVSDK_MISSION_TRANSFER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Mission transfer debugging is on.";
            _debugging = true;
        }
    }
}

std::weak_ptr<MavlinkMissionTransferServer::WorkItem>
MavlinkMissionTransferServer::receive_incoming_items_async(
    uint8_t type,
    uint32_t mission_count,
    uint8_t target_system,
    uint8_t target_component,
    ResultAndItemsCallback callback)
{
    auto ptr = std::make_shared<ReceiveIncomingMission>(
        _sender,
        _message_handler,
        _timeout_handler,
        type,
        _timeout_s_callback(),
        callback,
        mission_count,
        target_system,
        target_component,
        _debugging);

    _work_queue.push_back(ptr);

    return std::weak_ptr<WorkItem>(ptr);
}

std::weak_ptr<MavlinkMissionTransferServer::WorkItem>
MavlinkMissionTransferServer::send_outgoing_items_async(
    uint8_t type,
    const std::vector<ItemInt>& items,
    uint8_t target_system,
    uint8_t target_component,
    ResultCallback callback)
{
    auto ptr = std::make_shared<SendOutgoingMission>(
        _sender,
        _message_handler,
        _timeout_handler,
        type,
        items,
        _timeout_s_callback(),
        callback,
        target_system,
        target_component,
        _debugging);

    _work_queue.push_back(ptr);

    return std::weak_ptr<WorkItem>(ptr);
}

void MavlinkMissionTransferServer::do_work()
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    auto work = work_queue_guard.get_front();

    if (!work) {
        return;
    }

    if (!work->has_started()) {
        work->start();
    }
    if (work->is_done()) {
        work_queue_guard.pop_front();
    }
}

bool MavlinkMissionTransferServer::is_idle()
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    return (work_queue_guard.get_front() == nullptr);
}

MavlinkMissionTransferServer::WorkItem::WorkItem(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    uint8_t type,
    double timeout_s,
    bool debugging) :
    _sender(sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler),
    _type(type),
    _timeout_s(timeout_s),
    _debugging(debugging)
{}

MavlinkMissionTransferServer::WorkItem::~WorkItem() {}

bool MavlinkMissionTransferServer::WorkItem::has_started()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _started;
}

bool MavlinkMissionTransferServer::WorkItem::is_done()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _done;
}

MavlinkMissionTransferServer::ReceiveIncomingMission::ReceiveIncomingMission(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    uint8_t type,
    double timeout_s,
    ResultAndItemsCallback callback,
    uint32_t mission_count,
    uint8_t target_system_id,
    uint8_t target_component_id,
    bool debugging) :
    WorkItem(sender, message_handler, timeout_handler, type, timeout_s, debugging),
    _callback(callback),
    _mission_count(mission_count),
    _target_system_id(target_system_id),
    _target_component_id(target_component_id)
{}

MavlinkMissionTransferServer::ReceiveIncomingMission::~ReceiveIncomingMission()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::start()
{
    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_ITEM_INT,
        [this](const mavlink_message_t& message) { process_mission_item_int(message); },
        this);

    std::lock_guard<std::mutex> lock(_mutex);

    _items.clear();

    _started = true;
    _retries_done = 0;
    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    process_mission_count();
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::cancel()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.remove(_cookie);
    send_cancel_and_finish();
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::request_item()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_request_int_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                _target_component_id,
                _next_sequence,
                _type);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }

    ++_retries_done;
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::send_ack_and_finish()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                _target_component_id,
                MAV_MISSION_ACCEPTED,
                _type,
                0);
            return message;
        })) {
        callback_and_reset(Result::ConnectionError);
        return;
    }

    // We do not wait on anything coming back after this.
    callback_and_reset(Result::Success);
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::send_cancel_and_finish()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                _target_component_id,
                MAV_MISSION_OPERATION_CANCELLED,
                _type,
                0);
            return message;
        })) {
        callback_and_reset(Result::ConnectionError);
        return;
    }

    // We do not wait on anything coming back after this.
    callback_and_reset(Result::Cancelled);
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::process_mission_count()
{
    if (_mission_count == 0) {
        send_ack_and_finish();
        _timeout_handler.remove(_cookie);
        return;
    }

    _timeout_handler.refresh(_cookie);
    _next_sequence = 0;
    _retries_done = 0;
    _expected_count = _mission_count;
    request_item();
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::process_mission_item_int(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _timeout_handler.refresh(_cookie);

    mavlink_mission_item_int_t item_int;
    mavlink_msg_mission_item_int_decode(&message, &item_int);

    _items.push_back(ItemInt{
        item_int.seq,
        item_int.frame,
        item_int.command,
        item_int.current,
        item_int.autocontinue,
        item_int.param1,
        item_int.param2,
        item_int.param3,
        item_int.param4,
        item_int.x,
        item_int.y,
        item_int.z,
        item_int.mission_type});

    if (_next_sequence + 1 == _expected_count) {
        _timeout_handler.remove(_cookie);
        send_ack_and_finish();

    } else {
        _next_sequence = item_int.seq + 1;
        _retries_done = 0;
        request_item();
    }
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::process_timeout()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_retries_done >= retries) {
        callback_and_reset(Result::Timeout);
        return;
    }

    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    request_item();
}

void MavlinkMissionTransferServer::ReceiveIncomingMission::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result, _type, _items);
    }
    _callback = nullptr;
    _done = true;
}

MavlinkMissionTransferServer::SendOutgoingMission::SendOutgoingMission(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    uint8_t type,
    const std::vector<ItemInt>& items,
    double timeout_s,
    ResultCallback callback,
    uint8_t target_system_id,
    uint8_t target_component_id,
    bool debugging) :
    WorkItem(sender, message_handler, timeout_handler, type, timeout_s, debugging),
    _items(items),
    _callback(callback),
    _target_system_id(target_system_id),
    _target_component_id(target_component_id)
{
    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_REQUEST,
        [this](const mavlink_message_t& message) { process_mission_request(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_REQUEST_INT,
        [this](const mavlink_message_t& message) { process_mission_request_int(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_ACK,
        [this](const mavlink_message_t& message) { process_mission_ack(message); },
        this);
}

MavlinkMissionTransferServer::SendOutgoingMission::~SendOutgoingMission()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MavlinkMissionTransferServer::SendOutgoingMission::start()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _started = true;
    _retries_done = 0;
    _step = Step::SendCount;
    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);

    _next_sequence = 0;

    send_count();
}

void MavlinkMissionTransferServer::SendOutgoingMission::cancel()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.remove(_cookie);
    send_cancel_and_finish();
}

void MavlinkMissionTransferServer::SendOutgoingMission::send_count()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_count_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                _target_component_id,
                _items.size(),
                _type,
                0);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }

    if (_debugging) {
        LogDebug() << "Sending send_count, count: " << _items.size()
                   << ", retries: " << _retries_done;
    }

    ++_retries_done;
}

void MavlinkMissionTransferServer::SendOutgoingMission::send_cancel_and_finish()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                _target_component_id,
                MAV_MISSION_OPERATION_CANCELLED,
                _type,
                0);
            return message;
        })) {
        callback_and_reset(Result::ConnectionError);
        return;
    }

    // We do not wait on anything coming back after this.
    callback_and_reset(Result::Cancelled);
}

void MavlinkMissionTransferServer::SendOutgoingMission::process_mission_request(
    const mavlink_message_t& request_message)
{
    mavlink_mission_request_t request;
    mavlink_msg_mission_request_decode(&request_message, &request);

    std::lock_guard<std::mutex> lock(_mutex);

    // We only support int, so we nack this and thus tell the autopilot to use int.
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                request_message.sysid,
                request_message.compid,
                MAV_MISSION_UNSUPPORTED,
                _type,
                0);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }
    _timeout_handler.refresh(_cookie);
}

void MavlinkMissionTransferServer::SendOutgoingMission::process_mission_request_int(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_mission_request_int_t request_int;
    mavlink_msg_mission_request_int_decode(&message, &request_int);

    _step = Step::SendItems;

    if (_debugging) {
        LogDebug() << "Process mission_request_int, seq: " << request_int.seq
                   << ", next expected sequence: " << _next_sequence;
    }

    if (_next_sequence < request_int.seq) {
        // We should not go back to a previous one.
        // TODO: figure out if we should error here.
        LogWarn() << "mission_request_int: sequence incorrect";
        return;

    } else if (_next_sequence > request_int.seq) {
        // We have already sent that one before.
        if (_retries_done >= retries) {
            LogWarn() << "mission_request_int: retries exceeded";
            _timeout_handler.remove(_cookie);
            callback_and_reset(Result::Timeout);
            return;
        }

    } else {
        // Correct one, sending it the first time.
        _retries_done = 0;
    }

    _timeout_handler.refresh(_cookie);

    _next_sequence = request_int.seq;

    send_mission_item();
}

void MavlinkMissionTransferServer::SendOutgoingMission::send_mission_item()
{
    if (_next_sequence >= _items.size()) {
        LogErr() << "send_mission_item: sequence out of bounds";
        return;
    }

    if (_debugging) {
        LogDebug() << "Sending mission_item_int seq: " << _next_sequence
                   << ", retry: " << _retries_done;
    }

    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_item_int_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                _target_component_id,
                _next_sequence,
                _items[_next_sequence].frame,
                _items[_next_sequence].command,
                _items[_next_sequence].current,
                _items[_next_sequence].autocontinue,
                _items[_next_sequence].param1,
                _items[_next_sequence].param2,
                _items[_next_sequence].param3,
                _items[_next_sequence].param4,
                _items[_next_sequence].x,
                _items[_next_sequence].y,
                _items[_next_sequence].z,
                _type);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }

    ++_next_sequence;

    ++_retries_done;
}

void MavlinkMissionTransferServer::SendOutgoingMission::process_mission_ack(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (_debugging) {
        LogDebug() << "Received mission_ack type: " << static_cast<int>(mission_ack.type);
    }

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

    if (_next_sequence == _items.size()) {
        callback_and_reset(Result::Success);
    } else {
        callback_and_reset(Result::ProtocolError);
    }
}

void MavlinkMissionTransferServer::SendOutgoingMission::process_timeout()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_debugging) {
        LogDebug() << "Timeout triggered, retries: " << _retries_done;
    }

    if (_retries_done >= retries) {
        LogWarn() << "timeout: retries exceeded";
        callback_and_reset(Result::Timeout);
        return;
    }

    switch (_step) {
        case Step::SendCount:
            _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
            send_count();
            break;

        case Step::SendItems:
            // When waiting for items requested we should wait longer than
            // just our timeout, otherwise we give up too quickly.
            ++_retries_done;
            _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
            break;
    }
}

void MavlinkMissionTransferServer::SendOutgoingMission::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result);
    }
    _callback = nullptr;
    _done = true;
}

} // namespace mavsdk
