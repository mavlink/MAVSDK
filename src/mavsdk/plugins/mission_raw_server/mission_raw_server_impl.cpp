#include "mission_raw_server_impl.h"
#include "callback_list.tpp"
#include "mavlink_address.h"

namespace mavsdk {

template class CallbackList<MissionRawServer::Result, MissionRawServer::MissionPlan>;
template class CallbackList<MissionRawServer::MissionItem>;
template class CallbackList<uint32_t>;

MissionRawServerImpl::MissionRawServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
}

MissionRawServerImpl::~MissionRawServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

MavlinkMissionTransferServer::ItemInt
convert_mission_raw(const MissionRawServer::MissionItem transfer_mission_raw)
{
    MavlinkMissionTransferServer::ItemInt new_item_int{};

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

std::vector<MavlinkMissionTransferServer::ItemInt>
convert_to_int_items(const std::vector<MissionRawServer::MissionItem>& mission_raw)
{
    std::vector<MavlinkMissionTransferServer::ItemInt> int_items;

    for (const auto& item : mission_raw) {
        int_items.push_back(convert_mission_raw(item));
    }

    return int_items;
}

MissionRawServer::MissionItem
convert_item(const MavlinkMissionTransferServer::ItemInt& transfer_item)
{
    MissionRawServer::MissionItem new_item;

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

std::vector<MissionRawServer::MissionItem>
convert_items(const std::vector<MavlinkMissionTransferServer::ItemInt>& transfer_items)
{
    std::vector<MissionRawServer::MissionItem> new_items;
    new_items.reserve(transfer_items.size());

    for (const auto& transfer_item : transfer_items) {
        new_items.push_back(convert_item(transfer_item));
    }

    return new_items;
}

MissionRawServer::Result convert_result(MavlinkMissionTransferServer::Result result)
{
    switch (result) {
        case MavlinkMissionTransferServer::Result::Success:
            return MissionRawServer::Result::Success;
        case MavlinkMissionTransferServer::Result::ConnectionError:
            return MissionRawServer::Result::Error; // FIXME
        case MavlinkMissionTransferServer::Result::Denied:
            return MissionRawServer::Result::Error; // FIXME
        case MavlinkMissionTransferServer::Result::TooManyMissionItems:
            return MissionRawServer::Result::TooManyMissionItems;
        case MavlinkMissionTransferServer::Result::Timeout:
            return MissionRawServer::Result::Timeout;
        case MavlinkMissionTransferServer::Result::Unsupported:
            return MissionRawServer::Result::Unsupported;
        case MavlinkMissionTransferServer::Result::UnsupportedFrame:
            return MissionRawServer::Result::Unsupported;
        case MavlinkMissionTransferServer::Result::NoMissionAvailable:
            return MissionRawServer::Result::NoMissionAvailable;
        case MavlinkMissionTransferServer::Result::Cancelled:
            return MissionRawServer::Result::TransferCancelled;
        case MavlinkMissionTransferServer::Result::MissionTypeNotConsistent:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferServer::Result::InvalidSequence:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferServer::Result::CurrentInvalid:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferServer::Result::ProtocolError:
            return MissionRawServer::Result::Error; // FIXME
        case MavlinkMissionTransferServer::Result::InvalidParam:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransferServer::Result::IntMessagesNotSupported:
            return MissionRawServer::Result::Unsupported; // FIXME
        default:
            return MissionRawServer::Result::Unknown;
    }
}

void MissionRawServerImpl::init()
{
    _server_component_impl->add_capabilities(MAV_PROTOCOL_CAPABILITY_MISSION_INT);

    // Handle Initiate Upload
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_COUNT,
        [this](const mavlink_message_t& message) { process_mission_count(message); },
        this);

    // Handle Set Current from GCS
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_SET_CURRENT,
        [this](const mavlink_message_t& message) { process_mission_set_current(message); },
        this);

    // Handle Clears
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CLEAR_ALL,
        [this](const mavlink_message_t& message) { process_mission_clear(message); },
        this);
}

void MissionRawServerImpl::deinit()
{
    _server_component_impl->unregister_all_mavlink_message_handlers(this);
}

void MissionRawServerImpl::process_mission_count(const mavlink_message_t& message)
{
    UNUSED(message);

    // Decode the count
    _target_system_id = message.sysid;
    _target_component_id = message.compid;

    mavlink_mission_count_t count;
    mavlink_msg_mission_count_decode(&message, &count);
    _mission_count = count.count;

    // Mission Upload Inbound
    if (_last_download.lock()) {
        _incoming_mission_callbacks.queue(
            MissionRawServer::Result::Busy,
            MissionRawServer::MissionPlan{},
            [this](const auto& func) { _server_component_impl->call_user_callback(func); });
        return;
    }

    _last_download = _server_component_impl->mission_transfer_server().receive_incoming_items_async(
        MAV_MISSION_TYPE_MISSION,
        _mission_count,
        _target_system_id,
        _target_component_id,
        [this](
            MavlinkMissionTransferServer::Result result,
            std::vector<MavlinkMissionTransferServer::ItemInt> items) {
            _current_mission = items;
            auto converted_result = convert_result(result);
            auto converted_items = convert_items(items);
            _incoming_mission_callbacks.queue(
                converted_result, {converted_items}, [this](const auto& func) {
                    _server_component_impl->call_user_callback(func);
                });
            _mission_completed = false;
            set_current_seq(0);
        });
}

