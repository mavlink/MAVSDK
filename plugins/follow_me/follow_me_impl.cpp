#include "follow_me_impl.h"
#include "device_impl.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <functional>
#include <cmath>

namespace dronecore {

using namespace std::placeholders; // for `_1`

FollowMeImpl::FollowMeImpl() :
    PluginImplBase(),
    _time{},
    _ce_cookie(nullptr),
    _follow_target_info{},
    _follow_target_info_timer(_time),
    _estimatation_capabilities(0),
    _config{},
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
    load_device_config();
}

void FollowMeImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

// Loads current FollowMe configuration of the device.
void FollowMeImpl::load_device_config()
{
    LogInfo() << "Loading current FollowMe configuration from device...";
    _parent->get_param_float_async("NAV_MIN_FT_HT", std::bind(&FollowMeImpl::receive_param_min_height,
                                                              this,
                                                              _1, _2));
    _parent->get_param_float_async("NAV_FT_DST", std::bind(&FollowMeImpl::receive_param_follow_distance,
                                                           this, _1, _2));
    _parent->get_param_int_async("NAV_FT_FS", std::bind(&FollowMeImpl::receive_param_follow_direction,
                                                        this,
                                                        _1, _2));
    _parent->get_param_float_async("NAV_FT_RS", std::bind(&FollowMeImpl::receive_param_responsiveness,
                                                          this,
                                                          _1, _2));
}

void FollowMeImpl::platform_follow_target_info_provider(FollowMe::FollowTargetInfo &)
{
    // Fill Follow target info obtained from platform-specific Location framework.
}

void FollowMeImpl::reset_follow_target_info()
{
    _follow_target_info = FollowMe::FollowTargetInfo();
}

void FollowMeImpl::update_follow_target_info()
{
    // Request the callback to fill follow target info, if registered by application
    if (_get_follow_target_info_cb) {
        _get_follow_target_info_cb(_follow_target_info);
    } else { // if callback is not registered, request platform to fill.
        platform_follow_target_info_provider(_follow_target_info);
    }

    // for eph, epv
    if (_follow_target_info.lat || _follow_target_info.lon || _follow_target_info.alt) {
        _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::POS));
    }
    // for x, y velocity
    if (_follow_target_info.vx_ms || _follow_target_info.vy_ms || _follow_target_info.vz_ms) {
        _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::VEL));
    }
}

void FollowMeImpl::follow_target_info_handler()
{
    update_follow_target_info();
    send_follow_target_info();
    reset_follow_target_info();
}

void FollowMeImpl::send_follow_target_info()
{
    dl_time_t now = _time.steady_time();
    // needed by http://mavlink.org/messages/common#FOLLOW_TARGET
    uint64_t elapsed_msec = static_cast<uint64_t>(_time.elapsed_since_s(now) * 1000); // milliseconds
    const int32_t lat_int = static_cast<int32_t>(_follow_target_info.lat * 1e7);
    const int32_t lon_int = static_cast<int32_t>(_follow_target_info.lon * 1e7);
    const float alt = static_cast<float>(_follow_target_info.alt);
    const float vel[3] = { _follow_target_info.vx_ms, _follow_target_info.vy_ms, NAN };
    const float accel_unknown[3] = { NAN, NAN, NAN };
    const float attitude_q_unknown[4] = { 1.f, NAN, NAN, NAN };
    const float rates_unknown[3] = { NAN, NAN, NAN };
    uint64_t custom_state = 0;

    mavlink_message_t msg {};
    mavlink_msg_follow_target_pack(_parent->get_own_system_id(),
                                   _parent->get_own_component_id(),
                                   &msg,
                                   elapsed_msec,
                                   _estimatation_capabilities,
                                   lat_int,
                                   lon_int,
                                   alt,
                                   vel,
                                   accel_unknown,
                                   attitude_q_unknown,
                                   rates_unknown,
                                   _follow_target_info.pos_std_dev,
                                   custom_state);

    if (!_parent->send_message(msg)) {
        LogErr() << "send_follow_target_info() failed..";
    }
}

const FollowMe::Config &FollowMeImpl::get_config() const
{
    return _config;
}

