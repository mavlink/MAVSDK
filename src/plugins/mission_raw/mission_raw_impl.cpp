#include "mission_raw_impl.h"
#include "system.h"
#include "global_include.h"

namespace mavsdk {

using namespace std::placeholders; // for `_1`

MissionRawImpl::MissionRawImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionRawImpl::~MissionRawImpl()
{
    _parent->unregister_plugin(this);
}

void MissionRawImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ACK,
        std::bind(&MissionRawImpl::process_mission_ack, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CURRENT,
        std::bind(&MissionRawImpl::process_mission_current, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_REACHED,
        std::bind(&MissionRawImpl::process_mission_item_reached, this, _1),
        this);
}

void MissionRawImpl::enable() {}

void MissionRawImpl::disable()
{
    reset_mission_progress();
}

void MissionRawImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionRawImpl::reset_mission_progress()
{
    std::lock_guard<std::mutex> lock(_mission_progress.mutex);
    _mission_progress.last.current = -1;
    _mission_progress.last.total = -1;
    _mission_progress.last_reported.current = -1;
    _mission_progress.last_reported.total = -1;
}

void MissionRawImpl::process_mission_ack(const mavlink_message_t& message)
{
    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (mission_ack.type != MAV_MISSION_ACCEPTED ||
        mission_ack.mission_type != MAV_MISSION_TYPE_MISSION) {
        return;
    }

    // We assume that if the vehicle sends an ACCEPTED ack might have received
    // a new mission. In that case we need to notify our user.
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    if (_mission_changed.callback) {
        auto temp_callback = _mission_changed.callback;
        _parent->call_user_callback([temp_callback]() { temp_callback(true); });
    }
}

void MissionRawImpl::process_mission_current(const mavlink_message_t& message)
{
    mavlink_mission_current_t mission_current;
    mavlink_msg_mission_current_decode(&message, &mission_current);

    {
        std::lock_guard<std::mutex> lock(_mission_progress.mutex);
        _mission_progress.last.current = mission_current.seq;
    }

    report_progress_current();
}

void MissionRawImpl::process_mission_item_reached(const mavlink_message_t& message)
{
    mavlink_mission_item_reached_t mission_item_reached;
    mavlink_msg_mission_item_reached_decode(&message, &mission_item_reached);

    {
        std::lock_guard<std::mutex> lock(_mission_progress.mutex);
        _mission_progress.last.current = mission_item_reached.seq + 1;
    }

    report_progress_current();
}

MissionRaw::Result
MissionRawImpl::upload_mission(std::vector<MissionRaw::MissionItem> mission_items)
{
    auto prom = std::promise<MissionRaw::Result>();
    auto fut = prom.get_future();

    upload_mission_async(
        mission_items, [&prom](MissionRaw::Result result) { prom.set_value(result); });
    return fut.get();
}

void MissionRawImpl::upload_mission_async(
    const std::vector<MissionRaw::MissionItem>& mission_raw,
    const MissionRaw::result_callback_t& callback)
{
    if (_last_upload.lock()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(MissionRaw::Result::Busy);
            }
        });
        return;
    }

    if (!_parent->does_support_mission_int()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(MissionRaw::Result::Unsupported);
            }
        });
        return;
    }

    const auto int_items = convert_to_int_items(mission_raw);

    _last_upload = _parent->mission_transfer().upload_items_async(
        MAV_MISSION_TYPE_MISSION,
        int_items,
        [this, callback, int_items](MAVLinkMissionTransfer::Result result) {
            auto converted_result = convert_result(result);
            auto converted_items = convert_items(int_items);
            _parent->call_user_callback([callback, converted_result, converted_items]() {
                if (callback) {
                    callback(converted_result);
                }
            });
        });
}

MissionRaw::Result MissionRawImpl::cancel_mission_upload()
{
    auto ptr = _last_upload.lock();
    if (ptr) {
        ptr->cancel();
        return MissionRaw::Result::Success;
    } else {
        return MissionRaw::Result::Error;
    }
}

std::pair<MissionRaw::Result, std::vector<MissionRaw::MissionItem>>
MissionRawImpl::download_mission()
{
    auto prom = std::promise<std::pair<MissionRaw::Result, std::vector<MissionRaw::MissionItem>>>();
    auto fut = prom.get_future();

    download_mission_async(
        [&prom](MissionRaw::Result result, std::vector<MissionRaw::MissionItem> mission_items) {
            prom.set_value(std::make_pair<>(result, mission_items));
        });
    return fut.get();
}

