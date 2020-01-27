#include "mission_impl.h"
#include "mission_item_impl.h"
#include "system.h"
#include "global_include.h"
#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <cmath>

namespace mavsdk {

using namespace std::placeholders; // for `_1`

MissionImpl::MissionImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionImpl::~MissionImpl()
{
    _parent->unregister_plugin(this);
}

void MissionImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_REQUEST,
        std::bind(&MissionImpl::process_mission_request, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_REQUEST_INT,
        std::bind(&MissionImpl::process_mission_request_int, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ACK, std::bind(&MissionImpl::process_mission_ack, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_CURRENT,
        std::bind(&MissionImpl::process_mission_current, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_REACHED,
        std::bind(&MissionImpl::process_mission_item_reached, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_COUNT,
        std::bind(&MissionImpl::process_mission_count, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_INT,
        std::bind(&MissionImpl::process_mission_item_int, this, _1),
        this);
}

void MissionImpl::enable() {}

void MissionImpl::disable() {}

void MissionImpl::deinit()
{
    _parent->unregister_timeout_handler(_timeout_cookie);
    _parent->unregister_all_mavlink_message_handlers(this);
}

void MissionImpl::process_mission_request(const mavlink_message_t& unused)
{
    // We only support int, so we nack this and thus tell the autopilot to use int.
    UNUSED(unused);

    mavlink_message_t message;
    mavlink_msg_mission_ack_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        MAV_MISSION_UNSUPPORTED,
        MAV_MISSION_TYPE_MISSION);

    _parent->send_message(message);

    // Reset the timeout because we're still communicating.
    _parent->refresh_timeout_handler(_timeout_cookie);
}

void MissionImpl::process_mission_request_int(const mavlink_message_t& message)
{
    mavlink_mission_request_int_t mission_request_int;
    mavlink_msg_mission_request_int_decode(&message, &mission_request_int);

    if (mission_request_int.target_system != _parent->get_own_system_id() &&
        mission_request_int.target_component != _parent->get_own_component_id()) {
        LogWarn() << "Ignore mission request int that is not for us";
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::SET_MISSION_COUNT) {
            _activity.state = Activity::State::SET_MISSION_ITEM;
        }

        if (_activity.state != Activity::State::SET_MISSION_ITEM) {
            if (_activity.state != Activity::State::ABORTED) {
                LogWarn() << "Ignoring mission request int, not active";
            }
            return;
        }
    }

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.retries = 0;
        _mission_data.last_mission_item_to_upload = mission_request_int.seq;
    }

    upload_mission_item();

    // Reset the timeout because we're still communicating.
    _parent->refresh_timeout_handler(_timeout_cookie);
}

void MissionImpl::process_mission_ack(const mavlink_message_t& message)
{
    // LogDebug() << "Received mission ack";

    mavlink_mission_ack_t mission_ack;
    mavlink_msg_mission_ack_decode(&message, &mission_ack);

    if (mission_ack.target_system != _parent->get_own_system_id() &&
        mission_ack.target_component != _parent->get_own_component_id()) {
        LogWarn() << "Ignore mission ack that is not for us";
        return;
    }

    Mission::result_callback_t temp_callback;
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        temp_callback = _mission_data.result_callback;
        // For the odd case where mission_ack is processed twice in quick succession
        // we need to make sure we only copy the callback once and only get nullptr
        // for the subsequent case. Otherwise we might report a result twice and
        // upset a caller.
        _mission_data.result_callback = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        // LogDebug() << "Received mission ack: activity state is: " << int(_activity.state);

        if (_activity.state == Activity::State::ABORTED) {
            // We ignore acks if we just cancelled.
            LogWarn() << "Ignoring mission ack because just cancelled";
            return;
        }

        if (_activity.state == Activity::State::NONE) {
            LogWarn() << "Mission ack ignored";
            return;
        }

        // We got some response, so it wasn't a timeout and we can remove it.
        _parent->unregister_timeout_handler(_timeout_cookie);

        if (mission_ack.type == MAV_MISSION_ACCEPTED) {
            report_mission_result(temp_callback, Mission::Result::SUCCESS);
            LogInfo() << "Mission accepted";

            reset_mission_progress();

        } else if (mission_ack.type == MAV_MISSION_NO_SPACE) {
            LogErr() << "Error: too many waypoints: " << int(mission_ack.type);
            report_mission_result(temp_callback, Mission::Result::TOO_MANY_MISSION_ITEMS);

        } else if (
            mission_ack.type == MAV_MISSION_ERROR &&
            _activity.state == Activity::State::SET_MISSION_COUNT) {
            LogErr() << "Error: presumably still busy after cancelling";
            report_mission_result(temp_callback, Mission::Result::BUSY);
        } else {
            LogErr() << "Error: unknown mission ack: " << int(mission_ack.type);
            report_mission_result(temp_callback, Mission::Result::ERROR);
        }

        // We need to stop after this, no matter what.
        _activity.state = Activity::State::NONE;
    }
}

void MissionImpl::reset_mission_progress()
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    _mission_data.last_current_mavlink_mission_item = -1;
    _mission_data.last_reached_mavlink_mission_item = -1;
    _mission_data.last_current_reported_mission_item = -1;
    _mission_data.last_total_reported_mission_item = -1;
}

