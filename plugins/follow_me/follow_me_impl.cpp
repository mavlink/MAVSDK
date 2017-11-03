#include "follow_me_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <functional>
#include <cmath>

namespace dronecore {

FollowMeImpl::FollowMeImpl() :
    PluginImplBase()
{
}

FollowMeImpl::~FollowMeImpl()
{
}

void FollowMeImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&FollowMeImpl::process_heartbeat, this, _1), static_cast<void *>(this));
}

void FollowMeImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void FollowMeImpl::timeout_occurred()
{
#ifdef FOLLOW_ME_TESTING
    // FIXME: We're arbitarily changing latitude, longitude.
    // Need to make it Box or Circle for better visual demo
    double lat_offset = 0.00001904 * 1e7;
    double lon_offset = 0.000000954 * 1e7;
    // update current location coordinates
    _motion_report.lat_int -= static_cast<int32_t>(lat_offset);
    _motion_report.lon_int -= static_cast<int32_t>(lon_offset);

    _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::POS));
#else
    // Ideally GPS position should come from a platform-specific framwork.
#endif

#ifdef FOLLOW_ME_TESTING
    // update current eph & epv
    _motion_report.pos_std_dev[0] += _motion_report.pos_std_dev[1] += 0.05f;
    _motion_report.pos_std_dev[2] += 0.05f;

    // calculate z velocity if it's available
    _motion_report.vz += 0.5f;

    // calculate x,y velocity if it's
    _motion_report.vx += 0.05f;
    _motion_report.vy += 0.04f;

    _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::VEL));
#else
    // Ideally GPS info such as eph, epv, velocity should come from a platform-specific framwork.
#endif

    send_gcs_motion_report();
}

/**
 * @brief FollowMeImpl::start
 * @return
 */
FollowMe::Result FollowMeImpl::start()
{
    // FIXME: Will be replaced by CallEveryHandler class.
    _parent->register_timeout_handler(std::bind(&FollowMeImpl::timeout_occurred, this), 1.0,
                                      &_timeout_cookie);
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;

    return to_follow_me_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_MODE,
                   MavlinkCommands::Params {float(mode),
                                            float(custom_mode),
                                            float(custom_sub_mode),
                                            NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

/**
 * @brief FollowMeImpl::send_gcs_motion_report
 */
void FollowMeImpl::send_gcs_motion_report()
{
    dl_time_t now = steady_time();
    auto elapsed_sec = elapsed_since_s(now);

    // needed by http://mavlink.org/messages/common#FOLLOW_TARGET
    const float vel[3] = { _motion_report.vx, _motion_report.vy, NAN };
    const float accel_unknown[3] = { NAN, NAN, NAN };
    const float attitude_q_unknown[4] = { 1.f, NAN, NAN, NAN };
    const float rates_unknown[3] = { NAN, NAN, NAN };
    uint64_t custom_state = 0;

    mavlink_message_t msg {};
    mavlink_msg_follow_target_pack(_parent->get_own_system_id(),
                                   _parent->get_own_component_id(),
                                   &msg,
                                   elapsed_sec * 1000, /* in milliseconds */
                                   _estimatation_capabilities,
                                   _motion_report.lat_int,
                                   _motion_report.lon_int,
                                   _motion_report.alt,
                                   vel,
                                   accel_unknown,
                                   attitude_q_unknown,
                                   rates_unknown,
                                   _motion_report.pos_std_dev.data(),
                                   custom_state);

    if (_parent->send_message(msg)) {
        LogDebug() << "Sent FollowTarget to Device";
    } else {
        LogErr() << "Failed to send FollowTarget..";
    }
    // Register timer again for emitting motion reports periodically
    _parent->register_timeout_handler(std::bind(&FollowMeImpl::timeout_occurred, this), 1.0,
                                      &_timeout_cookie);
}

FollowMe::Result FollowMeImpl::stop() const
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    return to_follow_me_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_MODE,
                   MavlinkCommands::Params {float(mode),
                                            float(custom_mode),
                                            float(custom_sub_mode),
                                            NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

FollowMe::Result
FollowMeImpl::to_follow_me_result(MavlinkCommands::Result result) const
{
    switch (result) {
        case MavlinkCommands::Result::SUCCESS:
            return FollowMe::Result::SUCCESS;
        case MavlinkCommands::Result::NO_DEVICE:
            return FollowMe::Result::NO_DEVICE;
        case MavlinkCommands::Result::CONNECTION_ERROR:
            return FollowMe::Result::CONNECTION_ERROR;
        case MavlinkCommands::Result::BUSY:
            return FollowMe::Result::BUSY;
        case MavlinkCommands::Result::COMMAND_DENIED:
            return FollowMe::Result::COMMAND_DENIED;
        case MavlinkCommands::Result::TIMEOUT:
            return FollowMe::Result::TIMEOUT;
        default:
            return FollowMe::Result::UNKNOWN;
    }
}

void FollowMeImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    bool followme_mode_active = false;
    if (heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {

        px4::px4_custom_mode px4_custom_mode;
        px4_custom_mode.data = heartbeat.custom_mode;

        if (px4_custom_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_AUTO &&
            px4_custom_mode.sub_mode == px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET) {
            followme_mode_active = true;
            LogDebug() << "FollowMe mode!!";
        }
    }
    _followme_mode_active = followme_mode_active;
}

} // namespace dronecore
