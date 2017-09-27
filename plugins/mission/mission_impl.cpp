#include "mission_impl.h"
#include "mission_item_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include <cmath>

namespace dronecore {

MissionImpl::MissionImpl() :
    PluginImplBase(),
    _result_callback(nullptr),
    _last_current_mavlink_mission_item(-1),
    _last_reached_mavlink_mission_item(-1),
    _mission_items(),
    _mavlink_mission_item_messages(),
    _mavlink_mission_item_to_mission_item_indices(),
    _progress_callback(nullptr)
{
}

MissionImpl::~MissionImpl()
{
}

void MissionImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_REQUEST,
        std::bind(&MissionImpl::process_mission_request, this, _1), (void *)this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_REQUEST_INT,
        std::bind(&MissionImpl::process_mission_request_int, this, _1), (void *)this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ACK,
        std::bind(&MissionImpl::process_mission_ack, this, _1), (void *)this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CURRENT,
        std::bind(&MissionImpl::process_mission_current, this, _1), (void *)this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_REACHED,
        std::bind(&MissionImpl::process_mission_item_reached, this, _1), (void *)this);
}

void MissionImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionImpl::process_mission_request(const mavlink_message_t &unused)
{
    // We only support int, so we nack this and thus tell the autopilot to use int.
    UNUSED(unused);

    mavlink_message_t message;
    mavlink_msg_mission_ack_pack(_parent->get_own_system_id(),
                                 _parent->get_own_component_id(),
                                 &message,
                                 _parent->get_target_system_id(),
                                 _parent->get_target_component_id(),
                                 MAV_MISSION_UNSUPPORTED,
                                 MAV_MISSION_TYPE_MISSION);


    _parent->send_message(message);

    // Reset the timeout because we're still communicating.
    _parent->refresh_timeout_handler(_timeout_cookie);
}

void MissionImpl::process_mission_request_int(const mavlink_message_t &message)
{
    mavlink_mission_request_int_t mission_request_int;
    mavlink_msg_mission_request_int_decode(&message, &mission_request_int);

    if (mission_request_int.target_system != _parent->get_own_system_id() &&
        mission_request_int.target_component != _parent->get_own_component_id()) {

        LogWarn() << "Ignore mission request int that is not for us";
        return;
    }

    if (_activity != Activity::SET_MISSION) {
        LogWarn() << "Ignoring mission request int, not active";
        return;
    }

    send_mission_item(mission_request_int.seq);


    // Reset the timeout because we're still communicating.
    _parent->refresh_timeout_handler(_timeout_cookie);
}

void MissionImpl::process_mission_ack(const mavlink_message_t &message)
{
    if (_activity != Activity::SET_MISSION) {
        LogWarn() << "Error: not sure how to process Mission ack.";
        return;
    }

    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (mission_ack.target_system != _parent->get_own_system_id() &&
        mission_ack.target_component != _parent->get_own_component_id()) {

        LogWarn() << "Ignore mission ack that is not for us";
        return;
    }

    // We got some response, so it wasn't a timeout and we can remove it.
    _parent->unregister_timeout_handler(_timeout_cookie);

    if (mission_ack.type == MAV_MISSION_ACCEPTED) {

        // Reset current and reached; we don't want to get confused
        // from earlier messages.
        _last_current_mavlink_mission_item = -1;
        _last_reached_mavlink_mission_item = -1;

        report_mission_result(_result_callback, Mission::Result::SUCCESS);
        LogInfo() << "Mission accepted";
        _activity = Activity::NONE;
    } else if (mission_ack.type == MAV_MISSION_NO_SPACE) {
        LogErr() << "Error: too many waypoints: " << int(mission_ack.type);
        report_mission_result(_result_callback, Mission::Result::TOO_MANY_MISSION_ITEMS);
    } else {
        LogErr() << "Error: unknown mission ack: " << int(mission_ack.type);
        report_mission_result(_result_callback, Mission::Result::ERROR);
    }

}

void MissionImpl::process_mission_current(const mavlink_message_t &message)
{
    mavlink_mission_current_t mission_current;
    mavlink_msg_mission_current_decode(&message, &mission_current);

    if (_last_current_mavlink_mission_item != mission_current.seq) {
        _last_current_mavlink_mission_item = mission_current.seq;
        report_progress();
    }

    if (_activity == Activity::SET_CURRENT &&
        _last_current_mavlink_mission_item == mission_current.seq) {
        report_mission_result(_result_callback, Mission::Result::SUCCESS);
        _last_current_mavlink_mission_item = -1;
        _parent->unregister_timeout_handler(_timeout_cookie);
        _activity = Activity::NONE;
    }
}