void MissionRawImpl::download_mission_async(const MissionRaw::download_mission_callback_t& callback)
{
    if (_last_download.lock()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                std::vector<MissionRaw::MissionItem> empty_items;
                callback(MissionRaw::Result::Busy, empty_items);
            }
        });
        return;
    }

    _last_download = _parent->mission_transfer().download_items_async(
        MAV_MISSION_TYPE_MISSION,
        [this, callback](
            MAVLinkMissionTransfer::Result result,
            std::vector<MAVLinkMissionTransfer::ItemInt> items) {
            auto converted_result = convert_result(result);
            auto converted_items = convert_items(items);
            _parent->call_user_callback([callback, converted_result, converted_items]() {
                callback(converted_result, converted_items);
            });
        });
}

MissionRaw::Result MissionRawImpl::cancel_mission_download()
{
    auto ptr = _last_download.lock();
    if (ptr) {
        ptr->cancel();
        return MissionRaw::Result::Success;
    } else {
        return MissionRaw::Result::Error;
    }
}

MAVLinkMissionTransfer::ItemInt
MissionRawImpl::convert_mission_raw(const MissionRaw::MissionItem transfer_mission_raw)
{
    MAVLinkMissionTransfer::ItemInt new_item_int;

    new_item_int.seq = transfer_mission_raw.seq;
    new_item_int.frame = transfer_mission_raw.frame;
    new_item_int.command = transfer_mission_raw.command;
    new_item_int.current = transfer_mission_raw.current;
    new_item_int.autocontinue = transfer_mission_raw.autocontinue;
    new_item_int.param1 = transfer_mission_raw.param1;
    new_item_int.param2 = transfer_mission_raw.param2;
    new_item_int.param3 = transfer_mission_raw.param3;
    new_item_int.param4 = transfer_mission_raw.param4;
    new_item_int.x = transfer_mission_raw.x;
    new_item_int.y = transfer_mission_raw.y;
    new_item_int.z = transfer_mission_raw.z;
    new_item_int.mission_type = transfer_mission_raw.mission_type;

    return new_item_int;
}

std::vector<MAVLinkMissionTransfer::ItemInt>
MissionRawImpl::convert_to_int_items(const std::vector<MissionRaw::MissionItem>& mission_raw)
{
    std::vector<MAVLinkMissionTransfer::ItemInt> int_items;

    for (const auto& item : mission_raw) {
        int_items.push_back(convert_mission_raw(item));
    }

    std::lock_guard<std::mutex> lock(_mission_progress.mutex);
    _mission_progress.last.total = int_items.size();

    return int_items;
}

MissionRaw::MissionItem
MissionRawImpl::convert_item(const MAVLinkMissionTransfer::ItemInt& transfer_item)
{
    MissionRaw::MissionItem new_item;

    new_item.seq = transfer_item.seq;
    new_item.frame = transfer_item.frame;
    new_item.command = transfer_item.command;
    new_item.current = transfer_item.current;
    new_item.autocontinue = transfer_item.autocontinue;
    new_item.param1 = transfer_item.param1;
    new_item.param2 = transfer_item.param2;
    new_item.param3 = transfer_item.param3;
    new_item.param4 = transfer_item.param4;
    new_item.x = transfer_item.x;
    new_item.y = transfer_item.y;
    new_item.z = transfer_item.z;
    new_item.mission_type = transfer_item.mission_type;

    return new_item;
}

std::vector<MissionRaw::MissionItem>
MissionRawImpl::convert_items(const std::vector<MAVLinkMissionTransfer::ItemInt>& transfer_items)
{
    std::vector<MissionRaw::MissionItem> new_items;
    new_items.reserve(transfer_items.size());

    for (const auto& transfer_item : transfer_items) {
        new_items.push_back(convert_item(transfer_item));
    }

    std::lock_guard<std::mutex> lock(_mission_progress.mutex);
    _mission_progress.last.total = new_items.size();

    return new_items;
}

MissionRaw::Result MissionRawImpl::start_mission()
{
    auto prom = std::promise<MissionRaw::Result>();
    auto fut = prom.get_future();

    start_mission_async([&prom](MissionRaw::Result result) { prom.set_value(result); });
    return fut.get();
}

void MissionRawImpl::start_mission_async(const MissionRaw::result_callback_t& callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::MISSION, [this, callback](MAVLinkCommands::Result result, float) {
            report_flight_mode_change(callback, result);
        });
}

MissionRaw::Result MissionRawImpl::pause_mission()
{
    auto prom = std::promise<MissionRaw::Result>();
    auto fut = prom.get_future();

    pause_mission_async([&prom](MissionRaw::Result result) { prom.set_value(result); });
    return fut.get();
}

void MissionRawImpl::pause_mission_async(const MissionRaw::result_callback_t& callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::HOLD, [this, callback](MAVLinkCommands::Result result, float) {
            report_flight_mode_change(callback, result);
        });
}

