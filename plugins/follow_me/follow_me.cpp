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

FollowMe::Result FollowMe::start(const GCSPosition &gcs_pos)
{
    return _impl->start(gcs_pos);
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
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

#ifdef FOLLOW_ME_TESTING
const double DEF_LAT = 47.3977418; /**< @brief Default latitude of GCS */
const double DEF_LONG = 8.5455938; /**< @brief Default longitude of GCS */
const double DEF_ALT = 488.02; /**< @brief Default altitude of GCS */
#endif

} // namespace dronecore
