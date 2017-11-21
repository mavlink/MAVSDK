#include "follow_me.h"
#include "follow_me_impl.h"

namespace dronecore {

FollowMe::FollowMe(FollowMeImpl *impl) :
    _impl(impl)
{
}

FollowMe::~FollowMe()
{
}

const FollowMe::Config &FollowMe::get_config() const
{
    return _impl->get_config();
}

bool FollowMe::set_config(const FollowMe::Config &config)
{
    return _impl->set_config(config);
}

void FollowMe::register_follow_target_info_callback(FollowMe::follow_target_info_callback_t cb)
{
    _impl->register_follow_target_info_callback(cb);
}

void FollowMe::deregister_follow_target_info_callback()
{
    _impl->deregister_follow_target_info_callback();
}

std::string FollowMe::Config::to_str(FollowMe::Config::FollowDirection dir)
{
    switch (dir) {
        case FollowMe::Config::FollowDirection::FRONT_RIGHT:
            return "FRONT RIGHT";
        case FollowMe::Config::FollowDirection::BEHIND:
            return "BEHIND";
        case FollowMe::Config::FollowDirection::FRONT_LEFT:
            return "FRONT LEFT";
        case FollowMe::Config::FollowDirection::FRONT:
            return "FRONT";
        case FollowMe::Config::FollowDirection::NONE:
            return "NONE";
    }
    return nullptr; // to please compiler not to warn
}

FollowMe::Result FollowMe::start() const
{
    return _impl->start();
}

FollowMe::Result FollowMe::stop() const
{
    return _impl->stop();
}

std::string FollowMe::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::NO_DEVICE:
            return "No device";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::BUSY:
            return "Busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::CALLBACK_NOT_REGISTERED:
            return "Follow target info callback is not registered";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

const float FollowMe::Config::DEF_HEIGHT_M =
    8.0f; /**< @brief Default height (in meters) above home. */
const float FollowMe::Config::DEF_FOLLOW_DIST_M =
    8.0f; /**< @brief Default Follow distance (in meters) to target. */
const FollowMe::Config::FollowDirection FollowMe::Config::DEF_FOLLOW_DIR =
    FollowMe::Config::FollowDirection::BEHIND; /**< @brief Default side to follow target from. */
const float FollowMe::Config::DEF_RESPONSIVENSS =
    0.5f; /**< @brief Default responsiveness to target movement. */

const float FollowMe::Config::MIN_HEIGHT_M =
    8.0f; /**< @brief Min follow target height, in meters. */
const float FollowMe::Config::MIN_FOLLOW_DIST_M =
    1.0f;  /**< @brief Min distance to follow target from, in meters. */
const float FollowMe::Config::MIN_RESPONSIVENESS =
    0.0f; /**< @brief Min responsiveness to target movement. */
const float FollowMe::Config::MAX_RESPONSIVENESS =
    1.0f; /**< @brief Max responsiveness to target movement. */

} // namespace dronecore
