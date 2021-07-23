#include "mission_server_impl.h"

namespace mavsdk {

using MissionItem = MissionServer::MissionItem;

MissionServerImpl::MissionServerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionServerImpl::MissionServerImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionServerImpl::~MissionServerImpl()
{
    _parent->unregister_plugin(this);
}

MAVLinkMissionTransfer::ItemInt
convert_mission_raw(const MissionServer::MissionItem transfer_mission_raw)
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
convert_to_int_items(const std::vector<MissionServer::MissionItem>& mission_raw)
{
    std::vector<MAVLinkMissionTransfer::ItemInt> int_items;

    for (const auto& item : mission_raw) {
        int_items.push_back(convert_mission_raw(item));
    }

    return int_items;
}

MissionServer::MissionItem convert_item(const MAVLinkMissionTransfer::ItemInt& transfer_item)
{
    MissionServer::MissionItem new_item;

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

std::vector<MissionServer::MissionItem>
convert_items(const std::vector<MAVLinkMissionTransfer::ItemInt>& transfer_items)
{
    std::vector<MissionServer::MissionItem> new_items;
    new_items.reserve(transfer_items.size());

    for (const auto& transfer_item : transfer_items) {
        new_items.push_back(convert_item(transfer_item));
    }

    return new_items;
}

MissionServer::Result convert_result(MAVLinkMissionTransfer::Result result)
{
    switch (result) {
        case MAVLinkMissionTransfer::Result::Success:
            return MissionServer::Result::Success;
        case MAVLinkMissionTransfer::Result::ConnectionError:
            return MissionServer::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::Denied:
            return MissionServer::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::TooManyMissionItems:
            return MissionServer::Result::TooManyMissionItems;
        case MAVLinkMissionTransfer::Result::Timeout:
            return MissionServer::Result::Timeout;
        case MAVLinkMissionTransfer::Result::Unsupported:
            return MissionServer::Result::Unsupported;
        case MAVLinkMissionTransfer::Result::UnsupportedFrame:
            return MissionServer::Result::Unsupported;
        case MAVLinkMissionTransfer::Result::NoMissionAvailable:
            return MissionServer::Result::NoMissionAvailable;
        case MAVLinkMissionTransfer::Result::Cancelled:
            return MissionServer::Result::TransferCancelled;
        case MAVLinkMissionTransfer::Result::MissionTypeNotConsistent:
            return MissionServer::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidSequence:
            return MissionServer::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::CurrentInvalid:
            return MissionServer::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::ProtocolError:
            return MissionServer::Result::Error; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidParam:
            return MissionServer::Result::InvalidArgument; // FIXME
        case MAVLinkMissionTransfer::Result::IntMessagesNotSupported:
            return MissionServer::Result::Unsupported; // FIXME
        default:
            return MissionServer::Result::Unknown;
    }
}

void MissionServerImpl::init()
{
    _thread_mission = std::thread([this] {
        while (true) {
            if (_do_upload) {
                // Mission Upload Inbound
                if (_mission_data.last_download.lock()) {
                    _parent->call_user_callback([this]() {
                        if (_incoming_mission_callback) {
                            MissionServer::MissionPlan mission_plan{};
                            _incoming_mission_callback(MissionServer::Result::Busy, mission_plan);
                        }
                    });
                    return;
                }

                _mission_data.last_download =
                    _parent->mission_transfer().receive_incoming_items_async(
                        MAV_MISSION_TYPE_MISSION,
                        _mission_count,
                        _target_component,
                        [this](
                            MAVLinkMissionTransfer::Result result,
                            std::vector<MAVLinkMissionTransfer::ItemInt> items) {
                            _current_mission = items;
                            auto converted_result = convert_result(result);
                            auto converted_items = convert_items(items);
                            _parent->call_user_callback(
                                [this, converted_result, converted_items]() {
                                    _incoming_mission_callback(converted_result, {converted_items});

                                    _mission_completed = false;
                                    set_current_seq(0);
                                });
                        });
                _do_upload = false;
            }
        }
    });

    // Handle Initiate Upload
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_COUNT,
        [this](const mavlink_message_t& message) {
            LogDebug() << "Receive Mission Count in Server";

            // Decode the count
            _target_component = message.compid;
            mavlink_mission_count_t count;
            mavlink_msg_mission_count_decode(&message, &count);
            _mission_count = count.count;
            _do_upload = true;
        },
        this);

    // Handle Set Current from GCS
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_SET_CURRENT,
        [this](const mavlink_message_t& message) {
            LogDebug() << "Receive Mission Set Current";

            mavlink_mission_set_current_t set_current;
            mavlink_msg_mission_set_current_decode(&message, &set_current);

            if (_current_mission.size() == 0) {
                mavlink_message_t status_message;
                mavlink_msg_statustext_pack(
                    _parent->get_own_system_id(),
                    _parent->get_own_component_id(),
                    &status_message,
                    MAV_SEVERITY_ERROR,
                    "No Mission Loaded",
                    0,
                    0);
                _parent->send_message(status_message);
            } else if (_current_mission.size() <= set_current.seq) {
                mavlink_message_t status_message;
                mavlink_msg_statustext_pack(
                    _parent->get_own_system_id(),
                    _parent->get_own_component_id(),
                    &status_message,
                    MAV_SEVERITY_ERROR,
                    "Unknown Mission seq id",
                    0,
                    0);
                _parent->send_message(status_message);
            } else {
                set_current_seq(set_current.seq);
                mavlink_message_t mission_current;
                mavlink_msg_mission_current_pack(
                    _parent->get_own_system_id(),
                    _parent->get_own_component_id(),
                    &mission_current,
                    set_current.seq);
                _parent->send_message(mission_current);
            }
        },
        this);