void MissionImpl::process_mission_current(const mavlink_message_t& message)
{
    mavlink_mission_current_t mission_current;
    mavlink_msg_mission_current_decode(&message, &mission_current);

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.last_current_mavlink_mission_item = mission_current.seq;
    }

    report_progress();

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state != Activity::State::SET_CURRENT) {
            return;
        }
    }

    // We use these flags to make sure we only lock one mutex at a time,
    // and make sure the scope of the lock is obvious.
    bool set_current_successful = false;
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        if (_mission_data.last_current_mavlink_mission_item == mission_current.seq) {
            set_current_successful = true;
        }
    }
    if (set_current_successful) {
        report_mission_result(_mission_data.result_callback, Mission::Result::SUCCESS);
        _parent->unregister_timeout_handler(_timeout_cookie);

        {
            std::lock_guard<std::mutex> lock(_activity.mutex);
            _activity.state = Activity::State::NONE;
        }
    }
}

void MissionImpl::process_mission_item_reached(const mavlink_message_t& message)
{
    mavlink_mission_item_reached_t mission_item_reached;
    mavlink_msg_mission_item_reached_decode(&message, &mission_item_reached);

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.last_reached_mavlink_mission_item = mission_item_reached.seq;
    }

    report_progress();
}

void MissionImpl::process_mission_count(const mavlink_message_t& message)
{
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state != Activity::State::GET_MISSION_LIST) {
            return;
        }

        _activity.state = Activity::State::GET_MISSION_REQUEST;
    }

    mavlink_mission_count_t mission_count;
    mavlink_msg_mission_count_decode(&message, &mission_count);

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.num_mission_items_to_download = mission_count.count;
        _mission_data.next_mission_item_to_download = 0;
        _mission_data.retries = 0;
    }

    _parent->refresh_timeout_handler(_timeout_cookie);

    download_next_mission_item();
}

void MissionImpl::process_mission_item_int(const mavlink_message_t& message)
{
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state != Activity::State::GET_MISSION_REQUEST) {
            return;
        }
    }

    auto mission_item_int_ptr = std::make_shared<mavlink_mission_item_int_t>();
    mavlink_msg_mission_item_int_decode(&message, mission_item_int_ptr.get());

    // LogDebug() << "Received mission item int: " << int(mission_item_int_ptr->seq);

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        if (mission_item_int_ptr->seq == _mission_data.next_mission_item_to_download) {
            LogDebug() << "Received mission item " << _mission_data.next_mission_item_to_download;

            _mission_data.mavlink_mission_items_downloaded.push_back(mission_item_int_ptr);
            _mission_data.retries = 0;

            if (_mission_data.next_mission_item_to_download + 1 ==
                _mission_data.num_mission_items_to_download) {
                // Wrap things up if we're finished.
                _parent->unregister_timeout_handler(_timeout_cookie);

                mavlink_message_t ack_message;
                mavlink_msg_mission_ack_pack(
                    _parent->get_own_system_id(),
                    _parent->get_own_component_id(),
                    &ack_message,
                    _parent->get_system_id(),
                    _parent->get_autopilot_id(),
                    MAV_MISSION_ACCEPTED,
                    MAV_MISSION_TYPE_MISSION);

                _parent->send_message(ack_message);

                assemble_mission_items();

            } else {
                // Otherwise keep going.
                ++_mission_data.next_mission_item_to_download;
                _parent->refresh_timeout_handler(_timeout_cookie);
                download_next_mission_item();
            }

        } else {
            LogDebug() << "Received mission item " << int(mission_item_int_ptr->seq)
                       << " instead of " << _mission_data.next_mission_item_to_download
                       << " (ignored)";

            // Refresh because we at least still seem to be active.
            _parent->refresh_timeout_handler(_timeout_cookie);

            // And request it again in case our request got lost.
            download_next_mission_item();
            return;
        }
    }
}

void MissionImpl::upload_mission_async(
    const std::vector<std::shared_ptr<MissionItem>>& mission_items,
    const Mission::result_callback_t& callback)
{
    bool should_report_mission_result = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::ABORTED) {
            _activity.state = Activity::State::NONE;
        }

        if (_activity.state != Activity::State::NONE) {
            should_report_mission_result = true;
        }
    }

    if (should_report_mission_result) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    if (!_parent->does_support_mission_int()) {
        LogWarn() << "Mission int messages not supported";
        report_mission_result(callback, Mission::Result::ERROR);
        return;
    }

    copy_mission_item_vector(mission_items);

    assemble_mavlink_messages();

    _parent->register_timeout_handler(
        std::bind(&MissionImpl::process_timeout, this), RETRY_TIMEOUT_S, &_timeout_cookie);

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        _activity.state = Activity::State::SET_MISSION_COUNT;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.result_callback = callback;
        _mission_data.retries = 0;
    }

    send_count();
}

void MissionImpl::send_count()
{
    mavlink_message_t message;
    mavlink_msg_mission_count_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        _mission_data.mavlink_mission_item_messages.size(),
        MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        report_mission_result(_mission_data.result_callback, Mission::Result::ERROR);
        return;
    }
}

void MissionImpl::upload_mission_cancel()
{
    _parent->unregister_timeout_handler(_timeout_cookie);

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state != Activity::State::SET_MISSION_COUNT &&
            _activity.state != Activity::State::SET_MISSION_ITEM) {
            LogWarn() << "No mission upload in progress";
            return;
        }

        _activity.state = Activity::State::ABORTED;
    }

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);

        report_mission_result(_mission_data.result_callback, Mission::Result::CANCELLED);

        mavlink_message_t message;
        mavlink_msg_mission_ack_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &message,
            _parent->get_system_id(),
            _parent->get_autopilot_id(),
            MAV_MISSION_DENIED,
            MAV_MISSION_TYPE_MISSION);
        _parent->send_message(message);

        _mission_data.mavlink_mission_items_downloaded.clear();
        _mission_data.retries = 0;
        _mission_data.result_callback = nullptr;
    }
}

