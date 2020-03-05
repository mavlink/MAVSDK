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
        MAVLINK_MSG_ID_MISSION_CURRENT,
        std::bind(&MissionImpl::process_mission_current, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MISSION_ITEM_REACHED,
        std::bind(&MissionImpl::process_mission_item_reached, this, _1),
        this);
}

void MissionImpl::enable() {}

void MissionImpl::disable() {}

void MissionImpl::deinit()
{
    _parent->unregister_timeout_handler(_timeout_cookie);
    _parent->unregister_all_mavlink_message_handlers(this);
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
        // report_mission_result(_mission_data.result_callback, Mission::Result::SUCCESS);
        //_parent->unregister_timeout_handler(_timeout_cookie);
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

void MissionImpl::upload_mission_async(
    const std::vector<std::shared_ptr<MissionItem>>& mission_items,
    const Mission::result_callback_t& callback)
{
    if (_mission_data.last_upload.lock()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                callback(Mission::Result::BUSY);
            }
        });
        return;
    }

    if (!_parent->does_support_mission_int()) {
        LogWarn() << "Mission int messages not supported";
        // report_mission_result(callback, Mission::Result::ERROR);
        return;
    }

    const auto int_items = convert_to_int_items(mission_items);

    _mission_data.last_upload = _parent->mission_transfer().upload_items_async(
        MAV_MISSION_TYPE_MISSION,
        int_items,
        [this, callback](MAVLinkMissionTransfer::Result result) {
            auto converted_result = convert_result(result);
            _parent->call_user_callback([callback, converted_result]() {
                if (callback) {
                    callback(converted_result);
                }
            });
        });
}

void MissionImpl::upload_mission_cancel()
{
    auto ptr = _mission_data.last_upload.lock();
    if (ptr) {
        ptr->cancel();
    }
}

void MissionImpl::download_mission_async(
    const Mission::mission_items_and_result_callback_t& callback)
{
    if (_mission_data.last_download.lock()) {
        _parent->call_user_callback([callback]() {
            if (callback) {
                std::vector<std::shared_ptr<MissionItem>> empty_items;
                callback(Mission::Result::BUSY, empty_items);
            }
        });
        return;
    }

    _mission_data.last_download = _parent->mission_transfer().download_items_async(
        MAV_MISSION_TYPE_MISSION,
        [this, callback](
            MAVLinkMissionTransfer::Result result,
            std::vector<MAVLinkMissionTransfer::ItemInt> items) {
            auto result_and_items = convert_to_result_and_mission_items(result, items);
            _parent->call_user_callback([callback, result_and_items]() {
                callback(result_and_items.first, result_and_items.second);
            });
        });
}

