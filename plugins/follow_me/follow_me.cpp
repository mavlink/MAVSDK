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

bool FollowMe::is_active() const
{
    return _impl->is_active();
}

void FollowMe::set_curr_target_location(const TargetLocation &target_location)
{
    return _impl->set_curr_target_location(target_location);
}

void FollowMe::get_last_location(TargetLocation &last_location)
{
    return _impl->get_last_location(last_location);
}

std::string FollowMe::Config::to_str(FollowMe::Config::FollowDirection direction)
{
    switch (direction) {
        case FollowMe::Config::FollowDirection::FRONT_RIGHT:
            return "Front right";
        case FollowMe::Config::FollowDirection::BEHIND:
            return "Behind";
        case FollowMe::Config::FollowDirection::FRONT_LEFT:
            return "Front left";
        case FollowMe::Config::FollowDirection::FRONT:
            return "Front";
        default:
            return "None";
    }
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
        case Result::NOT_ACTIVE:
            return "FollowMe is not active";
        case Result::UNKNOWN:
        // FALLTHROUGH
        default:
            return "Unknown";
    }
}

} // namespace dronecore