void MissionImpl::download_mission_async(
    const Mission::mission_items_and_result_callback_t& callback)
{
    bool should_report_mission_items_and_result = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::ABORTED) {
            _activity.state = Activity::State::NONE;
        }

        if (_activity.state != Activity::State::NONE) {
            should_report_mission_items_and_result = true;
        }
    }

    if (should_report_mission_items_and_result) {
        report_mission_items_and_result(callback, Mission::Result::BUSY);
        return;
    }

    _parent->register_timeout_handler(
        std::bind(&MissionImpl::process_timeout, this), RETRY_TIMEOUT_S, &_timeout_cookie);

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        _activity.state = Activity::State::GET_MISSION_LIST;
    }

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        // Clear our internal cache and re-populate it.
        _mission_data.mavlink_mission_items_downloaded.clear();
        _mission_data.retries = 0;
        _mission_data.mission_items_and_result_callback = callback;
    }

    request_list();
}

void MissionImpl::request_list()
{
    mavlink_message_t message;
    mavlink_msg_mission_request_list_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        report_mission_items_and_result(
            _mission_data.mission_items_and_result_callback, Mission::Result::CANCELLED);
        return;
    }
}

void MissionImpl::download_mission_cancel()
{
    _parent->unregister_timeout_handler(_timeout_cookie);

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state != Activity::State::GET_MISSION_LIST &&
            _activity.state != Activity::State::GET_MISSION_REQUEST) {
            LogWarn() << "No mission download in progress";
            return;
        }

        _activity.state = Activity::State::ABORTED;
    }

    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);

        report_mission_items_and_result(
            _mission_data.mission_items_and_result_callback, Mission::Result::CANCELLED);

        mavlink_message_t message;
        mavlink_msg_mission_ack_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &message,
            _parent->get_system_id(),
            _parent->get_autopilot_id(),
            MAV_MISSION_DENIED,
            MAV_MISSION_TYPE_MISSION);
        _parent->send_message(message);

        _mission_data.mavlink_mission_items_downloaded.clear();
        _mission_data.retries = 0;
        _mission_data.mission_items_and_result_callback = nullptr;
    }
}

void MissionImpl::set_return_to_launch_after_mission(bool enable_rtl)
{
    _enable_return_to_launch_after_mission = enable_rtl;
}

bool MissionImpl::get_return_to_launch_after_mission()
{
    return _enable_return_to_launch_after_mission;
}

