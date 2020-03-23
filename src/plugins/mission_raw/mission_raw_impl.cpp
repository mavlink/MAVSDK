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

void MissionRawImpl::disable() {}

void MissionRawImpl::deinit()
{
    _parent->unregister_timeout_handler(_timeout_cookie);
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionRawImpl::reset_mission_progress()
{
    std::lock_guard<std::mutex> lock(_mission_data.mutex);
    _mission_data.last_current_mavlink_mission_item = -1;
    _mission_data.last_reached_mavlink_mission_item = -1;
    _mission_data.last_total_mavlink_mission_item = -1;
    _mission_data.last_current_reported_mavlink_mission_item = -1;
    _mission_data.last_total_reported_mavlink_mission_item = -1;
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
        // Local copy because we can't make a copy of member variable.
        auto callback = _mission_changed.callback;
        _parent->call_user_callback([callback]() { callback(); });
    }
}

void MissionRawImpl::process_mission_current(const mavlink_message_t& message)
{
    mavlink_mission_current_t mission_current;
    mavlink_msg_mission_current_decode(&message, &mission_current);

    {
        std::lock_guard<std::mutex> lock(_mission_data.mutex);
        _mission_data.last_current_mavlink_mission_item = mission_current.seq;
    }

    report_progress();
}

void MissionRawImpl::process_mission_item_reached(const mavlink_message_t& message)
{
    mavlink_mission_item_reached_t mission_item_reached;
    mavlink_msg_mission_item_reached_decode(&message, &mission_item_reached);

    {
        std::lock_guard<std::mutex> lock(_mission_data.mutex);
        _mission_data.last_reached_mavlink_mission_item = mission_item_reached.seq;
    }

    report_progress();
}

void MissionRawImpl::upload_mission_async(
    const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>& mission_raw,
    const MissionRaw::result_callback_t& callback)
{
    if (_mission_data.last_upload.lock()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(MissionRaw::Result::BUSY);
            }
        });
        return;
    }

    if (!_parent->does_support_mission_int()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(MissionRaw::Result::UNSUPPORTED);
            }
        });
        return;
    }

    const auto int_items = convert_to_int_items(mission_raw);

    _mission_data.last_upload = _parent->mission_transfer().upload_items_async(
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

void MissionRawImpl::upload_mission_cancel()
{
    auto ptr = _mission_data.last_upload.lock();
    if (ptr) {
        ptr->cancel();
    }
}

void MissionRawImpl::download_mission_async(
    const MissionRaw::mission_items_and_result_callback_t& callback)
{
    if (_mission_data.last_download.lock()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_items;
                callback(MissionRaw::Result::BUSY, empty_items);
            }
        });
        return;
    }

    _mission_data.last_download = _parent->mission_transfer().download_items_async(
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

void MissionRawImpl::download_mission_cancel()
{
    auto ptr = _mission_data.last_download.lock();
    if (ptr) {
        ptr->cancel();
    }
}

MAVLinkMissionTransfer::ItemInt MissionRawImpl::convert_mission_raw(
    const std::shared_ptr<MissionRaw::MavlinkMissionItemInt> transfer_mission_raw)
{
    MAVLinkMissionTransfer::ItemInt new_item_int;

    new_item_int.seq = transfer_mission_raw->seq;
    new_item_int.frame = transfer_mission_raw->frame;
    new_item_int.command = transfer_mission_raw->command;
    new_item_int.current = transfer_mission_raw->current;
    new_item_int.autocontinue = transfer_mission_raw->autocontinue;
    new_item_int.param1 = transfer_mission_raw->param1;
    new_item_int.param2 = transfer_mission_raw->param2;
    new_item_int.param3 = transfer_mission_raw->param3;
    new_item_int.param4 = transfer_mission_raw->param4;
    new_item_int.x = transfer_mission_raw->x;
    new_item_int.y = transfer_mission_raw->y;
    new_item_int.z = transfer_mission_raw->z;
    new_item_int.mission_type = transfer_mission_raw->mission_type;

    return new_item_int;
}

std::vector<MAVLinkMissionTransfer::ItemInt> MissionRawImpl::convert_to_int_items(
    const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>& mission_raw)
{
    std::vector<MAVLinkMissionTransfer::ItemInt> int_items;

    for (const auto& item : mission_raw) {
        int_items.push_back(convert_mission_raw(item));
    }

    _mission_data.last_total_mavlink_mission_item = int_items.size();

    return int_items;
}

MissionRaw::MavlinkMissionItemInt
MissionRawImpl::convert_item(const MAVLinkMissionTransfer::ItemInt& transfer_item)
{
    MissionRaw::MavlinkMissionItemInt new_item;

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

std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>
MissionRawImpl::convert_items(const std::vector<MAVLinkMissionTransfer::ItemInt>& transfer_items)
{
    std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> new_items;
    new_items.reserve(transfer_items.size());

    for (const auto& transfer_item : transfer_items) {
        new_items.push_back(
            std::make_shared<MissionRaw::MavlinkMissionItemInt>(convert_item(transfer_item)));
    }

    _mission_data.last_total_mavlink_mission_item = new_items.size();

    return new_items;
}

void MissionRawImpl::start_mission_async(const MissionRaw::result_callback_t& callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::MISSION, [this, callback](MAVLinkCommands::Result result, float) {
            report_flight_mode_change(callback, result);
        });
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
            return MissionRaw::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkCommands::Result::BUSY:
            return MissionRaw::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkCommands::Result::TIMEOUT:
            return MissionRaw::Result::TIMEOUT;
        case MAVLinkCommands::Result::IN_PROGRESS:
            return MissionRaw::Result::BUSY; // FIXME
        case MAVLinkCommands::Result::UNKNOWN_ERROR:
            return MissionRaw::Result::UNKNOWN;
        default:
            return MissionRaw::Result::UNKNOWN;
    }
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

void MissionRawImpl::set_current_mission_item_async(
    int current_mavlink, MissionRaw::result_callback_t& callback)
{
    if (current_mavlink < 0 && current_mavlink > _mission_data.last_total_mavlink_mission_item) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(MissionRaw::Result::INVALID_ARGUMENT);
                return;
            }
        });
    }

    _parent->mission_transfer().set_current_item_async(
        current_mavlink, [this, callback](MAVLinkMissionTransfer::Result result) {
            auto converted_result = convert_result(result);
            _parent->call_user_callback([callback, converted_result]() {
                if (callback) {
                    callback(converted_result);
                }
            });
        });
}