bool FollowMeImpl::set_config(const FollowMe::Config &config)
{
    // Valdidate configuration
    if (!is_config_ok(config)) {
        LogErr() << "set_config() failed. Last configuration is preserved.";
        return false;
    }

    auto height = config.min_height_m;
    auto distance = config.follow_dist_m;
    int32_t direction = static_cast<int32_t>(config.follow_dir);
    auto responsiveness = config.responsiveness;

    // Send configuration to Vehicle
    if (height != FollowMe::Config::DEF_HEIGHT_M)
        _parent->set_param_float_async("NAV_MIN_FT_HT", height,
                                       std::bind(&FollowMeImpl::receive_param_min_height,
                                                 this, _1, height));
    if (distance != FollowMe::Config::DEF_FOLLOW_DIST_M)
        _parent->set_param_float_async("NAV_FT_DST", distance,
                                       std::bind(&FollowMeImpl::receive_param_follow_distance,
                                                 this, _1, distance));
    if (config.follow_dir != FollowMe::Config::DEF_FOLLOW_DIR)
        _parent->set_param_int_async("NAV_FT_FS", direction,
                                     std::bind(&FollowMeImpl::receive_param_follow_direction,
                                               this, _1, direction));
    if (responsiveness != FollowMe::Config::DEF_RESPONSIVENSS)
        _parent->set_param_float_async("NAV_FT_RS", responsiveness,
                                       std::bind(&FollowMeImpl::receive_param_responsiveness,
                                                 this, _1, responsiveness));

    // FIXME: We've sent valid configuration to Vehicle.
    // But that doesn't mean configuration is applied, untill we receive confirmation.
    // For now we're hoping that it is applied successfully.
    return true;
}

bool FollowMeImpl::is_config_ok(const FollowMe::Config &config) const
{
    auto config_ok = false;

    if (config.min_height_m < FollowMe::Config::MIN_HEIGHT_M) {
        LogErr() << "Err: Min height must be atleast 8.0 meters";
    } else if (config.follow_dist_m < FollowMe::Config::MIN_FOLLOW_DIST_M) {
        LogErr() << "Err: Min Follow distance must be atleast 1.0 meter";
    } else if (config.follow_dir < FollowMe::Config::FollowDirection::FRONT_RIGHT ||
               config.follow_dir > FollowMe::Config::FollowDirection::NONE) {
        LogErr() << "Err: Invalid Follow direction";
    } else if (config.responsiveness < FollowMe::Config::MIN_RESPONSIVENESS ||
               config.responsiveness > FollowMe::Config::MAX_RESPONSIVENESS) {
        LogErr() << "Err: Responsiveness must be in range (0.0 to 1.0)";
    } else { // Config is OK
        config_ok = true;
    }

    return config_ok;
}

void FollowMeImpl::receive_param_min_height(bool success, float min_height_m)
{
    if (success) {
        LogInfo() << "NAV_MIN_FT_HT: " << min_height_m << "m";
        _config.min_height_m = min_height_m;
    } else {
        LogErr() << "Failed to set NAV_MIN_FT_HT: " << min_height_m << "m";
    }
}

void FollowMeImpl::receive_param_follow_distance(bool success, float follow_dist_m)
{
    if (success) {
        LogInfo() << "NAV_FT_DST: " << follow_dist_m << "m";
        _config.follow_dist_m = follow_dist_m;
    } else {
        LogErr() << "Failed to set NAV_FT_DST: " << follow_dist_m << "m";
    }
}

void FollowMeImpl::receive_param_follow_direction(bool success, int32_t dir)
{
    auto new_dir = FollowMe::Config::FollowDirection::NONE;
    switch (dir) {
        case 0: new_dir = FollowMe::Config::FollowDirection::FRONT_RIGHT; break;
        case 1: new_dir = FollowMe::Config::FollowDirection::BEHIND; break;
        case 2: new_dir = FollowMe::Config::FollowDirection::FRONT; break;
        case 3: new_dir = FollowMe::Config::FollowDirection::FRONT_LEFT; break;
        default: break;
    }
    auto curr_dir_str = FollowMe::Config::to_str(_config.follow_dir);
    auto new_dir_str = FollowMe::Config::to_str(new_dir);
    if (success) {
        LogInfo() << "NAV_FT_FS: " << new_dir_str;
        if (new_dir != FollowMe::Config::FollowDirection::NONE) {
            _config.follow_dir = new_dir;
        }
    } else {
        LogErr() << "Failed to set NAV_FT_FS: " <<  FollowMe::Config::to_str(new_dir);
    }
}

void FollowMeImpl::receive_param_responsiveness(bool success, float responsiveness)
{
    if (success) {
        LogInfo() << "NAV_FT_RS: " << responsiveness;
        _config.responsiveness = responsiveness;
    } else {
        LogErr() << "Failed to set NAV_FT_RS: " << responsiveness;
    }
}

void FollowMeImpl::register_follow_target_info_callback(FollowMe::follow_target_info_callback_t cb)
{
    _get_follow_target_info_cb = cb;
}

FollowMe::Result FollowMeImpl::start()
{
    _parent->add_call_every(std::bind(&FollowMeImpl::follow_target_info_handler, this), 1.0f,
                            &_ce_cookie);
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

FollowMe::Result FollowMeImpl::stop()
{
    _parent->remove_call_every(_ce_cookie);

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
        }
    }
    _followme_mode_active = followme_mode_active;
}

} // namespace dronecore
