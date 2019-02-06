#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "mavlink_passthrough_impl.h"

namespace dronecode_sdk {

MavlinkPassthrough::MavlinkPassthrough(System &system) :
    PluginBase(),
    _impl{new MavlinkPassthroughImpl(system)}
{}

MavlinkPassthrough::~MavlinkPassthrough() {}

MavlinkPassthrough::Result MavlinkPassthrough::send_message(const mavlink_message_t &message)
{
    return _impl->send_message(message);
}

void MavlinkPassthrough::subscribe_message_async(
    uint16_t message_id, std::function<void(const mavlink_message_t &)> callback)
{
    _impl->subscribe_message_async(message_id, callback);
}

std::string MavlinkPassthrough::result_str(Result result)
{
    switch (result) {
        case MavlinkPassthrough::Result::SUCCESS:
            return "Success";
        case MavlinkPassthrough::Result::CONNECTION_ERROR:
            return "Connection error";
        default:
            // FALLTHROUGH
        case MavlinkPassthrough::Result::UNKNOWN:
            return "Unknown";
    }
}

uint8_t MavlinkPassthrough::get_our_sysid() const
{
    return _impl->get_our_sysid();
}
uint8_t MavlinkPassthrough::get_our_compid() const
{
    return _impl->get_our_compid();
}

uint8_t MavlinkPassthrough::get_target_sysid() const
{
    return _impl->get_target_sysid();
}

uint8_t MavlinkPassthrough::get_target_compid() const
{
    return _impl->get_target_compid();
}

} // namespace dronecode_sdk