void MissionImpl::assemble_mavlink_messages()
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    _mission_data.mavlink_mission_item_messages.clear();

    bool last_position_valid = false; // This flag is to protect us from using an invalid x/y.
    MAV_FRAME last_frame;
    int32_t last_x;
    int32_t last_y;
    float last_z;

    unsigned item_i = 0;

    for (auto item : _mission_data.mission_items) {
        MissionItemImpl& mission_item_impl = (*(item)->_impl);

        if (mission_item_impl.is_position_finite()) {
            // Current is the 0th waypoint
            uint8_t current = ((_mission_data.mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            auto message = std::make_shared<mavlink_message_t>();
            mavlink_msg_mission_item_int_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                message.get(),
                _parent->get_system_id(),
                _parent->get_autopilot_id(),
                _mission_data.mavlink_mission_item_messages.size(),
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

            last_position_valid = true; // because we checked is_position_finite
            last_x = mission_item_impl.get_mavlink_x();
            last_y = mission_item_impl.get_mavlink_y();
            last_z = mission_item_impl.get_mavlink_z();
            last_frame = mission_item_impl.get_mavlink_frame();

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(std::pair<int, int>{
                static_cast<int>(_mission_data.mavlink_mission_item_messages.size()), item_i});
            _mission_data.mavlink_mission_item_messages.push_back(message);
        }

        if (std::isfinite(mission_item_impl.get_speed_m_s())) {
            // The speed has changed, we need to add a speed command.

            // Current is the 0th waypoint
            uint8_t current = ((_mission_data.mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            auto message_speed = std::make_shared<mavlink_message_t>();
            mavlink_msg_mission_item_int_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                message_speed.get(),
                _parent->get_system_id(),
                _parent->get_autopilot_id(),
                _mission_data.mavlink_mission_item_messages.size(),
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

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(std::pair<int, int>{
                static_cast<int>(_mission_data.mavlink_mission_item_messages.size()), item_i});
            _mission_data.mavlink_mission_item_messages.push_back(message_speed);
        }

        if (std::isfinite(mission_item_impl.get_gimbal_yaw_deg()) ||
            std::isfinite(mission_item_impl.get_gimbal_pitch_deg())) {
            if (_enable_absolute_gimbal_yaw_angle) {
                // We need to configure the gimbal to use an absolute angle.

                // Current is the 0th waypoint
                uint8_t current =
                    ((_mission_data.mavlink_mission_item_messages.size() == 0) ? 1 : 0);

                uint8_t autocontinue = 1;

                auto message_gimbal_configure = std::make_shared<mavlink_message_t>();
                mavlink_msg_mission_item_int_pack(
                    _parent->get_own_system_id(),
                    _parent->get_own_component_id(),
                    message_gimbal_configure.get(),
                    _parent->get_system_id(),
                    _parent->get_autopilot_id(),
                    _mission_data.mavlink_mission_item_messages.size(),
                    MAV_FRAME_MISSION,
                    MAV_CMD_DO_MOUNT_CONFIGURE,
                    current,
                    autocontinue,
                    MAV_MOUNT_MODE_MAVLINK_TARGETING,
                    0.0f, // stabilize roll
                    0.0f, // stabilize pitch
                    1.0f, // stabilize yaw, FIXME: for now we use this for an absolute yaw angle,
                          // because it works.
                    0,
                    0,
                    2.0f, // eventually this is the correct flag to set absolute yaw angle.
                    MAV_MISSION_TYPE_MISSION);

                _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                    std::pair<int, int>{
                        static_cast<int>(_mission_data.mavlink_mission_item_messages.size()),
                        item_i});
                _mission_data.mavlink_mission_item_messages.push_back(message_gimbal_configure);
            }

            // The gimbal has changed, we need to add a gimbal command.

            // Current is the 0th waypoint
            uint8_t current = ((_mission_data.mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            auto message_gimbal = std::make_shared<mavlink_message_t>();
            mavlink_msg_mission_item_int_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                message_gimbal.get(),
                _parent->get_system_id(),
                _parent->get_autopilot_id(),
                _mission_data.mavlink_mission_item_messages.size(),
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

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(std::pair<int, int>{
                static_cast<int>(_mission_data.mavlink_mission_item_messages.size()), item_i});
            _mission_data.mavlink_mission_item_messages.push_back(message_gimbal);
        }

        // FIXME: It is a bit of a hack to set a LOITER_TIME waypoint to add a delay.
        //        A better solution would be to properly use NAV_DELAY instead. This
        //        would not require us to keep the last lat/lon.
        // A loiter time of NAN is ignored but also a loiter time of 0 doesn't
        // make any sense and should be discarded.
        if (std::isfinite(mission_item_impl.get_loiter_time_s()) &&
            mission_item_impl.get_loiter_time_s() > 0.0f) {
            if (!last_position_valid) {
                // In the case where we get a delay without a previous position, we will have to
                // ignore it.
                LogErr() << "Can't set camera action delay without previous position set.";

            } else {
                // Current is the 0th waypoint
                uint8_t current =
                    ((_mission_data.mavlink_mission_item_messages.size() == 0) ? 1 : 0);

                uint8_t autocontinue = 1;

                std::shared_ptr<mavlink_message_t> message_delay(new mavlink_message_t());
                mavlink_msg_mission_item_int_pack(
                    _parent->get_own_system_id(),
                    _parent->get_own_component_id(),
                    message_delay.get(),
                    _parent->get_system_id(),
                    _parent->get_autopilot_id(),
                    _mission_data.mavlink_mission_item_messages.size(),
                    last_frame,
                    MAV_CMD_NAV_LOITER_TIME,
                    current,
                    autocontinue,
                    mission_item_impl.get_loiter_time_s(), // loiter time in seconds
                    NAN, // empty
                    0.0f, // radius around waypoint in meters ?
                    NAN, // don't change yaw
                    last_x,
                    last_y,
                    last_z,
                    MAV_MISSION_TYPE_MISSION);

                _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                    std::pair<int, int>{
                        static_cast<int>(_mission_data.mavlink_mission_item_messages.size()),
                        item_i});
                _mission_data.mavlink_mission_item_messages.push_back(message_delay);
            }

            if (mission_item_impl.get_fly_through()) {
                LogWarn() << "Conflicting options set: fly_through=true and loiter_time>0.";
            }
        }

        if (mission_item_impl.get_camera_action() != MissionItem::CameraAction::NONE) {
            // There is a camera action that we need to send.

            // Current is the 0th waypoint
            uint8_t current = ((_mission_data.mavlink_mission_item_messages.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            uint16_t command = 0;
            float param1 = NAN;
            float param2 = NAN;
            float param3 = NAN;
            switch (mission_item_impl.get_camera_action()) {
                case MissionItem::CameraAction::TAKE_PHOTO:
                    command = MAV_CMD_IMAGE_START_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    param2 = 0.0f; // no duration, take only one picture
                    param3 = 1.0f; // only take one picture
                    break;
                case MissionItem::CameraAction::START_PHOTO_INTERVAL:
                    command = MAV_CMD_IMAGE_START_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    param2 = mission_item_impl.get_camera_photo_interval_s();
                    param3 = 0.0f; // unlimited photos
                    break;
                case MissionItem::CameraAction::STOP_PHOTO_INTERVAL:
                    command = MAV_CMD_IMAGE_STOP_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    break;
                case MissionItem::CameraAction::START_VIDEO:
                    command = MAV_CMD_VIDEO_START_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    break;
                case MissionItem::CameraAction::STOP_VIDEO:
                    command = MAV_CMD_VIDEO_STOP_CAPTURE;
                    param1 = 0.0f; // all camera IDs
                    break;
                default:
                    LogErr() << "Error: camera action not supported";
                    break;
            }

            auto message_camera = std::make_shared<mavlink_message_t>();
            mavlink_msg_mission_item_int_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                message_camera.get(),
                _parent->get_system_id(),
                _parent->get_autopilot_id(),
                _mission_data.mavlink_mission_item_messages.size(),
                MAV_FRAME_MISSION,
                command,
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

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(std::pair<int, int>{
                static_cast<int>(_mission_data.mavlink_mission_item_messages.size()), item_i});
            _mission_data.mavlink_mission_item_messages.push_back(message_camera);
        }

        ++item_i;
    }

    // We need to decrement the item_i again because it was increased in the loop above
    // but the RTL item below still belongs to the last mission item.
    --item_i;

    if (_enable_return_to_launch_after_mission) {
        std::shared_ptr<mavlink_message_t> message_rtl(new mavlink_message_t());
        mavlink_msg_mission_item_int_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            message_rtl.get(),
            _parent->get_system_id(),
            _parent->get_autopilot_id(),
            _mission_data.mavlink_mission_item_messages.size(),
            MAV_FRAME_MISSION,
            MAV_CMD_NAV_RETURN_TO_LAUNCH,
            0, // current
            1, // autocontinue
            NAN, // loiter time in seconds
            NAN, // empty
            NAN, // radius around waypoint in meters ?
            NAN, // loiter at center of waypoint
            0,
            0,
            0,
            MAV_MISSION_TYPE_MISSION);

        _mission_data.mavlink_mission_item_to_mission_item_indices.insert(std::pair<int, int>{
            static_cast<int>(_mission_data.mavlink_mission_item_messages.size()), item_i});
        _mission_data.mavlink_mission_item_messages.push_back(message_rtl);
    }
}

void MissionImpl::assemble_mission_items()
{
    Mission::Result result = Mission::Result::SUCCESS;
    Mission::mission_items_and_result_callback_t callback;
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.mission_items.clear();
        _mission_data.mavlink_mission_item_to_mission_item_indices.clear();
        _enable_return_to_launch_after_mission = false;

        auto new_mission_item = std::make_shared<MissionItem>();
        bool have_set_position = false;

        if (_mission_data.mavlink_mission_items_downloaded.size() > 0) {
            // The first mission item needs to be a waypoint with position.
            if (_mission_data.mavlink_mission_items_downloaded.at(0)->command !=
                MAV_CMD_NAV_WAYPOINT) {
                LogErr() << "First mission item is not a waypoint";
                result = Mission::Result::UNSUPPORTED;
                return;
            }
        }

        if (_mission_data.mavlink_mission_items_downloaded.size() == 0) {
            LogErr() << "No downloaded mission items";
            result = Mission::Result::NO_MISSION_AVAILABLE;
            return;
        }

        int mavlink_item_i = 0;

        for (auto& it : _mission_data.mavlink_mission_items_downloaded) {
            LogDebug() << "Assembling Message: " << int(it->seq);

            if (it->command == MAV_CMD_NAV_WAYPOINT) {
                if (it->frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
                    LogErr() << "Waypoint frame not supported unsupported";
                    result = Mission::Result::UNSUPPORTED;
                    break;
                }

                if (have_set_position) {
                    // When a new position comes in, create next mission item.
                    _mission_data.mission_items.push_back(new_mission_item);
                    new_mission_item = std::make_shared<MissionItem>();
                    have_set_position = false;
                }

                new_mission_item->set_position(double(it->x) * 1e-7, double(it->y) * 1e-7);
                new_mission_item->set_relative_altitude(it->z);

                new_mission_item->set_fly_through(!(it->param1 > 0));

                have_set_position = true;

            } else if (it->command == MAV_CMD_DO_MOUNT_CONTROL) {
                if (int(it->z) != MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                    LogErr() << "Gimbal mount control mode unsupported";
                    result = Mission::Result::UNSUPPORTED;
                    break;
                }

                new_mission_item->set_gimbal_pitch_and_yaw(it->param1, it->param3);

            } else if (it->command == MAV_CMD_DO_MOUNT_CONFIGURE) {
                if (int(it->param1) != MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                    LogErr() << "Gimbal mount configure mode unsupported";
                    result = Mission::Result::UNSUPPORTED;
                    break;
                }

                // FIXME: ultimately param4 doesn't count anymore and
                //        param7 holds the truth.
                if (int(it->param4) == 1 || int(it->z) == 2) {
                    _enable_absolute_gimbal_yaw_angle = true;
                } else {
                    _enable_absolute_gimbal_yaw_angle = false;
                }

            } else if (it->command == MAV_CMD_IMAGE_START_CAPTURE) {
                if (it->param2 > 0 && int(it->param3) == 0) {
                    new_mission_item->set_camera_action(
                        MissionItem::CameraAction::START_PHOTO_INTERVAL);
                    new_mission_item->set_camera_photo_interval(double(it->param2));
                } else if (int(it->param2) == 0 && int(it->param3) == 1) {
                    new_mission_item->set_camera_action(MissionItem::CameraAction::TAKE_PHOTO);
                } else {
                    LogErr() << "Mission item START_CAPTURE params unsupported.";
                    result = Mission::Result::UNSUPPORTED;
                    break;
                }

            } else if (it->command == MAV_CMD_IMAGE_STOP_CAPTURE) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_PHOTO_INTERVAL);

            } else if (it->command == MAV_CMD_VIDEO_START_CAPTURE) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::START_VIDEO);

            } else if (it->command == MAV_CMD_VIDEO_STOP_CAPTURE) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_VIDEO);

            } else if (it->command == MAV_CMD_DO_CHANGE_SPEED) {
                if (int(it->param1) == 1 && it->param3 < 0 && int(it->param4) == 0) {
                    new_mission_item->set_speed(it->param2);
                } else {
                    LogErr() << "Mission item DO_CHANGE_SPEED params unsupported";
                    result = Mission::Result::UNSUPPORTED;
                }

            } else if (it->command == MAV_CMD_NAV_LOITER_TIME) {
                new_mission_item->set_loiter_time(it->param1);

            } else if (it->command == MAV_CMD_NAV_RETURN_TO_LAUNCH) {
                _enable_return_to_launch_after_mission = true;

            } else {
                LogErr() << "UNSUPPORTED mission item command (" << it->command << ")";
                result = Mission::Result::UNSUPPORTED;
                break;
            }

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(std::pair<int, int>{
                mavlink_item_i, static_cast<int>(_mission_data.mission_items.size())});

            ++mavlink_item_i;
        }

        // Don't forget to add last mission item.
        _mission_data.mission_items.push_back(new_mission_item);

        // Copy the callback out of the locked scope.
        callback = _mission_data.mission_items_and_result_callback;
    }

    report_mission_items_and_result(callback, result);

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        _activity.state = Activity::State::NONE;
    }
}

