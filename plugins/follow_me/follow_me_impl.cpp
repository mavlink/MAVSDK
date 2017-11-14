#include "follow_me_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <functional>
#include <cmath>
#include <array>

namespace dronecore {

using namespace std::placeholders; // for `_1`

FollowMeImpl::FollowMeImpl() :
    _time(),
    _timeout_cookie(nullptr),
    _motion_report(),
    _motion_report_timer(_time),
    _estimatation_capabilities(0),
    _config(),
    _followme_mode_active(false)
{
}

FollowMeImpl::~FollowMeImpl()
{
}

void FollowMeImpl::init()
{
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
    static int alternative = 1;
    // TODO: We're arbitarily changing latitude, longitude.
    // Need to make it Box or Circle for better visual demo
    const double lat_offset = 0.00014904 * 1e7;
    const double lon_offset = 0.000000954 * 1e7;

    // update current location coordinates
    // Using trivial logic to make drone oscillate
    if (alternative++ % 2 == 0) {
        _motion_report.lat_int += static_cast<int32_t>(lat_offset);
        _motion_report.lon_int += static_cast<int32_t>(lon_offset);
        // LogInfo() << "+ Lat: " << _motion_report.lat_int << ", Long: " << _motion_report.lon_int;
    } else {
        _motion_report.lat_int -= static_cast<int32_t>(lat_offset);
        _motion_report.lon_int -= static_cast<int32_t>(lon_offset);
        // LogInfo() << "- Lat: " << _motion_report.lat_int << ", Long: " << _motion_report.lon_int;
    }

    _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::POS));

    // update current eph & epv
    _motion_report.pos_std_dev[0] += 0.05f;
    _motion_report.pos_std_dev[1] += 0.05f;
    _motion_report.pos_std_dev[2] += 0.05f;

    // calculate z velocity if it's available
    _motion_report.vz += 0.5f;

    // calculate x,y velocity if it's
    _motion_report.vx += 0.05f;
    _motion_report.vy += 0.04f;

    _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::VEL));
#else
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // Ideally GPS position, eph, epv, velocity is provided by
    // a platform-specific framwork.
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
#endif

    // send to GCS position details to Vehicle
    send_gcs_motion_report();
}

FollowMe::Configuration FollowMeImpl::get_config() const
{
    return _config;
}

void FollowMeImpl::receive_param_min_height(bool success, float min_height_m)
{
    if (success) {
        LogInfo() << "NAV_FT_MIN_HT: " << min_height_m;
        _config.set_min_height_m(min_height_m);
    }
}

void FollowMeImpl::receive_param_follow_target_dist(bool success, float ft_dist_m)
{
    if (success) {
        LogInfo() << "NAV_FT_DST: " << ft_dist_m;
        _config.set_follow_target_dist_m(ft_dist_m);
    }
}

void FollowMeImpl::receive_param_follow_dir(bool success, int32_t dir)
{
    if (success) {
        LogInfo() << "NAV_FT_FS: " << dir;
        FollowMe::Configuration::FollowDirection d = FollowMe::Configuration::FollowDirection::NONE;
        switch (dir) {
            case 0: d = FollowMe::Configuration::FollowDirection::FRONT_RIGHT; break;
            case 1: d = FollowMe::Configuration::FollowDirection::BEHIND; break;
            case 2: d = FollowMe::Configuration::FollowDirection::FRONT; break;
            case 3: d = FollowMe::Configuration::FollowDirection::FRONT_LEFT; break;
            default: break;
        }
        if (d != FollowMe::Configuration::FollowDirection::NONE) {
            _config.set_follow_dir(d);
        }
    }
}

void FollowMeImpl::receive_param_dyn_fltr_alg_rsp(bool success, float rsp)
{
    if (success) {
        LogInfo() << "NAV_FT_RS: " << rsp;
        _config.set_dynamic_filter_algo_rsp_val(rsp);
    }
}

void FollowMeImpl::set_config(const FollowMe::Configuration &cfg)
{
    auto height = cfg.min_height_m();
    auto dist = cfg.follow_target_dist_m();
    int32_t dir = static_cast<int32_t>(cfg.follow_dir());
    auto rsp = cfg.dynamic_filter_alg_responsiveness();

    LogInfo() << "Going to set min height " << height << " mts";
    if (cfg.min_height_m() != FollowMe::Configuration::DEF_DIST_WRT_FT)
        _parent->set_param_float_async("NAV_FT_MIN_HT", height,
                                       std::bind(&FollowMeImpl::receive_param_min_height,
                                                 this, _1, height));

    if (cfg.follow_target_dist_m() != FollowMe::Configuration::DEF_DIST_WRT_FT)
        _parent->set_param_float_async("NAV_FT_DST", dist,
                                       std::bind(&FollowMeImpl::receive_param_follow_target_dist,
                                                 this, _1, dist));
    if (cfg.follow_dir() != FollowMe::Configuration::DEF_FOLLOW_DIR)
        _parent->set_param_int_async("NAV_FT_FS", dir,
                                     std::bind(&FollowMeImpl::receive_param_follow_dir,
                                               this, _1, dir));
    if (cfg.dynamic_filter_alg_responsiveness() != FollowMe::Configuration::DEF_DYN_FLT_ALG_RSP)
        _parent->set_param_float_async("NAV_FT_RS", rsp,
                                       std::bind(&FollowMeImpl::receive_param_dyn_fltr_alg_rsp,
                                                 this, _1, rsp));
}

/**
 * @brief FollowMeImpl::start
 * @return
 */
FollowMe::Result FollowMeImpl::start()
{
    _parent->add_call_every(std::bind(&FollowMeImpl::timeout_occurred, this), 0.6f, &_timeout_cookie);
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
    dl_time_t now = _time.steady_time();
    auto elapsed_msec = _time.elapsed_since_ms(now);

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
                                   static_cast<uint64_t>(elapsed_msec),
                                   _estimatation_capabilities,
                                   _motion_report.lat_int,
                                   _motion_report.lon_int,
                                   _motion_report.alt,
                                   vel,
                                   accel_unknown,
                                   attitude_q_unknown,
                                   rates_unknown,
                                   _motion_report.pos_std_dev,
                                   custom_state);

    if (_parent->send_message(msg)) {
        LogDebug() << "Sent FollowTarget to Device";
    } else {
        LogErr() << "Failed to send FollowTarget..";
    }
}

FollowMe::Result FollowMeImpl::stop()
{
    _parent->remove_call_every(_timeout_cookie);

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
