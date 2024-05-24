#include <algorithm>
#include "mavlink_mission_transfer_client.h"
#include "log.h"
#include "unused.h"

namespace mavsdk {

MavlinkMissionTransferClient::MavlinkMissionTransferClient(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    TimeoutSCallback timeout_s_callback,
    AutopilotCallback autopilot_callback) :
    _sender(sender),
    _message_handler(message_handler),
    _timeout_handler(timeout_handler),
    _timeout_s_callback(std::move(timeout_s_callback)),
    _autopilot_callback(std::move(autopilot_callback))
{
    if (const char* env_p = std::getenv("MAVSDK_MISSION_TRANSFER_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Mission transfer debugging is on.";
            _debugging = true;
        }
    }
}

std::weak_ptr<MavlinkMissionTransferClient::WorkItem>
MavlinkMissionTransferClient::upload_items_async(
    uint8_t type,
    uint8_t target_system_id,
    const std::vector<ItemInt>& items,
    const ResultCallback& callback,
    const ProgressCallback& progress_callback)
{
    if (!_int_messages_supported) {
        if (callback) {
            LogErr() << "Int messages are not supported.";
            callback(Result::IntMessagesNotSupported);
        }
        return {};
    }

    auto ptr = std::make_shared<UploadWorkItem>(
        _sender,
        _message_handler,
        _timeout_handler,
        type,
        items,
        _timeout_s_callback(),
        callback,
        progress_callback,
        _debugging,
        target_system_id,
        _autopilot_callback());

    _work_queue.push_back(ptr);

    return std::weak_ptr<WorkItem>(ptr);
}

std::weak_ptr<MavlinkMissionTransferClient::WorkItem>
MavlinkMissionTransferClient::download_items_async(
    uint8_t type,
    uint8_t target_system_id,
    ResultAndItemsCallback callback,
    ProgressCallback progress_callback)
{
    if (!_int_messages_supported) {
        if (callback) {
            LogErr() << "Int messages are not supported.";
            callback(Result::IntMessagesNotSupported, {});
        }
        return {};
    }

    auto ptr = std::make_shared<DownloadWorkItem>(
        _sender,
        _message_handler,
        _timeout_handler,
        type,
        _timeout_s_callback(),
        callback,
        progress_callback,
        _debugging,
        target_system_id);

    _work_queue.push_back(ptr);

    return std::weak_ptr<WorkItem>(ptr);
}

void MavlinkMissionTransferClient::clear_items_async(
    uint8_t type, uint8_t target_system_id, ResultCallback callback)
{
    auto ptr = std::make_shared<ClearWorkItem>(
        _sender,
        _message_handler,
        _timeout_handler,
        type,
        _timeout_s_callback(),
        callback,
        _debugging,
        target_system_id);

    _work_queue.push_back(ptr);
}

void MavlinkMissionTransferClient::set_current_item_async(
    int current, uint8_t target_system_id, ResultCallback callback)
{
    auto ptr = std::make_shared<SetCurrentWorkItem>(
        _sender,
        _message_handler,
        _timeout_handler,
        current,
        _timeout_s_callback(),
        callback,
        _debugging,
        target_system_id);

    _work_queue.push_back(ptr);
}

void MavlinkMissionTransferClient::do_work()
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

bool MavlinkMissionTransferClient::is_idle()
{
    LockedQueue<WorkItem>::Guard work_queue_guard(_work_queue);
    return (work_queue_guard.get_front() == nullptr);
}

MavlinkMissionTransferClient::WorkItem::WorkItem(
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

MavlinkMissionTransferClient::WorkItem::~WorkItem() {}

bool MavlinkMissionTransferClient::WorkItem::has_started()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _started;
}

bool MavlinkMissionTransferClient::WorkItem::is_done()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _done;
}

