#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "mavlink_passthrough_impl.h"

namespace mavsdk {

MavlinkPassthrough::MavlinkPassthrough(System& system) :
    PluginBase(),
    _impl{std::make_unique<MavlinkPassthroughImpl>(system)}
{}

MavlinkPassthrough::MavlinkPassthrough(std::shared_ptr<System> system) :
    PluginBase(),
    _impl{std::make_unique<MavlinkPassthroughImpl>(system)}
{}

MavlinkPassthrough::~MavlinkPassthrough() {}

MavlinkPassthrough::Result MavlinkPassthrough::send_message(mavlink_message_t& message)
{
    return _impl->send_message(message);
}

MavlinkPassthrough::Result MavlinkPassthrough::send_command_int(const CommandInt& command)
{
    return _impl->send_command_int(command);
}

MavlinkPassthrough::Result MavlinkPassthrough::send_command_long(const CommandLong& command)
{
    return _impl->send_command_long(command);
}

mavlink_message_t MavlinkPassthrough::make_command_ack_message(
    const uint8_t target_sysid,
    const uint8_t target_compid,
    const uint16_t command,
    MAV_RESULT result)
{
    return _impl->make_command_ack_message(target_sysid, target_compid, command, result);
}

std::pair<MavlinkPassthrough::Result, int32_t> MavlinkPassthrough::get_param_int(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    return _impl->get_param_int(name, maybe_component_id, extended);
}

std::pair<MavlinkPassthrough::Result, float> MavlinkPassthrough::get_param_float(
    const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended)
{
    return _impl->get_param_float(name, maybe_component_id, extended);
}

MavlinkPassthrough::MessageHandle
MavlinkPassthrough::subscribe_message(uint16_t message_id, const MessageCallback& callback)
{
    return _impl->subscribe_message(message_id, callback);
}

void MavlinkPassthrough::unsubscribe_message(MessageHandle handle)
{
    _impl->unsubscribe_message(handle);
}

std::ostream& operator<<(std::ostream& str, MavlinkPassthrough::Result const& result)
{
    switch (result) {
        default:
        // FALLTHROUGH
        case MavlinkPassthrough::Result::Unknown:
            return str << "Unknown";
        case MavlinkPassthrough::Result::Success:
            return str << "Success";
        case MavlinkPassthrough::Result::ConnectionError:
            return str << "Connection Error";
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

} // namespace mavsdk
