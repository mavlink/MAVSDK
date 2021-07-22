#include "mission_server_impl.h"

namespace mavsdk {

using MissionItem = MissionServer::MissionItem;
using CameraAction = MissionServer::MissionItem::CameraAction;

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
                            auto result_and_items =
                                convert_to_result_and_mission_items(result, items);
                            _parent->call_user_callback([this, result_and_items]() {
                                _incoming_mission_callback(
                                    result_and_items.first, result_and_items.second);
                            });
                        });
                _do_upload = false;
            }
        }
    });

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
}

std::pair<MissionServer::Result, MissionServer::MissionPlan>
MissionServerImpl::convert_to_result_and_mission_items(
    MAVLinkMissionTransfer::Result result,
    const std::vector<MAVLinkMissionTransfer::ItemInt>& int_items)
{
    std::pair<MissionServer::Result, MissionServer::MissionPlan> result_pair;

    result_pair.first = convert_result(result);
    if (result_pair.first != MissionServer::Result::Success) {
        return result_pair;
    }

    _mission_data.mavlink_mission_item_to_mission_item_indices.clear();

    {
        _enable_return_to_launch_after_mission = false;

        MissionItem new_mission_item{};
        bool have_set_position = false;

        for (const auto& int_item : int_items) {
            LogDebug() << "Assembling Message: " << int(int_item.seq);

            if (int_item.command == MAV_CMD_NAV_WAYPOINT) {
                if (int_item.frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
                    LogErr() << "Waypoint frame not supported unsupported";
                    result_pair.first = MissionServer::Result::Unsupported;
                    break;
                }

                if (have_set_position) {
                    // When a new position comes in, create next mission item.
                    result_pair.second.mission_items.push_back(new_mission_item);
                    new_mission_item = {};
                    have_set_position = false;
                }

                new_mission_item.latitude_deg = double(int_item.x) * 1e-7;
                new_mission_item.longitude_deg = double(int_item.y) * 1e-7;
                new_mission_item.relative_altitude_m = int_item.z;

                new_mission_item.is_fly_through = !(int_item.param1 > 0);
                new_mission_item.acceptance_radius_m = int_item.param2;

                have_set_position = true;

            } else if (int_item.command == MAV_CMD_DO_MOUNT_CONTROL) {
                if (int(int_item.z) != MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                    LogErr() << "Gimbal mount control mode unsupported";
                    result_pair.first = MissionServer::Result::Unsupported;
                    break;
                }

                new_mission_item.gimbal_pitch_deg = int_item.param1;
                new_mission_item.gimbal_yaw_deg = int_item.param3;

            } else if (int_item.command == MAV_CMD_DO_GIMBAL_MANAGER_PITCHYAW) {
                if (int_item.x !=
                    (GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK)) {
                    LogErr() << "Gimbal do pitchyaw flags unsupported";
                    result_pair.first = MissionServer::Result::Unsupported;
                    break;
                }

                new_mission_item.gimbal_pitch_deg = int_item.param1;
                new_mission_item.gimbal_yaw_deg = int_item.param2;

            } else if (int_item.command == MAV_CMD_DO_MOUNT_CONFIGURE) {
                if (int(int_item.param1) != MAV_MOUNT_MODE_MAVLINK_TARGETING) {
                    LogErr() << "Gimbal mount configure mode unsupported";
                    result_pair.first = MissionServer::Result::Unsupported;
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
                    new_mission_item.camera_action = CameraAction::StartPhotoInterval;
                    new_mission_item.camera_photo_interval_s = double(int_item.param2);
                } else if (int(int_item.param2) == 0 && int(int_item.param3) == 1) {
                    new_mission_item.camera_action = CameraAction::TakePhoto;
                } else {
                    LogErr() << "Mission item START_CAPTURE params unsupported.";
                    result_pair.first = MissionServer::Result::Unsupported;
                    break;
                }

            } else if (int_item.command == MAV_CMD_IMAGE_STOP_CAPTURE) {
                new_mission_item.camera_action = CameraAction::StopPhotoInterval;

            } else if (int_item.command == MAV_CMD_VIDEO_START_CAPTURE) {
                new_mission_item.camera_action = CameraAction::StartVideo;

            } else if (int_item.command == MAV_CMD_VIDEO_STOP_CAPTURE) {
                new_mission_item.camera_action = CameraAction::StopVideo;

            } else if (int_item.command == MAV_CMD_DO_CHANGE_SPEED) {
                if (int(int_item.param1) == 1 && int_item.param3 < 0 && int(int_item.param4) == 0) {
                    new_mission_item.speed_m_s = int_item.param2;
                } else {
                    LogErr() << "Mission item DO_CHANGE_SPEED params unsupported";
                    result_pair.first = MissionServer::Result::Unsupported;
                }

            } else if (int_item.command == MAV_CMD_NAV_LOITER_TIME) {
                // MAVSDK doesn't use LOITER_TIME anymore, but it is possible
                // a mission still uses it
                new_mission_item.loiter_time_s = int_item.param1;

            } else if (int_item.command == MAV_CMD_NAV_DELAY) {
                if (int_item.param1 != -1) {
                    // use delay in seconds directly
                    new_mission_item.loiter_time_s = int_item.param1;
                } else {
                    // TODO: we should support this by converting
                    // time of day data to delay in seconds
                    // leaving it out for now because a portable implementation
                    // is not trivial
                    LogErr() << "Mission item NAV_DELAY params unsupported";
                    result_pair.first = MissionServer::Result::Unsupported;
                }

            } else if (int_item.command == MAV_CMD_NAV_RETURN_TO_LAUNCH) {
                _enable_return_to_launch_after_mission = true;

            } else {
                LogErr() << "UNSUPPORTED mission item command (" << int_item.command << ")";
                result_pair.first = MissionServer::Result::Unsupported;
                break;
            }

            _mission_data.mavlink_mission_item_to_mission_item_indices.push_back(
                result_pair.second.mission_items.size());
        }

        // Don't forget to add last mission item.
        result_pair.second.mission_items.push_back(new_mission_item);
    }
    return result_pair;
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

} // namespace mavsdk