void MissionImpl::download_next_mission_item()
{
    mavlink_message_t message;
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        mavlink_msg_mission_request_int_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &message,
            _parent->get_system_id(),
            _parent->get_autopilot_id(),
            _mission_data.next_mission_item_to_download,
            MAV_MISSION_TYPE_MISSION);

        LogDebug() << "Requested mission item " << _mission_data.next_mission_item_to_download;
    }

    _parent->send_message(message);
}

void MissionImpl::start_mission_async(const Mission::result_callback_t& callback)
{
    bool should_report_mission_result = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::ABORTED) {
            _activity.state = Activity::State::NONE;
        }

        if (_activity.state != Activity::State::NONE) {
            should_report_mission_result = true;
        } else {
            _activity.state = Activity::State::SEND_COMMAND;
        }
    }

    if (should_report_mission_result) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::MISSION,
        std::bind(&MissionImpl::receive_command_result, this, std::placeholders::_1, callback));
}

void MissionImpl::pause_mission_async(const Mission::result_callback_t& callback)
{
    bool should_report_mission_result = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::ABORTED) {
            _activity.state = Activity::State::NONE;
        }

        if (_activity.state != Activity::State::NONE) {
            should_report_mission_result = true;
        } else {
            _activity.state = Activity::State::SEND_COMMAND;
        }
    }

    if (should_report_mission_result) {
        LogErr() << "We're busy because activity is: " << int(_activity.state);
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::HOLD,
        std::bind(&MissionImpl::receive_command_result, this, std::placeholders::_1, callback));
}

