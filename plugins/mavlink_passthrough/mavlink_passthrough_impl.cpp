#include <functional>
#include "mavlink_passthrough_impl.h"
#include "system.h"
#include "global_include.h"

namespace dronecode_sdk {

MavlinkPassthroughImpl::MavlinkPassthroughImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MavlinkPassthroughImpl::~MavlinkPassthroughImpl()
{
    _parent->unregister_plugin(this);
}

void MavlinkPassthroughImpl::init() {}

void MavlinkPassthroughImpl::deinit() {}

void MavlinkPassthroughImpl::enable() {}

void MavlinkPassthroughImpl::disable() {}

MavlinkPassthrough::Result MavlinkPassthroughImpl::send_message(const mavlink_message_t &message)
{
    if (!_parent->send_message(message)) {
        return MavlinkPassthrough::Result::CONNECTION_ERROR;
    }
    return MavlinkPassthrough::Result::SUCCESS;
}

void MavlinkPassthroughImpl::subscribe_message_async(
    uint16_t message_id, std::function<void(const mavlink_message_t &)> callback)
{
    _parent->register_mavlink_message_handler(message_id, callback, this);
}

uint8_t MavlinkPassthroughImpl::get_our_sysid() const
{
    return GCSClient::system_id;
}

uint8_t MavlinkPassthroughImpl::get_our_compid() const
{
    return GCSClient::component_id;
}

uint8_t MavlinkPassthroughImpl::get_target_sysid() const
{
    return _parent->get_system_id();
}

uint8_t MavlinkPassthroughImpl::get_target_compid() const
{
    return _parent->get_autopilot_id();
}

} // namespace dronecode_sdk