void MissionImpl::process_mission_item_reached(const mavlink_message_t &message)
{
    mavlink_mission_item_reached_t mission_item_reached;
    mavlink_msg_mission_item_reached_decode(&message, &mission_item_reached);

    if (_last_reached_mavlink_mission_item != mission_item_reached.seq) {
        _last_reached_mavlink_mission_item = mission_item_reached.seq;
        report_progress();
    }
}

void MissionImpl::send_mission_async(const std::vector<std::shared_ptr<MissionItem>>
                                     &mission_items,
                                     const Mission::result_callback_t &callback)
{
    if (_activity != Activity::NONE) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    if (!_parent->target_supports_mission_int()) {
        LogWarn() << "Mission int messages not supported";
        report_mission_result(callback, Mission::Result::ERROR);
        return;
    }

    copy_mission_item_vector(mission_items);

    assemble_mavlink_messages();

    _activity = Activity::SET_MISSION;

    mavlink_message_t message;
    mavlink_msg_mission_count_pack(_parent->get_own_system_id(),
                                   _parent->get_own_component_id(),
                                   &message,
                                   _parent->get_target_system_id(),
                                   _parent->get_target_component_id(),
                                   _mavlink_mission_item_messages.size(),
                                   MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        report_mission_result(callback, Mission::Result::ERROR);
    }

    _result_callback = callback;
}