void MissionRawImpl::report_progress()
{
    const auto temp_callback = _mission_data.progress_callback;
    if (temp_callback == nullptr) {
        return;
    }

    int current = current_mavlink_mission_item();
    int total = total_mavlink_mission_items();

    bool should_report = false;
    {
        std::lock_guard<std::mutex> lock(_mission_data.mutex);
        if (_mission_data.last_current_reported_mavlink_mission_item != current) {
            _mission_data.last_current_reported_mavlink_mission_item = current;
            should_report = true;
        }
        if (_mission_data.last_total_reported_mavlink_mission_item != total) {
            _mission_data.last_total_reported_mavlink_mission_item = total;
            should_report = true;
        }
    }

    if (should_report) {
        std::lock_guard<std::mutex> lock(_mission_data.mutex);
        _parent->call_user_callback([temp_callback, current, total]() {
            LogDebug() << "current: " << current << ", total: " << total;
            temp_callback(current, total);
        });
    }
}

bool MissionRawImpl::is_mission_finished() const
{
    std::lock_guard<std::mutex> lock(_mission_data.mutex);

    if (_mission_data.last_current_mavlink_mission_item < 0) {
        return false;
    }

    if (_mission_data.last_reached_mavlink_mission_item < 0) {
        return false;
    }

    if (_mission_data.last_total_mavlink_mission_item < 0) {
        return false;
    }

    return (
        _mission_data.last_reached_mavlink_mission_item ==
        _mission_data.last_total_mavlink_mission_item);
}

int MissionRawImpl::current_mavlink_mission_item() const
{
    // If the mission is finished, let's return the total as the current
    // to signal this.
    if (is_mission_finished()) {
        return total_mavlink_mission_items();
    }

    std::lock_guard<std::mutex> lock(_mission_data.mutex);

    return _mission_data.last_current_mavlink_mission_item;
}

int MissionRawImpl::total_mavlink_mission_items() const
{
    std::lock_guard<std::mutex> lock(_mission_data.mutex);

    return _mission_data.last_total_mavlink_mission_item;
}

void MissionRawImpl::subscribe_progress(MissionRaw::progress_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_data.mutex);
    _mission_data.progress_callback = callback;
}

void MissionRawImpl::subscribe_mission_changed(MissionRaw::mission_changed_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    _mission_changed.callback = callback;
}

MissionRaw::Result MissionRawImpl::convert_result(MAVLinkMissionTransfer::Result result)
{
    switch (result) {
        case MAVLinkMissionTransfer::Result::Success:
            return MissionRaw::Result::SUCCESS;
        case MAVLinkMissionTransfer::Result::ConnectionError:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::Denied:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::TooManyMissionItems:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::Timeout:
            return MissionRaw::Result::TIMEOUT;
        case MAVLinkMissionTransfer::Result::Unsupported:
            return MissionRaw::Result::UNSUPPORTED;
        case MAVLinkMissionTransfer::Result::UnsupportedFrame:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::NoMissionAvailable:
            return MissionRaw::Result::NO_MISSION_AVAILABLE;
        case MAVLinkMissionTransfer::Result::Cancelled:
            return MissionRaw::Result::CANCELLED;
        case MAVLinkMissionTransfer::Result::MissionTypeNotConsistent:
            return MissionRaw::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidSequence:
            return MissionRaw::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::CurrentInvalid:
            return MissionRaw::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::ProtocolError:
            return MissionRaw::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidParam:
            return MissionRaw::Result::INVALID_ARGUMENT; // FIXME
        default:
            return MissionRaw::Result::UNKNOWN;
    }
}

} // namespace mavsdk
