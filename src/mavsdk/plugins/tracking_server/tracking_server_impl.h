#pragma once

#include "plugins/tracking_server/tracking_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"

#include <optional>

namespace mavsdk {

class TrackingServerImpl : public ServerPluginImplBase {
public:
    explicit TrackingServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~TrackingServerImpl() override;

    void init() override;
    void deinit() override;

    void set_tracking_point_status(TrackingServer::TrackPoint tracked_point);

    void set_tracking_rectangle_status(TrackingServer::TrackRectangle tracked_rectangle);

    void set_tracking_off_status();

    TrackingServer::TrackingPointCommandHandle
    subscribe_tracking_point_command(const TrackingServer::TrackingPointCommandCallback& callback);
    void unsubscribe_tracking_point_command(TrackingServer::TrackingPointCommandHandle handle);

    TrackingServer::TrackingRectangleCommandHandle subscribe_tracking_rectangle_command(
        const TrackingServer::TrackingRectangleCommandCallback& callback);
    void
    unsubscribe_tracking_rectangle_command(TrackingServer::TrackingRectangleCommandHandle handle);

    TrackingServer::TrackingOffCommandHandle
    subscribe_tracking_off_command(const TrackingServer::TrackingOffCommandCallback& callback);
    void unsubscribe_tracking_off_command(TrackingServer::TrackingOffCommandHandle handle);

    TrackingServer::Result
    respond_tracking_point_command(TrackingServer::CommandAnswer command_answer);

    TrackingServer::Result
    respond_tracking_rectangle_command(TrackingServer::CommandAnswer command_answer);

    TrackingServer::Result
    respond_tracking_off_command(TrackingServer::CommandAnswer command_answer);

private:
    std::optional<mavlink_message_t>
    process_track_point_command(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_track_rectangle_command(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_track_off_command(const MavlinkCommandReceiver::CommandLong& command);

    bool is_command_sender_ok(const MavlinkCommandReceiver::CommandLong& command);
    MAV_RESULT mav_result_from_command_answer(TrackingServer::CommandAnswer command_answer);

    std::mutex _mutex{};
    CallbackList<TrackingServer::TrackPoint> _tracking_point_callbacks{};
    CallbackList<TrackingServer::TrackRectangle> _tracking_rectangle_callbacks{};
    CallbackList<int32_t> _tracking_off_callbacks{};

    uint8_t _tracking_point_command_sysid{0};
    uint8_t _tracking_point_command_compid{0};
    uint8_t _tracking_rectangle_command_sysid{0};
    uint8_t _tracking_rectangle_command_compid{0};
    uint8_t _tracking_off_command_sysid{0};
    uint8_t _tracking_off_command_compid{0};
};

} // namespace mavsdk