void MissionImpl::assemble_mavlink_messages()
{
    // TODO: delete all entries first
    _mavlink_mission_item_messages.clear();

    float last_speed_m_s = NAN;
    float last_gimbal_pitch_deg = NAN;
    float last_gimbal_yaw_deg = NAN;

    unsigned item_i = 0;
    for (auto item : _mission_items) {

        MissionItemImpl &mission_item_impl = (*(item)->_impl);

        if (mission_item_impl.is_position_finite()) {
            // Current is the 0th waypoint
            uint8_t current = ((_mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            auto message = std::make_shared<mavlink_message_t>();
            mavlink_msg_mission_item_int_pack(_parent->get_own_system_id(),
                                              _parent->get_own_component_id(),
                                              message.get(),
                                              _parent->get_target_system_id(),
                                              _parent->get_target_component_id(),
                                              _mavlink_mission_item_messages.size(),
                                              mission_item_impl.get_mavlink_frame(),
                                              mission_item_impl.get_mavlink_cmd(),
                                              current,
                                              mission_item_impl.get_mavlink_autocontinue(),
                                              mission_item_impl.get_mavlink_param1(),
                                              mission_item_impl.get_mavlink_param2(),
                                              mission_item_impl.get_mavlink_param3(),
                                              mission_item_impl.get_mavlink_param4(),
                                              mission_item_impl.get_mavlink_x(),
                                              mission_item_impl.get_mavlink_y(),
                                              mission_item_impl.get_mavlink_z(),
                                              MAV_MISSION_TYPE_MISSION);

            _mavlink_mission_item_to_mission_item_indices.insert(
                std::pair <int, int>
            {static_cast<int>(_mavlink_mission_item_messages.size()), item_i});
            _mavlink_mission_item_messages.push_back(message);
        }

        if (std::isfinite(mission_item_impl.get_speed_m_s()) &&
            !are_equal(last_speed_m_s, mission_item_impl.get_speed_m_s())) {

            // The speed has changed, we need to add a speed command.

            // Current is the 0th waypoint
            uint8_t current = ((_mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            std::shared_ptr<mavlink_message_t> message_speed(new mavlink_message_t());
            mavlink_msg_mission_item_int_pack(_parent->get_own_system_id(),
                                              _parent->get_own_component_id(),
                                              message_speed.get(),
                                              _parent->get_target_system_id(),
                                              _parent->get_target_component_id(),
                                              _mavlink_mission_item_messages.size(),
                                              MAV_FRAME_MISSION,
                                              MAV_CMD_DO_CHANGE_SPEED,
                                              current,
                                              autocontinue,
                                              1.0f, // ground speed
                                              mission_item_impl.get_speed_m_s(),
                                              -1.0f, // no throttle change
                                              0.0f, // absolute
                                              0,
                                              0,
                                              NAN,
                                              MAV_MISSION_TYPE_MISSION);

            _mavlink_mission_item_to_mission_item_indices.insert(
                std::pair <int, int>
            {static_cast<int>(_mavlink_mission_item_messages.size()), item_i});
            _mavlink_mission_item_messages.push_back(message_speed);

            last_speed_m_s = mission_item_impl.get_speed_m_s();
        }

        if ((std::isfinite(mission_item_impl.get_gimbal_yaw_deg()) ||
             std::isfinite(mission_item_impl.get_gimbal_pitch_deg())) &&
            (!are_equal(last_gimbal_yaw_deg, mission_item_impl.get_gimbal_yaw_deg()) ||
             !are_equal(last_gimbal_pitch_deg, mission_item_impl.get_gimbal_pitch_deg()))) {
            // The gimbal has changed, we need to add a gimbal command.

            // Current is the 0th waypoint
            uint8_t current = ((_mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            std::shared_ptr<mavlink_message_t> message_gimbal(new mavlink_message_t());
            mavlink_msg_mission_item_int_pack(_parent->get_own_system_id(),
                                              _parent->get_own_component_id(),
                                              message_gimbal.get(),
                                              _parent->get_target_system_id(),
                                              _parent->get_target_component_id(),
                                              _mavlink_mission_item_messages.size(),
                                              MAV_FRAME_MISSION,
                                              MAV_CMD_DO_MOUNT_CONTROL,
                                              current,
                                              autocontinue,
                                              mission_item_impl.get_gimbal_pitch_deg(), // pitch
                                              0.0f, // roll (yes it is a weird order)
                                              mission_item_impl.get_gimbal_yaw_deg(), // yaw
                                              NAN,
                                              0,
                                              0,
                                              MAV_MOUNT_MODE_MAVLINK_TARGETING,
                                              MAV_MISSION_TYPE_MISSION);

            _mavlink_mission_item_to_mission_item_indices.insert(
                std::pair <int, int>
            {static_cast<int>(_mavlink_mission_item_messages.size()), item_i});
            _mavlink_mission_item_messages.push_back(message_gimbal);

            last_gimbal_yaw_deg = mission_item_impl.get_gimbal_yaw_deg();
            last_gimbal_pitch_deg = mission_item_impl.get_gimbal_pitch_deg();
        }

        if (mission_item_impl.get_camera_action() != MissionItem::CameraAction::NONE) {
            // There is a camera action that we need to send.

            // Current is the 0th waypoint
            uint8_t current = ((_mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            uint16_t cmd = 0;
            float param1 = NAN;
            float param2 = NAN;
            float param3 = NAN;
            switch (mission_item_impl.get_camera_action()) {
                case MissionItem::CameraAction::TAKE_PHOTO:
                    cmd = MAV_CMD_IMAGE_START_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    param2 = 0.0f; // no duration, take only one picture
                    param3 = 1.0f; // only take one picture
                    break;
                case MissionItem::CameraAction::START_PHOTO_INTERVAL:
                    cmd = MAV_CMD_IMAGE_START_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    param2 = mission_item_impl.get_camera_photo_interval_s();
                    param3 = 0.0f; // unlimited photos
                    break;
                case MissionItem::CameraAction::STOP_PHOTO_INTERVAL:
                    cmd = MAV_CMD_IMAGE_STOP_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    break;
                case MissionItem::CameraAction::START_VIDEO:
                    cmd = MAV_CMD_VIDEO_START_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    break;
                case MissionItem::CameraAction::STOP_VIDEO:
                    cmd = MAV_CMD_VIDEO_STOP_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    break;
                default:
                    LogErr() << "Error: camera action not supported";
                    break;
            }

            std::shared_ptr<mavlink_message_t> message_camera(new mavlink_message_t());
            mavlink_msg_mission_item_int_pack(_parent->get_own_system_id(),
                                              _parent->get_own_component_id(),
                                              message_camera.get(),
                                              _parent->get_target_system_id(),
                                              _parent->get_target_component_id(),
                                              _mavlink_mission_item_messages.size(),
                                              MAV_FRAME_MISSION,
                                              cmd,
                                              current,
                                              autocontinue,
                                              param1,
                                              param2,
                                              param3,
                                              NAN,
                                              0,
                                              0,
                                              NAN,
                                              MAV_MISSION_TYPE_MISSION);

            _mavlink_mission_item_to_mission_item_indices.insert(
                std::pair <int, int>
            {static_cast<int>(_mavlink_mission_item_messages.size()), item_i});
            _mavlink_mission_item_messages.push_back(message_camera);

            last_gimbal_yaw_deg = mission_item_impl.get_gimbal_yaw_deg();
            last_gimbal_pitch_deg = mission_item_impl.get_gimbal_pitch_deg();
        }

        ++item_i;
    }
}

void MissionImpl::start_mission_async(const Mission::result_callback_t &callback)
{
    if (_activity != Activity::NONE) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = PX4_CUSTOM_SUB_MODE_AUTO_MISSION;

    _activity = Activity::SEND_COMMAND;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&MissionImpl::receive_command_result, this,
                  std::placeholders::_1, callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);

    _result_callback = callback;
}

void MissionImpl::pause_mission_async(const Mission::result_callback_t &callback)
{
    if (_activity != Activity::NONE) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    _activity = Activity::SEND_COMMAND;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&MissionImpl::receive_command_result, this,
                  std::placeholders::_1, callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);

    _result_callback = callback;
}

void MissionImpl::set_current_mission_item_async(int current, Mission::result_callback_t &callback)
{
    if (_activity != Activity::NONE) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    int mavlink_index = -1;
    // We need to find the first mavlink item which maps to the current mission item.
    for (auto it = _mavlink_mission_item_to_mission_item_indices.begin();
         it != _mavlink_mission_item_to_mission_item_indices.end();
         ++it) {
        if (it->second == current) {
            mavlink_index = it->first;
            break;
        }
    }

    // If we coudln't find it, the requested item is out of range and probably an invalid argument.
    if (mavlink_index < 0) {
        report_mission_result(callback, Mission::Result::INVALID_ARGUMENT);
        return;
    }

    mavlink_message_t message;
    mavlink_msg_mission_set_current_pack(_parent->get_own_system_id(),
                                         _parent->get_own_component_id(),
                                         &message,
                                         _parent->get_target_system_id(),
                                         _parent->get_target_component_id(),
                                         mavlink_index);

    if (!_parent->send_message(message)) {
        report_mission_result(callback, Mission::Result::ERROR);
    }

    _activity = Activity::SET_CURRENT;
    _result_callback = callback;
}

void MissionImpl::send_mission_item(uint16_t seq)
{
    LogDebug() << "Send mission item " << int(seq);
    if (seq >= _mavlink_mission_item_messages.size()) {
        LogErr() << "Mission item requested out of bounds.";
        return;
    }

    _parent->send_message(*_mavlink_mission_item_messages.at(seq));
}

void MissionImpl::copy_mission_item_vector(const std::vector<std::shared_ptr<MissionItem>>
                                           &mission_items)
{
    _mission_items.clear();

    // Copy over the shared_ptr into our own vector.
    for (auto item : mission_items) {
        _mission_items.push_back(item);
    }
}

void MissionImpl::report_mission_result(const Mission::result_callback_t &callback,
                                        Mission::Result result)
{
    if (callback == nullptr) {
        LogWarn() << "Callback is not set";
        return;
    }

    callback(result);
}

void MissionImpl::report_progress()
{
    if (_progress_callback == nullptr) {
        return;
    }

    _progress_callback(current_mission_item(), total_mission_items());
}

void MissionImpl::receive_command_result(MavlinkCommands::Result result,
                                         const Mission::result_callback_t &callback)
{
    if (_activity == Activity::SEND_COMMAND) {
        _activity = Activity::NONE;
    }

    // We got a command back, so we can get rid of the timeout handler.
    _parent->unregister_timeout_handler(_timeout_cookie);

    if (result == MavlinkCommands::Result::SUCCESS) {
        report_mission_result(callback, Mission::Result::SUCCESS);
    } else {
        report_mission_result(callback, Mission::Result::ERROR);
    }
}

bool MissionImpl::is_mission_finished() const
{
    if (_last_current_mavlink_mission_item < 0) {
        return false;
    }

    if (_last_reached_mavlink_mission_item < 0) {
        return false;
    }

    if (_mavlink_mission_item_messages.size() == 0) {
        return false;
    }

    // It is not straightforward to look at "current" because it jumps to 0
    // once the last item has been done. Therefore we have to lo decide using
    // "reached" here.
    return (unsigned(_last_reached_mavlink_mission_item + 1)
            == _mavlink_mission_item_messages.size());
}

int MissionImpl::current_mission_item() const
{
    // If the mission is finished, let's return the total as the current
    // to signal this.
    if (is_mission_finished()) {
        return total_mission_items();
    }

    // We want to return the current mission item and not the underlying
    // mavlink mission item. Therefore we check the index map.
    auto entry = _mavlink_mission_item_to_mission_item_indices.find(
                     _last_current_mavlink_mission_item);

    if (entry != _mavlink_mission_item_to_mission_item_indices.end()) {
        return entry->second;

    } else {
        // Somehow we couldn't find it in the map
        return -1;
    }
}

int MissionImpl::total_mission_items() const
{
    return _mission_items.size();
}

void MissionImpl::subscribe_progress(Mission::progress_callback_t callback)
{
    _progress_callback = callback;
}

} // namespace dronecore
