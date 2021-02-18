#pragma once

#include "plugins/tracking_server/tracking_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class TrackingServerImpl : public PluginImplBase {
public:
    explicit TrackingServerImpl(System& system);
    explicit TrackingServerImpl(std::shared_ptr<System> system);
    ~TrackingServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

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
};

} // namespace mavsdk