void MissionImpl::clear_mission_async(const Mission::result_callback_t& callback)
{
    bool should_report_mission_result = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::ABORTED) {
            _activity.state = Activity::State::NONE;
        }

        if (_activity.state != Activity::State::NONE) {
            should_report_mission_result = true;
        }
    }

    if (should_report_mission_result) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    _parent->register_timeout_handler(
        std::bind(&MissionImpl::process_timeout, this), RETRY_TIMEOUT_S, &_timeout_cookie);

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        _activity.state = Activity::State::MISSION_CLEAR;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.result_callback = callback;
        _mission_data.retries = 0;
    }

    clear_mission();
}

void MissionImpl::clear_mission()
{
    mavlink_message_t message;
    mavlink_msg_mission_clear_all_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        MAV_MISSION_TYPE_MISSION);

    if (!_parent->send_message(message)) {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        report_mission_result(_mission_data.result_callback, Mission::Result::ERROR);
        return;
    }
}

void MissionImpl::set_current_mission_item_async(int current, Mission::result_callback_t& callback)
{
    bool should_report_mission_result = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::ABORTED) {
            _activity.state = Activity::State::NONE;
        }

        if (_activity.state != Activity::State::NONE) {
            should_report_mission_result = true;
        }
    }

    if (should_report_mission_result) {
        report_mission_result(callback, Mission::Result::BUSY);
        return;
    }

    int mavlink_index = -1;
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        // We need to find the first mavlink item which maps to the current mission item.
        for (auto it = _mission_data.mavlink_mission_item_to_mission_item_indices.begin();
             it != _mission_data.mavlink_mission_item_to_mission_item_indices.end();
             ++it) {
            if (it->second == current) {
                mavlink_index = it->first;
                break;
            }
        }
    }

    // If we coudln't find it, the requested item is out of range and probably an invalid argument.
    if (mavlink_index < 0) {
        report_mission_result(callback, Mission::Result::INVALID_ARGUMENT);
        return;
    }

    mavlink_message_t message;
    mavlink_msg_mission_set_current_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        mavlink_index);

    if (!_parent->send_message(message)) {
        report_mission_result(callback, Mission::Result::ERROR);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        _activity.state = Activity::State::SET_CURRENT;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _mission_data.result_callback = callback;
    }
}

void MissionImpl::upload_mission_item()
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    LogDebug() << "Send mission item " << _mission_data.last_mission_item_to_upload;
    if (_mission_data.last_mission_item_to_upload >=
        int(_mission_data.mavlink_mission_item_messages.size())) {
        LogErr() << "Mission item requested out of bounds.";
        return;
    }

    _parent->send_message(
        *_mission_data.mavlink_mission_item_messages.at(_mission_data.last_mission_item_to_upload));
}

void MissionImpl::copy_mission_item_vector(
    const std::vector<std::shared_ptr<MissionItem>>& mission_items)
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    _mission_data.mission_items.clear();

    // Copy over the shared_ptr into our own vector.
    for (auto item : mission_items) {
        _mission_data.mission_items.push_back(item);
    }
}

void MissionImpl::report_mission_result(
    const Mission::result_callback_t& callback, Mission::Result result)
{
    const auto temp_callback = callback;

    if (temp_callback == nullptr) {
        LogWarn() << "Callback is not set";
        return;
    }

    _parent->call_user_callback([temp_callback, result]() { temp_callback(result); });
}

void MissionImpl::report_mission_items_and_result(
    const Mission::mission_items_and_result_callback_t& callback, Mission::Result result)
{
    const auto temp_callback = callback;
    if (temp_callback == nullptr) {
        LogWarn() << "Callback is not set";
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    if (result != Mission::Result::SUCCESS) {
        // Don't return garbage, better clear it.
        _mission_data.mission_items.clear();
    }
    _parent->call_user_callback([temp_callback, result, this]() {
        // This one is tricky because we keep the lock of the mission data during the callback.
        temp_callback(result, _mission_data.mission_items);
    });
}

void MissionImpl::report_progress()
{
    const auto temp_callback = _mission_data.progress_callback;
    if (temp_callback == nullptr) {
        return;
    }

    int current = current_mission_item();
    int total = total_mission_items();

    bool should_report = false;
    {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        if (_mission_data.last_current_reported_mission_item != current) {
            _mission_data.last_current_reported_mission_item = current;
            should_report = true;
        }
        if (_mission_data.last_total_reported_mission_item != total) {
            _mission_data.last_total_reported_mission_item = total;
            should_report = true;
        }
    }

    if (should_report) {
        std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
        _parent->call_user_callback([temp_callback, current, total]() {
            LogDebug() << "current: " << current << ", total: " << total;
            temp_callback(current, total);
        });
    }
}

void MissionImpl::receive_command_result(
    MAVLinkCommands::Result result, const Mission::result_callback_t callback)
{
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);
        if (_activity.state == Activity::State::SEND_COMMAND) {
            _activity.state = Activity::State::NONE;
        }
    }

    // We got a command back, so we can get rid of the timeout handler.
    _parent->unregister_timeout_handler(_timeout_cookie);

    if (result == MAVLinkCommands::Result::SUCCESS) {
        report_mission_result(callback, Mission::Result::SUCCESS);
    } else {
        report_mission_result(callback, Mission::Result::ERROR);
    }
}