void MissionRawImpl::report_flight_mode_change(
    MissionRaw::result_callback_t callback, MAVLinkCommands::Result result)
{
    if (!callback) {
        return;
    }

    _parent->call_user_callback(
        [callback, result]() { callback(command_result_to_mission_result(result)); });
}

MissionRaw::Result MissionRawImpl::command_result_to_mission_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return MissionRaw::Result::Success;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkCommands::Result::BUSY:
            return MissionRaw::Result::Busy;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkCommands::Result::TIMEOUT:
            return MissionRaw::Result::Timeout;
        case MAVLinkCommands::Result::IN_PROGRESS:
            return MissionRaw::Result::Busy; // FIXME
        case MAVLinkCommands::Result::UNKNOWN_ERROR:
            return MissionRaw::Result::Unknown;
        default:
            return MissionRaw::Result::Unknown;
    }
}

MissionRaw::Result MissionRawImpl::clear_mission()
{
    auto prom = std::promise<MissionRaw::Result>();
    auto fut = prom.get_future();

    clear_mission_async([&prom](MissionRaw::Result result) { prom.set_value(result); });
    return fut.get();
}

void MissionRawImpl::clear_mission_async(const MissionRaw::result_callback_t& callback)
{
    _parent->mission_transfer().clear_items_async(
        MAV_MISSION_TYPE_MISSION, [this, callback](MAVLinkMissionTransfer::Result result) {
            auto converted_result = convert_result(result);
            _parent->call_user_callback([callback, converted_result]() {
                if (callback) {
                    callback(converted_result);
                }
            });
        });
}

MissionRaw::Result MissionRawImpl::set_current_mission_item(int index)
{
    auto prom = std::promise<MissionRaw::Result>();
    auto fut = prom.get_future();

    set_current_mission_item_async(
        index, [&prom](MissionRaw::Result result) { prom.set_value(result); });
    return fut.get();
}

void MissionRawImpl::set_current_mission_item_async(
    int index, const MissionRaw::result_callback_t& callback)
{
    if (index < 0 && index >= _mission_progress.last.total) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(MissionRaw::Result::InvalidArgument);
                return;
            }
        });
    }

    _parent->mission_transfer().set_current_item_async(
        index, [this, callback](MAVLinkMissionTransfer::Result result) {
            auto converted_result = convert_result(result);
            _parent->call_user_callback([callback, converted_result]() {
                if (callback) {
                    callback(converted_result);
                }
            });
        });
}

void MissionRawImpl::report_progress_current()
{
    std::lock_guard<std::mutex> lock(_mission_progress.mutex);

    if (_mission_progress.callback == nullptr) {
        return;
    }

    bool should_report = false;
    {
        if (_mission_progress.last_reported.current != _mission_progress.last.current) {
            _mission_progress.last_reported.current = _mission_progress.last.current;
            should_report = true;
        }
        if (_mission_progress.last_reported.total != _mission_progress.last.total) {
            _mission_progress.last_reported.total = _mission_progress.last.total;
            should_report = true;
        }
    }

    if (should_report) {
        const auto last = _mission_progress.last;
        const auto temp_callback = _mission_progress.callback;
        _parent->call_user_callback([temp_callback, last]() { temp_callback(last); });
    }
}

void MissionRawImpl::mission_progress_async(MissionRaw::mission_progress_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_progress.mutex);
    _mission_progress.callback = callback;
}

MissionRaw::MissionProgress MissionRawImpl::mission_progress()
{
    std::lock_guard<std::mutex> lock(_mission_progress.mutex);
    return _mission_progress.last;
}

void MissionRawImpl::mission_changed_async(MissionRaw::mission_changed_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    _mission_changed.callback = callback;
}

MissionRaw::Result MissionRawImpl::convert_result(MAVLinkMissionTransfer::Result result)
{
    switch (result) {
        case MAVLinkMissionTransfer::Result::Success:
            return MissionRaw::Result::Success;
        case MAVLinkMissionTransfer::Result::ConnectionError:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::Denied:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::TooManyMissionItems:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::Timeout:
            return MissionRaw::Result::Timeout;
        case MAVLinkMissionTransfer::Result::Unsupported:
            return MissionRaw::Result::Unsupported;
        case MAVLinkMissionTransfer::Result::UnsupportedFrame:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::NoMissionAvailable:
            return MissionRaw::Result::NoMissionAvailable;
        case MAVLinkMissionTransfer::Result::Cancelled:
            return MissionRaw::Result::TransferCancelled;
        case MAVLinkMissionTransfer::Result::MissionTypeNotConsistent:
            return MissionRaw::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidSequence:
            return MissionRaw::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::CurrentInvalid:
            return MissionRaw::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::ProtocolError:
            return MissionRaw::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidParam:
            return MissionRaw::Result::InvalidArgument; // FIXME
        default:
            return MissionRaw::Result::Unknown;
    }
}

} // namespace mavsdk
