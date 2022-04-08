#include "mission_raw_server_impl.h"

namespace mavsdk {

MissionRawServerImpl::MissionRawServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
}

MissionRawServerImpl::~MissionRawServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

MavlinkMissionTransfer::ItemInt
convert_mission_raw(const MissionRawServer::MissionItem transfer_mission_raw)
{
    MavlinkMissionTransfer::ItemInt new_item_int{};

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

std::vector<MavlinkMissionTransfer::ItemInt>
convert_to_int_items(const std::vector<MissionRawServer::MissionItem>& mission_raw)
{
    std::vector<MavlinkMissionTransfer::ItemInt> int_items;

    for (const auto& item : mission_raw) {
        int_items.push_back(convert_mission_raw(item));
    }

    return int_items;
}

MissionRawServer::MissionItem convert_item(const MavlinkMissionTransfer::ItemInt& transfer_item)
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
convert_items(const std::vector<MavlinkMissionTransfer::ItemInt>& transfer_items)
{
    std::vector<MissionRawServer::MissionItem> new_items;
    new_items.reserve(transfer_items.size());

    for (const auto& transfer_item : transfer_items) {
        new_items.push_back(convert_item(transfer_item));
    }

    return new_items;
}

MissionRawServer::Result convert_result(MavlinkMissionTransfer::Result result)
{
    switch (result) {
        case MavlinkMissionTransfer::Result::Success:
            return MissionRawServer::Result::Success;
        case MavlinkMissionTransfer::Result::ConnectionError:
            return MissionRawServer::Result::Error; // FIXME
        case MavlinkMissionTransfer::Result::Denied:
            return MissionRawServer::Result::Error; // FIXME
        case MavlinkMissionTransfer::Result::TooManyMissionItems:
            return MissionRawServer::Result::TooManyMissionItems;
        case MavlinkMissionTransfer::Result::Timeout:
            return MissionRawServer::Result::Timeout;
        case MavlinkMissionTransfer::Result::Unsupported:
            return MissionRawServer::Result::Unsupported;
        case MavlinkMissionTransfer::Result::UnsupportedFrame:
            return MissionRawServer::Result::Unsupported;
        case MavlinkMissionTransfer::Result::NoMissionAvailable:
            return MissionRawServer::Result::NoMissionAvailable;
        case MavlinkMissionTransfer::Result::Cancelled:
            return MissionRawServer::Result::TransferCancelled;
        case MavlinkMissionTransfer::Result::MissionTypeNotConsistent:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransfer::Result::InvalidSequence:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransfer::Result::CurrentInvalid:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransfer::Result::ProtocolError:
            return MissionRawServer::Result::Error; // FIXME
        case MavlinkMissionTransfer::Result::InvalidParam:
            return MissionRawServer::Result::InvalidArgument; // FIXME
        case MavlinkMissionTransfer::Result::IntMessagesNotSupported:
            return MissionRawServer::Result::Unsupported; // FIXME
        default:
            return MissionRawServer::Result::Unknown;
    }
}

void MissionRawServerImpl::init()
{
    _server_component_impl->add_capabilities(MAV_PROTOCOL_CAPABILITY_MISSION_INT);

    _thread_mission = std::thread([this] {
        while (!_stop_work_thread) {
            std::unique_lock<std::mutex> lock(_work_mutex);
            if (!_work_queue.empty()) {
                auto task = _work_queue.front();
                _work_queue.pop();
                lock.unlock();
                task();
            } else {
                _wait_for_new_task.wait(lock);
            }
        }
    });

    // Handle Initiate Upload
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_COUNT,
        [this](const mavlink_message_t& message) {
            LogDebug() << "Receive Mission Count in Server";

            // Decode the count
            _target_component = message.compid;
            mavlink_mission_count_t count;
            mavlink_msg_mission_count_decode(&message, &count);
            _mission_count = count.count;

            // We need to queue this on a different thread or it will deadlock
            add_task([this, target_system_id = message.sysid]() {
                // Mission Upload Inbound
                if (_last_download.lock()) {
                    _server_component_impl->call_user_callback([this]() {
                        if (_incoming_mission_callback) {
                            MissionRawServer::MissionPlan mission_plan{};
                            _incoming_mission_callback(
                                MissionRawServer::Result::Busy, mission_plan);
                        }
                    });
                    return;
                }

                _server_component_impl->set_our_current_target_system_id(target_system_id);

                _last_download =
                    _server_component_impl->mission_transfer().receive_incoming_items_async(
                        MAV_MISSION_TYPE_MISSION,
                        _mission_count,
                        _target_component,
                        [this](
                            MavlinkMissionTransfer::Result result,
                            std::vector<MavlinkMissionTransfer::ItemInt> items) {
                            _current_mission = items;
                            auto converted_result = convert_result(result);
                            auto converted_items = convert_items(items);
                            _server_component_impl->call_user_callback([this,
                                                                        converted_result,
                                                                        converted_items]() {
                                if (_incoming_mission_callback) {
                                    _incoming_mission_callback(converted_result, {converted_items});
                                }

                                _mission_completed = false;
                                set_current_seq(0);
                            });
                        });
            });
        },
        this);

    // Handle Set Current from GCS
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_SET_CURRENT,
        [this](const mavlink_message_t& message) {
            LogDebug() << "Receive Mission Set Current";

            mavlink_mission_set_current_t set_current;
            mavlink_msg_mission_set_current_decode(&message, &set_current);

            if (_current_mission.size() == 0) {
                mavlink_message_t status_message;
                mavlink_msg_statustext_pack(
                    _server_component_impl->get_own_system_id(),
                    _server_component_impl->get_own_component_id(),
                    &status_message,
                    MAV_SEVERITY_ERROR,
                    "No Mission Loaded",
                    0,
                    0);
                _server_component_impl->send_message(status_message);
            } else if (_current_mission.size() <= set_current.seq) {
                mavlink_message_t status_message;
                mavlink_msg_statustext_pack(
                    _server_component_impl->get_own_system_id(),
                    _server_component_impl->get_own_component_id(),
                    &status_message,
                    MAV_SEVERITY_ERROR,
                    "Unknown Mission seq id",
                    0,
                    0);
                _server_component_impl->send_message(status_message);
            } else {
                set_current_seq(set_current.seq);
            }
        },
        this);

    // Handle Clears
    _server_component_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CLEAR_ALL,
        [this](const mavlink_message_t& message) {
            mavlink_mission_clear_all_t clear_all;
            mavlink_msg_mission_clear_all_decode(&message, &clear_all);
            if (clear_all.mission_type == MAV_MISSION_TYPE_ALL ||
                clear_all.mission_type == MAV_MISSION_TYPE_MISSION) {
                _current_mission.clear();
                _current_seq = 0;
                _server_component_impl->call_user_callback([this, clear_all]() {
                    if (_clear_all_callback) {
                        _clear_all_callback(clear_all.mission_type);
                    }
                });
            }

            // Send the MISSION_ACK
            mavlink_message_t ack_message;
            mavlink_msg_mission_ack_pack(
                _server_component_impl->get_own_system_id(),
                _server_component_impl->get_own_component_id(),
                &ack_message,
                message.sysid,
                message.compid,
                MAV_MISSION_RESULT::MAV_MISSION_ACCEPTED,
                clear_all.mission_type);
            _server_component_impl->send_message(ack_message);
        },
        this);
}

