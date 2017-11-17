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
    _follow_info{},
    _follow_info_timer(_time),
    _estimatation_capabilities(0),
    _config{},
    _followme_mode_active(false)
{
    // We can intialize follow info with Motion info provided by Android or iOS frameworks
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

#ifdef FOLLOW_ME_TESTING
void FollowMeImpl::update_fake_follow_info()
{
    // This is very hard-coded co-ordinates of the Ground Control station
    // for the Vechicle to follow.
    static struct SpiralPoint {
        double lat, lon;
    } spiral_points[] = {
        { 47.39779928779934, 8.54559461331354 },
        {         47.39780291, 8.54557048 },
        {        47.39779838, 8.5455517 },
        {       47.39778748, 8.54554499 },
        {      47.39777659, 8.54553561 },
        {     47.39776569, 8.54553292 },
        {    47.39774663, 8.54552622 },
        {   47.39772938, 8.54552488 },
        {         47.39771304, 8.54554231 },
        {        47.39770578, 8.5455745 },
        {       47.39770487, 8.54559596 },
        {      47.39770578, 8.54561741 },
        {     47.39770669, 8.54563887 },
        {    47.39771486, 8.54565765 },
        {   47.39773029, 8.54567642 },
        {  47.39775026, 8.54568447 },
        {         47.39776751, 8.54569118 },
        {        47.39778203, 8.54569118 },
        {       47.39779838, 8.54568447 },
        {      47.39781835, 8.54566972 },
        {     47.39782107, 8.54564692 },
        {    47.3978247, 8.54561876 },
        {   47.3978247, 8.54559193 },
        {  47.3978247, 8.54556511 },
        {         47.39782107, 8.54553427 },
        {        47.39780836, 8.54552756 },
        {       47.39779656, 8.54551549 },
        {      47.39777568, 8.54550342 },
        {     47.3977548, 8.54549671 },
        {    47.39773755, 8.54549671 },
        {   47.39771849, 8.54550208 },
        {         47.39770396, 8.54551415 },
        {        47.39769398, 8.54554097 },
        {       47.39768762, 8.54556243 },
        {      47.39768672, 8.54557852 },
        {     47.3976849, 8.54559998 },
        {    47.39768399, 8.54562278 },
        {   47.39768399, 8.54564155 },
        {  47.39769035, 8.54566569 },
        {         47.39770759, 8.54568983 },
        {        47.39772757, 8.54569922  },
        {       47.39774481, 8.54570727 },
        {      47.39776025, 8.54572202 },
        {     47.39778567, 8.54572336 },
        {    47.39780291, 8.54572202 },
        {   47.39782107, 8.54571263 },
        {  47.39783469, 8.54569788 },
        {         47.39783832, 8.54568179 },
        {        47.39784104, 8.54566569 },
        {       47.39784376, 8.54564424 },
    };

    // update current location coordinates
    _follow_info.lat = spiral_points[_spiral_idx].lat;
    _follow_info.lon = spiral_points[_spiral_idx].lon;
    // LogInfo() << "+ Lat: " << _follow_info.lat << ", Long: " << _follow_info.lon;
    _spiral_idx++;
    _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::POS));

    // update current eph & epv
    _follow_info.pos_std_dev[0] += 0.57f;
    _follow_info.pos_std_dev[1] += 0.57f;
    _follow_info.pos_std_dev[2] += 0.50f;

    _follow_info.vx_ms += 0.05f;
    _follow_info.vy_ms += 0.14f;
    _follow_info.vz_ms += 0.8f;
    _estimatation_capabilities |= (1 << static_cast<int>(ESTCapabilities::VEL));
}
#endif

void FollowMeImpl::update_follow_info()
{
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    // Ideally GPS position, eph, epv, velocity is provided by
    // a platform-specific framwork.
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
}

void FollowMeImpl::follow_info_handler()
{
#ifdef FOLLOW_ME_TESTING
    update_fake_follow_info();
#endif
    update_follow_info();
    send_follow_info();
}

const FollowMe::Config &FollowMeImpl::get_config() const
{
    return _config;
}

