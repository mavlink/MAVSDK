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
    _start = std::chrono::system_clock::now();

    _estimatation_capabilities = 0;

    // initialize current location coordinates
    _motion_report.lat_int = 47.3977418 * 1e7;
    _motion_report.lon_int = 8.5455938 * 1e7;
    _motion_report.alt = 500.04;

    // initialize current eph & epv
    _motion_report.pos_std_dev[0] = _motion_report.pos_std_dev[1] = 0.8f;
    _motion_report.pos_std_dev[2] = 0.0;

    // initialize z velocity as if it's available
    _motion_report.vz = 0.0;

    // initialize x,y velocity as if it's available
    auto direction = 0.13; // unit: Knot
    auto velocity = 5.0; // unit: m/s
    _motion_report.vx = cos(direction) * velocity;
    _motion_report.vy = sin(direction) * velocity;
}

FollowMeImpl::~FollowMeImpl()
{
}

void FollowMeImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&FollowMeImpl::process_heartbeat, this, _1), (void *)this);

}

void FollowMeImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void FollowMeImpl::timeout_occurred()
{
    // FIXME: This is HARD-coded as of now.
    // Ideally GPS info should come from a platform-specific framwork.

    // update current location coordinates
    _motion_report.lat_int += 10;
    _motion_report.lon_int += 5;
    _motion_report.alt += 10.0;

    _estimatation_capabilities |= (1 << static_cast<int>(FollowMe::ESTCapabilities::POS));

    // update current eph & epv
    _motion_report.pos_std_dev[0] += _motion_report.pos_std_dev[1] = 0.05f;
    _motion_report.pos_std_dev[2] = 0.05f;

    // calculate z velocity if it's available
    _motion_report.vz += 0.5f;

    // calculate x,y velocity if it's
    _motion_report.vx += 0.05;
    _motion_report.vy += 0.04;

    _estimatation_capabilities |= (1 << static_cast<int>(FollowMe::ESTCapabilities::VEL));

    send_gcs_motion_report();
}

FollowMe::Result FollowMeImpl::start()
{
    _parent->register_timeout_handler(std::bind(&FollowMeImpl::timeout_occurred, this), 1.0,
                                      &_timeout_cookie);
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;

    return get_result_from_mavcmd_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_MODE,
                   MavlinkCommands::Params {float(mode),
                                            float(custom_mode),
                                            float(custom_sub_mode),
                                            NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

void FollowMeImpl::send_gcs_motion_report()
{
    using namespace std::chrono;

    auto end = system_clock::now();
    auto elapsed_seconds = system_clock::to_time_t(end) - system_clock::to_time_t(_start);

    mavlink_message_t msg {};
    float vel[3] = { _motion_report.vx, _motion_report.vy, 0.f };
    float accel_unknown[3] = { 0, 0, 0 };
    float attitude_q_unknown[4] = { 1, 0, 0, 0 };
    float rates_unknown[3] = { 0, 0, 0 };

    mavlink_msg_follow_target_pack(_parent->get_own_system_id(),
                                   _parent->get_own_component_id(),
                                   &msg,
                                   elapsed_seconds * 1e-6,
                                   _estimatation_capabilities,
                                   _motion_report.lat_int,
                                   _motion_report.lon_int,
                                   _motion_report.alt,
                                   vel,
                                   accel_unknown,
                                   attitude_q_unknown,
                                   rates_unknown,
                                   _motion_report.pos_std_dev,
                                   0);

    std::cout << __func__ <<
              ((_parent->send_message(msg)) ? ("--------------------> sent follow_target") : ("############### Failed to send follow_target"))
              <<
              std::endl;
    // Register timer again for emitting motion reports periodically
    _parent->register_timeout_handler(std::bind(&FollowMeImpl::timeout_occurred, this), 1.0,
                                      &_timeout_cookie);
}

FollowMe::Result FollowMeImpl::stop()
{
    return FollowMe::Result::SUCCESS;
}

FollowMe::Result
FollowMeImpl::get_result_from_mavcmd_result(MavlinkCommands::Result result) const
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
            std::cout << "Hurray! Flight is in FollowMe mode!!" << std::endl;
        }
    }
    _followme_mode_active = followme_mode_active;
}




} // namespace dronecore