    // Handle Clears
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CLEAR_ALL,
        [this](const mavlink_message_t& message) {
            mavlink_mission_clear_all_t clear_all;
            mavlink_msg_mission_clear_all_decode(&message, &clear_all);
            if (clear_all.mission_type == MAV_MISSION_TYPE_ALL ||
                clear_all.mission_type == MAV_MISSION_TYPE_MISSION) {
                _current_mission.clear();
                _current_seq = 0;
                _parent->call_user_callback(
                    [this, clear_all]() { _clear_all_callback(clear_all.mission_type); });
            }

            // Send the MISSION_ACK
            mavlink_message_t ack_message;
            mavlink_msg_mission_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &ack_message,
                message.sysid,
                message.compid,
                MAV_MISSION_RESULT::MAV_MISSION_ACCEPTED,
                clear_all.mission_type);
            _parent->send_message(ack_message);
        },
        this);
}

void MissionServerImpl::deinit() {}

void MissionServerImpl::enable() {}

void MissionServerImpl::disable() {}

void MissionServerImpl::subscribe_incoming_mission(
    const MissionServer::IncomingMissionCallback callback)
{
    _incoming_mission_callback = callback;
}

MissionServer::MissionPlan MissionServerImpl::incoming_mission() const
{
    return {};
}

void MissionServerImpl::subscribe_current_item_changed(
    MissionServer::CurrentItemChangedCallback callback)
{
    _current_item_changed_callback = callback;
}

void MissionServerImpl::subscribe_clear_all(MissionServer::ClearAllCallback callback)
{
    _clear_all_callback = callback;
}

uint32_t MissionServerImpl::clear_all() const
{
    // TO-DO
    return {};
}

MissionServer::MissionItem MissionServerImpl::current_item_changed() const
{
    // TO-DO
    return {};
}

void MissionServerImpl::set_current_item_complete_async(
    const MissionServer::ResultCallback callback)
{
    mavlink_message_t mission_reached;
    mavlink_msg_mission_item_reached_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &mission_reached,
        _current_seq);
    _parent->send_message(mission_reached);
    if (_current_seq + 1 == _current_mission.size())
        _mission_completed = true;
    else
        set_current_seq(_current_seq + 1);
}

void MissionServerImpl::set_current_item_complete() const
{
    // TO-DO
    return;
}

void MissionServerImpl::set_current_seq(int32_t seq)
{
    if (_current_mission.size() <= static_cast<size_t>(seq))
        return;

    _current_seq = seq;

    auto converted_item = convert_item(_current_mission.at(_current_seq));
    _parent->call_user_callback(
        [this, converted_item]() { _current_item_changed_callback(converted_item); });
}

} // namespace mavsdk