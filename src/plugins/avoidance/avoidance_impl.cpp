#include "avoidance_impl.h"
#include "global_include.h"
#include "log.h"

namespace mavsdk {

AvoidanceImpl::AvoidanceImpl(System& system) : PluginImplBase(system), _mavlink_avoidance_item_messages()
{
    _parent->register_plugin(this);
}

AvoidanceImpl::~AvoidanceImpl()
{
    _parent->unregister_plugin(this);
}

void AvoidanceImpl::init() {}

void AvoidanceImpl::deinit() {}

void AvoidanceImpl::enable() {}

void AvoidanceImpl::disable() {}

void AvoidanceImpl::process_trajectory_representation_waypoints(const mavlink_message_t& message)
{
    mavlink_trajectory_representation_waypoints_t decoded;
    mavlink_msg_trajectory_representation_waypoints_decode(&message, &decoded);

    std::vector<Avoidance::Waypoint> waypoints;
    float pos_x[5], pos_y[5], pos_z[5];
    float vel_x[5], vel_y[5], vel_z[5];
    float acc_x[5], acc_y[5], acc_z[5];
    float pos_yaw[5], vel_yaw[5];
    uint16_t command[5];

    for (int i = 0;i < decoded.valid_points;i++) {
        waypoints.push_back(Avoidance::Waypoint({
                    decoded.pos_x[i], decoded.pos_y[i], decoded.pos_z[i],
                    decoded.vel_x[i], decoded.vel_y[i], decoded.vel_z[i],
                    decoded.acc_x[i], decoded.acc_y[i], decoded.acc_z[i],
                    decoded.pos_yaw[i], decoded.vel_yaw[i],
                    decoded.command[i]}));
    }

    set_trajectory_representation_waypoints(
            Avoidance::WaypointsTrajectory({waypoints,
                Avoidance::WaypointsTrajectory::Direction::RECEIVE}));

    if (_trajectory_representation_waypoints_subscription) {
        auto callback = _trajectory_representation_waypoints_subscription;
        auto arg = get_trajectory_representation_waypoints();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void AvoidanceImpl::set_trajectory_representation_waypoints(Avoidance::WaypointsTrajectory waypoints_trajectory)
{
    std::lock_guard<std::mutex> lock(_waypoints_trajectory_mutex);
    _waypoints_trajectory = waypoints_trajectory;
}

Avoidance::WaypointsTrajectory AvoidanceImpl::get_trajectory_representation_waypoints() const
{
    std::lock_guard<std::mutex> lock(_trajectory_representation_waypoints_mutex);
    return _trajectory_representation_waypoints;
}

void AvoidanceImpl::receive_trajectory_representation_waypoints_async(
    Avoidance::trajectory_representation_waypoints_callback_t& callback)
{
    _trajectory_representation_waypoints_subscription = callback;
}

} // namespace mavsdk
