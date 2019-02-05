#include "plugins/mavlink_backdoor/mavlink_backdoor.h"
#include "mavlink_backdoor_impl.h"

namespace dronecode_sdk {

MavlinkBackdoor::MavlinkBackdoor(System &system) :
    PluginBase(),
    _impl{new MavlinkBackdoorImpl(system)}
{}

MavlinkBackdoor::~MavlinkBackdoor() {}

MavlinkBackdoor::Result MavlinkBackdoor::send_message(const mavlink_message_t &message)
{
    return _impl->send_message(message);
}

void MavlinkBackdoor::subscribe_message_async(
    std::function<void(const mavlink_message_t &)> callback)
{
    _impl->subscribe_message_async(callback);
}

std::string MavlinkBackdoor::result_str(Result result)
{
    switch (result) {
        case MavlinkBackdoor::Result::SUCCESS:
            return "Success";
        case MavlinkBackdoor::Result::CONNECTION_ERROR:
            return "Connection error";
        default:
            // FALLTHROUGH
        case MavlinkBackdoor::Result::UNKNOWN:
            return "Unknown";
    }
}

} // namespace dronecode_sdk
