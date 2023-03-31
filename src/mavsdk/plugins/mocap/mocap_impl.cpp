#include "mocap_impl.h"
#include "system.h"
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
    _system_impl->register_plugin(this);
}

MocapImpl::MocapImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

MocapImpl::~MocapImpl()
{
    _system_impl->unregister_plugin(this);
}

Mocap::Result MocapImpl::set_vision_position_estimate(
    const Mocap::VisionPositionEstimate& vision_position_estimate)
{
    if (!_system_impl->is_connected()) {
        return Mocap::Result::NoSystem;
    }

    return send_vision_position_estimate(vision_position_estimate);
}

Mocap::Result
MocapImpl::set_attitude_position_mocap(const Mocap::AttitudePositionMocap& attitude_position_mocap)
{
    if (!_system_impl->is_connected()) {
        return Mocap::Result::NoSystem;
    }

    return send_attitude_position_mocap(attitude_position_mocap);
}

Mocap::Result MocapImpl::set_odometry(const Mocap::Odometry& odometry)
{
    if (!_system_impl->is_connected()) {
        return Mocap::Result::NoSystem;
    }

    return send_odometry(odometry);
}

Mocap::Result MocapImpl::send_vision_position_estimate(
    const Mocap::VisionPositionEstimate& vision_position_estimate)
{
    const uint64_t autopilot_time_usec =
        (!vision_position_estimate.time_usec) ?
            std::chrono::duration_cast<std::chrono::microseconds>(
                _system_impl->get_autopilot_time().now().time_since_epoch())
                .count() :
            std::chrono::duration_cast<std::chrono::microseconds>(
                _system_impl->get_autopilot_time()
                    .time_in(SystemTimePoint(
                        std::chrono::microseconds(vision_position_estimate.time_usec)))
                    .time_since_epoch())
                .count();

    std::array<float, 21> covariance{};

    // The covariance matrix needs to have length 21 or 1 with the one entry set to NaN.

    if (vision_position_estimate.pose_covariance.covariance_matrix.size() == 21) {
        std::copy(
            vision_position_estimate.pose_covariance.covariance_matrix.begin(),
            vision_position_estimate.pose_covariance.covariance_matrix.end(),
            covariance.begin());
    } else if (
        vision_position_estimate.pose_covariance.covariance_matrix.size() == 1 &&
        std::isnan(vision_position_estimate.pose_covariance.covariance_matrix[0])) {
        covariance[0] = NAN;
    } else {
        return Mocap::Result::InvalidRequestData;
    }

    mavlink_message_t message;
    mavlink_msg_vision_position_estimate_pack(
        _system_impl->get_own_system_id(),
        _system_impl->get_own_component_id(),
        &message,
        autopilot_time_usec,
        vision_position_estimate.position_body.x_m,
        vision_position_estimate.position_body.y_m,
        vision_position_estimate.position_body.z_m,
        vision_position_estimate.angle_body.roll_rad,
        vision_position_estimate.angle_body.pitch_rad,
        vision_position_estimate.angle_body.yaw_rad,
        covariance.data(),
        0); // FIXME: reset_counter not set

    return _system_impl->send_message(message) ? Mocap::Result::Success :
                                                 Mocap::Result::ConnectionError;
}

