#include "tracking_server_impl.h"
#include "callback_list.tpp"
#include <mutex>

namespace mavsdk {

template class CallbackList<TrackingServer::TrackPoint>;
template class CallbackList<TrackingServer::TrackRectangle>;
template class CallbackList<int32_t>;

TrackingServerImpl::TrackingServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    // FIXME: use other component ID?
    _server_component_impl->register_plugin(this);
}

TrackingServerImpl::~TrackingServerImpl()
{
    // FIXME: use other component ID?
    _server_component_impl->unregister_plugin(this);
}

void TrackingServerImpl::init()
{
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_CAMERA_TRACK_POINT,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_track_point_command(command);
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_CAMERA_TRACK_RECTANGLE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_track_rectangle_command(command);
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_CAMERA_STOP_TRACKING,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_track_off_command(command);
        },
        this);
}

void TrackingServerImpl::deinit()
{
    _server_component_impl->unregister_mavlink_command_handler(MAV_CMD_CAMERA_TRACK_POINT, this);
    _server_component_impl->unregister_mavlink_command_handler(
        MAV_CMD_CAMERA_TRACK_RECTANGLE, this);
    _server_component_impl->unregister_mavlink_command_handler(MAV_CMD_CAMERA_STOP_TRACKING, this);
}

void TrackingServerImpl::set_tracking_point_status(TrackingServer::TrackPoint tracked_point)
{
    mavlink_message_t message;
    mavlink_msg_camera_tracking_image_status_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        CAMERA_TRACKING_STATUS_FLAGS_ACTIVE,
        CAMERA_TRACKING_MODE_POINT,
        CAMERA_TRACKING_TARGET_DATA_IN_STATUS,
        tracked_point.point_x,
        tracked_point.point_y,
        tracked_point.radius,
        0.0f,
        0.0f,
        0.0f,
        0.0f);
    _server_component_impl->send_message(message);
}

void TrackingServerImpl::set_tracking_rectangle_status(
    TrackingServer::TrackRectangle tracked_rectangle)
{
    mavlink_message_t message;
    mavlink_msg_camera_tracking_image_status_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        CAMERA_TRACKING_STATUS_FLAGS_ACTIVE,
        CAMERA_TRACKING_MODE_RECTANGLE,
        CAMERA_TRACKING_TARGET_DATA_IN_STATUS,
        0.0f,
        0.0f,
        0.0f,
        tracked_rectangle.top_left_corner_x,
        tracked_rectangle.top_left_corner_y,
        tracked_rectangle.bottom_right_corner_x,
        tracked_rectangle.bottom_right_corner_y);
    _server_component_impl->send_message(message);
}

void TrackingServerImpl::set_tracking_off_status()
{
    mavlink_message_t message;
    mavlink_msg_camera_tracking_image_status_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        CAMERA_TRACKING_STATUS_FLAGS_IDLE,
        CAMERA_TRACKING_MODE_NONE,
        CAMERA_TRACKING_TARGET_DATA_NONE,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f);
    _server_component_impl->send_message(message);
}

TrackingServer::TrackingPointCommandHandle TrackingServerImpl::subscribe_tracking_point_command(
    const TrackingServer::TrackingPointCommandCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _tracking_point_callbacks.subscribe(callback);
}

void TrackingServerImpl::unsubscribe_tracking_point_command(
    TrackingServer::TrackingPointCommandHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tracking_point_callbacks.unsubscribe(handle);
}

TrackingServer::TrackingRectangleCommandHandle
TrackingServerImpl::subscribe_tracking_rectangle_command(
    const TrackingServer::TrackingRectangleCommandCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _tracking_rectangle_callbacks.subscribe(callback);
}

void TrackingServerImpl::unsubscribe_tracking_rectangle_command(
    TrackingServer::TrackingRectangleCommandHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tracking_rectangle_callbacks.unsubscribe(handle);
}

TrackingServer::TrackingOffCommandHandle TrackingServerImpl::subscribe_tracking_off_command(
    const TrackingServer::TrackingOffCommandCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _tracking_off_callbacks.subscribe(callback);
}

void TrackingServerImpl::unsubscribe_tracking_off_command(
    TrackingServer::TrackingOffCommandHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tracking_off_callbacks.unsubscribe(handle);
}

