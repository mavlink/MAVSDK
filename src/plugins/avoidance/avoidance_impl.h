#pragma once

#include "mavlink_include.h"
#include "plugins/avoidance/avoidance.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class AvoidanceImpl : public PluginImplBase {
public:
    AvoidanceImpl(System& system);
    ~AvoidanceImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void receive_trajectory_representation_waypoints_async(
        trajectory_representation_waypoints_callback_t callback);

    // Non-copyable
    AvoidanceImpl(const AvoidanceImpl&) = delete;
    const AvoidanceImpl& operator=(const AvoidanceImpl&) = delete;

private:
    void timeout_happened();

    void set_trajectory_representation_waypoints(Avoidance::WaypointsTrajectory trajectory);

    void process_trajectory_representation_waypoints(const mavlink_message_t& message);

    mutable std::mutex _trajectory_representation_waypoints_mutex{};
    Avoidance::WaypointsTrajectory _trajectory_representation_waypoints_mutex{std::vector<Avoidance::Waypoint>(), Avoidance::WaypointsTrajectory::Direction::RECEIVE};

    std::vector<std::shared_ptr<mavlink_message_t>> _mavlink_avoidance_item_messages;
};

} // namespace mavsdk