MavlinkMissionTransferClient::UploadWorkItem::UploadWorkItem(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    uint8_t type,
    const std::vector<ItemInt>& items,
    double timeout_s,
    ResultCallback callback,
    ProgressCallback progress_callback,
    bool debugging,
    uint8_t target_system_id,
    Autopilot autopilot) :
    WorkItem(sender, message_handler, timeout_handler, type, timeout_s, debugging),
    _items(items),
    _callback(callback),
    _progress_callback(progress_callback),
    _target_system_id(target_system_id),
    _autopilot(autopilot)
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

MavlinkMissionTransferClient::UploadWorkItem::~UploadWorkItem()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MavlinkMissionTransferClient::UploadWorkItem::start()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _started = true;
    if (_items.size() == 0) {
        callback_and_reset(Result::NoMissionAvailable);
        return;
    }

    for (unsigned i = 0; i < _items.size(); ++i) {
        if (_items[i].seq != i) {
            LogWarn() << "Invalid sequence";
            callback_and_reset(Result::InvalidSequence);
            return;
        }
    }

    int num_currents = 0;
    std::for_each(_items.cbegin(), _items.cend(), [&num_currents](const ItemInt& item) {
        num_currents += item.current;
    });
    if (num_currents != 1) {
        callback_and_reset(Result::CurrentInvalid);
        return;
    }

    if (std::any_of(_items.cbegin(), _items.cend(), [this](const ItemInt& item) {
            return item.mission_type != _type;
        })) {
        callback_and_reset(Result::MissionTypeNotConsistent);
        return;
    }

    update_progress(0.0f);

    _retries_done = 0;
    _step = Step::SendCount;
    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);

    _next_sequence = 0;

    send_count();
}

void MavlinkMissionTransferClient::UploadWorkItem::cancel()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.remove(_cookie);
    send_cancel_and_finish();
}

void MavlinkMissionTransferClient::UploadWorkItem::send_count()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_count_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
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

void MavlinkMissionTransferClient::UploadWorkItem::send_cancel_and_finish()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
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

void MavlinkMissionTransferClient::UploadWorkItem::process_mission_request(
    const mavlink_message_t& request_message)
{
    mavlink_mission_request_t request;
    mavlink_msg_mission_request_decode(&request_message, &request);

    if (_autopilot == Autopilot::ArduPilot) {
        // ArduCopter 3.6 sends MISSION_REQUEST (not _INT) but actually accepts ITEM_INT in reply

        // FIXME: this will mess with the sequence number.
        mavlink_message_t request_int_message;
        mavlink_msg_mission_request_int_pack(
            request_message.sysid,
            request_message.compid,
            &request_int_message,
            request.target_system,
            request.target_component,
            request.seq,
            request.mission_type);

        process_mission_request_int(request_int_message);
    } else {
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
}

void MavlinkMissionTransferClient::UploadWorkItem::process_mission_request_int(
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

    // We add in a step for the final ack, so plus one.
    update_progress(static_cast<float>(_next_sequence + 1) / static_cast<float>(_items.size() + 1));

    send_mission_item();
}

void MavlinkMissionTransferClient::UploadWorkItem::send_mission_item()
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
                MAV_COMP_ID_AUTOPILOT1,
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

void MavlinkMissionTransferClient::UploadWorkItem::process_mission_ack(
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
        update_progress(1.0f);
        callback_and_reset(Result::Success);
    } else {
        callback_and_reset(Result::ProtocolError);
    }
}

void MavlinkMissionTransferClient::UploadWorkItem::process_timeout()
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

void MavlinkMissionTransferClient::UploadWorkItem::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result);
    }
    _callback = nullptr;
    _done = true;
}

void MavlinkMissionTransferClient::UploadWorkItem::update_progress(float progress)
{
    if (_progress_callback != nullptr) {
        _progress_callback(progress);
    }
}

