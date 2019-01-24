#include "mission_raw_impl.h"
#include "system.h"
#include "global_include.h"
#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <cmath>

namespace dronecode_sdk {

using namespace std::placeholders; // for `_1`

MissionRawImpl::MissionRawImpl(System &system) : PluginImplBase(system)
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
        MAVLINK_MSG_ID_MISSION_COUNT,
        std::bind(&MissionRawImpl::process_mission_count, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_INT,
        std::bind(&MissionRawImpl::process_mission_item_int, this, _1),
        this);
}

void MissionRawImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionRawImpl::enable() {}

void MissionRawImpl::disable() {}

void MissionRawImpl::process_mission_ack(const mavlink_message_t &message)
{
    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (mission_ack.type != MAV_MISSION_ACCEPTED) {
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

void MissionRawImpl::download_mission_async(
    const MissionRaw::mission_items_and_result_callback_t &callback)
{
    {
        std::lock_guard<std::mutex> lock(_mission_download.mutex);
        if (_mission_download.state != MissionDownload::State::NONE) {
            if (callback) {
                std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_vec{};
                _parent->call_user_callback(
                    [callback, empty_vec]() { callback(MissionRaw::Result::BUSY, empty_vec); });
            }
            return;
        }

        _mission_download.state = MissionDownload::State::REQUEST_LIST;
        _mission_download.retries = 0;
        _mission_download.mavlink_mission_items_downloaded.clear();
        _mission_download.callback = callback;
    }

    _parent->register_timeout_handler(
        std::bind(&MissionRawImpl::do_download_step, this), 0.0, nullptr);
}

void MissionRawImpl::do_download_step()
{
    std::lock_guard<std::mutex> lock(_mission_download.mutex);
    switch (_mission_download.state) {
        case MissionDownload::State::NONE:
            LogWarn() << "Invalid state: do_download_step and State::NONE";
            break;
        case MissionDownload::State::REQUEST_LIST:
            request_list();
            break;
        case MissionDownload::State::REQUEST_ITEM:
            request_item();
            break;
        case MissionDownload::State::SHOULD_ACK:
            send_ack();
            break;
    }
}

void MissionRawImpl::request_list()
{
    // Requires _mission_download.mutex.

    if (_mission_download.retries++ >= 3) {
        // We tried multiple times without success, let's give up.
        _mission_download.state = MissionDownload::State::NONE;
        if (_mission_download.callback) {
            std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_vec{};
            auto callback = _mission_download.callback;
            _parent->call_user_callback(
                [callback, empty_vec]() { callback(MissionRaw::Result::TIMEOUT, empty_vec); });
        }
        return;
    }

    // LogDebug() << "Requesting mission list (" << _mission_download.retries << ")";

    mavlink_message_t message;
    mavlink_msg_mission_request_list_pack(GCSClient::system_id,
                                          GCSClient::component_id,
                                          &message,
                                          _parent->get_system_id(),
                                          _parent->get_autopilot_id(),
                                          MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        // This is a terrible and unexpected error. Therefore we don't even
        // retry but just give up.
        _mission_download.state = MissionDownload::State::NONE;
        if (_mission_download.callback) {
            std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_vec{};
            auto callback = _mission_download.callback;
            _parent->call_user_callback(
                [callback, empty_vec]() { callback(MissionRaw::Result::ERROR, empty_vec); });
        }
        return;
    }

    // We retry the list request and mission item request, so we use the lower timeout.
    _parent->register_timeout_handler(
        std::bind(&MissionRawImpl::do_download_step, this), RETRY_TIMEOUT_S, &_timeout_cookie);
}

void MissionRawImpl::process_mission_count(const mavlink_message_t &message)
{
    // LogDebug() << "Received mission count";

    std::lock_guard<std::mutex> lock(_mission_download.mutex);
    if (_mission_download.state != MissionDownload::State::REQUEST_LIST) {
        return;
    }

    mavlink_mission_count_t mission_count;
    mavlink_msg_mission_count_decode(&message, &mission_count);

    _mission_download.num_mission_items_to_download = mission_count.count;
    _mission_download.next_mission_item_to_download = 0;
    _mission_download.retries = 0;

    // We can get rid of the timeout and schedule and do the next step straightaway.
    _parent->unregister_timeout_handler(_timeout_cookie);

    _mission_download.state = MissionDownload::State::REQUEST_ITEM;

    // Let's just add this to the queue, this way we don't block the receive thread
    // and let go of the mutex as well.
    _parent->register_timeout_handler(
        std::bind(&MissionRawImpl::do_download_step, this), 0.0, nullptr);
}

void MissionRawImpl::request_item()
{
    // Requires _mission_download.mutex.

    if (_mission_download.retries++ >= 3) {
        // We tried multiple times without success, let's give up.
        _mission_download.state = MissionDownload::State::NONE;
        if (_mission_download.callback) {
            std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_vec{};
            auto callback = _mission_download.callback;
            _parent->call_user_callback(
                [callback, empty_vec]() { callback(MissionRaw::Result::TIMEOUT, empty_vec); });
        }
        return;
    }

    // LogDebug() << "Requesting mission item " << _mission_download.next_mission_item_to_download
    //            << " (" << _mission_download.retries << ")";

    mavlink_message_t message;
    mavlink_msg_mission_request_int_pack(GCSClient::system_id,
                                         GCSClient::component_id,
                                         &message,
                                         _parent->get_system_id(),
                                         _parent->get_autopilot_id(),
                                         _mission_download.next_mission_item_to_download,
                                         MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        // This is a terrible and unexpected error. Therefore we don't even
        // retry but just give up.
        _mission_download.state = MissionDownload::State::NONE;
        if (_mission_download.callback) {
            std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_vec{};
            auto callback = _mission_download.callback;
            _parent->call_user_callback(
                [callback, empty_vec]() { callback(MissionRaw::Result::ERROR, empty_vec); });
        }
        return;
    }

    // We retry the list request and mission item request, so we use the lower timeout.
    _parent->register_timeout_handler(
        std::bind(&MissionRawImpl::do_download_step, this), RETRY_TIMEOUT_S, &_timeout_cookie);
}

void MissionRawImpl::process_mission_item_int(const mavlink_message_t &message)
{
    // LogDebug() << "Received mission item int";

    std::lock_guard<std::mutex> lock(_mission_download.mutex);
    if (_mission_download.state != MissionDownload::State::REQUEST_ITEM) {
        return;
    }

    mavlink_mission_item_int_t mission_item_int;
    mavlink_msg_mission_item_int_decode(&message, &mission_item_int);

    if (mission_item_int.seq != _mission_download.next_mission_item_to_download) {
        LogWarn() << "Received mission item " << int(mission_item_int.seq) << " instead of "
                  << _mission_download.next_mission_item_to_download << " (ignored)";

        // The timeout will happen anyway and retry for this case.
        return;
    }

    auto new_item = std::make_shared<MissionRaw::MavlinkMissionItemInt>();

    new_item->target_system = mission_item_int.target_system;
    new_item->target_component = mission_item_int.target_component;
    new_item->seq = mission_item_int.seq;
    new_item->frame = mission_item_int.frame;
    new_item->command = mission_item_int.command;
    new_item->current = mission_item_int.current;
    new_item->autocontinue = mission_item_int.autocontinue;
    new_item->param1 = mission_item_int.param1;
    new_item->param2 = mission_item_int.param2;
    new_item->param3 = mission_item_int.param3;
    new_item->param4 = mission_item_int.param4;
    new_item->x = mission_item_int.x;
    new_item->y = mission_item_int.y;
    new_item->z = mission_item_int.z;
    new_item->mission_type = mission_item_int.mission_type;

    _mission_download.mavlink_mission_items_downloaded.push_back(new_item);
    ++_mission_download.next_mission_item_to_download;
    _mission_download.retries = 0;

    if (_mission_download.next_mission_item_to_download ==
        _mission_download.num_mission_items_to_download) {
        _mission_download.state = MissionDownload::State::SHOULD_ACK;
    }

    // We can remove timeout.
    _parent->unregister_timeout_handler(_timeout_cookie);

    // And schedule the next request.
    _parent->register_timeout_handler(
        std::bind(&MissionRawImpl::do_download_step, this), 0.0, nullptr);
}

void MissionRawImpl::send_ack()
{
    // Requires _mission_download.mutex.

    // LogDebug() << "Sending ack";

    mavlink_message_t message;
    mavlink_msg_mission_ack_pack(GCSClient::system_id,
                                 GCSClient::component_id,
                                 &message,
                                 _parent->get_system_id(),
                                 _parent->get_autopilot_id(),
                                 MAV_MISSION_ACCEPTED,
                                 MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        // This is a terrible and unexpected error. Therefore we don't even
        // retry but just give up.
        _mission_download.state = MissionDownload::State::NONE;
        if (_mission_download.callback) {
            std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> empty_vec{};
            auto callback = _mission_download.callback;
            _parent->call_user_callback(
                [callback, empty_vec]() { callback(MissionRaw::Result::ERROR, empty_vec); });
        }
        return;
    }

    // We did it, we are done.
    _mission_download.state = MissionDownload::State::NONE;

    if (_mission_download.callback) {
        std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> vec_copy =
            _mission_download.mavlink_mission_items_downloaded;
        auto callback = _mission_download.callback;
        _parent->call_user_callback(
            [callback, vec_copy]() { callback(MissionRaw::Result::SUCCESS, vec_copy); });
    }
}

void MissionRawImpl::download_mission_cancel() {}

void MissionRawImpl::subscribe_mission_changed(MissionRaw::mission_changed_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_mission_changed.mutex);
    _mission_changed.callback = callback;
}

} // namespace dronecode_sdk