bool FollowMeImpl::set_config(const FollowMe::Config &config)
{
    auto height = config.min_height_m;
    auto distance = config.follow_dist_m;
    int32_t direction = static_cast<int32_t>(config.follow_dir);
    auto responsiveness = config.responsiveness;

    // Valdidate configuration
    if (!is_config_ok(config)) {
        LogErr() << "set_config() failed. Last configuration is preserved.";
        return false;
    }

    // Send configuration to Vehicle
    if (height != FollowMe::Config::DEF_HEIGHT_M)
        _parent->set_param_float_async("NAV_MIN_FT_HT", height,
                                       std::bind(&FollowMeImpl::rcv_param_min_height,
                                                 this, _1, height));
    if (distance != FollowMe::Config::DEF_FOLLOW_DIST_M)
        _parent->set_param_float_async("NAV_FT_DST", distance,
                                       std::bind(&FollowMeImpl::rcv_param_follow_distance,
                                                 this, _1, distance));
    if (config.follow_dir != FollowMe::Config::DEF_FOLLOW_DIR)
        _parent->set_param_int_async("NAV_FT_FS", direction,
                                     std::bind(&FollowMeImpl::rcv_param_follow_direction,
                                               this, _1, direction));
    if (responsiveness != FollowMe::Config::DEF_RESPONSIVENSS)
        _parent->set_param_float_async("NAV_FT_RS", responsiveness,
                                       std::bind(&FollowMeImpl::rcv_param_responsiveness,
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

const FollowMe::FollowInfo &FollowMeImpl::get_follow_info() const
{
    return _follow_info;
}

void FollowMeImpl::set_follow_info(const FollowMe::FollowInfo &info)
{
    _follow_info = info;
}

void FollowMeImpl::rcv_param_min_height(bool success, float min_height_m)
{
    if (success) {
        LogInfo() << "NAV_MIN_FT_HT: curr: " << _config.min_height_m << "m -> new: " << min_height_m << "m";
        _config.min_height_m = min_height_m;
    } else {
        LogErr() << "Failed to set NAV_MIN_FT_HT: " << min_height_m << "m";
    }
}

void FollowMeImpl::rcv_param_follow_distance(bool success, float follow_dist_m)
{
    if (success) {
        LogInfo() << "NAV_FT_DST: curr: " << _config.follow_dist_m << "m -> new: " << follow_dist_m << "m";
        _config.follow_dist_m = follow_dist_m;
    } else {
        LogErr() << "Failed to set NAV_FT_DST: " << follow_dist_m << "m";
    }
}

void FollowMeImpl::rcv_param_follow_direction(bool success, int32_t dir)
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
        LogInfo() << "NAV_FT_FS: curr: " << curr_dir_str << " -> new: " << new_dir_str;
        if (new_dir != FollowMe::Config::FollowDirection::NONE) {
            _config.follow_dir = new_dir;
        }
    } else {
        LogErr() << "Failed to set NAV_FT_FS: " <<  FollowMe::Config::to_str(new_dir);
    }
}

void FollowMeImpl::rcv_param_responsiveness(bool success, float responsiveness)
{
    if (success) {
        LogInfo() << "NAV_FT_RS: curr: " << _config.responsiveness << " -> new: " << responsiveness;
        _config.responsiveness = responsiveness;
    } else {
        LogErr() << "Failed to set NAV_FT_RS: " << responsiveness;
    }
}

FollowMe::Result FollowMeImpl::start()
{
#ifdef FOLLOW_ME_TESTING
    _spiral_idx = 0;
#endif
    _parent->add_call_every(std::bind(&FollowMeImpl::follow_info_handler, this), 1.0f, &_ce_cookie);
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

void FollowMeImpl::send_follow_info()
{
    dl_time_t now = _time.steady_time();

    // needed by http://mavlink.org/messages/common#FOLLOW_TARGET
    uint64_t elapsed_msec = static_cast<uint64_t>(_time.elapsed_since_s(now) * 1000); // milliseconds
    const int32_t lat_int = static_cast<int32_t>(_follow_info.lat * 1e7);
    const int32_t lon_int = static_cast<int32_t>(_follow_info.lon * 1e7);
    const float alt = static_cast<float>(_follow_info.alt);
    const float vel[3] = { _follow_info.vx_ms, _follow_info.vy_ms, NAN };
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
                                   _follow_info.pos_std_dev,
                                   custom_state);

    if (!_parent->send_message(msg)) {
        LogErr() << "send_follow_info() failed..";
    }
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
            LogDebug() << "FollowMe mode!!";
        }
    }
    _followme_mode_active = followme_mode_active;
}

} // namespace dronecore