TrackingServer::Result
TrackingServerImpl::respond_tracking_point_command(TrackingServer::CommandAnswer command_answer)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_message_t message;
    mavlink_msg_command_ack_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        MAV_CMD_CAMERA_TRACK_POINT,
        mav_result_from_command_answer(command_answer),
        0,
        0,
        _tracking_point_command_sysid,
        _tracking_point_command_compid);

    return _server_component_impl->send_message(message) ? TrackingServer::Result::Success :
                                                           TrackingServer::Result::ConnectionError;
}

TrackingServer::Result
TrackingServerImpl::respond_tracking_rectangle_command(TrackingServer::CommandAnswer command_answer)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_message_t message;
    mavlink_msg_command_ack_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        MAV_CMD_CAMERA_TRACK_RECTANGLE,
        mav_result_from_command_answer(command_answer),
        0,
        0,
        _tracking_rectangle_command_sysid,
        _tracking_rectangle_command_compid);

    return _server_component_impl->send_message(message) ? TrackingServer::Result::Success :
                                                           TrackingServer::Result::ConnectionError;
}

TrackingServer::Result
TrackingServerImpl::respond_tracking_off_command(TrackingServer::CommandAnswer command_answer)
{
    std::lock_guard<std::mutex> lock(_mutex);

    mavlink_message_t message;
    mavlink_msg_command_ack_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        MAV_CMD_CAMERA_STOP_TRACKING,
        mav_result_from_command_answer(command_answer),
        0,
        0,
        _tracking_off_command_sysid,
        _tracking_off_command_compid);

    return _server_component_impl->send_message(message) ? TrackingServer::Result::Success :
                                                           TrackingServer::Result::ConnectionError;
}

std::optional<mavlink_message_t>
TrackingServerImpl::process_track_point_command(const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track point command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    TrackingServer::TrackPoint track_point{
        command.params.param1, command.params.param2, command.params.param3};

    std::lock_guard<std::mutex> lock(_mutex);
    _tracking_point_command_sysid = command.origin_system_id;
    _tracking_point_command_compid = command.origin_component_id;

    _tracking_point_callbacks.queue(track_point, [this](const auto& func) {
        _server_component_impl->call_user_callback(func);
    });

    // We don't send an ack but leave that to the user.
    return std::nullopt;
}

std::optional<mavlink_message_t> TrackingServerImpl::process_track_rectangle_command(
    const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track rectangle command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    TrackingServer::TrackRectangle track_rectangle{
        command.params.param1, command.params.param2, command.params.param3, command.params.param4};

    std::lock_guard<std::mutex> lock(_mutex);
    _tracking_rectangle_command_sysid = command.origin_system_id;
    _tracking_rectangle_command_compid = command.origin_component_id;

    _tracking_rectangle_callbacks.queue(track_rectangle, [this](const auto& func) {
        _server_component_impl->call_user_callback(func);
    });

    // We don't send an ack but leave that to the user.
    return std::nullopt;
}

std::optional<mavlink_message_t>
TrackingServerImpl::process_track_off_command(const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track off command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    _tracking_off_command_sysid = command.origin_system_id;
    _tracking_off_command_compid = command.origin_component_id;

    _tracking_off_callbacks.queue(
        0, [this](const auto& func) { _server_component_impl->call_user_callback(func); });

    // We don't send an ack but leave that to the user.
    return std::nullopt;
}

bool TrackingServerImpl::is_command_sender_ok(const MavlinkCommandReceiver::CommandLong& command)
{
    if (command.target_system_id != 0 &&
        command.target_system_id != _server_component_impl->get_own_system_id()) {
        return false;
    } else {
        return true;
    }
}

MAV_RESULT
TrackingServerImpl::mav_result_from_command_answer(TrackingServer::CommandAnswer command_answer)
{
    switch (command_answer) {
        case TrackingServer::CommandAnswer::Accepted:
            return MAV_RESULT_ACCEPTED;
        case TrackingServer::CommandAnswer::TemporarilyRejected:
            return MAV_RESULT_TEMPORARILY_REJECTED;
        case TrackingServer::CommandAnswer::Denied:
            return MAV_RESULT_DENIED;
        case TrackingServer::CommandAnswer::Unsupported:
            return MAV_RESULT_UNSUPPORTED;
        case TrackingServer::CommandAnswer::Failed:
            return MAV_RESULT_FAILED;
    }

    LogErr() << "Unknown CommandAnswer";
    return MAV_RESULT_FAILED;
}

} // namespace mavsdk