void MissionImpl::download_mission_cancel()
{
    auto ptr = _mission_data.last_download.lock();
    if (ptr) {
        ptr->cancel();
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

std::vector<MAVLinkMissionTransfer::ItemInt>
MissionImpl::convert_to_int_items(const std::vector<std::shared_ptr<MissionItem>>& mission_items)
{
    std::vector<MAVLinkMissionTransfer::ItemInt> int_items;

    bool last_position_valid = false; // This flag is to protect us from using an invalid x/y.
    MAV_FRAME last_frame;
    int32_t last_x;
    int32_t last_y;
    float last_z;

    unsigned item_i = 0;

    for (const auto& item : mission_items) {
        MissionItemImpl& mission_item_impl = (*(item)->_impl);

        if (mission_item_impl.is_position_finite()) {
            // Current is the 0th waypoint
            uint8_t current = ((int_items.size() == 0) ? 1 : 0);

            MAVLinkMissionTransfer::ItemInt next_item{
                static_cast<uint16_t>(int_items.size()),
                static_cast<uint8_t>(mission_item_impl.get_mavlink_frame()),
                static_cast<uint8_t>(mission_item_impl.get_mavlink_cmd()),
                current,
                mission_item_impl.get_mavlink_autocontinue(),
                mission_item_impl.get_mavlink_param1(),
                mission_item_impl.get_mavlink_param2(),
                mission_item_impl.get_mavlink_param3(),
                mission_item_impl.get_mavlink_param4(),
                mission_item_impl.get_mavlink_x(),
                mission_item_impl.get_mavlink_y(),
                mission_item_impl.get_mavlink_z(),
                MAV_MISSION_TYPE_MISSION};

            last_position_valid = true; // because we checked is_position_finite
            last_x = mission_item_impl.get_mavlink_x();
            last_y = mission_item_impl.get_mavlink_y();
            last_z = mission_item_impl.get_mavlink_z();
            last_frame = mission_item_impl.get_mavlink_frame();

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
            int_items.push_back(next_item);
        }

        if (std::isfinite(mission_item_impl.get_speed_m_s())) {
            // The speed has changed, we need to add a speed command.

            // Current is the 0th waypoint
            uint8_t current = ((int_items.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            MAVLinkMissionTransfer::ItemInt next_item{static_cast<uint16_t>(int_items.size()),
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
                                                      MAV_MISSION_TYPE_MISSION};

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
            int_items.push_back(next_item);
        }

        if (std::isfinite(mission_item_impl.get_gimbal_yaw_deg()) ||
            std::isfinite(mission_item_impl.get_gimbal_pitch_deg())) {
            if (_enable_absolute_gimbal_yaw_angle) {
                // We need to configure the gimbal to use an absolute angle.

                // Current is the 0th waypoint
                uint8_t current = ((int_items.size() == 0) ? 1 : 0);

                uint8_t autocontinue = 1;

                MAVLinkMissionTransfer::ItemInt next_item{
                    static_cast<uint16_t>(int_items.size()),
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
                    MAV_MISSION_TYPE_MISSION};

                _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                    std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
                int_items.push_back(next_item);
            }

            // The gimbal has changed, we need to add a gimbal command.

            // Current is the 0th waypoint
            uint8_t current = ((int_items.size() == 0) ? 1 : 0);

            uint8_t autocontinue = 1;

            MAVLinkMissionTransfer::ItemInt next_item{
                static_cast<uint16_t>(int_items.size()),
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
                MAV_MISSION_TYPE_MISSION};

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
            int_items.push_back(next_item);
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
                uint8_t current = ((int_items.size() == 0) ? 1 : 0);

                uint8_t autocontinue = 1;

                MAVLinkMissionTransfer::ItemInt next_item{
                    static_cast<uint16_t>(int_items.size()),
                    static_cast<uint8_t>(last_frame),
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
                    MAV_MISSION_TYPE_MISSION};

                _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                    std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
                int_items.push_back(next_item);
            }

            if (mission_item_impl.get_fly_through()) {
                LogWarn() << "Conflicting options set: fly_through=true and loiter_time>0.";
            }
        }

        if (mission_item_impl.get_camera_action() != MissionItem::CameraAction::NONE) {
            // There is a camera action that we need to send.

            // Current is the 0th waypoint
            uint8_t current = ((int_items.size() == 0) ? 1 : 0);

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

            MAVLinkMissionTransfer::ItemInt next_item{static_cast<uint16_t>(int_items.size()),
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
                                                      MAV_MISSION_TYPE_MISSION};

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
            int_items.push_back(next_item);
        }

        ++item_i;
    }

    // We need to decrement the item_i again because it was increased in the loop above
    // but the RTL item below still belongs to the last mission item.
    --item_i;

    if (_enable_return_to_launch_after_mission) {
        MAVLinkMissionTransfer::ItemInt next_item{static_cast<uint16_t>(int_items.size()),
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
                                                  MAV_MISSION_TYPE_MISSION};

        _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
            std::pair<int, int>{static_cast<int>(int_items.size()), item_i});
        int_items.push_back(next_item);
    }
    return int_items;
}

std::pair<Mission::Result, std::vector<std::shared_ptr<MissionItem>>>
MissionImpl::convert_to_result_and_mission_items(
    MAVLinkMissionTransfer::Result result,
    const std::vector<MAVLinkMissionTransfer::ItemInt>& int_items)
{
    std::pair<Mission::Result, std::vector<std::shared_ptr<MissionItem>>> result_pair;

    result_pair.first = convert_result(result);
    if (result_pair.first != Mission::Result::SUCCESS) {
        return result_pair;
    }

    Mission::mission_items_and_result_callback_t callback;
    {
        _enable_return_to_launch_after_mission = false;

        auto new_mission_item = std::make_shared<MissionItem>();
        bool have_set_position = false;

        int mavlink_item_i = 0;

        for (const auto& int_item : int_items) {
            LogDebug() << "Assembling Message: " << int(int_item.seq);

            if (int_item.command == MAV_CMD_NAV_WAYPOINT) {
                if (int_item.frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
                    LogErr() << "Waypoint frame not supported unsupported";
                    result_pair.first = Mission::Result::UNSUPPORTED;
                    break;
                }

                if (have_set_position) {
                    // When a new position comes in, create next mission item.
                    result_pair.second.push_back(new_mission_item);
                    new_mission_item = std::make_shared<MissionItem>();
                    have_set_position = false;
                }

                new_mission_item->set_position(
                    double(int_item.x) * 1e-7, double(int_item.y) * 1e-7);
                new_mission_item->set_relative_altitude(int_item.z);

                new_mission_item->set_fly_through(!(int_item.param1 > 0));

                have_set_position = true;

            } else if (int_item.command == MAV_CMD_DO_MOUNT_CONTROL) {
                if (int(int_item.z) != MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                    LogErr() << "Gimbal mount control mode unsupported";
                    result_pair.first = Mission::Result::UNSUPPORTED;
                    break;
                }

                new_mission_item->set_gimbal_pitch_and_yaw(int_item.param1, int_item.param3);

            } else if (int_item.command == MAV_CMD_DO_MOUNT_CONFIGURE) {
                if (int(int_item.param1) != MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                    LogErr() << "Gimbal mount configure mode unsupported";
                    result_pair.first = Mission::Result::UNSUPPORTED;
                    break;
                }

                // FIXME: ultimately param4 doesn't count anymore and
                //        param7 holds the truth.
                if (int(int_item.param4) == 1 || int(int_item.z) == 2) {
                    _enable_absolute_gimbal_yaw_angle = true;
                } else {
                    _enable_absolute_gimbal_yaw_angle = false;
                }

            } else if (int_item.command == MAV_CMD_IMAGE_START_CAPTURE) {
                if (int_item.param2 > 0 && int(int_item.param3) == 0) {
                    new_mission_item->set_camera_action(
                        MissionItem::CameraAction::START_PHOTO_INTERVAL);
                    new_mission_item->set_camera_photo_interval(double(int_item.param2));
                } else if (int(int_item.param2) == 0 && int(int_item.param3) == 1) {
                    new_mission_item->set_camera_action(MissionItem::CameraAction::TAKE_PHOTO);
                } else {
                    LogErr() << "Mission item START_CAPTURE params unsupported.";
                    result_pair.first = Mission::Result::UNSUPPORTED;
                    break;
                }

            } else if (int_item.command == MAV_CMD_IMAGE_STOP_CAPTURE) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_PHOTO_INTERVAL);

            } else if (int_item.command == MAV_CMD_VIDEO_START_CAPTURE) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::START_VIDEO);

            } else if (int_item.command == MAV_CMD_VIDEO_STOP_CAPTURE) {
                new_mission_item->set_camera_action(MissionItem::CameraAction::STOP_VIDEO);

            } else if (int_item.command == MAV_CMD_DO_CHANGE_SPEED) {
                if (int(int_item.param1) == 1 && int_item.param3 < 0 && int(int_item.param4) == 0) {
                    new_mission_item->set_speed(int_item.param2);
                } else {
                    LogErr() << "Mission item DO_CHANGE_SPEED params unsupported";
                    result_pair.first = Mission::Result::UNSUPPORTED;
                }

            } else if (int_item.command == MAV_CMD_NAV_LOITER_TIME) {
                new_mission_item->set_loiter_time(int_item.param1);

            } else if (int_item.command == MAV_CMD_NAV_RETURN_TO_LAUNCH) {
                _enable_return_to_launch_after_mission = true;

            } else {
                LogErr() << "UNSUPPORTED mission item command (" << int_item.command << ")";
                result_pair.first = Mission::Result::UNSUPPORTED;
                break;
            }

            _mission_data.mavlink_mission_item_to_mission_item_indices.insert(
                std::pair<int, int>{mavlink_item_i, static_cast<int>(result_pair.second.size())});

            ++mavlink_item_i;
        }

        // Don't forget to add last mission item.
        result_pair.second.push_back(new_mission_item);
    }
    return result_pair;
}

