#include "mocap_impl.h"
#include "system.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <array>
#include <ctime>

namespace mavsdk {

void MocapImpl::init() {}

void MocapImpl::deinit() {}

void MocapImpl::enable() {}

void MocapImpl::disable() {}

MocapImpl::MocapImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MocapImpl::~MocapImpl()
{
    _parent->unregister_plugin(this);
}

Mocap::Result MocapImpl::set_vision_position_estimate(
    const Mocap::VisionPositionEstimate& vision_position_estimate)
{
    if (!_parent->is_connected())
        return Mocap::Result::NO_SYSTEM;

    _visual_position_estimate_mutex.lock();
    _vision_position_estimate = vision_position_estimate;
    _visual_position_estimate_mutex.unlock();

    if (!send_vision_position_estimate())
        return Mocap::Result::CONNECTION_ERROR;

    return Mocap::Result::SUCCESS;
}

Mocap::Result
MocapImpl::set_attitude_position_mocap(const Mocap::AttitudePositionMocap& attitude_position_mocap)
{
    if (!_parent->is_connected())
        return Mocap::Result::NO_SYSTEM;

    _attitude_position_mocap_mutex.lock();
    _attitude_position_mocap = attitude_position_mocap;
    _attitude_position_mocap_mutex.unlock();

    if (!send_attitude_position_mocap())
        return Mocap::Result::CONNECTION_ERROR;

    return Mocap::Result::SUCCESS;
}

Mocap::Result MocapImpl::set_odometry(const Mocap::Odometry& odometry)
{
    if (!_parent->is_connected())
        return Mocap::Result::NO_SYSTEM;

    _odometry_mutex.lock();
    _odometry = odometry;
    _odometry_mutex.unlock();

    if (!send_odometry())
        return Mocap::Result::CONNECTION_ERROR;

    return Mocap::Result::SUCCESS;
}

bool MocapImpl::send_vision_position_estimate()
{
    _visual_position_estimate_mutex.lock();
    auto vision_position_estimate = _vision_position_estimate;
    _visual_position_estimate_mutex.unlock();

    const uint64_t autopilot_time_usec =
        (!vision_position_estimate.time_usec) ?
            std::chrono::duration_cast<std::chrono::microseconds>(
                _parent->get_autopilot_time().now().time_since_epoch())
                .count() :
            std::chrono::duration_cast<std::chrono::microseconds>(
                _parent->get_autopilot_time()
                    .time_in(dl_system_time_t(
                        std::chrono::microseconds(vision_position_estimate.time_usec)))
                    .time_since_epoch())
                .count();

    mavlink_message_t message;

    mavlink_msg_vision_position_estimate_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        autopilot_time_usec,
        vision_position_estimate.position_body.x_m,
        vision_position_estimate.position_body.y_m,
        vision_position_estimate.position_body.z_m,
        vision_position_estimate.angle_body.roll_rad,
        vision_position_estimate.angle_body.pitch_rad,
        vision_position_estimate.angle_body.yaw_rad,
        vision_position_estimate.pose_covariance.data(),
        vision_position_estimate.reset_counter);

    return _parent->send_message(message);
}

bool MocapImpl::send_attitude_position_mocap()
{
    _attitude_position_mocap_mutex.lock();
    auto attitude_position_mocap = _attitude_position_mocap;
    _attitude_position_mocap_mutex.unlock();

    const uint64_t autopilot_time_usec =
        (!attitude_position_mocap.time_usec) ?
            std::chrono::duration_cast<std::chrono::microseconds>(
                _parent->get_autopilot_time().now().time_since_epoch())
                .count() :
            std::chrono::duration_cast<std::chrono::microseconds>(
                _parent->get_autopilot_time()
                    .time_in(dl_system_time_t(
                        std::chrono::microseconds(attitude_position_mocap.time_usec)))
                    .time_since_epoch())
                .count();

    mavlink_message_t message;

    std::array<float, 4> q{};
    q[0] = attitude_position_mocap.q.w;
    q[1] = attitude_position_mocap.q.x;
    q[2] = attitude_position_mocap.q.y;
    q[3] = attitude_position_mocap.q.z;

    mavlink_msg_att_pos_mocap_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        autopilot_time_usec,
        q.data(),
        attitude_position_mocap.position_body.x_m,
        attitude_position_mocap.position_body.y_m,
        attitude_position_mocap.position_body.z_m,
        attitude_position_mocap.pose_covariance.data());

    return _parent->send_message(message);
}

bool MocapImpl::send_odometry()
{
    _odometry_mutex.lock();
    auto odometry = _odometry;
    _odometry_mutex.unlock();

    const uint64_t autopilot_time_usec =
        (!odometry.time_usec) ?
            std::chrono::duration_cast<std::chrono::microseconds>(
                _parent->get_autopilot_time().now().time_since_epoch())
                .count() :
            std::chrono::duration_cast<std::chrono::microseconds>(
                _parent->get_autopilot_time()
                    .time_in(dl_system_time_t(std::chrono::microseconds(odometry.time_usec)))
                    .time_since_epoch())
                .count();

    mavlink_message_t message;

    std::array<float, 4> q{};
    q[0] = odometry.q.w;
    q[1] = odometry.q.x;
    q[2] = odometry.q.y;
    q[3] = odometry.q.z;

    mavlink_msg_odometry_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        autopilot_time_usec,
        static_cast<uint8_t>(odometry.frame_id),
        static_cast<uint8_t>(MAV_FRAME_BODY_FRD),
        odometry.position_body.x_m,
        odometry.position_body.y_m,
        odometry.position_body.z_m,
        q.data(),
        odometry.speed_body.x_m_s,
        odometry.speed_body.y_m_s,
        odometry.speed_body.z_m_s,
        odometry.angular_velocity_body.roll_rad_s,
        odometry.angular_velocity_body.pitch_rad_s,
        odometry.angular_velocity_body.yaw_rad_s,
        odometry.pose_covariance.data(),
        odometry.velocity_covariance.data(),
        0,
        MAV_ESTIMATOR_TYPE_MOCAP);

    return _parent->send_message(message);
}

} // namespace mavsdk