MavlinkMissionTransferClient::DownloadWorkItem::DownloadWorkItem(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    uint8_t type,
    double timeout_s,
    ResultAndItemsCallback callback,
    ProgressCallback progress_callback,
    bool debugging,
    uint8_t target_system_id) :
    WorkItem(sender, message_handler, timeout_handler, type, timeout_s, debugging),
    _callback(callback),
    _progress_callback(progress_callback),
    _target_system_id(target_system_id)
{
    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_COUNT,
        [this](const mavlink_message_t& message) { process_mission_count(message); },
        this);

    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_ITEM_INT,
        [this](const mavlink_message_t& message) { process_mission_item_int(message); },
        this);
}

MavlinkMissionTransferClient::DownloadWorkItem::~DownloadWorkItem()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MavlinkMissionTransferClient::DownloadWorkItem::start()
{
    update_progress(0.0f);

    std::lock_guard<std::mutex> lock(_mutex);

    _items.clear();
    _started = true;
    _retries_done = 0;
    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    request_list();
}

void MavlinkMissionTransferClient::DownloadWorkItem::cancel()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.remove(_cookie);
    send_cancel_and_finish();
}

void MavlinkMissionTransferClient::DownloadWorkItem::request_list()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_request_list_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
                _type);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }

    ++_retries_done;
}

void MavlinkMissionTransferClient::DownloadWorkItem::request_item()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_request_int_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
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

void MavlinkMissionTransferClient::DownloadWorkItem::send_ack_and_finish()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
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

void MavlinkMissionTransferClient::DownloadWorkItem::send_cancel_and_finish()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_ack_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
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

void MavlinkMissionTransferClient::DownloadWorkItem::process_mission_count(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_mission_count_t count;
    mavlink_msg_mission_count_decode(&message, &count);

    if (count.count == 0) {
        send_ack_and_finish();
        _timeout_handler.remove(_cookie);
        return;
    }

    _timeout_handler.refresh(_cookie);
    _next_sequence = 0;
    _step = Step::RequestItem;
    _retries_done = 0;
    _expected_count = count.count;
    request_item();
}

void MavlinkMissionTransferClient::DownloadWorkItem::process_mission_item_int(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.refresh(_cookie);

    mavlink_mission_item_int_t item_int;
    mavlink_msg_mission_item_int_decode(&message, &item_int);

    // If we have already received the item previously, we have to ignore it.
    if (_next_sequence == item_int.seq) {
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
            update_progress(1.0f);
            send_ack_and_finish();

        } else {
            _next_sequence = item_int.seq + 1;
            _retries_done = 0;
            update_progress(
                static_cast<float>(_next_sequence) / static_cast<float>(_expected_count));
            request_item();
        }
    }
}

void MavlinkMissionTransferClient::DownloadWorkItem::process_timeout()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_retries_done >= retries) {
        callback_and_reset(Result::Timeout);
        return;
    }

    switch (_step) {
        case Step::RequestList:
            _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
            request_list();
            break;

        case Step::RequestItem:
            _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
            request_item();
            break;
    }
}

void MavlinkMissionTransferClient::DownloadWorkItem::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result, _items);
    }
    _callback = nullptr;
    _done = true;
}

void MavlinkMissionTransferClient::DownloadWorkItem::update_progress(float progress)
{
    if (_progress_callback != nullptr) {
        _progress_callback(progress);
    }
}

MavlinkMissionTransferClient::ClearWorkItem::ClearWorkItem(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    uint8_t type,
    double timeout_s,
    ResultCallback callback,
    bool debugging,
    uint8_t target_system_id) :
    WorkItem(sender, message_handler, timeout_handler, type, timeout_s, debugging),
    _callback(callback),
    _target_system_id(target_system_id)
{
    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_ACK,
        [this](const mavlink_message_t& message) { process_mission_ack(message); },
        this);
}

MavlinkMissionTransferClient::ClearWorkItem::~ClearWorkItem()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MavlinkMissionTransferClient::ClearWorkItem::start()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _started = true;
    _retries_done = 0;
    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    send_clear();
}

