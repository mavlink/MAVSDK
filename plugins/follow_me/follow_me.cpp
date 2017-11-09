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
