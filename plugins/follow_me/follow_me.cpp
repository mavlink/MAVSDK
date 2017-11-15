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

// FollowMe::Configuration methods
float FollowMe::Configuration::min_height_m() const
{
    return _min_height_m;
}

float FollowMe::Configuration::follow_target_dist_m() const
{
    return _follow_target_dist_m;
}

FollowMe::Configuration::FollowDirection FollowMe::Configuration::follow_dir() const
{
    return _follow_dir;
}

float FollowMe::Configuration::responsiveness() const
{
    return _dyn_flt_alg_responsiveness;
}

bool FollowMe::Configuration::set_min_height_m(float mht)
{
    if (mht < MIN_HEIGHT_ABOVE_HOME) {
        return false;
    }
    _min_height_m = mht;
    return true;
}

bool FollowMe::Configuration::set_follow_target_dist_m(float ft_dst)
{
    if (ft_dst < MIN_DIST_WRT_FT) {
        return false;
    }
    _follow_target_dist_m = ft_dst;
    return true;
}

void FollowMe::Configuration::set_follow_dir(FollowMe::Configuration::FollowDirection dir)
{
    _follow_dir = dir;
}

bool FollowMe::Configuration::set_responsiveness(float responsiveness)
{
    if (responsiveness < 0.f || responsiveness > 1.0f) {
        return false;
    }
    _dyn_flt_alg_responsiveness = responsiveness;
    return true;
}

std::string FollowMe::Configuration::to_str(FollowMe::Configuration::FollowDirection dir)
{
    switch (dir) {
        case FollowMe::Configuration::FollowDirection::FRONT_RIGHT:
            return "Front Right";
        case FollowMe::Configuration::FollowDirection::BEHIND:
            return "Behind";
        case FollowMe::Configuration::FollowDirection::FRONT_LEFT:
            return "Front Left";
        case FollowMe::Configuration::FollowDirection::FRONT:
            return "Front";
        case FollowMe::Configuration::FollowDirection::NONE:
            return "None";
    }
    return nullptr;
}

FollowMe::Result FollowMe::start() const
{
    return _impl->start();
}

FollowMe::Result FollowMe::stop() const
{
    return _impl->stop();
}

FollowMe::Configuration FollowMe::get_config() const
{
    return _impl->get_config();
}

void FollowMe::set_config(const FollowMe::Configuration &cfg)
{
    return _impl->set_config(cfg);
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
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecore
