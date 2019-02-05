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
    uint16_t message_id, std::function<void(const mavlink_message_t &)> callback)
{
    _impl->subscribe_message_async(message_id, callback);
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

uint8_t MavlinkBackdoor::get_our_sysid() const
{
    return _impl->get_our_sysid();
}
uint8_t MavlinkBackdoor::get_our_compid() const
{
    return _impl->get_our_compid();
}

uint8_t MavlinkBackdoor::get_target_sysid() const
{
    return _impl->get_target_sysid();
}

uint8_t MavlinkBackdoor::get_target_compid() const
{
    return _impl->get_target_compid();
}

} // namespace dronecode_sdk