void MavlinkMissionTransferClient::ClearWorkItem::cancel()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.remove(_cookie);
    // This is presumably not used or exposed because it is quick.
}

void MavlinkMissionTransferClient::ClearWorkItem::send_clear()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_clear_all_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
                _type);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }

    ++_retries_done;
}

void MavlinkMissionTransferClient::ClearWorkItem::process_timeout()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_retries_done >= retries) {
        callback_and_reset(Result::Timeout);
        return;
    }

    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    send_clear();
}

void MavlinkMissionTransferClient::ClearWorkItem::process_mission_ack(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    _timeout_handler.remove(_cookie);

    switch (mission_ack.type) {
        case MAV_MISSION_ACCEPTED:
            callback_and_reset(Result::Success);
            return;
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
}

void MavlinkMissionTransferClient::ClearWorkItem::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result);
    }
    _callback = nullptr;
    _done = true;
}

void MavlinkMissionTransferClient::set_int_messages_supported(bool supported)
{
    _int_messages_supported = supported;
}

MavlinkMissionTransferClient::SetCurrentWorkItem::SetCurrentWorkItem(
    Sender& sender,
    MavlinkMessageHandler& message_handler,
    TimeoutHandler& timeout_handler,
    int current,
    double timeout_s,
    ResultCallback callback,
    bool debugging,
    uint8_t target_system_id) :
    WorkItem(
        sender, message_handler, timeout_handler, MAV_MISSION_TYPE_MISSION, timeout_s, debugging),
    _current(current),
    _callback(callback),
    _target_system_id(target_system_id)
{
    _message_handler.register_one(
        MAVLINK_MSG_ID_MISSION_CURRENT,
        [this](const mavlink_message_t& message) { process_mission_current(message); },
        this);
}

MavlinkMissionTransferClient::SetCurrentWorkItem::~SetCurrentWorkItem()
{
    _message_handler.unregister_all(this);
    _timeout_handler.remove(_cookie);
}

void MavlinkMissionTransferClient::SetCurrentWorkItem::start()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _started = true;

    // If we coudln't find it, the requested item is out of range and probably an invalid argument.
    if (_current < 0) {
        callback_and_reset(Result::CurrentInvalid);
        return;
    }

    _retries_done = 0;
    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    send_current_mission_item();
}

void MavlinkMissionTransferClient::SetCurrentWorkItem::cancel()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _timeout_handler.remove(_cookie);
    // This is presumably not used or exposed because it is quick.
}

void MavlinkMissionTransferClient::SetCurrentWorkItem::send_current_mission_item()
{
    if (!_sender.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_mission_set_current_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                _target_system_id,
                MAV_COMP_ID_AUTOPILOT1,
                _current);
            return message;
        })) {
        _timeout_handler.remove(_cookie);
        callback_and_reset(Result::ConnectionError);
        return;
    }

    ++_retries_done;
}

void MavlinkMissionTransferClient::SetCurrentWorkItem::process_mission_current(
    const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_mission_current_t mission_current;
    mavlink_msg_mission_current_decode(&message, &mission_current);

    _timeout_handler.remove(_cookie);

    if (_current == mission_current.seq) {
        _current = mission_current.seq;
        callback_and_reset(Result::Success);
        return;
    } else {
        _timeout_handler.refresh(_cookie);
        send_current_mission_item();
        return;
    }
}

void MavlinkMissionTransferClient::SetCurrentWorkItem::process_timeout()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_retries_done >= retries) {
        callback_and_reset(Result::Timeout);
        return;
    }

    _cookie = _timeout_handler.add([this]() { process_timeout(); }, _timeout_s);
    send_current_mission_item();
}

void MavlinkMissionTransferClient::SetCurrentWorkItem::callback_and_reset(Result result)
{
    if (_callback) {
        _callback(result);
    }
    _callback = nullptr;
    _done = true;
}
} // namespace mavsdk