Mocap::Result
MocapImpl::send_attitude_position_mocap(const Mocap::AttitudePositionMocap& attitude_position_mocap)
{
    const uint64_t autopilot_time_usec =
        (!attitude_position_mocap.time_usec) ?
            std::chrono::duration_cast<std::chrono::microseconds>(
                _system_impl->get_autopilot_time().now().time_since_epoch())
                .count() :
            std::chrono::duration_cast<std::chrono::microseconds>(
                _system_impl->get_autopilot_time()
                    .time_in(SystemTimePoint(
                        std::chrono::microseconds(attitude_position_mocap.time_usec)))
                    .time_since_epoch())
                .count();

    mavlink_message_t message;

    std::array<float, 4> q{};
    q[0] = attitude_position_mocap.q.w;
    q[1] = attitude_position_mocap.q.x;
    q[2] = attitude_position_mocap.q.y;
    q[3] = attitude_position_mocap.q.z;

    std::array<float, 21> covariance{};

    // The covariance matrix needs to have length 21 or 1 with the one entry set to NaN.

    if (attitude_position_mocap.pose_covariance.covariance_matrix.size() == 21) {
        std::copy(
            attitude_position_mocap.pose_covariance.covariance_matrix.begin(),
            attitude_position_mocap.pose_covariance.covariance_matrix.end(),
            covariance.begin());
    } else if (
        attitude_position_mocap.pose_covariance.covariance_matrix.size() == 1 &&
        std::isnan(attitude_position_mocap.pose_covariance.covariance_matrix[0])) {
        covariance[0] = NAN;
    } else {
        return Mocap::Result::InvalidRequestData;
    }

    mavlink_msg_att_pos_mocap_pack(
        _system_impl->get_own_system_id(),
        _system_impl->get_own_component_id(),
        &message,
        autopilot_time_usec,
        q.data(),
        attitude_position_mocap.position_body.x_m,
        attitude_position_mocap.position_body.y_m,
        attitude_position_mocap.position_body.z_m,
        covariance.data());

    return _system_impl->send_message(message) ? Mocap::Result::Success :
                                                 Mocap::Result::ConnectionError;
}

Mocap::Result MocapImpl::send_odometry(const Mocap::Odometry& odometry)
{
    const uint64_t autopilot_time_usec =
        (!odometry.time_usec) ?
            std::chrono::duration_cast<std::chrono::microseconds>(
                _system_impl->get_autopilot_time().now().time_since_epoch())
                .count() :
            std::chrono::duration_cast<std::chrono::microseconds>(
                _system_impl->get_autopilot_time()
                    .time_in(SystemTimePoint(std::chrono::microseconds(odometry.time_usec)))
                    .time_since_epoch())
                .count();

    mavlink_message_t message;

    std::array<float, 4> q{};
    q[0] = odometry.q.w;
    q[1] = odometry.q.x;
    q[2] = odometry.q.y;
    q[3] = odometry.q.z;

    std::array<float, 21> pose_covariance{};
    std::array<float, 21> velocity_covariance{};

    // The covariance matrix needs to have length 21 or 1 with the one entry set to NaN.

    if (odometry.pose_covariance.covariance_matrix.size() == 21) {
        std::copy(
            odometry.pose_covariance.covariance_matrix.begin(),
            odometry.pose_covariance.covariance_matrix.end(),
            pose_covariance.begin());
    } else if (
        odometry.pose_covariance.covariance_matrix.size() == 1 &&
        std::isnan(odometry.pose_covariance.covariance_matrix[0])) {
        pose_covariance[0] = NAN;
    } else {
        return Mocap::Result::InvalidRequestData;
    }

    if (odometry.velocity_covariance.covariance_matrix.size() == 21) {
        std::copy(
            odometry.velocity_covariance.covariance_matrix.begin(),
            odometry.velocity_covariance.covariance_matrix.end(),
            velocity_covariance.begin());
    } else if (
        odometry.velocity_covariance.covariance_matrix.size() == 1 &&
        std::isnan(odometry.velocity_covariance.covariance_matrix[0])) {
        velocity_covariance[0] = NAN;
    } else {
        return Mocap::Result::InvalidRequestData;
    }

    mavlink_msg_odometry_pack(
        _system_impl->get_own_system_id(),
        _system_impl->get_own_component_id(),
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
        pose_covariance.data(),
        velocity_covariance.data(),
        0,
        MAV_ESTIMATOR_TYPE_MOCAP,
        0);

    return _system_impl->send_message(message) ? Mocap::Result::Success :
                                                 Mocap::Result::ConnectionError;
}

} // namespace mavsdk