void MissionRawServerImpl::deinit()
{
    _stop_work_thread = true;
    _wait_for_new_task.notify_all();
    _thread_mission.join();
}

void MissionRawServerImpl::subscribe_incoming_mission(
    const MissionRawServer::IncomingMissionCallback callback)
{
    _incoming_mission_callback = callback;
}

MissionRawServer::MissionPlan MissionRawServerImpl::incoming_mission() const
{
    return {};
}

void MissionRawServerImpl::subscribe_current_item_changed(
    MissionRawServer::CurrentItemChangedCallback callback)
{
    _current_item_changed_callback = callback;
}

void MissionRawServerImpl::subscribe_clear_all(MissionRawServer::ClearAllCallback callback)
{
    _clear_all_callback = callback;
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

    mavlink_message_t mission_reached;
    mavlink_msg_mission_item_reached_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &mission_reached,
        static_cast<uint16_t>(_current_seq));
    _server_component_impl->send_message(mission_reached);

    if (_current_seq + 1 == _current_mission.size()) {
        _mission_completed = true;
    }
    // This will publish 0 - N mission current
    // mission_current mission.size() is end of mission!
    set_current_seq(_current_seq + 1);
}

void MissionRawServerImpl::set_current_seq(std::size_t seq)
{
    if (_current_mission.size() < static_cast<size_t>(seq)) {
        return;
    }

    _current_seq = seq;

    // If mission is over, just set item to last one again
    auto item = seq == _current_mission.size() ? _current_mission.back() :
                                                 _current_mission.at(_current_seq);
    auto converted_item = convert_item(item);
    _server_component_impl->call_user_callback([this, converted_item]() {
        if (_current_item_changed_callback) {
            _current_item_changed_callback(converted_item);
        }
    });

    mavlink_message_t mission_current;
    mavlink_msg_mission_current_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &mission_current,
        static_cast<uint16_t>(_current_seq));
    _server_component_impl->send_message(mission_current);
}

} // namespace mavsdk