void MissionImpl::start_mission_async(const Mission::result_callback_t& callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::MISSION, [this, callback](MAVLinkCommands::Result result, float) {
            report_flight_mode_change(callback, result);
        });
}

void MissionImpl::pause_mission_async(const Mission::result_callback_t& callback)
{
    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::HOLD, [this, callback](MAVLinkCommands::Result result, float) {
            report_flight_mode_change(callback, result);
        });
}

void MissionImpl::report_flight_mode_change(
    Mission::result_callback_t callback, MAVLinkCommands::Result result)
{
    if (!callback) {
        return;
    }

    _parent->call_user_callback(
        [callback, result]() { callback(command_result_to_mission_result(result)); });
}

Mission::Result MissionImpl::command_result_to_mission_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Mission::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return Mission::Result::ERROR; // FIXME
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return Mission::Result::ERROR; // FIXME
        case MAVLinkCommands::Result::BUSY:
            return Mission::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Mission::Result::ERROR; // FIXME
        case MAVLinkCommands::Result::TIMEOUT:
            return Mission::Result::TIMEOUT;
        case MAVLinkCommands::Result::IN_PROGRESS:
            return Mission::Result::BUSY; // FIXME
        case MAVLinkCommands::Result::UNKNOWN_ERROR:
            return Mission::Result::UNKNOWN;
        default:
            return Mission::Result::UNKNOWN;
    }
}

