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
}

void MissionRawImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionRawImpl::enable() {}

void MissionRawImpl::disable() {}

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
            return MissionRaw::Result::ERROR; // FIXME
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

    return new_items;
}

void MissionRawImpl::download_mission_async(
    const MissionRaw::mission_items_and_result_callback_t& callback)
{
    _parent->mission_transfer().download_items_async(
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
    // TODO: Implement cancel.
}

void MissionRawImpl::subscribe_mission_changed(MissionRaw::mission_changed_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    _mission_changed.callback = callback;
}

} // namespace mavsdk