bool MissionImpl::is_mission_finished() const
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);

    if (_mission_data.last_current_mavlink_mission_item < 0) {
        return false;
    }

    if (_mission_data.last_reached_mavlink_mission_item < 0) {
        return false;
    }

    if (_mission_data.mavlink_mission_item_messages.size() == 0) {
        return false;
    }

    // It is not straightforward to look at "current" because it jumps to 0
    // once the last item has been done. Therefore we have to lo decide using
    // "reached" here.
    // It seems that we never receive a reached when RTL is initiated after
    // a mission, and we need to account for that.
    const unsigned rtl_correction = _enable_return_to_launch_after_mission ? 2 : 1;

    return (
        unsigned(_mission_data.last_reached_mavlink_mission_item + rtl_correction) ==
        _mission_data.mavlink_mission_item_messages.size());
}

int MissionImpl::current_mission_item() const
{
    // If the mission is finished, let's return the total as the current
    // to signal this.
    if (is_mission_finished()) {
        return total_mission_items();
    }

    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);

    // We want to return the current mission item and not the underlying
    // mavlink mission item. Therefore we check the index map.
    auto entry = _mission_data.mavlink_mission_item_to_mission_item_indices.find(
        _mission_data.last_current_mavlink_mission_item);

    if (entry != _mission_data.mavlink_mission_item_to_mission_item_indices.end()) {
        return entry->second;

    } else {
        // Somehow we couldn't find it in the map
        return -1;
    }
}

int MissionImpl::total_mission_items() const
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    return static_cast<int>(_mission_data.mission_items.size());
}

void MissionImpl::subscribe_progress(Mission::progress_callback_t callback)
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    _mission_data.progress_callback = callback;
}

void MissionImpl::process_timeout()
{
    bool should_retry = false;
    {
        std::lock_guard<std::mutex> lock(_activity.mutex);

        switch (_activity.state) {
            case Activity::State::NONE:
                // FALLTHROUGH
            case Activity::State::ABORTED:
                // FALLTHROUGH
            case Activity::State::SEND_COMMAND:
                // FALLTHROUGH
            case Activity::State::SET_CURRENT:
                break;

            case Activity::State::SET_MISSION_COUNT:
                // FALLTHROUGH
            case Activity::State::SET_MISSION_ITEM:
                // FALLTHROUGH
            case Activity::State::GET_MISSION_LIST:
                // FALLTHROUGH
            case Activity::State::GET_MISSION_REQUEST:
                // FALLTHROUGH
            case Activity::State::MISSION_CLEAR:
                should_retry = true;
                break;
            default:
                LogWarn() << "unknown mission timeout";
                break;
        }
    }

    if (should_retry) {
        _mission_data.mutex.lock();
        if (_mission_data.retries++ > MAX_RETRIES) {
            _mission_data.retries = 0;

            std::lock_guard<std::mutex> lock(_activity.mutex);
            LogWarn() << "Mission handling timed out.";
            if (_activity.state == Activity::State::GET_MISSION_LIST ||
                _activity.state == Activity::State::GET_MISSION_REQUEST) {
                LogWarn() << "Downloading mission timed out...";
                report_mission_items_and_result(
                    _mission_data.mission_items_and_result_callback, Mission::Result::TIMEOUT);
            } else if (
                _activity.state == Activity::State::SET_MISSION_COUNT ||
                _activity.state == Activity::State::SET_MISSION_ITEM) {
                LogWarn() << "Uploading mission timed out...";
                report_mission_result(_mission_data.result_callback, Mission::Result::TIMEOUT);
            } else if (_activity.state == Activity::State::MISSION_CLEAR) {
                LogWarn() << "Clearing mission timed out...";
                report_mission_result(_mission_data.result_callback, Mission::Result::TIMEOUT);
            }

            _activity.state = Activity::State::NONE;
            _mission_data.mutex.unlock();

        } else {
            _mission_data.mutex.unlock();

            _parent->register_timeout_handler(
                std::bind(&MissionImpl::process_timeout, this), RETRY_TIMEOUT_S, &_timeout_cookie);

            {
                std::lock_guard<std::mutex> lock(_activity.mutex);
                if (_activity.state == Activity::State::GET_MISSION_LIST) {
                    LogWarn() << "Retrying requesting mission list...";
                    request_list();
                } else if (_activity.state == Activity::State::GET_MISSION_REQUEST) {
                    LogWarn() << "Retrying requesting mission item...";
                    download_next_mission_item();
                } else if (_activity.state == Activity::State::SET_MISSION_COUNT) {
                    LogWarn() << "Retrying send mission count...";
                    send_count();
                } else if (_activity.state == Activity::State::SET_MISSION_ITEM) {
                    LogWarn() << "Retrying send mission count...";
                    upload_mission_item();
                } else if (_activity.state == Activity::State::MISSION_CLEAR) {
                    LogWarn() << "Retrying to clear mission...";
                    clear_mission();
                }
            }
        }
    }
}

