#include "follow_me.h"
#include "follow_me_impl.h"

namespace dronecore {

FollowMe::FollowMe(System &system) :
    PluginBase(),
    _impl { new FollowMeImpl(system) }
{
}

FollowMe::~FollowMe()
{
}

const FollowMe::Config &FollowMe::get_config() const
{
    return _impl->get_config();
}

FollowMe::Result FollowMe::set_config(const FollowMe::Config &config)
{
    return _impl->set_config(config);
}

bool FollowMe::is_active() const
{
    return _impl->is_active();
}

void FollowMe::set_target_location(const TargetLocation &target_location)
{
    return _impl->set_target_location(target_location);
}

const FollowMe::TargetLocation &FollowMe::get_last_location() const
{
    return _impl->get_last_location();
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
        case Result::NO_SYSTEM:
            return "No system";
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
        case Result::SET_CONFIG_FAILED:
            return "Failed to set configuration";
        case Result::UNKNOWN:
        // FALLTHROUGH
        default:
            return "Unknown";
    }
}

} // namespace dronecore