void MissionRawServerImpl::process_mission_set_current(const mavlink_message_t& message)
{
    LogDebug() << "Receive Mission Set Current";

    mavlink_mission_set_current_t set_current;
    mavlink_msg_mission_set_current_decode(&message, &set_current);

    if (_current_mission.size() == 0) {
        const char text[50] = "No Mission Loaded";
        _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t response_message;
            mavlink_msg_statustext_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &response_message,
                MAV_SEVERITY_ERROR,
                text,
                0,
                0);
            return response_message;
        });
    } else if (_current_mission.size() <= set_current.seq) {
        const char text[50] = "Unknown Mission seq id";
        _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t response_message;
            mavlink_msg_statustext_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &response_message,
                MAV_SEVERITY_ERROR,
                text,
                0,
                0);
            return response_message;
        });
    } else {
        set_current_seq(set_current.seq);
    }
}

void MissionRawServerImpl::process_mission_clear(const mavlink_message_t message)
{
    mavlink_mission_clear_all_t clear_all;
    mavlink_msg_mission_clear_all_decode(&message, &clear_all);
    if (clear_all.mission_type == MAV_MISSION_TYPE_ALL ||
        clear_all.mission_type == MAV_MISSION_TYPE_MISSION) {
        _current_mission.clear();
        _current_seq = 0;
        _clear_all_callbacks.queue(clear_all.mission_type, [this](const auto& func) {
            _server_component_impl->call_user_callback(func);
        });
    }

    // Send the MISSION_ACK
    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t response_message;
        mavlink_msg_mission_ack_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &response_message,
            message.sysid,
            message.compid,
            MAV_MISSION_RESULT::MAV_MISSION_ACCEPTED,
            clear_all.mission_type,
            0);
        return response_message;
    });
}

MissionRawServer::IncomingMissionHandle MissionRawServerImpl::subscribe_incoming_mission(
    const MissionRawServer::IncomingMissionCallback& callback)
{
    return _incoming_mission_callbacks.subscribe(callback);
}

void MissionRawServerImpl::unsubscribe_incoming_mission(
    MissionRawServer::IncomingMissionHandle handle)
{
    _incoming_mission_callbacks.unsubscribe(handle);
}

MissionRawServer::MissionPlan MissionRawServerImpl::incoming_mission() const
{
    // FIXME: this doesn't look right.
    return {};
}

MissionRawServer::CurrentItemChangedHandle MissionRawServerImpl::subscribe_current_item_changed(
    const MissionRawServer::CurrentItemChangedCallback& callback)
{
    return _current_item_changed_callbacks.subscribe(callback);
}

void MissionRawServerImpl::unsubscribe_current_item_changed(
    MissionRawServer::CurrentItemChangedHandle handle)
{
    _current_item_changed_callbacks.unsubscribe(handle);
}

MissionRawServer::ClearAllHandle
MissionRawServerImpl::subscribe_clear_all(const MissionRawServer::ClearAllCallback& callback)
{
    return _clear_all_callbacks.subscribe(callback);
}

void MissionRawServerImpl::unsubscribe_clear_all(MissionRawServer::ClearAllHandle handle)
{
    _clear_all_callbacks.unsubscribe(handle);
}

uint32_t MissionRawServerImpl::clear_all() const
{
    // TO-DO
    return {};
}

MissionRawServer::MissionItem MissionRawServerImpl::current_item_changed() const
{
    // TO-DO
    return {};
}

void MissionRawServerImpl::set_current_item_complete()
{
    if (_current_seq + 1 > _current_mission.size()) {
        return;
    }

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_mission_item_reached_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint16_t>(_current_seq));
        return message;
    });

    if (_current_seq + 1 == _current_mission.size()) {
        _mission_completed = true;
    }
    // This will publish 0 - N mission current
    // mission_current mission.size() is end of mission!
    set_current_seq(_current_seq + 1);
}

void MissionRawServerImpl::set_current_seq(std::size_t seq)
{
    if (_current_mission.size() < static_cast<size_t>(seq) || _current_mission.empty()) {
        return;
    }

    _current_seq = seq;

    // If mission is over, just set item to last one again
    auto item = seq == _current_mission.size() ? _current_mission.back() :
                                                 _current_mission.at(_current_seq);
    auto converted_item = convert_item(item);
    _current_item_changed_callbacks.queue(converted_item, [this](const auto& func) {
        _server_component_impl->call_user_callback(func);
    });

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_mission_current_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint16_t>(_current_seq),
            0,
            0,
            0,
            0,
            0,
            0);
        return message;
    });
}

} // namespace mavsdk