void MissionImpl::clear_mission_async(const Mission::result_callback_t& callback)
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

void MissionImpl::set_current_mission_item_async(int current, Mission::result_callback_t& callback)
{
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

    _parent->mission_transfer().set_current_item_async(
            MAV_MISSION_TYPE_MISSION,
            mavlink_index,
            [this, callback](MAVLinkMissionTransfer::Result result) {
                auto converted_result = convert_result(result);
                _parent->call_user_callback([callback, converted_result]() {
                    if (callback) {
                        callback(converted_result);
                    }
                });
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

bool MissionImpl::is_mission_finished() const
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);

    if (_mission_data.last_current_mavlink_mission_item < 0) {
        return false;
    }

    if (_mission_data.last_reached_mavlink_mission_item < 0) {
        return false;
    }

    if (_mission_data.mavlink_mission_item_to_mission_item_indices.size() == 0) {
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
        _mission_data.mavlink_mission_item_to_mission_item_indices.size());
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
    return static_cast<int>(
        _mission_data.mavlink_mission_item_to_mission_item_indices.rbegin()->second + 1);
}

void MissionImpl::subscribe_progress(Mission::progress_callback_t callback)
{
    std::lock_guard<std::recursive_mutex> lock(_mission_data.mutex);
    _mission_data.progress_callback = callback;
}

Mission::Result MissionImpl::convert_result(MAVLinkMissionTransfer::Result result)
{
    switch (result) {
        case MAVLinkMissionTransfer::Result::Success:
            return Mission::Result::SUCCESS;
        case MAVLinkMissionTransfer::Result::ConnectionError:
            return Mission::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::Denied:
            return Mission::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::TooManyMissionItems:
            return Mission::Result::TOO_MANY_MISSION_ITEMS;
        case MAVLinkMissionTransfer::Result::Timeout:
            return Mission::Result::TIMEOUT;
        case MAVLinkMissionTransfer::Result::Unsupported:
            return Mission::Result::UNSUPPORTED;
        case MAVLinkMissionTransfer::Result::UnsupportedFrame:
            return Mission::Result::UNSUPPORTED;
        case MAVLinkMissionTransfer::Result::NoMissionAvailable:
            return Mission::Result::NO_MISSION_AVAILABLE;
        case MAVLinkMissionTransfer::Result::Cancelled:
            return Mission::Result::CANCELLED;
        case MAVLinkMissionTransfer::Result::MissionTypeNotConsistent:
            return Mission::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidSequence:
            return Mission::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::CurrentInvalid:
            return Mission::Result::INVALID_ARGUMENT; // FIXME
        case MAVLinkMissionTransfer::Result::ProtocolError:
            return Mission::Result::ERROR; // FIXME
        case MAVLinkMissionTransfer::Result::InvalidParam:
            return Mission::Result::INVALID_ARGUMENT; // FIXME
        default:
            return Mission::Result::UNKNOWN;
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