Mission::Result MissionImpl::import_qgroundcontrol_mission(
    Mission::mission_items_t& mission_items, const std::string& qgc_plan_file)
{
    std::ifstream file(qgc_plan_file);
    if (!file) {
        return Mission::Result::FAILED_TO_OPEN_QGC_PLAN;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    const auto raw_json = ss.str();

    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    JSONCPP_STRING err;
    const bool ok =
        reader->parse(raw_json.c_str(), raw_json.c_str() + raw_json.length(), &root, &err);
    if (!ok) {
        LogErr() << "Parse error: " << err;
        return Mission::Result::FAILED_TO_PARSE_QGC_PLAN;
    }

    mission_items.clear();

    return import_mission_items(mission_items, root);
}

// Build a mission item out of command, params and add them to the mission vector.
Mission::Result MissionImpl::build_mission_items(
    MAV_CMD command,
    std::vector<double> params,
    std::shared_ptr<MissionItem>& new_mission_item,
    Mission::mission_items_t& all_mission_items)
{
    Mission::Result result = Mission::Result::SUCCESS;

    // Choosen "Do-While(0)" loop for the convenience of using `break` statement.
    do {
        if (command == MAV_CMD_NAV_WAYPOINT || command == MAV_CMD_NAV_TAKEOFF ||
            command == MAV_CMD_NAV_LAND) {
            if (new_mission_item->has_position_set()) {
                if (command == MAV_CMD_NAV_TAKEOFF) {
                    LogWarn() << "Converted takeoff mission item to normal waypoint";
                } else if (command == MAV_CMD_NAV_LAND) {
                    LogWarn() << "Converted land mission item to normal waypoint";
                }
                all_mission_items.push_back(new_mission_item);
                new_mission_item = std::make_shared<MissionItem>();
            }

            if (command == MAV_CMD_NAV_WAYPOINT) {
                auto is_fly_through = !(int(params[0]) > 0);
                new_mission_item->set_fly_through(is_fly_through);
            }
            auto lat = params[4], lon = params[5];
            new_mission_item->set_position(lat, lon);

            auto rel_alt = float(params[6]);
            new_mission_item->set_relative_altitude(rel_alt);

        } else if (command == MAV_CMD_DO_MOUNT_CONTROL) {
            auto pitch = float(params[0]), yaw = float(params[2]);
            new_mission_item->set_gimbal_pitch_and_yaw(pitch, yaw);

        } else if (command == MAV_CMD_NAV_LOITER_TIME) {
            auto loiter_time_s = float(params[0]);
            new_mission_item->set_loiter_time(loiter_time_s);

        } else if (command == MAV_CMD_IMAGE_START_CAPTURE) {
            auto photo_interval = int(params[1]), photo_count = int(params[2]);

            if (photo_interval > 0 && photo_count == 0) {
                new_mission_item->set_camera_action(
                    MissionItem::CameraAction::START_PHOTO_INTERVAL);
                new_mission_item->set_camera_photo_interval(photo_interval);
            } else if (photo_interval == 0 && photo_count == 1) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::TAKE_PHOTO);
            } else {
                LogErr() << "Mission item START_CAPTURE params unsupported.";
                result = Mission::Result::UNSUPPORTED;
                break;
            }

        } else if (command == MAV_CMD_IMAGE_STOP_CAPTURE) {
            new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_PHOTO_INTERVAL);

        } else if (command == MAV_CMD_VIDEO_START_CAPTURE) {
            new_mission_item->set_camera_action(MissionItem::CameraAction::START_VIDEO);

        } else if (command == MAV_CMD_VIDEO_STOP_CAPTURE) {
            new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_VIDEO);

        } else if (command == MAV_CMD_DO_CHANGE_SPEED) {
            enum { AirSpeed = 0, GroundSpeed = 1 };
            auto speed_type = int(params[0]);
            auto speed_m_s = float(params[1]);
            auto throttle = params[2];
            auto is_absolute = (params[3] == 0);

            if (speed_type == int(GroundSpeed) && throttle < 0 && is_absolute) {
                new_mission_item->set_speed(speed_m_s);
            } else {
                LogErr() << command << "Mission item DO_CHANGE_SPEED params unsupported";
                result = Mission::Result::UNSUPPORTED;
                break;
            }
        } else {
            LogWarn() << "UNSUPPORTED mission item command (" << command << ")";
        }
    } while (false); // Executed once per method invokation.

    return result;
}

Mission::Result MissionImpl::import_mission_items(
    Mission::mission_items_t& all_mission_items, const Json::Value& qgc_plan_json)
{
    const auto json_mission_items = qgc_plan_json["mission"];
    auto new_mission_item = std::make_shared<MissionItem>();

    // Iterate mission items and build Mavsdk mission items.
    for (auto& json_mission_item : json_mission_items["items"]) {
        // Parameters of Mission item & MAV command of it.
        MAV_CMD command = static_cast<MAV_CMD>(json_mission_item["command"].asInt());

        // Extract parameters of each mission item
        std::vector<double> params;
        for (auto& p : json_mission_item["params"]) {
            if (p.type() == Json::nullValue) {
                // QGC sets params as `null` if they should be unchanged.
                params.push_back(double(NAN));
            } else {
                params.push_back(p.asDouble());
            }
        }

        Mission::Result result =
            build_mission_items(command, params, new_mission_item, all_mission_items);
        if (result != Mission::Result::SUCCESS) {
            break;
        }
    }
    // Don't forget to add the last mission which possibly didn't have position set.
    all_mission_items.push_back(new_mission_item);
    return Mission::Result::SUCCESS;
}

} // namespace mavsdk
