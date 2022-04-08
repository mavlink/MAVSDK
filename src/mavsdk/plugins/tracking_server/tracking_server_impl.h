#pragma once

#include "plugins/tracking_server/tracking_server.h"
#include "server_plugin_impl_base.h"
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

    void subscribe_tracking_point_command(TrackingServer::TrackingPointCommandCallback callback);

    void
    subscribe_tracking_rectangle_command(TrackingServer::TrackingRectangleCommandCallback callback);

    void subscribe_tracking_off_command(TrackingServer::TrackingOffCommandCallback callback);

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
    TrackingServer::TrackingPointCommandCallback _tracking_point_callback{nullptr};
    TrackingServer::TrackingRectangleCommandCallback _tracking_rectangle_callback{nullptr};
    TrackingServer::TrackingOffCommandCallback _tracking_off_callback{nullptr};

    uint8_t _tracking_point_command_sysid{0};
    uint8_t _tracking_point_command_compid{0};
    uint8_t _tracking_rectangle_command_sysid{0};
    uint8_t _tracking_rectangle_command_compid{0};
    uint8_t _tracking_off_command_sysid{0};
    uint8_t _tracking_off_command_compid{0};
};

} // namespace mavsdk
